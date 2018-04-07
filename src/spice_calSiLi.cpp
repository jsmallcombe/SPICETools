#include "spice_calSiLi.h"

double BiPeaks[6]={481.6935,553.8372,565.8473,975.651,1047.795,1059.805};

bool YNQ(){
	cout<<" y/n?"<<flush;
	char yn;
	cin>>yn;
	return(yn=='Y'||yn=='y');
}	
	

void SiLiCalParse(int argc, char *argv[]){
	SPICEcmdline inp(argc,argv);
	
	bool FixOnly=inp.IsPresent("Fix");
	bool UseFitCharge=inp.IsPresent("UseFitCharge");
	if(UseFitCharge)cout<<endl<<"Using Fit Charge";
		
	TChain *DataChain=inp.AnalysisTree();
	
	if(inp.LoadCal(DataChain)){
// 		
		if(UseFitCharge)TChannel::SetIntegration("SPI",1,EPriority::kForce);
		else TChannel::SetIntegration("SP",125,EPriority::kForce);
		TChannel::SetUseCalFileIntegration("SP",true,EPriority::kForce);
		
		TH2* charge=ChargeParse(DataChain,UseFitCharge);
		if(charge){
			
			vector<double> offset(120,0),gain(120,0);
			if(!FixOnly){
				CalSiLiBi207(charge,offset,gain);
				TChannel::WriteCalFile("SiLiEnergyCalibrated.cal");
			}
			
			TH2* Energy=EnergyParse(DataChain,UseFitCharge);
			if(Energy){
				
				TH1D* proj=Energy->ProjectionY("EnergyTotalProjection");
				
				if(!FixOnly){
				
					TH1D* charNge=charge->ProjectionX("ChargeChanProj");
					TH1D* enerN=Energy->ProjectionX("EnergyChanProj");
					TH1D offhist("offhist","offhist",2000,-2000,200);
					TH1D gainhist("gainhist","gainhist",20000,0,20);
					
					vector<int> goodchan(120,1);
					for(int i=0;i<120;i++){
						if(charNge->GetBinContent(i+1)>200){
							if(enerN->GetBinContent(i+1)>200){	
								offhist.Fill(offset[i]);
								gainhist.Fill(gain[i]);
							}else{
								cout<<endl<<"SEGMENT "<<i<<" CALIBRATION FAIL"<<flush;
								goodchan[i]=0;
							}
						}
					}
					
					
					double gM=gainhist.GetMean();
					double gE=gainhist.GetStdDev();
					double oM=offhist.GetMean();
					double oE=offhist.GetStdDev();
					
					for(int i=0;i<120;i++){
						if(goodchan[i]&&charNge->GetBinContent(i+1)>200){
							if(abs(offset[i]-oM)>oE*2&&abs(gain[i]-gM)>gE*2){
								cout<<endl<<"SEGMENT "<<i<<" SUSPECT POOR CAL"<<flush;
								goodchan[i]=0;
							}
						}
					}
					
					proj->SetLineColor(2);
// 					for(int i=0;i<120;i++){
// 						if(!goodchan[i]){
// 							stringstream ss;
// 							ss<<"SPICE_"<<i<<"_calibrated";
// 							TH1D* th=Energy->ProjectionY(ss.str().c_str(),i+1,i+1);
// 							th->SetTitle(ss.str().c_str());
// 							
// 							jhistquestion* check1 = new jhistquestion("Keep","Retry","Abandon");
// 							
// 							check1->GetCanvas()->cd();
// 							th->DrawCopy();
// 							TH1* tot=proj->DrawCopy("histsame");
// 							tot->Scale(th->Integral()/proj->Integral());
// 							delete th;
// 							
// 							switch (check1->WaitAnswer()) {
// 								case 1: break;
// 								case 2: CalSiLiBi207Single(charge,i);
// 									break;
// 								default:
// 									ZeroChannel(i);
// 									break;
// 							}
// 
// 						}
// 					}
// 					
					delete enerN;
					delete charNge;
				}
				
				
				
				TChannel::WriteCalFile("SiLiEnergyCalibrated.cal");
				new ReFitMasterPanel(DataChain,UseFitCharge,charge,Energy);
				
// 				Energy->SetShowProjectionY(1);
// 				char command[80];
// 				while(2>1){
// 					TGInputDialog Dialog(gClient->GetDefaultRoot(), gClient->GetDefaultRoot(),"Recal Channel Number :", "q", command);
// 					unsigned int seg;
// 					sscanf(command, "%d", &seg);
// 					if(seg<120){
// 						CalSiLiBi207Single(charge,seg);
// 					}else{
// 						break;
// 					}
// 				}
// 				TChannel::WriteCalFile("SiLiEnergyCalibrated.cal");		
// 				TCanvas c1("CalResult","CalResult",800,400);
// 				c1.Divide(2,1);
// 				c1.cd(1);
// 				proj->DrawCopy();
// 				c1.cd(2);
// 				Energy->DrawCopy("colz");
// 				c1.Update();
// // 				c1.WaitPrimitive(); //will return with a double clicked
// 				
				
			}
			
			
			
			
			

			delete charge; //Or write to output
		}
	}
	
	return;
}



void CalSiLiBi207Single(TH2* charge,int seg){if(!charge)return;
	
	TChannel* chan=TSiLiHit::GetSiLiHitChannel(seg);
	chan->DestroyENGCal();
	
	stringstream ss;
	ss<<"SPICE_"<<seg<<"_charge";
	TH1D* hist=charge->ProjectionY(ss.str().c_str(),seg+1,seg+1);
	if(hist->Integral()<200){
		delete hist;
		cout<<endl<<ss.str()<<" INSUFFICIENT DATA"<<flush;
		return;
	}
	ss<<"Charge Fit SPICE "<<seg;
	hist->SetTitle(ss.str().c_str());
	
	
	bool nend=true;
	while(nend){
	        
		double AA=GetHistClickVal(hist,"Select K peak 1",false);
		double BB=GetHistClickVal(hist,"Select K peak 2",false);	
		higher_jd(AA,BB);
		double offset=AA;
		double gain=BB;
		
		jhistquestion* check2 = new jhistquestion("Keep","Retry","Abandon");
		bool success=Fit207BiCal(hist,offset,gain,check2->GetCanvas());
		
		switch (check2->WaitAnswer()) {
			case 1: if(success){
					chan->AddENGCoefficient(offset);
					chan->AddENGCoefficient(gain);
				}
				nend=false;				
				break;
			case 2: break;
			default:
				ZeroChannel(chan);
				nend=false;
				break;
		}
	}
	delete hist;	
}


// 
	
TH2* ChargeParse(TChain* DataChain,bool UseFitCharge){
	
	TSiLi *sili = 0;
	Int_t nentries = DataChain->GetEntries();
	if(DataChain->GetBranchStatus("TSiLi")){
		DataChain->SetBranchAddress("TSiLi",&sili);
	}else{
		cout<<endl<<"NO TSiLi Branch in Tree "<<endl;
		return 0;
	}

	gROOT->cd();
	TH2F *chargechan;
	if(UseFitCharge)chargechan = new TH2F("fitcharge_v_chan","fitcharge_v_chan",120,0,120,2000,10,4010);
	else chargechan = new TH2F("charge_v_chan","charge_v_chan",120,0,120,2000,100,20100);
	
	cout<<endl<<endl<<"Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop
	
		for(unsigned int i=0;i<sili->GetMultiplicity();i++){
			TSiLiHit* SS=sili->GetSiLiHit(i);
			int s=SS->GetSegment();
			if(UseFitCharge)chargechan->Fill(s,SS->GetFitCharge());
			else chargechan->Fill(s,SS->GetCharge());
		}
	
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	cout << "100 \% COMPLETE          \n";
	
	return chargechan;
}	
	

TH2* EnergyParse(TChain* DataChain,bool UseFitCharge){
	
	TSiLi *sili = 0;
	Int_t nentries = DataChain->GetEntries();
	if(DataChain->GetBranchStatus("TSiLi")){
		DataChain->SetBranchAddress("TSiLi",&sili);
	}else{
		cout<<endl<<"NO TSiLi Branch in Tree "<<endl;
		return 0;
	}
	TH2F *energychan= new TH2F("Energy_v_Chan","Energy_v_Chan",120,0,120,2000,10,2010);
	
	cout<<endl<<endl<<"Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop
	
		if(UseFitCharge)sili->UseFitCharge();//makes energy use fit charge. Note, GetCharge still returns original
		
		for(unsigned int i=0;i<sili->GetMultiplicity();i++){
			TSiLiHit* SS=sili->GetSiLiHit(i);
			energychan->Fill(SS->GetSegment(),SS->GetEnergy());
		}
	
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	cout << "100 \% COMPLETE          \n";
	
	return energychan;
}	

void CalSiLiBi207(TH2* hist,vector<double>&Voffest,vector<double>&Vgain){if(!hist)return;

	double AA=GetHistClickVal(hist,"Select K peak 1");//Getting a click
	double BB=GetHistClickVal(hist,"Select K peak 2");//Getting a click
	higher_jd(AA,BB);
// 	double AA=1400;
// 	double BB=2600;
	 
	
	for(int i=1;i<=120;i++){
		stringstream ss;
		ss<<"SPICE_"<<i-1<<"_charge";
		TH1D* project=hist->ProjectionY(ss.str().c_str(),i,i);
		project->SetTitle(ss.str().c_str());
		
		if(project->Integral()<200){delete project; continue;}
		
		double offset=AA;
		double gain=BB;
		bool success=Fit207BiCalSpec(project,offset,gain);
		
		if(success){
			int seg=i-1;
			TChannel* chan=TSiLiHit::GetSiLiHitChannel(seg);
			if(chan){
				Voffest[seg]=offset;Vgain[seg]=gain;
				chan->DestroyENGCal();
				chan->AddENGCoefficient(offset);
				chan->AddENGCoefficient(gain);
// 				cout<<endl<<seg<<" "<<Voffest[seg]<<" "<<Vgain[seg]<<flush;
			}
		}
		
		delete project;
	}
	
}



int Fit207BiCalSpec(TH1* hist,double& offset,double& gain){if(!hist)return 0;
	double AA=offset;
	double BB=gain;
	gain=abs((BiPeaks[3]-BiPeaks[0])/(BB-AA));

// 	TSpectrum sp(3);
// 	hist->GetXaxis()->SetRangeUser(AA-(200./gain),AA+(200./gain));
// 	gErrorIgnoreLevel=kError;//Suppress the peak buffer full (and all) warnings
// 	sp.Search(hist,2.,"",0.06);
// 	offset=sp.GetPositionX()[0];
// 	
// 	hist->GetXaxis()->SetRangeUser(BB-(200./gain),BB+(200./gain));
// 	sp.Search(hist,2.,"",0.06);
// 	gain=sp.GetPositionX()[0];
	
	FindBinRangeMaxD(hist,AA,200./gain);
	FindBinRangeMaxD(hist,BB,200./gain);
	offset=AA;
	gain=BB;
	
	return Fit207BiCal(hist,offset,gain);
}


int Fit207BiCal(TH1* hist,double& offset,double& gain,TCanvas* draw){if(!hist)return 0;
	double AA=offset;
	double BB=gain;

	FindLocalMax(hist,AA);
	FindLocalMax(hist,BB);

	gain=((BiPeaks[3]-BiPeaks[0])/(BB-AA));
	offset=BiPeaks[0]-(gain*AA);
	offset+=BiPeaks[3]-(gain*BB);
	offset/=2.0;
	
	hist->GetXaxis()->SetRangeUser(1,-1);
	
// 	hist->Rebin(2);
	
	vector< jPeakDat > fInput;
	fInput.push_back(jPeakDat(AA,0,0,0,0));
	fInput.push_back(jPeakDat((BiPeaks[1]-BiPeaks[0])/gain,1,3./gain));
	fInput.push_back(jPeakDat((BiPeaks[2]-BiPeaks[1])/gain,1,3./gain,0.75,0.5));
	
	vector< jPeakDat > fInputB;
	fInputB.push_back(jPeakDat(BB,0,0,0,0));
	fInputB.push_back(jPeakDat((BiPeaks[4]-BiPeaks[3])/gain,1,3./gain,0.3,0.25));
	fInputB.push_back(jPeakDat((BiPeaks[5]-BiPeaks[4])/gain,1,3./gain,0.75,0.5));
	
	// An all singing all dancing function to fit N peaks that are in close enough proximity to assume constant peak parameters
	// int=1 specified to attempt to use step background int=0 uses linear
	stringstream ss;
	ss<<1./gain<<"+"<<0.9/gain;

	FullFitHolder* fitA=Ultrapeak::PeakFit(hist,AA-50./gain,AA+(BiPeaks[2]-BiPeaks[0]+50)/gain,fInput,4,0,ss.str());
	FullFitHolder* fitB=Ultrapeak::PeakFit(hist,BB-50./gain,BB+(BiPeaks[5]-BiPeaks[3]+50)/gain,fInputB,4,0,ss.str());
	

	if(fitA&&fitB){
		TGraphErrors xy;
		
		for(int i=0;i<3;i++){
			xy.SetPoint(xy.GetN(),fitA->CVal(Ultrapeak::VPC(i)),BiPeaks[i]);
			xy.SetPointError(xy.GetN()-1,fitA->CVal(Ultrapeak::VPCe(i)),0);
		}	
		for(int i=0;i<3;i++){
			xy.SetPoint(xy.GetN(),fitB->CVal(Ultrapeak::VPC(i)),BiPeaks[i+3]);
			xy.SetPointError(xy.GetN()-1,fitB->CVal(Ultrapeak::VPCe(i)),0);
		}	
		
		TF1 line("pol1","pol1",0,1000000);
		xy.Fit(&line,"0QW");
		xy.Fit(&line,"0");
		offset=line.GetParameter(0);
		gain=line.GetParameter(1);
		
		if(draw){
			draw->Clear();
			
			draw->Update();
			draw->Divide(2,1);
			draw->cd(1);
			gPad->Update();
			xy.DrawClone("ap");
			line.SetLineWidth(1);
			line.DrawCopy("same");	
			
			draw->cd(2);
			gPad->Update();
			hist->DrawCopy();
			if(fitA)fitA->DrawCopy("same");
			if(fitB)fitB->DrawCopy("same");

			draw->Update();
		}
		
// 		cout<<endl<<endl<<"FIT Coef";
// 		cout<<endl<<endl<<line.GetParameter(0);
// 		cout<<endl<<endl<<line.GetParameter(1);
// 		cout<<endl<<endl;
		
		delete fitA;
		delete fitB;
		return 1;
	}
	
	if(fitA)delete fitA;
	if(fitB)delete fitB;
	return 0;	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  //gloabl variables
//all the mneumonic for all the preamps (ordered from 1-15)
  string preamps[8][15] = {
  //RB 
  {"SPI00XN6E", "SPI00XN56", "SPI00XN3E", "SPI00XN26", "SPI00XN0E", "SPI00XN01", "SPI00XN19", "SPI00XN31", "SPI00XN49", "SPI00XN61", "SPI00XN6C", "SPI00XN54", "SPI00XN3C", "SPI00XN24", "SPI00XN0C"},
  //RG
  {"SPI00XN62", "SPI00XN4A", "SPI00XN32", "SPI00XN1A", "SPI00XN02", "SPI00XN0D", "SPI00XN25", "SPI00XN3D", "SPI00XN55", "SPI00XN6D", "SPI00XN60", "SPI00XN48", "SPI00XN30", "SPI00XN18", "SPI00XN00"},
  //RR
  {"SPI00XN17", "SPI00XN2F", "SPI00XN47", "SPI00XN5F", "SPI00XN77", "SPI00XN6A", "SPI00XN52", "SPI00XN3A", "SPI00XN22", "SPI00XN0A", "SPI00XN15", "SPI00XN2D", "SPI00XN45", "SPI00XN5D", "SPI00XN75"},
  //RW
  {"SPI00XN0B", "SPI00XN23", "SPI00XN3B", "SPI00XN53", "SPI00XN6B", "SPI00XN76", "SPI00XN5E", "SPI00XN46", "SPI00XN2E", "SPI00XN16", "SPI00XN09", "SPI00XN21", "SPI00XN39", "SPI00XN51", "SPI00XN69"},
  //LB
  {"SPI00XN11", "SPI00XN29", "SPI00XN41", "SPI00XN59", "SPI00XN71", "SPI00XN64", "SPI00XN4C", "SPI00XN34", "SPI00XN1C", "SPI00XN04", "SPI00XN0F", "SPI00XN27", "SPI00XN3F", "SPI00XN57", "SPI00XN6F"},
  //LG
  {"SPI00XN05", "SPI00XN1D", "SPI00XN35", "SPI00XN4D", "SPI00XN65", "SPI00XN70", "SPI00XN58", "SPI00XN40", "SPI00XN28", "SPI00XN10", "SPI00XN03", "SPI00XN1B", "SPI00XN33", "SPI00XN4B", "SPI00XN63"},
  //LR
  {"SPI00XN74", "SPI00XN5C", "SPI00XN44", "SPI00XN2C", "SPI00XN14", "SPI00XN07", "SPI00XN1F", "SPI00XN37", "SPI00XN4F", "SPI00XN67", "SPI00XN72", "SPI00XN5A", "SPI00XN42", "SPI00XN2A", "SPI00XN12"},
  //LW 
  {"SPI00XN68", "SPI00XN50", "SPI00XN38", "SPI00XN20", "SPI00XN08", "SPI00XN13", "SPI00XN2B", "SPI00XN43", "SPI00XN5B", "SPI00XN73", "SPI00XN66", "SPI00XN4E", "SPI00XN36", "SPI00XN1E", "SPI00XN06"},
    
  
  };
  
  string preampnames[8]={"RB","RG","RR","RW","LB","LG","LR","LW"};
  

void MakeSiLiBlankCal(string calname) {
  
  ofstream outfile(calname.c_str(), std::ofstream::out);

  int select;
  cout << "Please select tig10 cable order "<<endl;
  cout<<"Preamps consecutively in a single collector"<<endl;
  cout<<" 1.   LW-LR-LG-LB-----RB-RG-RR-RW " <<endl;
  cout<<" 2.   RW-RR-RG-RB-----LB-LG-LR-LW " <<endl;
  cout<<" 3.   LB-LG-LR-LW-----RB-RG-RR-RW" <<endl;
  cout<<" 4.   RB-RG-RR-RW-----LB-LG-LR-LW" <<endl;
  cout<<" 5.   OTHER" <<endl;
  cin >> select;    

  if(select>0&&select<5){
	int collector;
	cout << "What is the collector number?:\t\t"<<endl;
	cin >> collector;

	int tig=1;
	int chan=0;
	
	int preA[8]={7,6,5,4,0,1,2,3};
	int preB[8]={3,2,1,0,4,5,6,7};
	int preC[8]={4,5,6,7,0,1,2,3};
	int preD[8]={0,1,2,3,4,5,6,7};
      
//       string preampnames[8]={"RB","RG","RR","RW","LB","LG","LR","LW"};
  
      for (int i=0; i<8; i++){
	      int p;
	      	switch(select) {
			case 1 : p=preA[i]; break;
			case 2 : p=preB[i]; break;
			case 3 : p=preC[i]; break;
			case 4 : p=preD[i]; break;
		}
	
	  int chan=0;
	  if(i%2)chan=5;
	  //if(p%6==0)chan=10;
	
	  write_preamp_to_file(p,collector,tig,chan,outfile);
    
	  tig++;
	if(i%2)tig++;
      
      }
  }else{ 
    for (int p=0; p<8; p++){
      cout <<endl<< "For Channel Preamp "<<preampnames[p]<<endl;
      preamp_question(p,outfile);
    }
  }
  
outfile.close();
}
      

string tig10address(unsigned int fCollector,unsigned int fTig,unsigned int fChan){
     std::stringstream stream;
     stream << "0x0" << std::hex << fCollector << "00"<< std::hex << fTig << "0" <<std::hex <<fChan;
     return stream.str();
}


void write_preamp_to_file(unsigned int fPreamp,unsigned int fCollector,unsigned int fTig,unsigned int fChan, ofstream& fOutfile){
	if(fPreamp>7){cout<<endl<<fPreamp<<" NOT A PREAMP"<<endl;return;}
	if(fTig<1||fTig>12){cout<<endl<<fTig<<" NOT A TIG10"<<endl;return;}
	if(fCollector>15){cout<<endl<<fCollector<<" NOT A COLLECTOR"<<endl;return;}
	if(fChan>9){cout<<endl<<fChan<<" NOT A TIG10 CHANNEL"<<endl;return;}
	if(!fOutfile.is_open()){cout<<endl<<" NO FILE"<<endl;return;}
	
	for(int j=0; j<15; j++){
	  
	  string tigaddress=tig10address(fCollector,fTig,fChan);
	  int number = (((fCollector-1)*120)+(fTig-1)*10)+fChan;

	  fOutfile <<  "//====================================//" << endl;
	  fOutfile << preamps[fPreamp][j] << " " << tigaddress <<" {"<<endl;   
	  fOutfile << "	Name:	" << preamps[fPreamp][j] << " " <<tigaddress <<endl;  
	  fOutfile << "	Number:	" << number << endl;
	  fOutfile << "	Address:	" << tigaddress << endl;
	  fOutfile << "	Digitizer:	Tig10" << endl << "	EngCoeff:	0 1" << endl << "	Integration:	125" << endl << "	ENGChi2:	 1" << endl <<"}" <<endl;
	
	  fChan++;
	  if(fChan==10){
	    fChan=0;
	    fTig++;
	    if(fTig>12){
	      fTig=0;
	      fCollector++;
	    }
	  }
	}
}

  
void preamp_question(unsigned int fPreamp, ofstream &fOutfile){
  if(fPreamp>7){cout<<endl<<fPreamp<<" NOT A PREAMP"<<endl;return;}
      int collector;
      cout << "What is the collector number?:\t\t"<<endl;
      cin >> collector;
      
      int port_num_in_collector;
      cout << "What is the collector port number of the tig10 (1-12)?:\t\t" << endl;
      cin >> port_num_in_collector; 
      
      int tignumber;
      cout << "Tig10 Channel number (0-9)?:\t\t" << endl;
      cin >> tignumber;  

    
  write_preamp_to_file(fPreamp,collector,port_num_in_collector,tignumber,fOutfile);
}



void ZeroChannel(int chan){
	ZeroChannel(TSiLiHit::GetSiLiHitChannel(chan));
}

void ZeroChannel(TChannel* chan){
	chan->DestroyENGCal();
	chan->AddENGCoefficient(0);
	chan->AddENGCoefficient(0);
}


/////////////////////////////////////////


ClassImp(ReFitMasterPanel);

ReFitMasterPanel::ReFitMasterPanel(TChain* Data,bool fit,TH2* Ch1,TH2* Eh2):TGMainFrame(gClient->GetRoot(), 100, 100,kVerticalFrame){
	if(!Data)return;
	DataChain=Data;
	
	TGHorizontalFrame* fHframe0 = new TGHorizontalFrame(this, 0, 0, 0);  
	fCanvas1 = new TRootEmbeddedCanvas("", fHframe0, 600, 600);
	fCanvas1->GetCanvas()->SetBorderMode(0);
	fCanvas2 = new TRootEmbeddedCanvas("", fHframe0, 600, 600);
	fCanvas2->GetCanvas()->SetBorderMode(0);
	fCanvas2->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "ReFitMasterPanel", this,"ClickedCanvas(Int_t,Int_t,Int_t,TObject*)");
	

// 	fCanvas1->GetCanvas()->SetMargin(0.1,0.01,0.05,0.01);	
	fHframe0->AddFrame(fCanvas1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));
	fHframe0->AddFrame(fCanvas2, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));	
	
	
	AddFrame(fHframe0, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));


	TGHorizontalFrame* fHframe1 = new TGHorizontalFrame(this, 0, 0, 0);
	ftbutton1 = new TGTextButton(fHframe1," End and Save ",0);
	ftbutton1->Connect("Clicked()","ReFitMasterPanel",this,"EndSave()");	
	ftbutton2 = new TGTextButton(fHframe1," ReSort ",0);
	ftbutton2->Connect("Clicked()","ReFitMasterPanel",this,"ReSort()");
	
	fHframe1->AddFrame(ftbutton1, new TGLayoutHints(kLHintsExpandX | 2, 2, 2, 2));
	fHframe1->AddFrame(ftbutton2,new TGLayoutHints(kLHintsExpandX | 2, 2, 2, 2));	
	AddFrame(fHframe1, new TGLayoutHints(kLHintsExpandX));
	
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();
	
	Chist=0;
	Ehist=0;
	proj=0;
	
	UseFitCharge=fit;
	
	if(Ch1) Chist=(TH2*)Ch1->Clone("ChargeTotalSiliReFit");
	else Chist=ChargeParse(DataChain,UseFitCharge);
	
	if(Eh2){
		fCanvas2->GetCanvas()->cd();
		Ehist=(TH2*)Eh2->Clone("EnergyTotalSiliReFit");
		Ehist->Draw("colz");
		proj=Eh2->ProjectionY();
		proj->SetTitle("Total Energy Projection");
		fCanvas1->GetCanvas()->cd();
		proj->Draw("hist");
	}else ReSort();
	
	pointbin=-1;
	UNLOCKED=true;
}

void ReFitMasterPanel::EndSave(){
	TChannel::WriteCalFile("SiLiEnergyCalibrated.cal");
	gApplication->Terminate();
}
	

void ReFitMasterPanel::ReSort(){if(!DataChain)return;
	if(Ehist)delete Ehist;
	Ehist=0;
	Ehist=EnergyParse(DataChain,UseFitCharge);
	if(Ehist){
		fCanvas2->GetCanvas()->cd();
		Ehist->Draw("colz");
		fCanvas2->GetCanvas()->Modified();
		fCanvas2->GetCanvas()->Update();
		if(proj)delete proj;
		proj=Ehist->ProjectionY();
		proj->SetTitle("Total Energy Projection");
		fCanvas1->GetCanvas()->cd();
		proj->Draw("hist");
		fCanvas1->GetCanvas()->Modified();
		fCanvas1->GetCanvas()->Update();
	}
}

void ReFitMasterPanel::ClickedCanvas(Int_t event, Int_t px, Int_t py, TObject *selected_ob){
	
		TCanvas* cCan=fCanvas2->GetCanvas();
		
		double Y=cCan->AbsPixeltoY(py);
		double X=cCan->AbsPixeltoX(px);	
		double y1=cCan->GetUymin(),y2=cCan->GetUymax();
		double x1=cCan->GetUxmin(),x2=cCan->GetUxmax();
		
		bool incan=(Y<y2&&Y>y1&&X>x1&&X<x2);
		
		if (event==kMouseEnter){if(incan)
			cCan->SetCrosshair(1);else
			cCan->SetCrosshair(0);
		}
		
		int sel=(int)X;
		if(sel>119||sel<0)sel=-1;
		if(sel!=pointbin){
			pointbin=sel;
			cout<<" "<<pointbin<<flush;
			if(Ehist){
				
				fCanvas1->GetCanvas()->cd();
				
				if(proj){
					if(pointbin<0){
						proj->Draw();
						proj->SetTitle("Total Energy Projection");
					}else{
						TH1* pr=Ehist->ProjectionY("",pointbin+1,pointbin+1);
						stringstream ss;
						ss<<"Channel "<<pointbin<<" Energy";
						pr->SetTitle(ss.str().c_str());
						pr->SetLineColor(2);
						pr->DrawCopy("hist");
						TH1* tot=proj->DrawCopy("histsame");
						tot->Scale(pr->Integral()/proj->Integral());
						delete pr;
					}
				}
				
				fCanvas1->GetCanvas()->Modified();
				fCanvas1->GetCanvas()->Update();
			}
			
		}
		
		if(event==kButton1Up)if(incan){
			if(pointbin>=0&&pointbin<120){
				int seg=pointbin;
				ZeroChannel(seg);
				stringstream ss;
				TH1D* hist=Chist->ProjectionY(ss.str().c_str(),seg+1,seg+1);
				ss<<"Charge Fit SPICE "<<seg;
				hist->SetTitle(ss.str().c_str());
				if(hist->Integral()<200){
					cout<<endl<<ss.str()<<" INSUFFICIENT DATA"<<flush;
				}else{
					cout<<endl<<"IS THIS"<<endl;
					new silicalchan(hist,seg);
				}
					cout<<endl<<"THE PROBLEM"<<endl;
				delete hist;
			}
		}
}



// CalSiLiBi207Single(Chist,3);

ClassImp(silicalchan);


silicalchan::silicalchan(TH1* h,int seg) : TGMainFrame(gClient->GetRoot(), 100, 100,kVerticalFrame),fCanvas1(0),k_mode(1),hist(0),segment(seg){

	fCanvas1 = new TRootEmbeddedCanvas("", this, 800, 600);
	fCanvas1->GetCanvas()->SetBorderMode(0);
	AddFrame(fCanvas1, new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0));	
	
	fCanvas1->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "silicalchan", this,"ClickedCanvas(Int_t,Int_t,Int_t,TObject*)");

	
	fBgroup1 = new TGButtonGroup(this,"",kChildFrame);// Another button group
		TGTextButton* ftbutton = new TGTextButton(fBgroup1,"Accept",1);	
		ftbutton->Connect("Clicked()","silicalchan",this,"Accept()");
		ftbutton = new TGTextButton(fBgroup1,"Retry",2);
		ftbutton->Connect("Clicked()","silicalchan",this,"Retry()");
		ftbutton = new TGTextButton(fBgroup1,"Abandon",3);	
		ftbutton->Connect("Clicked()","silicalchan",this,"SendCloseMessage()");

		fBgroup1->SetLayoutHints(new TGLayoutHints(0,10, 10, 0, 0));
		fBgroup1->Show();
		
	AddFrame(fBgroup1,new TGLayoutHints(kLHintsCenterX, 0,0, 0, 0));
	
	
	if(h){
		hist=(TH1*)h->Clone();
		fCanvas1->GetCanvas()->cd();
		hist->Draw("hist");
		fCanvas1->GetCanvas()->Update();
	}
	
	
	MapSubwindows();
	Resize(GetDefaultSize());
	MapWindow();
}



void silicalchan::ClickedCanvas(Int_t event, Int_t px, Int_t py, TObject *selected_ob){
	if(k_mode){
		TCanvas* cCan=Can();
		
		double Y=cCan->AbsPixeltoY(py);
		double X=cCan->AbsPixeltoX(px);	
		double y1=cCan->GetUymin(),y2=cCan->GetUymax();
		double x1=cCan->GetUxmin(),x2=cCan->GetUxmax();
		
		bool incan=(Y<y2&&Y>y1&&X>x1&&X<x2);
		
		if (event==kMouseEnter){if(incan)
			cCan->SetCrosshair(1);else
			cCan->SetCrosshair(0);
		}
		
		if(event==kButton1Up)if(incan){
			cout<<endl<<" Clicked "<<X<<flush;
			FindLocalMax(hist,X);
			
			Kclicks.push_back(X);
			double YY= hist->GetBinContent(hist->GetXaxis()->FindBin(X));
			cCan->cd();
			gPad->Update();
			TMarker bob(X, YY, 23);
			bob.DrawMarker(X,YY);
			bob.PaintMarker(X,YY);
			cCan->Update();
			
			if(Kclicks.size()>1){
				offset=Kclicks[0];
				gain=Kclicks[1];
				higher_jd(offset,gain);
				bool success=Fit207BiCal(hist,offset,gain,cCan);
				if(success){
					k_mode=false;
				}else{
					Kclicks.clear();
				}
			}
		}
	}
}




void silicalchan::Accept(){
	TChannel* chan=TSiLiHit::GetSiLiHitChannel(segment);
	if(chan){
		chan->DestroyENGCal();
		chan->AddENGCoefficient(offset);
		chan->AddENGCoefficient(gain);
	}
	this->SendCloseMessage();
}

void silicalchan::Retry(){
	TCanvas* cCan=Can();
	cCan->Clear();
	cCan->cd();
	gPad->Update();
	if(hist){
		hist->Draw("hist");
		cCan->Update();
	}
	Kclicks.clear();
	k_mode=true;
}


/////////////////////////////////////////////////////////////////////////////


void AddWave(string calfile,string wavefile){
	
	double base[120]={0};
	double rise[120]={0};
	double decay[120]={0};
	
	ifstream wave(wavefile.c_str());
	
// 	0 -4351.68 0.0721898 18.8922 0.00567208 4781.18 3.03104
	
	if(!wave.good())return;
	
	int seg;
	double a,b,c,d,e,f;
	while(wave>>seg>>a>>b>>c>>d>>e>>f){
		if(seg>-1&&seg<120){
			base[seg]=a;
			rise[seg]=c;
			decay[seg]=e;
		}
	}

	ifstream calin(calfile.c_str());
	if(!calin.good())return;
	
	string outname=calfile.substr(0,calfile.find(".cal"))+"wave.cal";
	
	ofstream calout(outname.c_str());
	
	string line;
	unsigned int fSegment=120;
	bool found=false;
	
	while(getline(calin,line)){
		
		if(!found){
			if(line.find("SPI00XN")<=1&&line.size()>1){
	// 			cout << "b" <<endl;
				stringstream parsestream;
				parsestream<<line;
				string first;
				parsestream>>first;
				string buf;
				buf.assign(first,7,2);
				fSegment = (uint16_t)strtol(buf.c_str(), NULL, 16);
				found=true;
			}
		}
		
		
		if(line.find("}")<=line.size()){
			if(found&&fSegment<120){
				calout<<"	BaseLine: "<<base[fSegment]<<endl;
				calout<<"	RiseTime: "<<rise[fSegment]<<endl;
				calout<<"	DecayTime: "<<decay[fSegment]<<endl;
			}
			found=false;
		}

		calout<<line<<endl;
	}

	calout.close();
	calin.close();
	wave.close();
	
}