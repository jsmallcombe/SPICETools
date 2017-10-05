////////////////////////////////////////////////
////////////////// HISTOGRAMS //////////////////
////////////////////////////////////////////////

outfile->cd();

	TH3F *S3_map3;
	TH2F *S3_theta;
	TH1F* S3_multot;
	
	vector< TH1F* > front_back,front_backgate,S3_sectormult,S3_ringmult,S3_mult,S3ring_sum,S3sector_sum,S3RS_t,S3RS_tgate;
	vector< TH2F* > S3_map,S3_dedx,frontVback,frontVbackGated,S3flat;
	vector< TH3F* > S3_d3dx,S3RS_t3;
	
	

	outfile->mkdir("S3");
	outfile->cd("S3");
		S3_map3= new TH3F("S3_map3","S3_map3;Z [mm];X [mm];Y [mm]",80,-50,50,80,-40,40,100,-40,40);
		S3_theta= new TH2F("S3_theta","S3_theta;Total Energy [keV];Theta [rad]",1000,0,control[S3EnergyLimit],200,0.2,1.2);
		
		int k=0;
		for(unsigned short i=0;i<4;i++){
			
			string sfx="";
			if(MultiS3){
				if(!s3used[i])continue;
				s3index[i]=k;
				k++;
				stringstream ss;
				ss<<i;
				sfx=ss.str();
				string tf="S3/Pos"+sfx;
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
			}else{i=4;}
			
			S3_map.push_back(new TH2F(("S3_map"+sfx).c_str(),("S3_map;X [mm];Y [mm]"+sfx).c_str(),800,-40,40,800,-40,40));

			if(Telescope){
				S3_dedx.push_back( new TH2F(("S3_dedx"+sfx).c_str(),("S3_dedx"+sfx+";Total Energy [keV];dE/dx [arb.]").c_str(),1000,0,control[S3EnergyLimit],1000,0,control[S3EnergyLimit]*.4));
				S3_d3dx.push_back( new TH3F(("S3_d3dx"+sfx).c_str(),("S3_d3dx"+sfx+";Total Energy [keV];dE/dx [arb.];Theta [rad]").c_str(),100,0,control[S3EnergyLimit],100,0,control[S3EnergyLimit]*.4,100,0,pi/2));
				S3RS_t3.push_back( new TH3F(("S3RS_t3"+sfx).c_str(),("S3RS_t3"+sfx+";#Deltat [10/16 ns];dE/dx [arb.];Total Energy [keV]").c_str(),100,-500,500,200,0,control[S3EnergyLimit],250,0,control[S3EnergyLimit]));
			}else{
				frontVback.push_back( new TH2F(("frontVback"+sfx).c_str(),("frontVback"+sfx+";Ring Energy [keV];Sector Energy [keV]").c_str(),1000,0,control[S3EnergyLimit],1000,0,control[S3EnergyLimit]));
				frontVbackGated.push_back( new TH2F(("frontVbackGated"+sfx).c_str(),("frontVbackGated"+sfx+";Ring Energy [keV];Sector Energy [keV]").c_str(),1000,0,control[S3EnergyLimit],1000,0,control[S3EnergyLimit]));
				front_back.push_back( new TH1F(("front_back"+sfx).c_str(),("front_back"+sfx+";Ring Energy-Sector Energy [keV];").c_str(),1000,-25000,25000));
				front_backgate.push_back( new TH1F(("front_backgate"+sfx).c_str(),("front_backgate"+sfx+";Ring Energy-Sector Energy [keV];").c_str(),1000,-25000,25000));
			}

			S3RS_t.push_back( new TH1F(("S3RS_t"+sfx).c_str(),("S3RS_t"+sfx+";#Deltat [10/16 ns]").c_str(),1024,-512,512));
			S3RS_tgate.push_back( new TH1F(("S3RS_tgate"+sfx).c_str(),("S3RS_tgate"+sfx+";#Deltat [10/16 ns]").c_str(),1024,-512,512));

			S3_sectormult.push_back( new TH1F(("S3_sectormult"+sfx).c_str(),("S3_sectormult"+sfx+";Sector Multiplicity;Counts").c_str(),20,0,20));
			S3_ringmult.push_back( new TH1F(("S3_ringmult"+sfx).c_str(),("S3_ringmult"+sfx+";Ring Multiplicity;Counts").c_str(),20,0,20));
			S3_mult.push_back( new TH1F(("S3_mult"+sfx).c_str(),("S3_mult"+sfx+"+;S3 Multiplicity;Counts").c_str(),20,0,20));
			
			S3flat.push_back( new TH2F(("S3flat"+sfx).c_str(),("S3flat"+sfx+";Channel;Energy [keV]").c_str(),56,0,56,1000,0,control[S3EnergyLimit]));
			S3ring_sum.push_back( new TH1F(("S3ring_sum"+sfx).c_str(),("S3ring_sum"+sfx+";Energy [keV]").c_str(),2000,0,control[S3EnergyLimit]));
			S3sector_sum.push_back( new TH1F(("S3sector_sum"+sfx).c_str(),("S3sector_sum"+sfx+";Energy [keV]").c_str(),2000,0,control[S3EnergyLimit]));
		}
		
		
		
		outfile->cd("S3");
		if(MultiS3) S3_multot = new TH1F("S3_mult","S3_mult;S3 Multiplicity;Counts",20,0,20);
// 		outfile->mkdir("S3/elements");
// 		outfile->cd("S3/elements");
// 		
// 			TH1F *S3rings[24];
// 			for(int i=0;i<24;i++){
// 				stringstream ss;ss<<"S3_ring_energy_"<<i;
// 				S3rings[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,control[S3EnergyLimit]);axislab(S3rings[i],"Energy [keV]");
// 			}  
// 			
// 			TH1F *S3sectors[32];
// 			for(int i=0;i<32;i++){
// 				stringstream ss;ss<<"S3_sector_energy_"<<i;
// 				S3sectors[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,control[S3EnergyLimit]);axislab(S3sectors[i],"Energy [keV]");
// 			}
// 		outfile->mkdir("S3/sectorcal");
// 		outfile->cd("S3/sectorcal");
// 			TH2F *S3sectorsout[32];
// 			for(int i=0;i<32;i++){
// 				stringstream sss;sss<<"S3_sector_"<<i;
// 				S3sectorsout[i] = new TH2F(sss.str().c_str(),sss.str().c_str(),1000,0,control[S3EnergyLimit],24,0,24);
// 			}  
	outfile->cd();	

	TH2F *SiLiflat,*SiLiring,*SiLisector,*SiLipreamp,*SiLi_map;
	TH2F *SiLi_smirnov,*SiLi_smirnov_C,*SiLi_signoise;
	TH1F *SiLi_raw,*SiLi_addback,*SiLi_rejected,*SiLi_rejected_sum,*SiLi_fit_time;
	TH2F *SiLi_fit_timeE,*SiLi_fit_timeRF;
	TH1F *silirings[10];
	TH1F *silienergy[120];
	TH1F *silifitc[120];
	TH1F *SiLi_mult,*SiLiAdd_mult;
	
	if(DS){
	outfile->mkdir("SiLi");
	outfile->cd("SiLi");
		SiLiflat= new TH2F("SiLiflat","SiLi_raw_flat",120,0,120,1000,0,2000);axislab(SiLiflat,"Channel","Electron Energy [keV]");
		SiLiring= new TH2F("SiLiring","SiLi_raw_ring",10,0,10,1000,0,2000);axislab(SiLiring,"Ring","Electron Energy [keV]");
		SiLisector= new TH2F("SiLisector","SiLi_raw_sector",12,0,12,1000,0,2000);axislab(SiLisector,"Sector","Electron Energy [keV]");
		SiLipreamp= new TH2F("SiLipreamp","SiLipreamp",8,0,8,1000,0,2000);axislab(SiLisector,"Preamp","Electron Energy [keV]");
		SiLi_map= new TH2F("SiLi_map","SiLi_raw_map",1000,-50,50,1000,-50,50);axislab(SiLi_map,"X [mm]","Y [mm]");
		SiLi_raw= new TH1F("SiLi_raw","SiLi_raw",2000,0,2000);axislab(SiLi_raw,"Energy [keV]");
		SiLi_addback= new TH1F("SiLi_addback","SiLi_addback",2000,0,2000);axislab(SiLi_addback,"Electron Energy [keV]");
		SiLi_rejected= new TH1F("SiLi_rejected","SiLi_rejected",2000,0,2000);axislab(SiLi_rejected,"Electron Energy [keV]");
		SiLi_rejected_sum= new TH1F("SiLi_rejected_sum","SiLi_rejected_sum",2000,0,2000);axislab(SiLi_rejected_sum,"Electron Energy [keV]");
		SiLi_fit_time= new TH1F("SiLi_fit_time","SiLi_fit_time",512,0,256);axislab(SiLi_fit_time,"Electron Fit Time [10 ns]");
		
		if(UseFitCharge){
			SiLi_smirnov= new TH2F("SiLi_smirnov","SiLi_smirnov",512,0,2E6,512,0,2048);axislab(SiLi_smirnov,"Wave Fit Smirnov parameter","Electron Energy [keV]");
			SiLi_smirnov_C= new TH2F("SiLi_smirnov_C","SiLi_smirnov_C",512,0,5000,512,0,2048);axislab(SiLi_smirnov_C,"Wave Fit Smirnov parameter/Charge","Electron Energy [keV]");
			SiLi_signoise= new TH2F("SiLi_signoise","SiLi_signoise",1024,0,0.2,512,0,2048);axislab(SiLi_smirnov_C,"Wave Fit Noise/Signal","Electron Energy [keV]");
		}
		SiLi_fit_timeE= new TH2F("SiLi_fit_timeE","SiLi_fit_timeE",512,0,256,1000,0,2000);axislab(SiLi_fit_timeE,"Electron Fit Time [10 ns]","Electron Energy [keV]");
		SiLi_fit_timeRF= new TH2F("SiLi_fit_timeRF","SiLi_fit_timeRF",512,0,256,128,-16,16);axislab(SiLi_fit_timeRF,"Electron Fit Time [10 ns]","RF Fit Time [10 ns]");
		outfile->mkdir("SiLi/rings");
		outfile->cd("SiLi/rings");
			for(int i=0;i<10;i++){
				stringstream ss;ss<<"SiLi_raw_ring_energy_"<<i;
				silirings[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,2000);axislab(silirings[i],"Electron Energy [keV]");
			}
		outfile->mkdir("SiLi/elements");
		outfile->cd("SiLi/elements");
			for(int i=0;i<120;i++){
				stringstream ss;ss<<"SPICE_energy_"<<i;
				silienergy[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,2000);axislab(silienergy[i],"Electron Energy [keV]");
			}
		if(UseFitCharge){
		outfile->mkdir("SiLi/fit_charge");
		outfile->cd("SiLi/fit_charge");
			for(int i=0;i<120;i++){
				stringstream ss;ss<<"SPICE_fit_charge_"<<i;
				silifitc[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,5000);axislab(silienergy[i],"Fit Charge");
			} 
		}
		outfile->cd("SiLi");
		
		SiLi_mult= new TH1F("SiLi_mult","SiLi_mult;SPICE Multiplicity;Counts",20,0,20);
		SiLiAdd_mult= new TH1F("SiLiAdd_mult","SiLiAdd_mult;SPICE Addback Multiplicity;Counts",20,0,20);
	outfile->cd();
	}
	
	TH1F *Gamma_no_add,*Gamma_suppressed,*Gamma_singles_no_bgo,*Tig_mult;
	TH2F *Gamma_Gamma_no_add,*Gamma_Core,*TigressHitMap,*TigressHitMapLow,*TigressETheta;
	TH3F *TigressHitMap3,*TigressEETheta,*TigressEEdTheta;
	
	outfile->mkdir("Tigress");
	outfile->cd("Tigress");
		Gamma_no_add= new TH1F("Gamma_no_add","Gamma_no_add",2000,6,2006);axislab(Gamma_no_add,"#gamma Energy [keV]");
		Gamma_Gamma_no_add= new TH2F("Gamma_Gamma_no_add","Gamma_Gamma_no_add",1000,6,2006,1000,6,2006);axislab(Gamma_Gamma_no_add,"#gamma Energy [keV]","#gamma Energy [keV]");
		Gamma_Core= new TH2F("Gamma_Core","Gamma_Core",64,0,64,2000,0,2000);axislab(Gamma_Core,"Core No.","#gamma Energy [keV]");
		TigressHitMap3= new TH3F("TigressHitMap3","TigressHitMap3",128,-200,200,128,-200,200,128,-200,200);axislab(TigressHitMap3,"X [mm]","Y [mm]","Z [mm]");
		TigressHitMap= new TH2F("TigressHitMap","TigressHitMap",200,-TMath::Pi(),TMath::Pi(),200,0,TMath::Pi());axislab(TigressHitMap,"#theta [rad]","#phi [rad]");
		TigressHitMapLow= new TH2F("TigressHitMapLow","TigressHitMapLow",100,-TMath::Pi(),TMath::Pi(),100,0,TMath::Pi());axislab(TigressHitMapLow,"#theta [rad]","#phi [rad]");
		Gamma_suppressed= new TH1F("Gamma_suppressed","Gamma_suppressed",2000,0,2000);axislab(Gamma_suppressed,"#gamma Energy [keV]");
		Gamma_singles_no_bgo= new TH1F("Gamma_singles_no_bgo","Gamma_singles_no_bgo",2000,0,2000);axislab(Gamma_singles_no_bgo,"#gamma Energy [keV]");

		outfile->mkdir("Tigress/Angles");
		outfile->cd("Tigress/Angles");
			TigressETheta= new TH2F("TigressETheta","TigressETheta",1000,0,1000,100,0,TMath::Pi());axislab(TigressETheta,"#gamma Energy [keV]","#theta [rad]");
			TigressEETheta= new TH3F("TigressEETheta","TigressEETheta",1000,0,1000,350,0,1050,8,0,TMath::Pi());axislab(TigressEETheta,"#gamma' Energy [keV]","#gamma Energy [keV]","#theta [rad]");
			TigressEEdTheta= new TH3F("TigressEEdTheta","TigressEEdTheta",350,0,1050,350,0,1050,180,0,TMath::Pi());axislab(TigressEEdTheta,"#gamma Energy [keV]","#gamma Energy [keV]","#Delta#theta [rad]");
		outfile->cd("Tigress");
		
		Tig_mult= new TH1F("Tig_mult","Tig_mult;Tigress Multiplicity;Counts",20,0,20);
	outfile->cd();
	
	
	TH1F *GG_t, *ee_t, *SiLi_S3_t, *SiLi_S3_tw, *Gamma_S3_t, *Gamma_SiLi_t, *Gamma_SiLi_tw, *GG_tgate, *ee_tgate, *SiLi_S3_tgate, *Gamma_S3_tgate, *Gamma_SiLi_tgate, *S3_rf, *SiLi_rf, *Gamma_rf, *S3_rfgate, *SiLi_rfgate, *Gamma_rfgate;
	TH2F *SiLi_S3_t2, *Gamma_S3_t2, *Gamma_SiLi_t2, *SiLi_rf2, *EGamma_rf2, *S3RS_RF, *S3_SiLi_RF, *Gamma_SiLi_RF, *Gamma_S3_RF, *S3RS_RFgated, *S3_SiLi_RFgated, *Gamma_SiLi_RFgated, *Gamma_S3_RFgated;
	TH3F *Gamma_SiLi_t3, *S3_SiLi_RFe, *Gamma_S3_RFe, *Gamma_SiLi_SiliRF;
	
	outfile->mkdir("CoinTimegates");
	outfile->cd("CoinTimegates");
		outfile->mkdir("CoinTimegates/Time1D");
		outfile->cd("CoinTimegates/Time1D");
			 GG_t = new TH1F("GG_t","GG_t",1024,-512,512);axislab(GG_t,"#Deltat [10/16 ns]");
			 ee_t = new TH1F("ee_t","ee_t",1024,-1024,1024);axislab(ee_t,"#Deltat [10/16 ns]");
			 Gamma_S3_t= new TH1F("Gamma_S3_t","Gamma_S3_t",1024,-512,512);axislab(Gamma_S3_t,"#Deltat [10/16 ns]");
			if(DS){ SiLi_S3_t= new TH1F("SiLi_S3_t","SiLi_S3_t",1024,-1024,1024);axislab(SiLi_S3_t,"#Deltat [10/16 ns]");
			SiLi_S3_tw= new TH1F("SiLi_S3_tw","SiLi_S3_tw",2048,-10240,10240);axislab(SiLi_S3_t,"#Deltat [10/16 ns]");
			Gamma_SiLi_t= new TH1F("Gamma_SiLi_t","Gamma_SiLi_t",1024,-512,512);axislab(Gamma_SiLi_t,"#Deltat [10/16 ns]");
			Gamma_SiLi_tw= new TH1F("Gamma_SiLi_tw","Gamma_SiLi_tw",2048,-10240,10240);axislab(Gamma_SiLi_t,"#Deltat [10/16 ns]");}
		outfile->mkdir("CoinTimegates/Gates");
		outfile->cd("CoinTimegates/Gates");
			 GG_tgate = new TH1F("GG_tgate","GG_tgate",1024,-512,512);axislab(GG_tgate,"#Deltat [10/16 ns]");
			 ee_tgate = new TH1F("ee_tgate","ee_tgate",1024,-1024,1024);axislab(ee_tgate,"#Deltat [10/16 ns]");
			 Gamma_S3_tgate= new TH1F("Gamma_S3_tgate","Gamma_S3_tgate",1024,-512,512); axislab(Gamma_S3_tgate,"#Deltat [10/16 ns]");
			if(DS){SiLi_S3_tgate= new TH1F("SiLi_S3_tgate","SiLi_S3_tgate",1024,-1024,1024);axislab(SiLi_S3_tgate,"#Deltat [10/16 ns]");
			Gamma_SiLi_tgate= new TH1F("Gamma_SiLi_tgate","Gamma_SiLi_tgate",1024,-512,512); axislab(Gamma_SiLi_tgate,"#Deltat [10/16 ns]");}
		outfile->mkdir("CoinTimegates/TimeEnergy");
		outfile->cd("CoinTimegates/TimeEnergy");
			 Gamma_S3_t2= new TH2F("Gamma_S3_t2","Gamma_S3_t2",2000,0,2000,1024,-512,512);axislab(Gamma_S3_t2,"Gamma Energy [keV]","#Deltat [10/16 ns]");
			if(DS) {SiLi_S3_t2= new TH2F("SiLi_S3_t2","SiLi_S3_t2",2000,0,2000,1024,-1024,1024);axislab(SiLi_S3_t2,"Electron Energy [keV]","#Deltat [10/16 ns]");
			Gamma_SiLi_t2= new TH2F("Gamma_SiLi_t2","Gamma_SiLi_t2",2000,0,2000,1024,-512,512);axislab(Gamma_SiLi_t2,"Gamma Energy [keV]","#Deltat [10/16 ns]");
			Gamma_SiLi_t3= new TH3F("Gamma_SiLi_t3","Gamma_SiLi_t3",400,0,1000,400,0,1000,1024,-512,512);axislab(Gamma_SiLi_t3,"Gamma Energy [keV]","Electron Energy [keV]","#Deltat [10/16 ns]");}
	outfile->cd();
	
	outfile->mkdir("RFTimegates");
	outfile->cd("RFTimegates");
		outfile->mkdir("RFTimegates/Time1D");
		outfile->cd("RFTimegates/Time1D");
			 S3_rf = new TH1F("S3_rf","S3_rf",1024,-128,128);axislab(S3_rf,"Cfd-rf #Deltat [10 ns]");
			if(DS){ SiLi_rf= new TH1F("SiLi_rf","SiLi_rf",1024,-256,256);axislab(SiLi_rf,"Electron T-rf #Deltat [10 ns]");}
			 Gamma_rf= new TH1F("Gamma_rf","Gamma_rf",1024,-128,128);axislab(Gamma_rf,"Cfd-rf #Deltat [10 ns]");
		outfile->mkdir("RFTimegates/Time1DGates");
		outfile->cd("RFTimegates/Time1DGates");
			 S3_rfgate = new TH1F("S3_rfgate","S3_rfgate",1024,-128,128);axislab(S3_rfgate,"Cfd-rf #Deltat [10 ns]");
			if(DS){ SiLi_rfgate= new TH1F("SiLi_rfgate","SiLi_rfgate",1024,-256,256);axislab(SiLi_rfgate,"Electron T-rf #Deltat [10 ns]");}
			 Gamma_rfgate= new TH1F("Gamma_rfgate","Gamma_rfgate",1024,-128,128);axislab(Gamma_rfgate,"Cfd-rf #Deltat [10 ns]");
		outfile->mkdir("RFTimegates/Time1DvsEnergy");
		outfile->cd("RFTimegates/Time1DvsEnergy");
			if(DS){ SiLi_rf2= new TH2F("SiLi_rf2","SiLi_rf2",2000,0,2000,1024,-256,256);axislab(SiLi_rf2,"Electron Energy [keV]","Electron T-rf #Deltat [10 ns]");}
			 EGamma_rf2= new TH2F("EGamma_rf2","EGamma_rf2",2000,0,2000,1024,-128,128);axislab(EGamma_rf2,"#gamma Energy [keV]","Cfd-rf #Deltat [10 ns]");
		outfile->mkdir("RFTimegates/Time2D");
		outfile->cd("RFTimegates/Time2D");
			 S3RS_RF= new TH2F("S3RS_RF","S3RS_RF",512,-128,128,512,-128,128);axislab(S3RS_RF,"#deltaE Cfd-rf #Deltat [10 ns]","E Cfd-rf #Deltat [10 ns]");
			if(DS){ S3_SiLi_RF= new TH2F("S3_SiLi_RF","S3_SiLi_RF",512,-128,128,512,0,256);axislab(S3_SiLi_RF,"S3 Cfd-rf #Deltat [10 ns]","Electron T-rf #Deltat [10 ns]");
			Gamma_SiLi_RF= new TH2F("Gamma_SiLi_RF","Gamma_SiLi_RF",512,-128,128,512,0,256);axislab(Gamma_SiLi_RF,"#gamma Cfd-rf #Deltat [10 ns]","Electron T-rf #Deltat [10 ns]");}
			 Gamma_S3_RF= new TH2F("Gamma_S3_RF","Gamma_S3_RF",512,-128,128,512,-128,128);axislab(Gamma_S3_RF,"S3 Cfd-rf #Deltat [10 ns]","#gamma Cfd-rf #Deltat [10 ns]");
		outfile->mkdir("RFTimegates/Time2Dcoinctimegated");
		outfile->cd("RFTimegates/Time2Dcoinctimegated");
			 S3RS_RFgated= new TH2F("S3RS_RFgated","S3RS_RFgated",512,-128,128,512,-128,128);axislab(S3RS_RFgated,"#deltaE Cfd-rf #Deltat [10 ns]","E Cfd-rf #Deltat [10 ns]");
			if(DS){ S3_SiLi_RFgated= new TH2F("S3_SiLi_RFgated","S3_SiLi_RFgated",512,-128,128,512,0,256);axislab(S3_SiLi_RFgated,"S3 Cfd-rf #Deltat [10 ns]","Electron T-rf #Deltat [10 ns]");
			Gamma_SiLi_RFgated= new TH2F("Gamma_SiLi_RFgated","Gamma_SiLi_RFgated",512,-128,128,512,0,256);axislab(Gamma_SiLi_RFgated,"#gamma Cfd-rf #Deltat [10 ns]","Electron T-rf #Deltat [10 ns]");}
			 Gamma_S3_RFgated= new TH2F("Gamma_S3_RFgated","Gamma_S3_RFgated",512,-128,128,512,-128,128);axislab(Gamma_S3_RFgated,"S3 Cfd-rf #Deltat [10 ns]","#gamma Cfd-rf #Deltat [10 ns]");
		outfile->mkdir("RFTimegates/Time2DvsEnergy");
		outfile->cd("RFTimegates/Time2DvsEnergy");
			if(DS){ S3_SiLi_RFe= new TH3F("S3_SiLi_RFe","S3_SiLi_RFe",256,-128,128,256,0,256,512,0,1024);axislab(S3_SiLi_RFe,"S3 Cfd-rf #Deltat [10 ns]","Electron T-rf #Deltat [10 ns]","Electron Energy [keV]");
			if(DS) Gamma_SiLi_SiliRF= new TH3F("Gamma_SiLi_SiliRF","Gamma_SiLi_SiliRF",512,0,2048,512,0,2048,300,0,300);axislab(Gamma_SiLi_SiliRF,"#gamma Energy [keV]","Electron Energy [keV]","Electron T-rf #Deltat [10 ns]");}
			 Gamma_S3_RFe= new TH3F("Gamma_S3_RFe","Gamma_S3_RFe",256,-128,128,256,-128,128,512,0,1024);axislab(Gamma_S3_RFe,"S3 Cfd-rf #Deltat [10 ns]","#gamma Cfd-rf #Deltat [10 ns]","#gamma Energy [keV]");
	outfile->cd();
	
	TH1F* Gamma_RFgated,*Gamma_RFantigated,*SiLi_RFgated,*SiLi_RFantigated;
	TH2F* SiLiGamma__RFgated,*GammaGamma_RFgated;
	
	outfile->mkdir("RF1Dgated");
	outfile->cd("RF1Dgated");	
		Gamma_RFgated= new TH1F("Gamma_RFgated","Gamma_RFgated",2000,0,2000);
		Gamma_RFantigated= new TH1F("Gamma_RFantigated","Gamma_RFantigated",2000,0,2000);	
		if(DS){ SiLi_RFgated= new TH1F("SiLi_RFgated","SiLi_RFgated",2000,0,2000);
		SiLi_RFantigated= new TH1F("SiLi_RFantigated","SiLi_RFantigated",2000,0,2000);
		SiLiGamma__RFgated= new TH2F("SiLiGamma__RFgated","SiLiGamma__RFgated",500,0,1000,500,0,1000);}
		GammaGamma_RFgated= new TH2F("GammaGamma_RFgated","GammaGamma_RFgated",500,0,1000,500,0,1000);
	outfile->cd();
	
	TH1F *eventrate,*eventratehours,*runtime,*runtimeraw,*timeadda,*fileentry; 
	TH2F *runtime_sili,*runtime_silinoise,*runtime_gamma,*eventN_sili,*eventN_silinoise,*eventN_gamma,*fileN_sili,*fileN_silinoise,*fileN_gamma;
	TH3F *eventN_gammagamma ,*eventN_gammasili,*runtime_gammagamma,*runtime_gammasili;
	
	outfile->mkdir("RunTime");
	outfile->cd("RunTime");
		 eventrate = new TH1F("eventrate","eventrate",10000,0,tstamprange);axislab(eventrate,"Time [10 ns]");
		 eventratehours = new TH1F("eventratehours","eventratehours",10000,0,tstamprange/timestamp_hour);axislab(eventratehours,"Time [hours]");
		 runtime = new TH1F("runtime","runtime",10000,0,(long)(10000*(nentries/9999)));axislab(runtime,"Entry No.","Time [10 ns]");
		 runtimeraw = new TH1F("runtimeraw","runtimeraw",10000,0,(long)(10000*(nentries/9999)));axislab(runtimeraw,"Entry No.","Time [10 ns]");
		 timeadda = new TH1F("timeadda","timeadda",10000,0,(long)(10000*(nentries/9999)));axislab(timeadda,"Entry No.","Time [10 ns]");
		 
		 runtime_gamma = new TH2F("runtime_gamma","runtime_gamma",1024,0,tstamprange,1024,0,2048);axislab(runtime_gamma,"Time [10 ns]","#gamma Energy [keV]");
		 runtime_gammagamma = new TH3F("runtime_gammagamma","runtime_gammagamma",128,0,tstamprange,1024,0,2048,1024,0,2048);axislab(runtime_gammagamma,"Time [10 ns]","#gamma Energy [keV]","#gamma Energy [keV]");
		 eventN_gamma = new TH2F("eventN_gamma","eventN_gamma",1024,0,(long)(1024*(nentries/1023)),1024,0,2048);axislab(eventN_gamma,"Entry No.","#gamma Energy [keV]");
		 eventN_gammagamma = new TH3F("eventN_gammagamma","eventN_gammagamma",128,0,(long)(128*(nentries/127)),1024,0,2048,1024,0,2048);axislab(eventN_gammagamma,"Entry No.","#gamma Energy [keV]","#gamma Energy [keV]");
		 fileN_gamma = new TH2F("fileN_gamma","fileN_gamma",filelist.size(),0,filelist.size(),1024,0,2048);axislab(fileN_gamma,"Entry No.","#gamma Energy [keV]");

// 		int fileN=DataChain->GetListOfFiles()->GetSize();		 
		 
		if(DS){
		 runtime_sili = new TH2F("runtime_sili","runtime_sili",1024,0,tstamprange,1024,0,2048);axislab(runtime_sili,"Time [10 ns]","Electron Energy [keV]");
		 runtime_silinoise = new TH2F("runtime_silinoise","runtime_silinoise",1024,0,tstamprange,1024,0,2048);axislab(runtime_silinoise,"Time [10 ns]","Electron Energy [keV]");
		 runtime_gammasili = new TH3F("runtime_gammasili","runtime_gammasili",128,0,tstamprange,1024,0,2048,1024,0,2048);axislab(runtime_gammasili,"Time [10 ns]","#gamma Energy [keV]","Electron Energy [keV]");
		 eventN_sili = new TH2F("eventN_sili","eventN_sili",1024,0,(long)(1024*(nentries/1023)),1024,0,2048);axislab(eventN_sili,"Entry No.","Electron Energy [keV]");
		 eventN_silinoise = new TH2F("eventN_silinoise","eventN_silinoise",1024,0,(long)(1024*(nentries/1023)),1024,0,2048);axislab(eventN_silinoise,"Entry No.","Electron Energy [keV]");
		 eventN_gammasili = new TH3F("eventN_gammasili","eventN_gammasili",128,0,(long)(128*(nentries/127)),1024,0,2048,1024,0,2048);axislab(eventN_gammasili,"Entry No.","#gamma Energy [keV]","Electron Energy [keV]");
		 fileN_sili = new TH2F("fileN_sili","fileN_sili",filelist.size(),0,filelist.size(),1024,0,2048);axislab(fileN_sili,"File No.","Electron Energy [keV]");
		 fileN_silinoise = new TH2F("fileN_silinoise","fileN_silinoise",filelist.size(),0,filelist.size(),1024,0,2048);axislab(fileN_silinoise,"File No.","Electron Energy [keV]");
		}
		 
		 fileentry = new TH1F("fileentry","fileentry",filelist.size(),0,filelist.size());axislab(fileentry,"","EOF Entry No.");
		for(unsigned int i=0;i<filelist.size();i++){
			fileentry->SetBinContent(i,fileentriessum[i]);
			fileentry->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			if(DS) fileN_sili->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			if(DS) fileN_silinoise->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			fileN_gamma->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
		}
		
		TGraph* sorttime=new TGraph();
		TGraph* guessend=new TGraph();
	outfile->cd();

	TH3F* GammaS3dedx;
	vector< TH2F* >  S3particleGated;
	vector< TH2F* >  GUncorrectedring,Gcorrectedring;
	vector< TH1F* >  GammaPG,SiLiPG,GUnshifted,GUncorrected;
	vector< TH1F* >  SiLiPGRF,PGmult;
	vector< TH2F* >  GammaEPG,SiLiEPG,PGmultGamma,PGmultSili;
	vector< TH2F* >  GammaSiLiPG,GammaGammaPG;
	vector< TH2F* >  TigressEThetaPG;
	vector< TH3F* >  TigressEEThetaPG,TigressEEdThetaPG;
	vector<	vector< TH2F* > >  TigressDopplerAngle;
	vector<	vector< TH1F* > >  RingGroupGammaSingles;
	
	if(ParticleGate.size()>0){
		outfile->mkdir("ParticleGates");
		outfile->cd("ParticleGates");
			
			if(Telescope){
				GammaS3dedx= new TH3F("GammaS3dedx","GammaS3dedx;Total Energy [keV];dE/dx [arb.];#gamma Energy [keV]",100,0,control[S3EnergyLimit],100,0,control[S3EnergyLimit]*.4,1000,0,2000);
			}
			
			for(int n=0;n<ParticleGate.size();n++){
				string t=ParticleGate[n].title;
				string tf="ParticleGates/"+t;
				
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
					TH1F* Gammapg= new TH1F(("Gamma_"+t).c_str(),("Gamma_"+t).c_str(),2500,0,2500);axislab(Gammapg,"#gamma Energy [keV]");
					GammaPG.push_back(Gammapg);
							
					TH2F* GammaEpg= new TH2F(("GammaE_"+t).c_str(),("GammaE_"+t).c_str(),1000,0,2000,500,0,control[S3EnergyLimit]);axislab(GammaEpg,"#gamma Energy [keV]","Total Silicon Energy [keV]");
					GammaEPG.push_back(GammaEpg);
					
					TH2F* GammaGammapg= new TH2F(("GammaGamma_"+t).c_str(),("GammaGamma_"+t).c_str(),1000,0,2000,1000,0,2000);axislab(GammaGammapg,"#gamma Energy [keV]","#gamma Energy [keV]");
					GammaGammaPG.push_back(GammaGammapg);
							
					if(DS){
					TH1F* SiLipg= new TH1F(("SiLi_"+t).c_str(),("SiLi_"+t).c_str(),2000,0,2000);axislab(SiLipg,"Electron Energy [keV]");
					SiLiPG.push_back(SiLipg);
					SiLipg= new TH1F(("SiLi_"+t+"_RFgated").c_str(),("SiLi_"+t+"_RFgated").c_str(),2000,0,2000);axislab(SiLipg,"Electron Energy [keV]");
					SiLiPGRF.push_back(SiLipg);
					TH2F* SiLiEpg= new TH2F(("SiLiE_"+t).c_str(),("SiLiE_"+t).c_str(),1000,0,2000,500,0,control[S3EnergyLimit]);axislab(SiLiEpg,"Electron Energy [keV]","Total Silicon Energy [keV]");
					SiLiEPG.push_back(SiLiEpg);
					TH2F* GammaSilipg= new TH2F(("GammaSili_"+t).c_str(),("GammaSili_"+t).c_str(),500,0,1500,500,0,1500);axislab(GammaSilipg,"#gamma Energy [keV]","Electron Energy [keV]");
					GammaSiLiPG.push_back(GammaSilipg);
					}
					
					TH2F* TigressEThetapg= new TH2F(("GammaTheta_"+t).c_str(),("GammaETheta_"+t).c_str(),2000,0,2000,100,0,TMath::Pi());axislab(TigressEThetapg,"#gamma Energy [keV]","#theta_{#gamma} [rad]");
					TigressEThetaPG.push_back(TigressEThetapg);
					
					TH3F* TigressEEThetapg= new TH3F(("GammaThetaGamma_"+t).c_str(),("GammaThetaGamma_"+t).c_str(),1000,0,2000,500,0,1500,90,0,TMath::Pi());axislab(TigressEEThetapg,"#gamma_{1} Energy [keV]","#gamma_{2} Energy [keV]","#theta_{#gamma_{1}}  [rad]");
					TigressEEThetaPG.push_back(TigressEEThetapg);
					
					TH3F* TigressEEdThetapg= new TH3F(("GammaGammaAngSep_"+t).c_str(),("GammaGammaAngSep_"+t).c_str(),500,0,1500,500,0,1500,90,0,TMath::Pi());axislab(TigressEEdThetapg,"#gamma Energy [keV]","#gamma Energy [keV]","#Delta#theta [rad]");
					TigressEEdThetaPG.push_back(TigressEEdThetapg);
					
					
					TH1F* pgmult= new TH1F(("Multiplicity_"+t).c_str(),("Multiplicity_"+t+";Multiplicity").c_str(),10,0,10);
					PGmult.push_back(pgmult);
					
					if(MultiParticles){
						TH2F* pgmultg= new TH2F(("Multiplicity_v_gamma_"+t).c_str(),("Multiplicity_v_gamma_"+t+";Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2000,0,2000);
						PGmultGamma.push_back(pgmultg);
						if(DS){
							TH2F* pgmults= new TH2F(("Multiplicity_v_sili_"+t).c_str(),("Multiplicity_v_sili_"+t+";Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2000,0,2000);
							PGmultSili.push_back(pgmults);
						}
					}
					
					TH2F* S3Pgtd;
					if(Telescope)S3Pgtd= new TH2F(("S3particleGated"+t).c_str(),("S3particleGated"+t+";Total Energy [keV];dE/dx [arb.]").c_str(),500,0,control[S3EnergyLimit],500,0,control[S3EnergyLimit]*.4);
					else S3Pgtd = new TH2F(("S3particleGated"+t).c_str(),("S3particleGated"+t+";Total Energy [keV];Theta [rad]").c_str(),500,0,control[S3EnergyLimit],100,0.2,1.2);
					S3particleGated.push_back(S3Pgtd);
					
					
					if(ParticleGate[n].use_beta){
						outfile->mkdir((tf+"/Doppler").c_str());
						outfile->cd((tf+"/Doppler").c_str());
						
							TH1F* gshift= new TH1F(("GammaUnshifted_"+t).c_str(),("GammaUnshifted_"+t).c_str(),2500,0,2500);axislab(gshift,"#gamma Energy [keV]");
							GUnshifted.push_back(gshift);
						
							TH1F* guncor= new TH1F(("GammaUncorrected_"+t).c_str(),("GammaUncorrected_"+t).c_str(),2500,0,2500);axislab(gshift,"#gamma Energy [keV]");
							GUncorrected.push_back(guncor);
							
							TH2F* guncorR= new TH2F(("GammaUncorrectedRing_"+t).c_str(),("GammaUncorrectedRing_"+t).c_str(),1000,0,2000,24,0,24);axislab(gshift,"#gamma Energy [keV]","S3 Ring Number");
							GUncorrectedring.push_back(guncorR);

							TH2F* gcorR= new TH2F(("GammaCorrectedRing_"+t).c_str(),("GammaCorrectedRing_"+t).c_str(),1000,0,2000,24,0,24);axislab(gshift,"#gamma Energy [keV]","S3 Ring Number");
							Gcorrectedring.push_back(gcorR);
						
							TigressDopplerAngle.push_back(vector< TH2F* >());
							for(int r=0;r<24;r++){
								stringstream subname;subname<<"TigressDopplerAngle_"<<t<<"_Ring"<<r;
								TH2F* Tigressdopplerangle= new TH2F(subname.str().c_str(),subname.str().c_str(),100,0,TMath::Pi(),2000,0,2000);axislab(Tigressdopplerangle,"#theta [rad]","#gamma Energy [keV]");
								TigressDopplerAngle[n].push_back(Tigressdopplerangle);
							}

						if(ringgroups.size()){
							outfile->mkdir((tf+"/RingGroups").c_str());
							outfile->cd((tf+"/RingGroups").c_str());
							
							vector< TH1F* > rggs;
							for(unsigned int r=0;r<ringgroups.size();r++){
								stringstream histname;
								histname<<"GammaCorrect_Rings_"<<ringgroups[r].first<<"-"<<ringgroups[r].second<<t;
								rggs.push_back(new TH1F(histname.str().c_str(),histname.str().c_str(),2500,0,2500));
								axislab(rggs[r],"#gamma Energy [keV]");
							}
							RingGroupGammaSingles.push_back(rggs);
						}
					}
					

			}
		outfile->cd();
	}

	
	
	vector< TH1F* >  MultiPartTig,MultiPartSPICE;
	vector< TH2F* >  PhiTheta,multisA,multisB,multigA,multigB;
	
	if(ParticleGate.size()>1&&MultiParticles){
		outfile->mkdir("MultiParticleGates");
		outfile->cd("MultiParticleGates");
		
		for(int n=0;n<ParticleGate.size();n++){
			string t=ParticleGate[n].title;
			
			for(int m=n+1;m<ParticleGate.size();m++){
				string T=ParticleGate[m].title;
			
				string tf="MultiParticleGates/"+t+T;
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
				if(DS){
					TH1F* spmt= new TH1F(("SiLi_"+t+T).c_str(),("SiLi_"+t+T).c_str(),2000,0,2000);axislab(spmt,"Electron Energy [keV]");
					MultiPartSPICE.push_back(spmt);

					TH2F* pgmults= new TH2F(("Mult_sili_"+t).c_str(),("Mult_sili_"+t+";Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2000,0,2000);
					multisA.push_back(pgmults);
					pgmults= new TH2F(("Mult_sili_"+T).c_str(),("Mult_sili_"+T+";Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2000,0,2000);
					multisB.push_back(pgmults);
				}
				
				TH1F* tigmt= new TH1F(("Gamma_"+t+T).c_str(),("Gamma_"+t+T).c_str(),2500,0,2500);axislab(tigmt,"#gamma Energy [keV]");
				MultiPartTig.push_back(tigmt);
				
				TH2F* pgmultg= new TH2F(("Mult_gamma_"+t).c_str(),("Mult_gamma_"+t+";Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2500,0,2500);
				multigA.push_back(pgmultg);
				pgmultg= new TH2F(("Mult_gamma_"+T).c_str(),("Mult_gamma_"+T+";Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2500,0,2500);
				multigB.push_back(pgmultg);
				
				TH2F* phth= new TH2F(("Angle_"+t+T).c_str(),("Angle_"+t+T+";#Delta#Theta;#Delta#Phi").c_str(),128,0,pi,128,0,pi);
				PhiTheta.push_back(phth);
			}
		}
		outfile->cd();
	}
		

	
	TH1F*ee_singles,*ee_sum,*ee_sum_gtfifty,*silirawtotal;	TH2F*ee_vs,*ee_singleVsum,*ee_singleVsum_gt50,*ee_sum_distance,*ee_sum_ring,*e_single_ring,*e_single_preamp,*e_vs_rawtotal,*ee_sum_vs_rawtotal,*ee_single_gamma,*ee_sum_gamma;
	TH3F*e_e_y,*ee_e_y;
	
	if(DoDoubleElectrons&&DS){
		outfile->mkdir("NonNeighbourSiLiPairs");
		outfile->cd("NonNeighbourSiLiPairs");	
				ee_singles= new TH1F("ee_singles","ee_singles",2000,0,2000);axislab(ee_singles,"Electron Pair Single Energy [keV]");
				ee_sum= new TH1F("ee_sum","ee_sum",2000,0,2000);axislab(ee_sum,"Electron Pair Summed Energy [keV]");
				ee_vs= new TH2F("ee_vs","ee_vs",500,0,1500,500,0,1500);axislab(ee_vs,"Electron Pair Single Energy [keV]","Electron Pair Single Energy [keV]");
				ee_singleVsum= new TH2F("ee_singleVsum","ee_singleVsum",500,0,1500,500,0,1500);axislab(ee_singleVsum,"Electron Pair Single Energy [keV]","Electron Pair Sum Energy [keV]");
				ee_sum_gtfifty= new TH1F("ee_sum_gtfifty","ee_sum_gtfifty",2000,0,2000);axislab(ee_sum_gtfifty,"Electron Pair Summed Energy [keV]");
				ee_singleVsum_gt50= new TH2F("ee_singleVsum_gt50","ee_singleVsum_gt50",500,0,1500,500,0,1500);axislab(ee_singleVsum_gt50,"Electron Pair Single Energy [keV]","Electron Pair Sum Energy [keV]");
				ee_sum_distance= new TH2F("ee_sum_distance","ee_sum_distance",500,0,1500,100,0,100);axislab(ee_sum_distance,"Electron Pair Summed Energy [keV]","Pixel Separation [mm]");
				ee_sum_ring= new TH2F("ee_sum_ring","ee_sum_ring",500,0,1500,10,0,10);axislab(ee_sum_ring,"Electron Pair Summed Energy [keV]","Ring No.");
				e_single_ring= new TH2F("e_single_ring","e_single_ring",500,0,1500,10,0,10);axislab(e_single_ring,"Electron Pair Single Energy [keV]","Ring No.");
				e_single_preamp= new TH2F("e_single_preamp","e_single_preamp",500,0,1500,8,0,8);axislab(e_single_preamp,"Electron Pair Single Energy [keV]","Ring No.");
				e_vs_rawtotal= new TH2F("e_vs_rawtotal","e_vs_rawtotal",500,0,1500,600,0,1800);axislab(e_vs_rawtotal,"Electron Pair Single Energy [keV]","Raw Electron Sum Total [keV]");
				ee_sum_vs_rawtotal= new TH2F("ee_sum_vs_rawtotal","ee_sum_vs_rawtotal",500,0,1500,600,0,1800);axislab(ee_sum_vs_rawtotal,"Electron Pair Summed Energy [keV]","Raw Electron Sum Total [keV]");
				silirawtotal= new TH1F("silirawtotal","silirawtotal",2000,0,2000); axislab(silirawtotal,"Raw Electron Sum Total [keV]");
				ee_single_gamma= new TH2F("ee_single_gamma","ee_single_gamma",500,0,1500,500,0,1500);axislab(ee_single_gamma,"Electron Pair Single Energy [keV]","#gamma Energy [keV]");
				ee_sum_gamma= new TH2F("ee_sum_gamma","ee_sum_gamma",500,0,1500,500,0,1500);axislab(ee_sum_gamma,"Electron Pair Summed Energy [keV]","#gamma Energy [keV]");
				e_e_y= new TH3F("e_e_y","e_e_y",500,0,1500,500,0,1500,500,0,1500);axislab(e_e_y,"Electron Pair Single Energy [keV]","Electron Pair Single Energy [keV]","#gamma Energy [keV]");
				ee_e_y= new TH3F("ee_e_y","ee_e_y",500,0,1500,500,0,1500,500,0,1500);axislab(e_e_y,"Electron Pair Summed Energy [keV]","Electron Pair Single Energy [keV]","#gamma Energy [keV]");
		outfile->cd();	
	}

	TH1D *SiLi_singles,*Gamma_singles,*GammaSiLiPlus;
	TH2F *Gamma_SiLi,*Gamma_Gamma,*GammaSiLiPlus_Gamma,*GammaSiLiPlus_SiLi;
	TH3F *Gamma_Gamma_Gamma;
	
	Gamma_singles= new TH1D("Gamma_singles","Gamma_singles",2000,0,2000);axislab(Gamma_singles,"#gamma Energy [keV]");	
	Gamma_Gamma= new TH2F("Gamma_Gamma","Gamma_Gamma",1000,0,2000,1000,0,2000);axislab(Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]");
	Gamma_Gamma_Gamma= new TH3F("Gamma_Gamma_Gamma","Gamma_Gamma_Gamma",500,0,2000,500,0,2000,500,0,2000);axislab(Gamma_Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]","#gamma Energy [keV]");
	if(DS){SiLi_singles= new TH1D("SiLi_singles","SiLi_singles",2000,0,2000);axislab(SiLi_singles,"Electron Energy [keV]");
	Gamma_SiLi= new TH2F("Gamma_SiLi","Gamma_SiLi",1000,0,2000,1000,0,2000);axislab(Gamma_SiLi,"#gamma Energy [keV]","Electron Energy [keV]");}
	
	if(DS){
		GammaSiLiPlus= new TH1D("GammaSiLiPlus","GammaSiLiPlus",4000,0,4000);axislab(GammaSiLiPlus,"#gamma+SiLi Energy [keV]");
		GammaSiLiPlus_Gamma= new TH2F("GammaSiLiPlus_Gamma","GammaSiLiPlus_Gamma",2000,0,4000,1000,0,2000);axislab(GammaSiLiPlus_Gamma,"#gamma+SiLi Energy [keV]","#gamma Energy [keV]");
		GammaSiLiPlus_SiLi= new TH2F("GammaSiLiPlus_SiLi","GammaSiLiPlus_SiLi",2000,0,4000,1000,0,2000);axislab(GammaSiLiPlus_SiLi,"#gamma+SiLi Energy [keV]","SiLi Energy [keV]");
	}
	
// 	outfile->mkdir("MeanDoppler");
// 	outfile->cd("MeanDoppler");		
// 		TH1D* SiLi_singles_dop= new TH1D("SiLi_singles_dop","SiLi_singles_dop",2000,0,2000);axislab(SiLi_singles_dop,"Electron Energy [keV]");
// 		TH1D* Gamma_singles_dop= new TH1D("Gamma_singles_dop","Gamma_singles_dop",2000,0,2000);axislab(Gamma_singles_dop,"#gamma Energy [keV]");
// 		TH2F* Gamma_SiLi_dop= new TH2F("Gamma_SiLi_dop","Gamma_SiLi_dop",1000,0,2000,1000,0,2000);axislab(Gamma_SiLi_dop,"#gamma Energy [keV]","Electron Energy [keV]");
// 		TH2F* Gamma_Gamma_dop= new TH2F("Gamma_Gamma_dop","Gamma_Gamma_dop",1000,0,2000,1000,0,2000);axislab(Gamma_Gamma_dop,"#gamma Energy [keV]","#gamma Energy [keV]");
// 	outfile->cd();

	
gROOT->cd();
