{
	bool CutBottom=false;
	//found this was needed for some experiments where there was too much noise at low values
	
	TSiLi sili;
	TChannel::ReadCalFile("CalibrationFile.cal");

	double walkcoef=0.002601418;// in units 10ns /keV

	TFile in("silis3.root","READ");
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
	
// 	TFile out("S3times.root","RECREATE");
// 	out.cd();	
// 		TH2F* SectorTimes=new TH2F("S3RS_time_sectors","S3RS_time_sectors;#Deltat [ns];Sector",1024,-64,64,32,0,32);
// 	gROOT->cd();
// 	
	
	for(unsigned int i=0;i<120;i++){
		TH1D* seg = graph->ProjectionY("sect",i+1,i+1);
		
		if(seg->Integral()<100)continue;
		double mean=seg->GetMean();
		
		if(CutBottom)seg->GetXaxis()->SetRange(seg->GetXaxis()->FindBin(mean),seg->GetNbinsX());
		
		
		double offset =seg->GetXaxis()->GetBinCenter(seg->GetMaximumBin());
		
		seg->GetXaxis()->SetRangeUser(offset-30,offset+30);
		
		offset = seg->GetMean();
		
		TChannel* chan=TSiLiHit::GetSiLiHitChannel(i);
		
// 		cout<<endl<<i<<" "<<chan;
		if(chan){
			cout<<endl<<i<<" "<<offset;
			chan->DestroyTIMECal();
			chan->AddTIMECoefficient(-offset/10.0);
			chan->AddTIMECoefficient(walkcoef);
			chan->AddTIMECoefficient(1);
		}
 		delete seg;
	}

// 	out.Write();
// 	out.Close();
// 	new TBrowser;
	
	TChannel::WriteCalFile("SiLiTimeAdjusted.cal");
	
}


