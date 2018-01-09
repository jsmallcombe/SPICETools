
{
	
	// grsisort -l
	// gSystem->Load("libJanalysistools.so");
	// jsigfig(1,1);
	// .x /home/S1708/SPICETools/scripts/TigRecal.C

	
	TChannel::ReadCalFile("Tig.cal");

	
	TFile in("tigrecal.root","READ");
	if(!in.IsOpen())return;
	gROOT->cd();
	
	TH2* graph;

	TIter next(in.GetListOfKeys());
	TKey *key;
	while ((key = (TKey*)next())){
		TClass *cl = gROOT->GetClass(key->GetClassName());
		if (cl->InheritsFrom("TH2")){
			graph=(TH2*)key->ReadObj()->Clone("CloneGraph");
			break;
		}
	}
	
	vector<vector<double>> p;
	vector<vector<bool>> b;

	////////////////////////// 70Se Data /////////////////////////////
// 	p.push_back(vector<double>{160,177.9,187.5,191.437,205});
// 	b.push_back(vector<bool>{1,1,0});
// 
// 	p.push_back(vector<double>{272,279.01,285});
// 	b.push_back(vector<bool>{1});	
// 
// 	p.push_back(vector<double>{305,311.39,324.81,330});
// 	b.push_back(vector<bool>{1,1});	
// 
// 	p.push_back(vector<double>{405,415.222,423.150,425.3,433});
// 	b.push_back(vector<bool>{1,0,0});	
// 
// 	p.push_back(vector<double>{560,573.9,583.1,590});
// 	b.push_back(vector<bool>{1,1});	
// 
// 	p.push_back(vector<double>{763,774.73,790});
// 	b.push_back(vector<bool>{1});	
// 	
// 	p.push_back(vector<double>{855,862.03,870});
// 	b.push_back(vector<bool>{1});	
// 
// 	p.push_back(vector<double>{1525,1536.1,1522.55,1545});
// 	b.push_back(vector<bool>{1,0});
// 	
// 	p.push_back(vector<double>{930,944.51,952});
// 	b.push_back(vector<bool>{1});
// 	
// 	p.push_back(vector<double>{1020,1039.513,1034.40,1050});
// 	b.push_back(vector<bool>{1,0});
// 	
// 	p.push_back(vector<double>{1085,1094.4,1098});
// 	b.push_back(vector<bool>{1});
// 	
// 	p.push_back(vector<double>{1690,1707.61,1713.00,1720});
// 	b.push_back(vector<bool>{1,0});
// 	
// 	p.push_back(vector<double>{1770,1780.52,1790});
// 	b.push_back(vector<bool>{1});
	////////////////////////// 70Se Data /////////////////////////////


	////////////////////////// 152Sm Data (160Er) /////////////////////////////
	
	p.push_back(vector<double>{105,121.7817,125.8,135});
	b.push_back(vector<bool>{1,1});
	
	p.push_back(vector<double>{253,264.1,275});
	b.push_back(vector<bool>{1});
	
	p.push_back(vector<double>{360,375.8,385});
	b.push_back(vector<bool>{1});
	
	p.push_back(vector<double>{450,463.7,473});
	b.push_back(vector<bool>{1});
	
	p.push_back(vector<double>{1376,1386,1396});
	b.push_back(vector<bool>{1});
	
	////////////////////////// 152Sm Data (160Er) /////////////////////////////	
	
	TFile out("tigrecaloutput.root","RECREATE");
	gROOT->cd();

	for(unsigned int i=0;i<64;i++){
		stringstream tt;
		tt<<"CrystalE"<<i;
		TH1D* seg = graph->ProjectionY(tt.str().c_str(),i+1,i+1);
		
		if(seg->Integral()<100)continue;
			
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
		TChannel* chanA=TChannel::FindChannelByName((ss.str()+"A").c_str());
		TChannel* chanB=TChannel::FindChannelByName((ss.str()+"B").c_str());
		
		std::vector<Float_t> old_cal;
		if(chanA){old_cal=chanA->GetENGCoeff();}
		else if(chanB){old_cal=chanB->GetENGCoeff();}
		else continue;
		   
		if(old_cal.size()<2)continue;
		old_cal.push_back(0);
		
		vector<double> fitE;
		vector<double> trueE;
		
		for(int i=0;i<p.size();i++){
			vector< jPeakDat > fInput;
			double P=p[i][1];
			FindBinRangeMaxD(seg,P,5.);
			
			fInput.push_back(jPeakDat(P,true,5));
			for(int j=2;j+1<p[i].size();j++)fInput.push_back(jPeakDat(p[i][j]-p[i][j-1],true,0.2));	 
			FullFitHolder* fitA = Ultrapeak::PeakFit(seg,p[i][0],p[i][p[i].size()-1],fInput,2,1);

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
		
						
			if(chanA){
				chanA->DestroyENGCal();
				chanA->AddENGCoefficient(line.GetParameter(0));
				chanA->AddENGCoefficient(line.GetParameter(1));
				chanA->AddENGCoefficient(line.GetParameter(2));
			}
			
			if(chanB){
				chanB->DestroyENGCal();
				chanB->AddENGCoefficient(line.GetParameter(0));
				chanB->AddENGCoefficient(line.GetParameter(1));
				chanB->AddENGCoefficient(line.GetParameter(2));
			}	
			
			
	// 		TCanvas* C1=new TCanvas("can","cam",1900,800);
	// 		C1->cd();
	// 		gPad->Update();//This is a line you sometimes need after chaging pads when drawing in script
	// // 			points.DrawClone("ap");
	// 			seg->DrawCopy();
	// 		C1->WaitPrimitive(); //Root functions that will wait for you to look at canvas (close or double click it)
			
			out.cd();
				seg->Write();
				stringstream ww;
				ww<<"PolFit"<<i;
				points.Write(ww.str().c_str());
			gROOT->cd();
		}
			
		delete seg;
	}

	out.Write();
	out.Close();
// 	new TBrowser;
	
	TChannel::WriteCalFile("TigReCaled.cal");
	
}

