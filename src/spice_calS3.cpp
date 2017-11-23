#include "spice_calS3.h"
#include "spice_input.h"


int S3CalParse(int argc, char *argv[],TApplication* app){
	SPICEcmdline inp(argc,argv);
	
	string outputfile=OrDefault("S3energycal.root",inp.RootFile());
		
	TChain *DataChain=inp.AnalysisTree();
		
	if(inp.LoadCal(DataChain)){
		int select=-1;
		if(inp.IsPresent("arraypos"))select=inp.Next("arraypos");
		
		if(inp.IsPresent("energyfix")){
			cout<<endl<<" One Point Calibration From Beam Data "<<endl;
			double energy_select=inp.Next("energyfix");
			S3FixCalBeam(DataChain,outputfile,app,energy_select,inp.IsPresent("high"),select);
		}else{
			S3CalParseChain(DataChain,outputfile,app,inp.IsPresent("high"),select);
		}
		
	}
	delete DataChain;

	return 0;
}



void S3CalParseChain(TChain* DataChain,string outputfile,TApplication* app,bool gain,int select){
	
	
	if(select>=0){
		cout<<endl<<"Multi S3. Calibrating array position "<<select<<flush; 		
	}
	
	int gn=1;
	if(gain)gn*=10;
	
	TChannel::SetIntegration("BAE",125,EPriority::kForce);
	TChannel::SetIntegration("SPE",125,EPriority::kForce);
	TChannel::SetUseCalFileIntegration("SP",true,EPriority::kForce);
	TChannel::SetUseCalFileIntegration("BA",true,EPriority::kForce);

	TS3 *s3 = 0;
	Int_t nentries = DataChain->GetEntries();

	if(DataChain->GetBranchStatus("TS3")){
		DataChain->SetBranchAddress("TS3",&s3);
	}else{
		cout<<endl<<"NO TS3 Branch in Tree "<<endl;
		return;
	}
	
	cout<<endl<<"Creating output file : "<<outputfile<<flush;

	gROOT->cd();
		TFile *outfile = new TFile(outputfile.c_str(),"RECREATE");
	outfile->cd();
	
	TChannel* chan[56]={0};	
	
	outfile->mkdir("ChargeFits");
	outfile->cd("ChargeFits");
	
	TH1F *chargering[24];
	for(int i=0;i<24;i++){
		stringstream ss;
		ss<<"S3_ring_"<<i<<"_charge";
		chargering[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,1000*gn);
	}
	TH1F *chargesector[32];
	for(int i=0;i<32;i++){
		stringstream ss;
		ss<<"S3_sector_"<<i<<"_charge";
		chargesector[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,1000*gn);
	}
	
	TH2F *chargechan=new TH2F("chargechan","chargechan",56,0,56,5000,0,1000*gn);
	
	
	
	outfile->cd();
	

	////////////////// MAIN PROCESS OF THE CHAIN //////////////////	
	
	cout<<endl<<endl<<"Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop


		//Get hits (rings)
		for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
			TS3Hit* SR=s3->GetRingHit(i);
			
			if(select>=0)if(SR->GetArrayPosition()!=select)continue;
			
			unsigned int s=SR->GetSegment();
			if(s<24){
				chargering[s]->Fill(SR->GetCharge());
				chargechan->Fill(s,SR->GetCharge());
				if(!chan[s])chan[s]=SR->GetChannel();
			}
		}

		//Get hits for E S3 (sectors)
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			if(select>=0)if(SS->GetArrayPosition()!=select)continue;
			
			unsigned int s=SS->GetSegment();
			if(s<32){
				chargesector[s]->Fill(SS->GetCharge());
				chargechan->Fill(s+24,SS->GetCharge());
				if(!chan[s+24])chan[s+24]=SS->GetChannel();
			}
		}
		
	
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	cout << "100 \% COMPLETE          \n";
	
	
	
	const Float_t E_MeV[3]= {5.8048,5.4856,5.1566};
	
	
	double YY=GetHistClickVal(chargechan,"Select Y");//Getting a click
	double gradguess=YY/E_MeV[1];
	double r=(E_MeV[0]-E_MeV[2])*gradguess*4.0;	
	double rl=YY-r;	
	double ru=YY+r;
	
	TH1F *hist[56];
	for(int i=0;i<24;i++)hist[i]=chargering[i];
	for(int i=0;i<32;i++)hist[i+24]=chargesector[i];
	
	
	TCanvas* c1=new TCanvas("CalResult","CalResult",1200,400);//creating it here saves annoying popup
	

	for(int i=0;i<56;i++){
		if(!chan[i]) continue;
		if(hist[i]->Integral()<50)continue;
		
		TSpectrum Spec(3);
		gErrorIgnoreLevel=kError;//Suppress the peak buffer full (and all) warnings
		
// 		Spec.Search(hist[i],3,"",0.7);
// 		TH1F* tmp=(TH1F*)hist[i]->Clone("tmp");
		TH1F* tmp=hist[i];
// 		tmp->Rebin(10);
		tmp->GetXaxis()->SetRangeUser(rl,ru);
// 		cout<<endl<<"RANGE "<<rl<<" "<<ru<<endl;
		Spec.Search(tmp,3,"nobackground",0.3);
// 		delete tmp;
		Double_t *peak_pos = Spec.GetPositionX();
		vector<double> sort_pos;
		
		for(int p=0;p<3;p++)sort_pos.push_back(peak_pos[p]);
		std::sort(sort_pos.begin(), sort_pos.end());
		
		Double_t par_buff[12];
/*		
		for(Int_t ip=0; ip<3; ip++){
			TF1 fpeak(Form("ch%dpk%d",i,ip+1),"[0]*exp(-0.5*pow((x-[1])/([2]+(x-[1])*[3]*(x-[1])),2))",0.97*sort_pos[ip],1.03*sort_pos[ip]);
			fpeak.SetParameter(1,sort_pos[ip]);
			fpeak.SetParameter(2,0.01*sort_pos[ip]);
			hist[i]->Fit(&fpeak,"0RQ+");
			fpeak.GetParameters(&par_buff[ip*4]);
			
			par_buff[ip*4+1]=sort_pos[ip];
		}*/	
		
		TF1 ftot(Form("ch%dtot",i),"[0]*exp(-0.5*pow((x-[1])/([2]+(x<[1])*[3]*(x-[1])),2))+[4]*exp(-0.5*pow((x-[5])/([6]+(x<[5])*[7]*(x-[5])),2))+[8]*exp(-0.5*pow((x-[9])/([10]+(x<[9])*[11]*(x-[9])),2))",0.93*sort_pos[0],1.07*sort_pos[2]);

// 		ftot.SetParameters(par_buff);
		
		
		vector< double > pc;
		for(Int_t ip=0; ip<3; ip++)pc.push_back(sort_pos[ip]);
		std::sort(pc.begin(),pc.end());
		
		double max=hist[i]->GetBinContent(hist[i]->GetMaximumBin());
		
		for(Int_t ip=0; ip<3; ip++){
			ftot.SetParameter(ip*4,max);
			ftot.SetParameter(ip*4+1,pc[ip]);
			ftot.SetParameter(ip*4+2,50/gradguess);
			ftot.SetParameter(ip*4+3,-0.2);
					  
			ftot.SetParLimits(ip*4+1,pc[ip]-150/gradguess,pc[ip]+150/gradguess);
			ftot.SetParLimits(ip*4+2,10/gradguess,500/gradguess);
		}
		
		ftot.SetLineColor(3);
		hist[i]->Fit(&ftot,"QR+");
		
		
// 		Double_t par[12];
// 		Double_t err[12];
// 		for(Int_t j=0; j<12; j++){
// 			par[j] = ftot.GetParameter(j);
// 			err[j] = ftot.GetParError(j);
// 		}
// 		TH1F hres(Form("hres_%d",i),Form("res_ch%d",i),1000,5000,6000);	
// 		for(Int_t j=0; j<3; j++)
// 		{
// 			Int_t bin = (Int_t)( (E_MeV[j]-5.)*1000.);
// 			Float_t Sgm = par[4*j+2]/par[4*j+1];
// 			Float_t DltSgm = err[4*j+2]/par[4*j+1];
// 			
// 			cout<<endl<<Sgm<<" "<<DltSgm<<endl;
// 			hres.SetBinContent(bin,Sgm);
// 			hres.SetBinError(bin,DltSgm);
// 		}
// 		TF1 fres(Form("ch%dres",i),"[0]*TMath::Sqrt([1]/x)",5000,6000);
// 		fres.SetParameter(0,0.5);
// 		fres.SetParameter(1,5.);
// 		hres.Fit(&fres,"QR+");
// 		float sigma=fres.GetParameter(0);
// 		float e0=fres.GetParameter(1);	
		

		
		TGraphErrors Grph;
		Grph.SetPoint(0,ftot.GetParameter(1),E_MeV[2]*1000.);
		Grph.SetPointError(0,ftot.GetParError(1),0);
		Grph.SetPoint(1,ftot.GetParameter(5),E_MeV[1]*1000.);
		Grph.SetPointError(1,ftot.GetParError(5),0);
		Grph.SetPoint(2,ftot.GetParameter(9),E_MeV[0]*1000.);
		Grph.SetPointError(2,ftot.GetParError(9),0);
// 		
		TF1 pl(Form("ch%dres",i),"pol1",0,100000);
		Grph.Fit(&pl,"0QW");
		Grph.Fit(&pl,"0Q");
		
		chan[i]->DestroyENGCal();
		chan[i]->AddENGCoefficient(pl.GetParameter(0));
		chan[i]->AddENGCoefficient(pl.GetParameter(1));
		
// 		cout<<i<<" "<<flush;
	}
	
	
	outfile->cd();
	
	TH1F *ringtot= new TH1F("RingEnergyTotal","RingEnergyTotal",10000,0,10000);
	TH1F *sectot= new TH1F("SecEnergyTotal","SecEnergyTotal",10000,0,10000);
	TH2F *energychan=new TH2F("EnergyChan","EnergyChan",56,0,56,5000,0,10000);
	
	gROOT->cd();

	////////////////// MAIN PROCESS OF THE CHAIN //////////////////	
	
	cout<<endl<<endl<<"RE-Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop

		//Get hits (rings)
		for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
			TS3Hit* SR=s3->GetRingHit(i);
			
			if(select>=0)if(SR->GetArrayPosition()!=select)continue;
			
			ringtot->Fill(SR->GetEnergy());
			energychan->Fill(SR->GetRing(),SR->GetEnergy());
		}

		//Get hits for E S3 (sectors)
		for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
			TS3Hit* SS=s3->GetSectorHit(i);
			
			if(select>=0)if(SS->GetArrayPosition()!=select)continue;
			
			sectot->Fill(SS->GetEnergy());
			energychan->Fill(SS->GetSector()+24,SS->GetEnergy());
		}
		
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	cout << "100 \% COMPLETE          \n";

	TChannel::WriteCalFile("S3EnergyCalibrated.cal");
	
// 	TCanvas c1("CalResult","CalResult",1200,400);
	c1->Divide(3,1);
	c1->cd(1);
	ringtot->DrawCopy();
	c1->cd(2);
	sectot->DrawCopy();
	c1->cd(3);
	energychan->DrawCopy("colz");
	c1->Update();
	
	cout<<endl<<"WRITING FILE"<<endl;
	outfile->cd();
	outfile->Write();
	outfile->Close();
	cout<<endl<<"SAVED"<<endl<<endl;

	if(app)
		c1->Connect("Closed()", "TApplication", app, "Terminate()");
	
	
	//TApplication will cause processing to stop her but canvas to remain functional until canvas clicked or closed
// 	c1->WaitPrimitive(); //will return with a double click
	
}

void TESTPRINT(){
		cout<<endl<<"Setting terminate"<<endl;
	
	
}


void MakeBlankS3cal(string mnemonic, string caloutname){
    string basicsettings=mnemonic;
    int col,tigr,tigs;
    
    cout<<endl<<"Collectors Number : "<<flush;
    cin>>col;
    cout<<endl<<"Rings 1st Tig10 : "<<flush;
    cin>>tigr;
    cout<<endl<<"Sectors 1st Tig10 : "<<flush;
    cin>>tigs;
    
    
    bool rings=true;
    string NP="N";
    int tig=tigr;

    bool bam=false;
    if(mnemonic.find("BA")==0){
	cout<<endl<<"Bambino mapping adjust. "<<flush;
	bam=true;
    }
 
    ofstream calout(caloutname.c_str());
     
    for(int i=0;i<32;i++){
        if(!(i%10)&&i>0)tig++;
         
        stringstream name;
        stringstream address;
	
	int rs=i;
	if(bam){
		if(rings){
			if(i<8)rs=7-i;
			else if(i<16)rs=23-i;
			else rs=39-i;
			
		}else{
			rs=i;
			
			//Flip the connectors
			if(rs<16)rs=15-rs;
			else rs=47-rs;
			
// 			//flip rotation
			rs=31-rs;
// 			
			//offset
			rs-=7;
			if(rs<0)rs+=32;

// 			rs+=9;
// 			if(rs>31)rs-=32;
		}
	}
         
        name<<basicsettings<<NP<<std::setw(2)<<std::setfill('0')<<rs<<"x";
        address<<"0x"<<col<<std::setw(3)<<std::setfill('0')<<tig<<std::setw(2)<<std::setfill('0')<<(i%10);
        int number=((col-1)*12+(tig-1))*10+(i%10);
         
        calout<<endl;
        calout<<"//====================================//"<<endl;
        calout<<name.str()<<" "<<address.str()<<" {"<<endl;
        calout<<" Name:   "<<name.str()<<" "<<address.str()<<endl;
        calout<<" Number: "<<number<<endl;
        calout<<" Address:    "<<address.str()<<endl;
        calout<<" Digitizer:  Tig10"<<endl;
        calout<<" EngCoeff:   0.0 1.0"<<endl;
        calout<<" Integration:    125"<<endl;
        calout<<" ENGChi2:     1"<<endl;
        calout<<"}"<<endl;
 
        if(i==23&&rings){
            rings=false;
            NP="P";
            i=-1;
	    tig=tigs;
            continue;
        }
    }
     
    calout.close();
}






void S3FixCalBeam(TChain* DataChain,string outputfile,TApplication* app,double energyring0,bool gain,int select){
	
	
	if(select>=0){
		cout<<endl<<"Multi S3. Calibrating array position "<<select<<flush; 		
	}
	
	int gn=1;
	if(gain)gn*=10;
	
	TChannel::SetIntegration("BAE",125,EPriority::kForce);
	TChannel::SetIntegration("SPE",125,EPriority::kForce);
	TChannel::SetUseCalFileIntegration("SP",true,EPriority::kForce);
	TChannel::SetUseCalFileIntegration("BA",true,EPriority::kForce);
	
	
	TS3 *s3 = 0;
	Int_t nentries = DataChain->GetEntries();

	if(DataChain->GetBranchStatus("TS3")){
		DataChain->SetBranchAddress("TS3",&s3);
	}else{
		cout<<endl<<"NO TS3 Branch in Tree "<<endl;
		return;
	}
	
	TChannel* chan[56]={0};
	
	gROOT->cd();
	TFile *outfile = new TFile(outputfile.c_str(),"RECREATE");
	
	outfile->cd();
		TH1F *chargering0= new TH1F("ring0","ring0",10000,0,100000*gn);
	gROOT->cd();
		
	cout<<endl<<endl<<"Processing Ring 0"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop

		//Get hits (rings)
		for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
			TS3Hit* SR=s3->GetRingHit(i);
			
			if(select>=0)if(SR->GetArrayPosition()!=select)continue;
			
			unsigned int s=SR->GetSegment();
			if(s<24){
				if(s==0)chargering0->Fill(SR->GetCharge());
				if(!chan[s]&&SR->GetEnergy()>1)chan[s]=SR->GetChannel();
			}
		}
		
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	
	
	double XX=GetHistClickVal(chargering0,"Select Below Highest Peak",false);//Getting a click
	double XXX=GetHistClickVal(chargering0,"Select Above Highest Peak",false);//Getting a click

	higher_jd(XX,XXX);
	
	chargering0->GetXaxis()->SetRange(chargering0->GetXaxis()->FindBin(XX),chargering0->GetXaxis()->FindBin(XXX));

	double grad0=energyring0/chargering0->GetMean();
	
	cout<<endl<<"Energy Ring 0 "<<energyring0<<" keV"<<endl;
	cout<<endl<<"Mean Charge Ring 0 "<<chargering0->GetMean()<<endl;
	cout<<endl<<"grad Ring 0 "<<grad0<<endl;
	
	
	outfile->cd();
	TH1F *gradsector[32];
	for(int i=0;i<32;i++){
		stringstream ss;
		ss<<"S3_sector_"<<i<<"_grad";
		gradsector[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),10000,0,50);
	}
	gROOT->cd();	
	
	cout<<endl<<endl<<"Processing Sectors"<<endl;
	
	outfile->cd();
	TH2F *sectorcharge= new TH2F("sectorcharge","sectorcharge",32,0,32,10000,0,100000*gn);
	gROOT->cd();
	
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop

		
		if(!(s3->GetRingMultiplicity()==1&&s3->GetSectorMultiplicity()==1))continue;
		
		
		TS3Hit* SS=s3->GetSectorHit(0);
		if(select>=0)if(SS->GetArrayPosition()!=select)continue;
		unsigned int s=SS->GetSegment();
		if(s>31)continue;
			
		sectorcharge->Fill(s,SS->GetCharge());
		
		TS3Hit* SR=s3->GetRingHit(0);
		if(select>=0)if(SR->GetArrayPosition()!=select)continue;
		if(SR->GetSegment()!=0)continue;
		if(SR->GetCharge()<XX)continue;
		if(SR->GetCharge()>XXX)continue;
		
		
		gradsector[s]->Fill((SR->GetCharge()*grad0)/SS->GetCharge());
			
		if(!chan[s+24]&&SS->GetEnergy()>1){
			chan[s+24]=SS->GetChannel();
		}
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}	
	
	double sl=GetHistClickVal(sectorcharge,"Select Good Segment Range",false);//Getting a click
	double sh=GetHistClickVal(sectorcharge,"Select Good Segment Range",false);//Getting a click
	
	higher_jd(sl,sh);
	
	unsigned int SL,SH;
	SL=sl;
	SH=sh;
	
	double gradS[32];
	for(int i=0;i<32;i++)gradS[i] = gradsector[i]->GetMean();
		
	outfile->cd();
	TH1F *gradring[24];
	for(int i=0;i<24;i++){
		stringstream ss;
		ss<<"S3_ring_"<<i<<"_grad";
		gradring[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),10000,0,50);
	}
	gROOT->cd();
		
	cout<<endl<<endl<<"Processing Rings"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop

		if(!(s3->GetRingMultiplicity()==1&&s3->GetSectorMultiplicity()==1))continue;
		
		TS3Hit* SS=s3->GetSectorHit(0);
		if(select>=0)if(SS->GetArrayPosition()!=select)continue;
		
		TS3Hit* SR=s3->GetRingHit(0);
		if(select>=0)if(SR->GetArrayPosition()!=select)continue;
		
		unsigned int r=SR->GetSegment();
		if(r==0)continue;
		unsigned int s=SS->GetSegment();
		
		if(s<SL||s>SH)continue;
		
		if(r<24&&s<32){
			if(chan[s+24])
				gradring[r]->Fill((SS->GetCharge()*gradS[s])/SR->GetCharge());
		}
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}		
	
	double gradR[24];
	for(int i=1;i<24;i++)gradR[i] = gradring[i]->GetMean();
	gradR[0]=grad0;
	
	for(int i=0;i<24;i++)if(chan[i]){
		chan[i]->DestroyENGCal();
		chan[i]->AddENGCoefficient(0);
		chan[i]->AddENGCoefficient(gradR[i]);
	}
	for(int i=0;i<32;i++)if(chan[i+24]){
		chan[i+24]->DestroyENGCal();
		chan[i+24]->AddENGCoefficient(0);
		chan[i+24]->AddENGCoefficient(gradS[i]);
	}
		
		
	outfile->cd();
	TH2F *energychan=new TH2F("EnergyChan","EnergyChan",56,0,56,2000,energyring0*0.1,energyring0*1.5);
	gROOT->cd();
	
	cout<<endl<<endl<<"RE-Processing files"<<endl;
	for(int jentry=0;jentry<nentries;jentry++){
		DataChain->GetEntry(jentry);  //start the loop
		
		if(!(s3->GetRingMultiplicity()==1&&s3->GetSectorMultiplicity()==1))continue;

		TS3Hit* SR=s3->GetRingHit(0);
		if(select>=0)if(SR->GetArrayPosition()!=select)continue;
		energychan->Fill(SR->GetRing(),SR->GetEnergy());

		TS3Hit* SS=s3->GetSectorHit(0);
		if(select>=0)if(SS->GetArrayPosition()!=select)continue;
		energychan->Fill(SS->GetSector()+24,SS->GetEnergy());
		
		if(jentry%1000 == 0) 
		cout << setiosflags(ios::fixed) << std::setprecision(2) << 100. * (double)jentry/nentries << " % complete."<< "\r" << flush;
	}
	cout << "100 \% COMPLETE          \n";
	
	TChannel::WriteCalFile("S3BeamCalibrated.cal");
	

	
	TCanvas *C1=new TCanvas();
	C1->cd();
	energychan->DrawCopy("colz");
	C1->Update();
	gPad->Update();
	
	outfile->cd();
	outfile->Write();
	outfile->Close();	
	
	C1->WaitPrimitive();
}
