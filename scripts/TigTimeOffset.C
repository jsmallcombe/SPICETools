{
	TSiLi sili;
	TChannel::ReadCalFile("CalibrationFile.cal");

	
	TFile in("tigs3.root","READ");
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
	

	
	for(unsigned int i=0;i<64;i++){
		TH1D* seg = graph->ProjectionX("sect",i+1,i+1);
		
		if(seg->Integral()<100)continue;
			
		
		seg->GetXaxis()->SetRange(seg->GetMaximumBin()-3,seg->GetMaximumBin()+3);
		double mean=seg->GetMean();
		
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
		
		mean-=49.06;
		if(chanA){
			cout<<endl<<i<<" A "<<mean;
			cout<<endl<<i<<" A "<<ss.str()+"A";
			chanA->DestroyTIMECal();
			chanA->AddTIMECoefficient(mean/10.0);
			chanA->AddTIMECoefficient(0);
			chanA->AddTIMECoefficient(0);
		}
		
		if(chanB){
// 			cout<<endl<<i<<" B "<<mean;
			chanB->DestroyTIMECal();
			chanB->AddTIMECoefficient(mean/10.0);
			chanB->AddTIMECoefficient(0);
			chanB->AddTIMECoefficient(0);
		}
		
 		delete seg;
	}

// 	out.Write();
// 	out.Close();
// 	new TBrowser;
	
	TChannel::WriteCalFile("TigTimeAdjusted.cal");
	
}

