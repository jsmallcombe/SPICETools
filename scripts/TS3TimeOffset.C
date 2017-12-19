{
	TS3 bob;
	
// 	TChain* AnalysisTree = new TChain("AnalysisTree","AnalysisTree");	
//      AnalysisTree->Add("/home/jsmallcombe/Se70Sept2017data/newanal/analysis42506_021.root");
	
	TTree* AnalysisTree = (TTree*)_file0->Get("AnalysisTree");

	TS3 *s3 = 0;
	if(AnalysisTree->FindBranch("TS3"))AnalysisTree->SetBranchAddress("TS3",&s3);else return;

	TChannel::ReadCalFile("CalibrationFile.cal");

	TFile out("S3times.root","RECREATE");
	out.cd();	
		TH2F* SectorTimes=new TH2F("S3RS_time_sectors","S3RS_time_sectors;#Deltat [ns];Sector",1024,-64,64,32,0,32);
		TH2F* RingTimes=new TH2F("S3RS_time_rings","S3RS_time_rings;#Deltat [ns];Ring",1024,-64,64,24,0,24);
		TH2F* SectorCor=new TH2F("SectorCor","SectorCor;#Deltat [ns];Sector",1024,-64,64,32,0,32);
		TH2F* RingCor=new TH2F("RingCor","RingCor;#Deltat [ns];Ring",1024,-64,64,24,0,24);
		TH1F* Total=new TH1F("Total","Total;#Deltat [ns]",1024,-64,64);
		TH1F* TotalCor=new TH1F("TotalCor","TotalCor;#Deltat [ns]",1024,-64,64);
	gROOT->cd();
	
	
	TChannel* chan[56]={0};	
	
	long nentries = AnalysisTree->GetEntries();
	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			if(!chan[SS->GetSegment()+24]){
				chan[SS->GetSegment()+24]=SS->GetChannel();
				chan[SS->GetSegment()+24]->DestroyTIMECal();
			}
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
				if(!chan[SR->GetSegment()]){
					chan[SR->GetSegment()]=SR->GetChannel();
					chan[SR->GetSegment()]->DestroyTIMECal();
				}
				
				double TT=SR->GetTime()-SS->GetTime();
				
				if(SR->GetSegment()==0){
					SectorTimes->Fill(TT,SS->GetSegment());
				}
				
				Total->Fill(TT);
				
			}
			
			
		}

	}
	
	
	double sectortime[32];
	for(unsigned int i=0;i<32;i++){
		TH1D* sect = SectorTimes->ProjectionX("sect",i+1,i+1);
		
		sect->GetXaxis()->SetRange(sect->GetMaximumBin()-10,sect->GetMaximumBin()+10);
// 		sectortime[i]=-sect->GetXaxis()->GetBinCenter(sect->GetMaximumBin());
		sectortime[i]=-sect->GetMean();
		if(chan[i+24]){
			cout<<endl<<i<<" "<<sectortime[i];
			chan[i+24]->DestroyTIMECal();
			chan[i+24]->AddTIMECoefficient(sectortime[i]/10.0);
			chan[i+24]->AddTIMECoefficient(0);
			chan[i+24]->AddTIMECoefficient(0);

		}
		delete sect;
	}
	cout<<endl;
	

	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
// 				double TT=SR->GetCfd()-(SS->GetCfd()-sectortime[SS->GetSegment()]);
				double TT=SR->GetTime()-SS->GetTime();
				
				RingTimes->Fill(TT,SR->GetSegment());
			}
		}

	}	
	
	
	double ringtime[24];
	for(unsigned int i=0;i<24;i++){
		TH1D* sect = RingTimes->ProjectionX("sect",i+1,i+1);
		sect->GetXaxis()->SetRange(sect->GetMaximumBin()-10,sect->GetMaximumBin()+10);
// 		ringtime[i]=sect->GetXaxis()->GetBinCenter(sect->GetMaximumBin());
		ringtime[i]=sect->GetMean();
		if(chan[i]){
			chan[i]->DestroyTIMECal();
			chan[i]->AddTIMECoefficient(ringtime[i]/10.0);
			chan[i]->AddTIMECoefficient(0);
			chan[i]->AddTIMECoefficient(0);
		}
		delete sect;
	}
	
	
	
	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
				
// 				double TT=(SR->GetCfd()-ringtime[SR->GetSegment()])-(SS->GetCfd()-sectortime[SS->GetSegment()]);
				double TT=SR->GetTime()-SS->GetTime();


				RingCor->Fill(TT,SR->GetSegment());
					SectorCor->Fill(TT,SS->GetSegment());
				TotalCor->Fill(TT);
			}
		}
	}	
	


	out.Write();
// 	out.Close();
	new TBrowser;
	
	TChannel::WriteCalFile("S3TimeAdjustedCalibrationFile.cal");
	
}


