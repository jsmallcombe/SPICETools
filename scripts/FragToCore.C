{
	TFragment bob;
	
	
	TChain* FragmentTree = new TChain("FragmentTree","FragmentTree");	
// 	FragmentTree->Add("frag/fragment42906_*");
// 	FragmentTree->Add("frag/fragment42907_*");
// 	
	FragmentTree->Add("frag/fragment42906_000.root");
	FragmentTree->Add("frag/fragment42907_000.root");
	
// 	TTree* FragmentTree = (TTree*)_file0->Get("FragmentTree");



	
	TFragment *frag = 0;
	if(FragmentTree->FindBranch("TFragment"))FragmentTree->SetBranchAddress("TFragment",&frag);else return;

// 	TChannel::ReadCalFile("CalibrationFile.cal");
	TChannel::ReadCalFile("TigABCoresReCaled.cal");
	
	TChannel::SetIntegration("TI",125,EPriority::kForce);
	TChannel::SetUseCalFileIntegration("TI",true,EPriority::kForce);
	
	TFile out("ABcores.root","RECREATE");
	out.cd();	
		TH2F* Acores=new TH2F("Acores","Acores;Core;Energy [keV]",64,0,64,2048,0,2048);
		TH2F* Bcores=new TH2F("Bcores","Bcores;Core;Energy [keV]",64,0,64,2048,0,2048);
		TH1F* Energy=new TH1F("Energy","Energy;Energy [keV]",100000,0,100000);
	gROOT->cd();

	long nentries = FragmentTree->GetEntries();
	for(long jentry=0;jentry<nentries;jentry++){
		FragmentTree->GetEntry(jentry); 
		
		TChannel* chan = frag->GetChannel();
		
		if(frag->GetEnergy()==0)continue;
		if(chan == nullptr) continue;
		
		if(chan->GetMnemonic()->System() != TMnemonic::ESystem::kTigress) continue;
		if((chan->GetMnemonic()->SubSystem() == TMnemonic::EMnemonic::kG) &&
			(chan->GetSegmentNumber() == 0 || chan->GetSegmentNumber() == 9)) { // it is a core
	      
			if(chan->GetMnemonic()->OutputSensor() == TMnemonic::EMnemonic::kB) {
				Bcores->Fill(frag->GetArrayNumber(),frag->GetEnergy());
			}else{
				Acores->Fill(frag->GetArrayNumber(),frag->GetEnergy());				
			}      
			
			Energy->Fill(frag->GetEnergy());
		}
		
// 		cout<<jentry<< flush; 
		
		if(jentry%1000 == 0){
			cout << setiosflags(ios::fixed) << std::setprecision(2) << 100.*(double)jentry/nentries << " % complete."<<"\r" << flush; 
		}
	}
	
	out.Write();
	out.Close();
}

