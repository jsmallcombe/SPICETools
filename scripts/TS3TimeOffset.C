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
		TH2F* SectorTimes=new TH2F("S3RS_time_sectors","S3RS_time_sectors;#Deltat [10/16 ns];Sector",1024,-512,512,32,0,32);
		TH2F* RingTimes=new TH2F("S3RS_time_rings","S3RS_time_rings;#Deltat [10/16 ns];Ring",1024,-512,512,24,0,24);
		TH2F* SectorCor=new TH2F("SectorCor","SectorCor;#Deltat [10/16 ns];Sector",1024,-512,512,32,0,32);
		TH2F* RingCor=new TH2F("RingCor","RingCor;#Deltat [10/16 ns];Ring",1024,-512,512,24,0,24);
		TH1F* Total=new TH1F("Total","Total;#Deltat [10/16 ns]",1024,-512,512);
		TH1F* TotalCor=new TH1F("TotalCor","TotalCor;#Deltat [10/16 ns]",1024,-512,512);
	gROOT->cd();
	
	long nentries = AnalysisTree->GetEntries();
	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
				double TT=SR->GetCfd()-SS->GetCfd();
				
				if(SR->GetSegment()==0){
					SectorTimes->Fill(TT,SS->GetSegment());
				}
			}
		}

	}

	double sectortime[32];
	for(unsigned int i=0;i<32;i++){
		TH1D* sect = SectorTimes->ProjectionX("sect",i+1,i+1);
// 		sectortime[i]=-sect->GetXaxis()->GetBinCenter(sect->GetMaximumBin());
		sectortime[i]=-sect->GetMean();
		cout<<endl<<i<<" "<<sectortime[i];
		delete sect;
	}
	
	
	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
				double TT=SR->GetCfd()-(SS->GetCfd()-sectortime[SS->GetSegment()]);
				
				RingTimes->Fill(TT,SR->GetSegment());
			}
		}

	}	
	
	double ringtime[24];
	for(unsigned int i=0;i<24;i++){
		TH1D* sect = RingTimes->ProjectionX("sect",i+1,i+1);
// 		ringtime[i]=sect->GetXaxis()->GetBinCenter(sect->GetMaximumBin());
		ringtime[i]=sect->GetMean();
		delete sect;
	}
	
	
	
	for(long jentry=0;jentry<nentries;jentry++){
		AnalysisTree->GetEntry(jentry); 
		
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
				TS3Hit* SR=s3->GetRingHit(j);
				
				Total->Fill(SR->GetCfd()-SS->GetCfd());
				
				double TT=(SR->GetCfd()-ringtime[SR->GetSegment()])-(SS->GetCfd()-sectortime[SS->GetSegment()]);


				RingCor->Fill(TT,SR->GetSegment());
					SectorCor->Fill(TT,SS->GetSegment());
				TotalCor->Fill(TT);
			}
		}
	}	
	


	out.Write();
	out.Close();
}


