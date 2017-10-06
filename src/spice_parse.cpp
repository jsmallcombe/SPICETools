#include "spice_parse.h"



int SpiceParse(int argc, char *argv[]){
  cmdinteruptflag=0;	
 // Register signals 
  signal(SIGINT, cmdsigfunction); 
  //This bit was added to allow the rootfile to get saves if you ctrl+c on the command line

  
////////////////// PROCESS INPUT FILE LIST //////////////////	
  
	SPICEcmdline inp(argc,argv);
	
	string outputfile=OrDefault("spice_check.root",inp.RootFile());
		
	bool frag=false;
	TChain *DataChain=inp.FragmentTree();
	if(DataChain->GetEntries()){
		frag=true;
	}else{
		delete DataChain;
		DataChain=inp.AnalysisTree();
	}
	
	int ret=0;
	if(inp.LoadCal(DataChain)){
		
		bool UseFitCharge=inp.IsPresent("UseFitCharge");
		if(UseFitCharge)cout<<endl<<"Using SiLi Waveform fit charge.";
		bool FitShapes=inp.IsPresent("FitShape");
		if(FitShapes)cout<<endl<<"Fitting Waveform Shapes..";
		bool MohRotate=inp.IsPresent("MohRotate");
		if(MohRotate)cout<<endl<<"Producing Rotation Histogram.";
		
		processchain(DataChain,outputfile,frag,UseFitCharge,FitShapes,MohRotate);
		ret=1;
	}
	delete DataChain;
	return ret;
}


void processchain(TChain* DataChain,string outputfile,bool frag,bool UseFitCharge,bool FitShapes,bool MohRotate){
	
	if(UseFitCharge){
		TChannel::SetIntegration("SPI",1);
	}else TChannel::SetIntegration("SP",125);
	TChannel::SetUseCalFileIntegration("SP",true);	
	
	//This extra line is needed to use 2 different integrations for the 2 different charge calcs.
	double integ=1;if(UseFitCharge)integ=125;
	
	TFragment *fragment = 0;  
	TSiLi *sili = 0;  
	TSiLiHit *sili_hit;

	Int_t nentries = DataChain->GetEntries();

	unsigned int wavelength=0;
	
	if(frag){
		DataChain->SetBranchAddress("TFragment",&fragment);
		
		//Quick loop to find the waveform length stored in this file
		unsigned int silichecked=0;
		for(int jentry=0;jentry<nentries;jentry++){
			DataChain->GetEntry(jentry); 
			
			if(fragment->GetChannel())if(fragment->GetChannel()->GetClassType() == TSiLi::Class()){
				silichecked++;
				if(fragment->HasWave()){
					wavelength=fragment->GetWaveform()->size();
					break;
				}
			}
			if(silichecked>1000);
		}
	}else{
		DataChain->SetBranchAddress("TSiLi",&sili);
	}
	


	////////////////// PREPARE OUTPUT FILE //////////////////

	int numberofwaves=50;	
	
	cout<<endl<<"Creating output file : "<<outputfile<<flush;

	gROOT->cd();
	TFile *outfile = new TFile(outputfile.c_str(),"RECREATE");
	outfile->cd();
	
	TH1F *charge[120];
	TH1F *fitcharge[120];
	outfile->mkdir("singles");
	outfile->cd("singles");
	for(int i=0;i<120;i++){
		stringstream ss;
		ss<<"SPICE_charge_"<<i;
		charge[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),20000,0,20000);
		stringstream SS;
		SS<<"SPICE_fitcharge_"<<i;
		fitcharge[i] = new TH1F(SS.str().c_str(),SS.str().c_str(),8000,0,4000);
	}
	outfile->cd();
	
	TH2F *waveforms[120];
	if(frag&&wavelength){
		outfile->mkdir("waveforms");
		outfile->cd("waveforms");
		for(int i=0;i<120;i++){
			stringstream ss;
			ss<<"SPICE_waveforms_"<<i;
			waveforms[i] = new TH2F(ss.str().c_str(),ss.str().c_str(),numberofwaves,0,numberofwaves,wavelength,0,wavelength);
		}
	}
	outfile->cd();
	
// 	TSiLi::FitSiLiShape=1;
	if(!frag)FitShapes=false;
	
	double spRT=TSiLi::sili_default_rise;
	double spDT=TSiLi::sili_default_decay;
	double spBL=TSiLi::sili_default_baseline;
	TH1F *basehist[120];
	TH1F *risehist[120];
	TH1F *decayhist[120];
	TH1F *basehistcut[120];
	TH1F *risehistcut[120];
	TH1F *decayhistcut[120];
	TH2F *basehist2d[120];
	TH2F *risehist2d[120];
	TH2F *decayhist2d[120];
	TH2F *decay,*base,*rise;
	TH2F *decaycut,*basecut,*risecut;
	if(FitShapes){
		outfile->mkdir("fitshapes");
		outfile->cd("fitshapes");
		for(int i=0;i<120;i++){
			stringstream bb,rr,dd;
			bb<<"SPICE_baseline_"<<i;
			basehist[i] = new TH1F(bb.str().c_str(),bb.str().c_str(),1000,spBL-1000,spBL+1000);
			rr<<"SPICE_risetime_"<<i;
			risehist[i] = new TH1F(rr.str().c_str(),rr.str().c_str(),1000,spRT*0.3+1,spRT*1.5-1);
			dd<<"SPICE_decaytime_"<<i;
			decayhist[i] = new TH1F(dd.str().c_str(),dd.str().c_str(),1000,spDT*0.3+1,spDT*3-1);
			bb<<"_cut";
			basehistcut[i] = new TH1F(bb.str().c_str(),bb.str().c_str(),1000,spBL-1000,spBL+1000);
			rr<<"_cut";
			risehistcut[i] = new TH1F(rr.str().c_str(),rr.str().c_str(),1000,spRT*0.3+1,spRT*1.5);
			dd<<"_cut";
			decayhistcut[i] = new TH1F(dd.str().c_str(),dd.str().c_str(),1000,spDT*0.3+1,spDT*3-1);
		}
		outfile->mkdir("fitshapesE");
		outfile->cd("fitshapesE");
		for(int i=0;i<120;i++){
			stringstream bb,rr,dd;
			bb<<"SPICE_baseline_2D_"<<i;
			basehist2d[i] = new TH2F(bb.str().c_str(),bb.str().c_str(),500,spBL-1000,spBL+1000,500,0,5000);
			rr<<"SPICE_risetime_2D_"<<i;
			risehist2d[i] = new TH2F(rr.str().c_str(),rr.str().c_str(),500,spRT*0.3+1,spRT*1.5-1,500,0,5000);
			dd<<"SPICE_decaytime_2D_"<<i;
			decayhist2d[i] = new TH2F(dd.str().c_str(),dd.str().c_str(),500,spDT*0.3+1,spDT*3-1,500,0,5000);
		}
		outfile->cd();
		stringstream bb,rr,dd;
		bb<<"SPICE_baseline";
		base = new TH2F(bb.str().c_str(),bb.str().c_str(),500,spBL-1000,spBL+1000,120,0,120);
		rr<<"SPICE_risetime";
		rise = new TH2F(rr.str().c_str(),rr.str().c_str(),500,spRT*0.3+1,spRT*1.5-1,120,0,120);
		dd<<"SPICE_decaytime";
		decay = new TH2F(dd.str().c_str(),dd.str().c_str(),500,spDT*0.3+1,spDT*3-1,120,0,120);
		bb<<"_cut";
		basecut = new TH2F(bb.str().c_str(),bb.str().c_str(),500,spBL-1000,spBL+1000,120,0,120);
		rr<<"_cut";
		risecut = new TH2F(rr.str().c_str(),rr.str().c_str(),500,spRT*0.3+1,spRT*1.5-1,120,0,120);
		dd<<"_cut";
		decaycut = new TH2F(dd.str().c_str(),dd.str().c_str(),500,spDT*0.3+1,spDT*3-1,120,0,120);
	}
	outfile->cd();
	
	if(!frag)MohRotate=false;
	TH3I *mohrotate;
	if(MohRotate){
		outfile->cd();
		mohrotate = new TH3I("MohRotate","MohRotate",120,0,120,1000,-5000,-1000,1000,0,10000);
	}
	
	TH2F* energy2D = new TH2F("energy2D","energy2D",120,0,120,3000,0,3000);
	TH2F* charge2D = new TH2F("charge2D","charge2D",120,0,120,2000,0,20000);
	TH2F* fitcharge2D = new TH2F("fitcharge2D","fitcharge2D",120,0,120,1000,0,4000);
	TH1F* total = new TH1F("total","total",2000,0,2000);

	gROOT->cd();
	
	TH2F* timeenergy;
	TH1F* eventrate;
	if(frag){
		vector<long>start,end;
		for(int i=0;i<5;i++){
			DataChain->GetEntry(i);
			start.push_back(fragment->GetTimeStamp());
			DataChain->GetEntry(nentries-i-1);
			end.push_back(fragment->GetTimeStamp());
		}
		std::sort(start.begin(), start.end());
		std::sort(end.begin(), end.end());
		outfile->cd();
			timeenergy = new TH2F("energytime","energytime",2000,start[2],end[2],3000,0,3000);
			eventrate = new TH1F("rate","rate",2000,start[2],end[2]);
			eventrate->SetMinimum(0);
		gROOT->cd();
	}
	

	////////////////// MAIN PROCESS OF THE CHAIN //////////////////	
	
	int wavecount=0;
	double wavestep=nentries/numberofwaves;
	
	int wavecounts[120]={0};
	
	TStopwatch stopper;
	int thousandth=nentries/1000;
	TGraph runspeedcheck;
	
	cout<<endl<<endl<<"Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop
		wavecount=floor(jentry/wavestep);
		
		int ssize=0;
		
		if(frag){
			TChannel* chan = fragment->GetChannel();
			if(chan)if(chan->GetClassType() == TSiLi::Class())ssize=1;
			eventrate->Fill(fragment->GetTimeStamp());
		}else{
			ssize=sili->GetMultiplicity();
		}	
			
			
		for(int i=0;i<ssize;i++){
			if(frag){
				sili_hit = new TSiLiHit(*fragment);
			}else{
				sili_hit = sili->GetSiLiHit(i);
			}
			
			///PROCESS energy
			if(UseFitCharge){
				sili_hit->UseFitCharge();//makes energy use fit charge. Note, GetCharge still returns original
			}

			int s=sili_hit->GetSegment();
			if(s>=0&&s<120&&sili_hit->GetEnergy()>10){
				
				fitcharge[s]->Fill(sili_hit->GetFitCharge());
				charge[s]->Fill(sili_hit->GetCharge()/integ);
				energy2D->Fill(s,sili_hit->GetEnergy());
				total->Fill(sili_hit->GetEnergy());
				charge2D->Fill(s,sili_hit->GetCharge()/integ);
				fitcharge2D->Fill(s,sili_hit->GetFitCharge());
	
				if(frag){
					timeenergy->Fill(sili_hit->GetTimeStamp(),sili_hit->GetEnergy());
				}
			}
			
			if(frag&&wavecounts[s]<=wavecount&&wavelength){
				if(fragment->HasWave()){
					const vector<Short_t>* wave=fragment->GetWaveform();
					if(wave->size()>0){
						wavecounts[s]++;
						for(unsigned int i=0;i<wave->size();i++){
							waveforms[s]->SetBinContent(wavecounts[s],i+1,wave->at(i));
						}
					}
				}
			}
			
			if(s>=0&&s<120&FitShapes){
				TPulseAnalyzer* pulse=TSiLiHit::FitFrag(*fragment,2,s);
				if(pulse){
					if(pulse->Get_bflag()){
						basehist[s]->Fill(pulse->Get_wpar_baselinefin());
						risehist[s]->Fill(pulse->Get_wpar_rise());
						decayhist[s]->Fill(pulse->Get_wpar_decay());
						basehist2d[s]->Fill(pulse->Get_wpar_baselinefin(),sili_hit->GetFitCharge());
						risehist2d[s]->Fill(pulse->Get_wpar_rise(),sili_hit->GetFitCharge());
						decayhist2d[s]->Fill(pulse->Get_wpar_decay(),sili_hit->GetFitCharge());
						base->Fill(pulse->Get_wpar_baselinefin(),s);
						rise->Fill(pulse->Get_wpar_rise(),s);
						decay->Fill(pulse->Get_wpar_decay(),s);
						if(sili_hit->GetFitCharge()>1000){
							basehistcut[s]->Fill(pulse->Get_wpar_baselinefin());
							risehistcut[s]->Fill(pulse->Get_wpar_rise());
							decayhistcut[s]->Fill(pulse->Get_wpar_decay());
							basecut->Fill(pulse->Get_wpar_baselinefin(),s);
							risecut->Fill(pulse->Get_wpar_rise(),s);
							decaycut->Fill(pulse->Get_wpar_decay(),s);
						}
					}
					delete pulse;
				}
			}
			
			if(s>=0&&s<120&MohRotate){
				if(fragment->HasWave()){
					const vector<Short_t>* wave=fragment->GetWaveform();
					if(wave->size()>0){
						mohrotate->Fill(s,wave->at(0),fragment->GetCharge()/integ);
					}
				}
			}
			
			if(frag)delete sili_hit;
		}	

		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
		
		if(jentry%thousandth == 0){
			runspeedcheck.SetPoint(runspeedcheck.GetN(),100. * (double)jentry/nentries,stopper.RealTime());
			stopper.Continue();
		}
		
		if(cmdinteruptflag){
			cout<<endl<<"Interrupted. Terminating and saving file at "<< std::setprecision(2) << 100. * (double)jentry/nentries<< " %."<<endl;
// 			break;
			goto interuptbreak;
		}
	}
	cout << "100 \% COMPLETE          \n";
	interuptbreak:
	cout<<endl<<"WRITING FILE"<<endl;
	
	if(FitShapes){
		ofstream outfile("WaveShapeParam.txt");
		for(int i=0;i<120;i++){
			outfile<<i;
			outfile<<" "<<basehistcut[i]->GetMean()<<" "<<basehistcut[i]->GetStdDev();
			outfile<<" "<<risehistcut[i]->GetMean()<<" "<<risehistcut[i]->GetStdDev();
			outfile<<" "<<decayhistcut[i]->GetMean()<<" "<<decayhistcut[i]->GetStdDev();
			outfile<<endl;
		}
		outfile.close();
	}
	
	
	outfile->cd();
	outfile->Write();
	runspeedcheck.Write("runspeedcheck");
	outfile->Close();
	

	cout<<endl<<"SAVED"<<endl<<endl;	
}

