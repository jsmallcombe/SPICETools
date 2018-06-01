
{
	
//	grsisort -l
// 	gSystem->Load("libJanalysistools.so");
// 	jsigfig(1,1);
//	 .x /home/S1708/SPICETools/scripts/TigCoreRecal.C

	
	TChannel::ReadCalFile("Tig.cal");

	TFile in("ABcores.root","READ");
	if(!in.IsOpen())return;
	gROOT->cd();
	
	TH2* ABcores[2]={0,0};
	
	if(in.Get("Acores")){
		ABcores[0]=(TH2*)in.Get("Acores");
	}else{
		return;
	}
	if(in.Get("Bcores")){
		ABcores[1]=(TH2*)in.Get("Bcores");
	}	
	

	// Define peak fits
	
	vector<vector<double>> p;
	vector<vector<bool>> b;

	////////////////////////// Prescale Point /////////////////////////////
	// If calibration is very poor or non exist specify a peak to set a scale
	// Pick an isolated peak at high energy.
	
	bool prescale = true;
	double prescalepoint[3]={1200,1408.00,1600};
	//Search range down, true energy, search range up
	
	
	////////////////////////// 152Eu Data /////////////////////////////
	
	p.push_back(vector<double>{948,964.08,980});
	b.push_back(vector<bool>{1});

	p.push_back(vector<double>{113,121.78,131});
	b.push_back(vector<bool>{1});	
	
	p.push_back(vector<double>{330,344.28,355});
	b.push_back(vector<bool>{1});	
	
	p.push_back(vector<double>{238,244.70,252});
	b.push_back(vector<bool>{1});	
	
	p.push_back(vector<double>{1387,1408.00,1417.5,1418});
	b.push_back(vector<bool>{1,0});	
	

	p.push_back(vector<double>{1070,1085.90,1089.737,1095});
	b.push_back(vector<bool>{1,0});	
	
	p.push_back(vector<double>{1099,1112.10,1120});
	b.push_back(vector<bool>{1});	
	
	p.push_back(vector<double>{766,778.90,787});
	b.push_back(vector<bool>{1});			
	
	
	////////////////////////// Fit /////////////////////////////	
	
	TFile out("TigABCoresReCaled.root","RECREATE");
	gROOT->cd();
	
	string stab[2]={"A","B"};
	
	for(unsigned int i=0;i<64;i++){
	for(unsigned int ab=0;ab<2;ab++){

		stringstream tt;
		tt<<"CrystalE"<<i<<stab[ab];
		
			
		unsigned int tig=i/4;
		tig++;
		int letter=(i%4);
		
		std::stringstream ss;
		ss<<"TIG"<< std::setfill('0') << std::setw(2) << tig;		
	 
		switch(letter) {
			case 0:ss<<"B";
				break;
			case 1:ss<<"G";
				break;
			case 2:ss<<"R";
				break;
			default:ss<<"W";
				break;
		}		
		ss<<"N00";
		TChannel* chan=0;
		if(ab)chan=TChannel::FindChannelByName((ss.str()+"B").c_str());
		else chan=TChannel::FindChannelByName((ss.str()+"A").c_str()); 
		
		std::vector<Float_t> old_cal;
		if(chan){old_cal=chan->GetENGCoeff();}
		else continue;
		
		if(!ABcores[ab]){
			chan->DestroyENGCal();
			chan->AddENGCoefficient(0);
			chan->AddENGCoefficient(0);
			continue;
		}
		
		TH1D* seg = ABcores[ab]->ProjectionY(tt.str().c_str(),i+1,i+1);

		if(seg->Integral()<100){
			chan->DestroyENGCal();
			chan->AddENGCoefficient(0);
			chan->AddENGCoefficient(0);
			continue;
		}

		   
		if(old_cal.size()<2)continue;
		old_cal.push_back(0);
		
		vector<double> fitE;
		vector<double> trueE;
		
		double scale=1;
		if(prescale){
			seg->GetXaxis()->SetRangeUser(prescalepoint[0],prescalepoint[2]);
			scale=seg->GetXaxis()->GetBinCenter(seg->GetMaximumBin())/prescalepoint[1];
			seg->GetXaxis()->SetRange();
		}
		
		for(int i=0;i<p.size();i++){

			vector< jPeakDat > fInput;
			double P=p[i][1]*scale;
			FindBinRangeMaxD(seg,P,5.*scale);
			
			fInput.push_back(jPeakDat(P,true,5));
			for(int j=2;j+1<p[i].size();j++)fInput.push_back(jPeakDat((p[i][j]-p[i][j-1])*scale,true,0.2*scale));	 
			FullFitHolder* fitA = Ultrapeak::PeakFit(seg,p[i][0]*scale,p[i][p[i].size()-1]*scale,fInput,2,1);

			if(fitA){
// 					Ultrapeak::DrawPeak(fitA,0,seg);
				seg->GetListOfFunctions()->Add(fitA->Clone());
				
				for(int j=1;j+1<p[i].size();j++){
					if(b[i][j-1]){
						trueE.push_back(p[i][j]);
						fitE.push_back(fitA->CVal(Ultrapeak::VPC(j-1)));
					}
				}

				//If we are done with fitA, delete it and zero the pointer.
				delete fitA;fitA=0;
			}	
		}	
			
		TGraph points;
		
		for(int i=0;i<fitE.size();i++){
			double c=old_cal[0]-fitE[i];
			double b=old_cal[1];
			double a=old_cal[2];
			
			double charge=(fitE[i]-old_cal[0])/old_cal[1];
			if(old_cal[2]){
				double sq=b*b-4*a*c;
				if(sq<=0)continue;
				
				charge=(sqrt(sq)-b)/(2*a);
				if(charge<=0)charge=-(sqrt(sq)+b)/(2*a);
				
			}
			
			points.SetPoint(points.GetN(),charge,trueE[i]);
		}
		
		if(points.GetN()>3){

			TF1 line("pol2","pol2",0,1000000);
			points.Fit(&line,"0QW");
			points.Fit(&line,"");
			points.SetMarkerStyle(20);
		
			cout<<endl;
			cout<<endl<<old_cal[0]<<" "<<line.GetParameter(0);
			cout<<endl<<old_cal[1]<<" "<<line.GetParameter(1);
			cout<<endl<<old_cal[2]<<" "<<line.GetParameter(2);
			cout<<endl;
					
			chan->DestroyENGCal();
			chan->AddENGCoefficient(line.GetParameter(0));
			chan->AddENGCoefficient(line.GetParameter(1));
			chan->AddENGCoefficient(line.GetParameter(2));

	// 		TCanvas* C1=new TCanvas("can","cam",1900,800);
	// 		C1->cd();
	// 		gPad->Update();//This is a line you sometimes need after chaging pads when drawing in script
	// // 			points.DrawClone("ap");
	// 			seg->DrawCopy();
	// 		C1->WaitPrimitive(); //Root functions that will wait for you to look at canvas (close or double click it)
			
			out.cd();
				seg->Write();
				stringstream ww;
				ww<<"PolFit"<<i<<stab[ab];
				points.Write(ww.str().c_str());
			gROOT->cd();
		}
			
		delete seg;
	}}
	
	in.Close();
	out.Write();
	out.Close();
// 	new TBrowser;
	
	TChannel::WriteCalFile("TigABCoresReCaled.cal");
	
}

