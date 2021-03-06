////////////////////////////////////////////////
////////////////// HISTOGRAMS //////////////////
////////////////////////////////////////////////

bool debugX=(debug&&ParticleGate.size()<4);
int Gbins=1;
if(BigTigHist)Gbins=2;

outfile->cd();
// 	printf("Line number %d\n", __LINE__);
	TH3 *S3_map3;
	TH2 *S3_theta;
	
	TH1* S3_multot;//D because over max value for float hist
	vector< TH1* > S3_sectormult,S3_ringmult,S3_mult;
	
	vector< TH1* > fb_time,fb_timegated,front_back,front_backgated,S3ring_sum,S3sector_sum,S3RS_t,S3RS_tgate,chanhits;
	vector< TH2* > S3_map,S3_pixmap,S3_dedx,frontVback,frontVbackGated,S3flat;
	vector< TH3* > S3_d3dx,S3RS_t3;
	
	vector< TH2* > RingGrpThetaPhi;
	
	outfile->mkdir("S3");
	outfile->cd("S3");
		S3_map3= new TH3F("S3_map3","S3_map3;Z [mm];X [mm];Y [mm]",50,-50,50,80,-40,40,80,-40,40);
		S3_theta= new TH2F("S3_theta","S3_theta;Total Energy [keV];Theta [rad]",1024,0,control[S3EnergyLimit],200,0.2,1.2);
		
		int k=0;
		for(unsigned short i=0;i<MAXS3;i++){
			
			string sfx="";
			if(MultiS3){
				if(!s3used[i])continue;
				s3index[i]=k;
				S3OffsetVector[k]=S3OffsetTmp[i];//nothing to do with histograms but want to use s3index
				k++;
				stringstream ss;
				ss<<i;
				sfx=ss.str();
				string tf="S3/Pos"+sfx;
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
			}else{i=MAXS3;}

			chanhits.push_back(new TH1F(("S3_chanhits"+sfx).c_str(),("S3_chanhits"+sfx+";Channel;Raw Counts").c_str(),56,0,56));
			S3_map.push_back(new TH2F(("S3_map"+sfx).c_str(),("S3_map"+sfx+";X [mm];Y [mm]").c_str(),800,-40,40,800,-40,40));
			S3_pixmap.push_back(new TH2F(("S3_pixmap"+sfx).c_str(),("S3_pixmap"+sfx+";Ring;Sector").c_str(),24,0,24,32,0,32));
			
			S3RS_t.push_back( new TH1F(("S3RS_t"+sfx).c_str(),("S3RS_t"+sfx+";#Deltat [ns]").c_str(),1024,-512,512));
			S3RS_tgate.push_back( new TH1F(("S3RS_tgate"+sfx).c_str(),("S3RS_tgate"+sfx+";#Deltat [ns]").c_str(),1024,-512,512));

			if(Telescope){
				S3_dedx.push_back( new TH2F(("S3_dedx"+sfx).c_str(),("S3_dedx"+sfx+";Total Energy [keV];dE/dx [arb.]").c_str(),1000,0,control[S3EnergyLimit],1000,0,control[S3EnergyLimit]*.4));
				S3_d3dx.push_back( new TH3F(("S3_d3dx"+sfx).c_str(),("S3_d3dx"+sfx+";Total Energy [keV];dE/dx [arb.];Theta [rad]").c_str(),100,0,control[S3EnergyLimit],100,0,control[S3EnergyLimit]*.4,100,0,pi/2));
				S3RS_t3.push_back( new TH3F(("S3RS_t3"+sfx).c_str(),("S3RS_t3"+sfx+";#Deltat [ns];dE/dx [arb.];Total Energy [keV]").c_str(),100,-500,500,200,0,control[S3EnergyLimit],250,0,control[S3EnergyLimit]));
			}else{
				frontVback.push_back( new TH2F(("frontVback"+sfx).c_str(),("frontVback"+sfx+";Ring Energy [keV];Sector Energy [keV]").c_str(),512,0,control[S3EnergyLimit],512,0,control[S3EnergyLimit]));
				frontVbackGated.push_back( new TH2F(("frontVbackGated"+sfx).c_str(),("frontVbackGated"+sfx+";Ring Energy [keV];Sector Energy [keV]").c_str(),512,0,control[S3EnergyLimit],512,0,control[S3EnergyLimit]));
				front_back.push_back( new TH1F(("front_back"+sfx).c_str(),("front_back"+sfx+";Ring Energy-Sector Energy [keV];").c_str(),1000,-25000,25000));
				front_backgated.push_back( new TH1F(("front_backgated"+sfx).c_str(),("front_backgated"+sfx+";Ring Energy-Sector Energy [keV];").c_str(),1000,-25000,25000));
				fb_time.push_back( new TH1F(("frontVbackTime"+sfx).c_str(),("frontVbackTime"+sfx+";#Deltat [ns]").c_str(),1024,-512,512));
				fb_timegated.push_back( new TH1F(("frontVbackTimeGated"+sfx).c_str(),("frontVbackTimeGated"+sfx+";#Deltat [ns]").c_str(),1024,-512,512));
			}

			S3_sectormult.push_back( new TH1D(("S3_sectormult"+sfx).c_str(),("S3_sectormult"+sfx+";Sector Multiplicity;Counts").c_str(),20,0,20));
			S3_ringmult.push_back( new TH1D(("S3_ringmult"+sfx).c_str(),("S3_ringmult"+sfx+";Ring Multiplicity;Counts").c_str(),20,0,20));
			S3_mult.push_back( new TH1D(("S3_mult"+sfx).c_str(),("S3_mult"+sfx+"+;S3 Multiplicity;Counts").c_str(),20,0,20));
			
			S3flat.push_back( new TH2F(("S3flat"+sfx).c_str(),("S3flat"+sfx+";Channel;Energy [keV]").c_str(),56,0,56,1000,0,control[S3EnergyLimit]));
			S3ring_sum.push_back( new TH1F(("S3ring_sum"+sfx).c_str(),("S3ring_sum"+sfx+";Energy [keV]").c_str(),2000,0,control[S3EnergyLimit]));
			S3sector_sum.push_back( new TH1F(("S3sector_sum"+sfx).c_str(),("S3sector_sum"+sfx+";Energy [keV]").c_str(),2000,0,control[S3EnergyLimit]));
		}
		
		if(DoRingGroups){
			outfile->mkdir("S3/RingGrpTheta");
			outfile->cd("S3/RingGrpTheta");
			for(unsigned int r=0;r<ringgroups.size();r++){
				stringstream histname;
				histname<<"RingsGrp_"<<ringgroups[r].first<<"-"<<ringgroups[r].second<<"_ThetaPhi";
				RingGrpThetaPhi.push_back(new TH2F(histname.str().c_str(),(histname.str()+";Theta (rad);Phi (rad)").c_str(),360,0,TMath::Pi(),720,-TMath::Pi(),TMath::Pi()));
			}
		}
		
		outfile->cd("S3");
		if(MultiS3) S3_multot = new TH1D("S3_mult","S3_mult;S3 Multiplicity;Counts",20,0,20);
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
	TH2F *SiLi_smirnov,*SiLi_smirnov_C,*SiLi_signoiseraw,*SiLi_signoise,*SiLi_smirnov_Ccut,*SiLi_signoisecut;
	TH1F *SiLi_raw,*SiLi_rawm1,*SiLi_addback,*SiLi_unaddback,*SiLi_rejected,*SiLi_rejected_sum,*SiLi_fit_time;
	TH2F *SiLi_fit_timeS,*SiLi_fit_timeE;
	TH1F *silirings[10];
	TH1F *silienergy[120];
	TH1F *silifitc[120];
	TH1D *SiLi_mult,*SiLiAdd_mult;
	TH1F *SiLi_magshad,*SiLi_nagshad;
	TH2F *SiLi_shadRing,*SiLi_nagshadRing;
	TH2F *SiLiGamma_nagshad;
	TH2F *SiLiGamma_shad;
	TH2F* SiLiSegT,*SiLieeT;
	
	TH1 *SiLi_singlespc;
	TH1F *SiLi_magshadpc,*SiLi_nagshadpc;
	TH2F *SiLi_shadRingpc,*SiLi_nagshadRingpc;
    TH3 *GammaSiLipreamp,*GammaSiLiring,*GammaSiLisector;
    TH3 *GammaSiLiChan;
	
	if(DS){
	outfile->mkdir("SiLi");
	outfile->cd("SiLi");
		SiLiflat= new TH2F("SiLiflat","SiLi_raw_flat",120,0,120,1000,0,2000);axislab(SiLiflat,"Channel","Electron Energy [keV]");
		SiLiring= new TH2F("SiLiring","SiLi_raw_ring",10,0,10,1000,0,2000);axislab(SiLiring,"Ring","Electron Energy [keV]");
		SiLisector= new TH2F("SiLisector","SiLi_raw_sector",12,0,12,1000,0,2000);axislab(SiLisector,"Sector","Electron Energy [keV]");
		SiLipreamp= new TH2F("SiLipreamp","SiLipreamp",8,0,8,1000,0,2000);axislab(SiLisector,"Preamp","Electron Energy [keV]");
		
		SiLi_map= new TH2F("SiLi_map","SiLi_raw_map",1000,-50,50,1000,-50,50);axislab(SiLi_map,"X [mm]","Y [mm]");
        
        if(debug){
            GammaSiLipreamp= new TH3I("GammaSiLipreamp","GammaSiLipreamp;Preamp;#gamma Energy [keV];Electron Energy [keV];",8,0,8,1024,0,2048,128,0,1024);
            GammaSiLiring= new TH3I("GammaSiLiring","GammaSiLiring;Ring;#gamma Energy [keV];Electron Energy [keV];",10,0,10,1024,0,2048,128,0,1024);
            GammaSiLisector= new TH3I("GammaSiLisector","GammaSiLisector;Sector;#gamma Energy [keV];Electron Energy [keV];",12,0,12,1024,0,2048,256,0,2048);
            
            GammaSiLiChan= new TH3S("GammaSiLiChan","GammaSiLiChan;#gamma Energy [keV];Electron Energy [keV];Channel",2048,0,2048*Gbins,1024,0,2048,120,0,120);
        }

		if(SPICELimits){
			SiLi_singlespc= new TH1D("SiLi_singlesLimits","SiLi_singlesLimits",2048,0,2048);
			axislab(SiLi_singlespc,"Electron Energy [keV]");
		}

 		outfile->mkdir("SiLi/Magnets");
 		outfile->cd("SiLi/Magnets");
			if(SPICELimits){
				SiLi_magshadpc= new TH1F("SiLi_MagnetShadowLimits","SiLi_MagnetShadowPreCutLimits",2000,0,2000);axislab(SiLi_magshadpc,"Energy [keV]");
				SiLi_nagshadpc= new TH1F("SiLi_NoShadowLimits","SiLi_NoShadowLimits",2000,0,2000);axislab(SiLi_nagshadpc,"Energy [keV]");
				SiLi_shadRingpc= new TH2F("SiLi_ShadowRingLimits","SiLi_ShadowRingLimits",10,0,10,1000,0,2000);axislab(SiLi_shadRingpc,"Ring","Electron Energy [keV]");
				SiLi_nagshadRingpc= new TH2F("SiLi_NoShadowRingLimits","SiLi_NoShadowRingLimits",10,0,10,1000,0,2000);axislab(SiLi_nagshadRingpc,"Ring","Electron Energy [keV]");
			}
			SiLi_magshad= new TH1F("SiLi_MagnetShadow","SiLi_MagnetShadow",2000,0,2000);axislab(SiLi_magshad,"Energy [keV]");
			SiLi_nagshad= new TH1F("SiLi_NoShadow","SiLi_NoShadow",2000,0,2000);axislab(SiLi_nagshad,"Energy [keV]");
			SiLi_shadRing= new TH2F("SiLi_ShadowRing","SiLi_ShadowRing",10,0,10,1000,0,2000);axislab(SiLi_shadRing,"Ring","Electron Energy [keV]");
			SiLi_nagshadRing= new TH2F("SiLi_NoShadowRing","SiLi_NoShadowRing",10,0,10,1000,0,2000);axislab(SiLi_nagshadRing,"Ring","Electron Energy [keV]");
			SiLiGamma_nagshad= new TH2F("Gamma_SiLi_NoShaddow","Gamma_SiLi_NoShaddow;#gamma Energy [keV];Electron Energy [keV]",1000,0,2000,500,0,2000);
			SiLiGamma_shad= new TH2F("Gamma_SiLi_Shaddow","Gamma_SiLi_Shaddow;#gamma Energy [keV];Electron Energy [keV]",1000,0,2000,500,0,2000);
 		outfile->mkdir("SiLi/AddBack");
 		outfile->cd("SiLi/AddBack");
			SiLi_raw= new TH1F("SiLi_raw","SiLi_raw",2000,0,2000);axislab(SiLi_raw,"Energy [keV]");
			SiLi_rawm1= new TH1F("SiLi_raw_mult1","SiLi_raw_mult1",2000,0,2000);axislab(SiLi_rawm1,"Energy [keV]");
			SiLi_addback= new TH1F("SiLi_addback","SiLi_addback",2000,0,2000);axislab(SiLi_addback,"Electron Energy [keV]");
			SiLi_unaddback= new TH1F("SiLi_unaddback","SiLi_unaddback",2000,0,2000);axislab(SiLi_unaddback,"Electron Energy [keV]");
			SiLi_rejected= new TH1F("SiLi_rejected","SiLi_rejected",2000,0,2000);axislab(SiLi_rejected,"Electron Energy [keV]");
			SiLi_rejected_sum= new TH1F("SiLi_rejected_sum","SiLi_rejected_sum",2000,0,2000);axislab(SiLi_rejected_sum,"Electron Energy [keV]");
 		outfile->mkdir("SiLi/TimeAndNoise");
 		outfile->cd("SiLi/TimeAndNoise");
			SiLi_fit_time= new TH1F("SiLi_fit_time","SiLi_fit_time",512,0,256);axislab(SiLi_fit_time,"Electron Fit Time [10 ns]");
			if(UseFitCharge){
				
				SiLi_signoiseraw= new TH2F("SiLi_signoiseraw","SiLi_signoiseraw",256,0,0.2,256,0,2048);axislab(SiLi_signoiseraw,"Wave Fit Noise/Signal","Electron Energy [keV]");
				SiLi_signoise= new TH2F("SiLi_signoise","SiLi_signoise",512,0,0.2,512,0,2048);axislab(SiLi_signoise,"Wave Fit Noise/Signal","Electron Energy [keV]");
				SiLi_signoisecut= new TH2F("SiLi_signoisecut","SiLi_signoisecut",512,0,0.2,256,0,2048);axislab(SiLi_signoisecut,"Wave Fit Noise/Signal","Electron Energy [keV]");
				SiLi_smirnov= new TH2F("SiLi_smirnov","SiLi_smirnov",512,0,2E6,512,0,2048);axislab(SiLi_smirnov,"Wave Fit Smirnov parameter","Electron Energy [keV]");
				SiLi_smirnov_C= new TH2F("SiLi_smirnov_C","SiLi_smirnov_C",512,0,5000,512,0,2048);axislab(SiLi_smirnov_C,"Wave Fit Smirnov parameter/Charge","Electron Energy [keV]");
				SiLi_smirnov_Ccut= new TH2F("SiLi_smirnov_Ccut","SiLi_smirnov_Ccut",512,0,5000,256,0,2048);axislab(SiLi_smirnov_Ccut,"Wave Fit Smirnov parameter/Charge","Electron Energy [keV]");
				
			}
			SiLi_fit_timeS= new TH2F("SiLi_fit_timeS","SiLi_fit_timeS",512,0,256,120,0,120);axislab(SiLi_fit_timeS,"Electron Fit Time [10 ns]","Spice Segment");
			SiLi_fit_timeE= new TH2F("SiLi_fit_timeE","SiLi_fit_timeE",512,0,256,1000,0,2000);axislab(SiLi_fit_timeE,"Electron Fit Time [10 ns]","Electron Energy [keV]");
			SiLiSegT= new TH2F("SiLiSegTS3","SiLiSegTS3",120,0,120,512,-512,512);axislab(SiLiSegT,"SPICE Segment","SiLi-S3 #Deltat [ns]");
			if(DoDoubleElectrons){SiLieeT= new TH2F("SiLieeT","SiLieeT",120,0,120,1024,-512,512);axislab(SiLieeT,"SPICE Segment","SiLi-SiLi #Deltat [ns]");}
// 		outfile->mkdir("SiLi/rings");
// 		outfile->cd("SiLi/rings");
// 			for(int i=0;i<10;i++){
// 				stringstream ss;ss<<"SiLi_raw_ring_energy_"<<i;
// 				silirings[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,2000);axislab(silirings[i],"Electron Energy [keV]");
// 			}
		outfile->mkdir("SiLi/elements");
		outfile->cd("SiLi/elements");
			for(int i=0;i<120;i++){
				stringstream ss;ss<<"SPICE_energy_"<<i;
				silienergy[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,2000);axislab(silienergy[i],"Electron Energy [keV]");
			}
// 		if(UseFitCharge){
// 		outfile->mkdir("SiLi/fit_charge");
// 		outfile->cd("SiLi/fit_charge");
// 			for(int i=0;i<120;i++){
// 				stringstream ss;ss<<"SPICE_fit_charge_"<<i;
// 				silifitc[i] = new TH1F(ss.str().c_str(),ss.str().c_str(),1000,0,5000);axislab(silienergy[i],"Fit Charge");
// 			} 
// 		}
		outfile->cd("SiLi");
		
		SiLi_mult= new TH1D("SiLi_mult","SiLi_mult;SPICE Multiplicity;Counts",20,0,20);
		SiLiAdd_mult= new TH1D("SiLiAdd_mult","SiLiAdd_mult;SPICE Addback Multiplicity;Counts",20,0,20);
	outfile->cd();
	}
	
	TH1 *Gamma_no_add,*Gamma_suppressed,*Gamma_singles_no_bgo,*Tig_mult,*Tig_rawmult,*Tig_bgomult;
	TH2F *Gamma_Gamma_no_add,*Gamma_Core,*Gamma_Core_Charge,*TigressHitMap,*TigressHitMapLow,*TigressETheta,*Tig_S3_t;
	TH3F *TigressHitMap3,*TigressEETheta,*TigressEEdTheta;
	TH1 *Gamma_widerange;

	
	outfile->mkdir("Tigress");
	outfile->cd("Tigress");
		Gamma_widerange= new TH1D("Gamma_widerange","Gamma_widerange",8192*Gbins,0,8192*Gbins);axislab(Gamma_widerange,"#gamma Energy [keV]");	
		Gamma_no_add= new TH1F("Gamma_no_add","Gamma_no_add",2000,6,2006);axislab(Gamma_no_add,"#gamma Energy [keV]");
		Gamma_Gamma_no_add= new TH2F("Gamma_Gamma_no_add","Gamma_Gamma_no_add",512,6,2000*Gbins,512,6,2000*Gbins+6);axislab(Gamma_Gamma_no_add,"#gamma Energy [keV]","#gamma Energy [keV]");
		Gamma_Core= new TH2F("Gamma_Core","Gamma_Core",64,0,64,2048*Gbins,0,2048*Gbins);axislab(Gamma_Core,"Core No.","#gamma Energy [keV]");
		Gamma_Core_Charge= new TH2F("Gamma_Core_Charge","Gamma_Core_Charge",64,0,64,4096,0,40000);axislab(Gamma_Core_Charge,"Core No.","#gamma Charge");
		TigressHitMap3= new TH3F("TigressHitMap3","TigressHitMap3",128,-200,200,128,-200,200,128,-200,200);axislab(TigressHitMap3,"X [mm]","Y [mm]","Z [mm]");
		TigressHitMap= new TH2F("TigressHitMap","TigressHitMap",200,-TMath::Pi(),TMath::Pi(),200,0,TMath::Pi());axislab(TigressHitMap,"#phi [rad]","#theta [rad]");
		TigressHitMapLow= new TH2F("TigressHitMapLow","TigressHitMapLow",100,-TMath::Pi(),TMath::Pi(),100,0,TMath::Pi());axislab(TigressHitMapLow,"#phi [rad]","#theta [rad]");
		Gamma_suppressed= new TH1F("Gamma_suppressed","Gamma_suppressed",2048*Gbins,0,2048*Gbins);axislab(Gamma_suppressed,"#gamma Energy [keV]");
		Gamma_singles_no_bgo= new TH1F("Gamma_singles_no_bgo","Gamma_singles_no_bgo",2048*Gbins,0,2048*Gbins);axislab(Gamma_singles_no_bgo,"#gamma Energy [keV]");
		

		outfile->mkdir("Tigress/Angles");
		outfile->cd("Tigress/Angles");
			TigressETheta= new TH2F("TigressETheta","TigressETheta",1024*Gbins,0,2048*Gbins,12,0,12);axislab(TigressETheta,"#gamma Energy [keV]","Segment Theta Ring");
			TigressEETheta= new TH3F("TigressEETheta","TigressEETheta",1024,0,2048*Gbins,1024,0,2048*Gbins,12,0,12);axislab(TigressEETheta,"Gating-#gamma Energy [keV]","#gamma Energy [keV]","Segment Theta Ring");
			TigressEEdTheta= new TH3F("GammaGammaSeparation","GammaGammaSeparation",512,0,2048*Gbins,512,0,2048*Gbins,64,0,3.15);axislab(TigressEEdTheta,"#gamma Energy [keV]","#gamma Energy [keV]","Separation [rad]");
		outfile->cd("Tigress");
		
		Tig_mult= new TH1D("Tig_mult","Tig_mult;Tigress Multiplicity;Counts",20,0,20);
		Tig_rawmult= new TH1D("Tig_rawmult","Tig_rawmult;Tigress Multiplicity;Counts",20,0,20);
		Tig_bgomult= new TH1D("Tig_bgomult","Tig_bgomult;Tigress Multiplicity;Counts",20,0,20);
		Tig_S3_t= new TH2F("Tig_S3_t","Tig_S3_t",1024,-512,512,64,0,64);axislab(Tig_S3_t,"Gamma S3 #Deltat [ns]","Crystal Number");
	outfile->cd();
	
	
	TH1F *GG_t, *ee_t, *SiLi_S3_t, *SiLi_S3_twide, *Gamma_S3_t, *Gamma_S3_twide, *Gamma_SiLi_t, *Gamma_SiLi_twide, *GG_tgate, *ee_tgate, *SiLi_S3_tgate, *Gamma_S3_tgate, *Gamma_SiLi_tgate, *S3_rf, *SiLi_rf, *Gamma_rf;
	TH1F *S3_rfgate, *SiLi_rfgate, *Gamma_rfgate;
	TH2F *SiLi_S3_t2, *Gamma_S3_t2, *Gamma_S3_walk, *Gamma_SiLi_t2B, *Gamma_SiLi_t2, *ESiLi_rf2, *EGamma_rf2, *S3_SiLi_RF, *Gamma_SiLi_RF, *Gamma_S3_RF, *S3_SiLi_RFgated, *Gamma_SiLi_RFgated, *Gamma_S3_RFgated;
	TH3F *Gamma_Gamma_t3,*Gamma_SiLi_t3, *S3_SiLi_RFe, *Gamma_S3_RFe, *Gamma_SiLi_SiliRF;
	TH1F* Gamma_RFgated;
	TH2F* Gamma_S3_t2RF,*ESiLi_rfcycle,*EGamma_rfcycle;

	
	outfile->mkdir("CoinTimegates");
	outfile->cd("CoinTimegates");
		outfile->mkdir("CoinTimegates/Time1D");
		outfile->cd("CoinTimegates/Time1D");
			 GG_t = new TH1F("GG_t","GG_t",1024,-512,512);axislab(GG_t,"#Deltat [ns]");
			 Gamma_S3_t= new TH1F("Gamma_S3_t","Gamma_S3_t",1024,-512,512);axislab(Gamma_S3_t,"#Deltat [ns]");
			 Gamma_S3_twide= new TH1F("Gamma_S3_twide","Gamma_S3_twide",2048,-10240,10240);axislab(Gamma_S3_twide,"#Deltat [ns]");
			if(DS){
				if(DoDoubleElectrons){ee_t = new TH1F("ee_t","ee_t",1024,-1024,1024);axislab(ee_t,"#Deltat [ns]");}
				SiLi_S3_t= new TH1F("SiLi_S3_t","SiLi_S3_t",1024,-1024,1024);axislab(SiLi_S3_t,"#Deltat [ns]");
				SiLi_S3_twide= new TH1F("SiLi_S3_twide","SiLi_S3_twide",2048,-10240,10240);axislab(SiLi_S3_twide,"#Deltat [ns]");
				Gamma_SiLi_t= new TH1F("Gamma_SiLi_t","Gamma_SiLi_t",1024,-512,512);axislab(Gamma_SiLi_t,"#Deltat [ns]");
				Gamma_SiLi_twide= new TH1F("Gamma_SiLi_twide","Gamma_SiLi_twide",2048,-10240,10240);axislab(Gamma_SiLi_twide,"#Deltat [ns]");
			}
		outfile->mkdir("CoinTimegates/Gates");
		outfile->cd("CoinTimegates/Gates");
			 GG_tgate = new TH1F("GG_tgate","GG_tgate",1024,-512,512);axislab(GG_tgate,"#Deltat [ns]");
			 Gamma_S3_tgate= new TH1F("Gamma_S3_tgate","Gamma_S3_tgate",1024,-512,512); axislab(Gamma_S3_tgate,"#Deltat [ns]");
			if(DS){
				if(DoDoubleElectrons){ee_tgate = new TH1F("ee_tgate","ee_tgate",1024,-1024,1024);axislab(ee_tgate,"#Deltat [ns]");}
				SiLi_S3_tgate= new TH1F("SiLi_S3_tgate","SiLi_S3_tgate",1024,-1024,1024);axislab(SiLi_S3_tgate,"#Deltat [ns]");
				Gamma_SiLi_tgate= new TH1F("Gamma_SiLi_tgate","Gamma_SiLi_tgate",1024,-512,512); axislab(Gamma_SiLi_tgate,"#Deltat [ns]");
			}
		outfile->mkdir("CoinTimegates/TimeEnergy");
		outfile->cd("CoinTimegates/TimeEnergy");
			Gamma_S3_t2= new TH2F("Gamma_S3_t2","Gamma_S3_t2",2048,0,2048*Gbins,1024,-2048,2048);axislab(Gamma_S3_t2,"#gamma Energy [keV]","#Deltat [ns]");
			if(TigressTimeWalk){Gamma_S3_walk= new TH2F("Gamma_S3_walk","Gamma_S3_walk;#gamma Energy [keV];#Deltat [ns]",2048,0,2048,1024,-2048,2048);}
			Gamma_Gamma_t3= new TH3F("Gamma_Gamma_t3","Gamma_Gamma_t3",512,0,2048*Gbins,512,0,2048*Gbins,64,-512,512);axislab(Gamma_Gamma_t3,"#gamma Energy [keV]","#gamma Energy [keV]","#Deltat [ns]");
			if(DS) {SiLi_S3_t2= new TH2F("SiLi_S3_t2","SiLi_S3_t2",2048,0,2048*Gbins,1024,-1024,1024);axislab(SiLi_S3_t2,"Electron Energy [keV]","#Deltat [ns]");
			Gamma_SiLi_t2= new TH2F("Gamma_SiLi_t2","Gamma_SiLi_t2",2048,0,2048*Gbins,1024,-1024,1024);axislab(Gamma_SiLi_t2,"#gamma Energy [keV]","#Deltat [ns]");
			Gamma_SiLi_t2B= new TH2F("Gamma_SiLi_t2B","Gamma_SiLi_t2B",1024,0,2048*Gbins,1024,-1024,1024);axislab(Gamma_SiLi_t2B,"Electron Energy [keV]","#Deltat [ns]");
			Gamma_SiLi_t3= new TH3F("Gamma_SiLi_t3","Gamma_SiLi_t3",512,0,2048*Gbins,512,0,2048*Gbins,256,-1024,1024);axislab(Gamma_SiLi_t3,"#gamma Energy [keV]","Electron Energy [keV]","#Deltat [ns]");}
	outfile->cd();
	
	outfile->mkdir("RFTimegates");
	outfile->cd("RFTimegates");
		outfile->mkdir("RFTimegates/Time1D");
		outfile->cd("RFTimegates/Time1D");
			S3_rf = new TH1F("S3_rf","S3_rf",1024,-1024,1024);axislab(S3_rf,"Cfd-rf #Deltat [ns]");
			Gamma_rf= new TH1F("Gamma_rf","Gamma_rf",1024,-1024,1024);axislab(Gamma_rf,"Cfd-rf #Deltat [ns]");
			if(DS){ SiLi_rf= new TH1F("SiLi_rf","SiLi_rf",1024,-2048,2048);axislab(SiLi_rf,"Electron T-rf #Deltat [ns]");}
				
			if(S3RFgating){S3_rfgate = new TH1F("S3_rfgate","S3_rfgate",1024,-1024,1024);axislab(S3_rfgate,"Cfd-rf #Deltat [ns]");}
			if(GammaRFgating){ Gamma_rfgate= new TH1F("Gamma_rfgate","Gamma_rfgate",1024,-1024,1024);axislab(Gamma_rfgate,"Cfd-rf #Deltat [ns]");}
			if(DS&&SiliRFgating){ SiLi_rfgate= new TH1F("SiLi_rfgate","SiLi_rfgate",1024,-2048,2048);axislab(SiLi_rfgate,"Electron T-rf #Deltat [ns]");}
		
		outfile->mkdir("RFTimegates/TimeVsTime");
		outfile->cd("RFTimegates/TimeVsTime");
			if(DS){
				S3_SiLi_RF= new TH2F("S3_SiLi_RF","S3_SiLi_RF",512,-1024,1024,512,-1024,1024);axislab(S3_SiLi_RF,"S3 Cfd-rf #Deltat [ns]","Electron T-rf #Deltat [ns]");
				S3_SiLi_RFgated= new TH2F("S3_SiLi_RFgated","S3_SiLi_RFgated",512,-1024,1024,512,-1024,1024);axislab(S3_SiLi_RFgated,"S3 Cfd-rf #Deltat [ns]","Electron T-rf #Deltat [ns]");
				Gamma_SiLi_RF= new TH2F("Gamma_SiLi_RF","Gamma_SiLi_RF",512,-1024,1024,512,-1024,1024);axislab(Gamma_SiLi_RF,"#gamma Cfd-rf #Deltat [ns]","Electron T-rf #Deltat [ns]");
				Gamma_SiLi_RFgated= new TH2F("Gamma_SiLi_RFgated","Gamma_SiLi_RFgated",512,-1024,1024,512,-1024,1024);axislab(Gamma_SiLi_RFgated,"#gamma Cfd-rf #Deltat [ns]","Electron T-rf #Deltat [ns]");
			}			
				
			Gamma_S3_RF= new TH2F("Gamma_S3_RF","Gamma_S3_RF",512,-1024,1024,512,-1024,1024);axislab(Gamma_S3_RF,"S3 Cfd-rf #Deltat [ns]","#gamma Cfd-rf #Deltat [ns]");	
			Gamma_S3_RFgated= new TH2F("Gamma_S3_RFgated","Gamma_S3_RFgated",512,-1024,1024,512,-1024,1024);axislab(Gamma_S3_RFgated,"S3 Cfd-rf #Deltat [ns]","#gamma Cfd-rf #Deltat [ns]");			 

		outfile->mkdir("RFTimegates/vsEnergy");
		outfile->cd("RFTimegates/vsEnergy");
			if(GammaRFgating){Gamma_RFgated= new TH1F("Gamma_RFgated","Gamma_RFgated",2000,0,2000);axislab(Gamma_RFgated,"#gamma Energy [keV]");}		
			EGamma_rf2= new TH2F("EGamma_rf2","EGamma_rf2",2048,0,2048*Gbins,1024,-1024,1024);axislab(EGamma_rf2,"#gamma Energy [keV]","Cfd-rf #Deltat [ns]");
			if(debug){
                EGamma_rfcycle=new TH2F("EGamma_rfcycle","EGamma_rfcycle",1024,0,2048*Gbins,168,0,TRFperiodps*2);axislab(EGamma_rfcycle,"#gamma Energy [keV]","Cfd-rf #Deltat [ps]");
            }
            
			Gamma_S3_RFe= new TH3F("Gamma_S3_RFe","Gamma_S3_RFe",128,-1024,1024,128,-1024,1024,512,0,1024*Gbins);axislab(Gamma_S3_RFe,"S3 Cfd-rf #Deltat [ns]","#gamma Cfd-rf #Deltat [ns]","#gamma Energy [keV]");			 

			if(GammaRFgating||S3RFgating){
				Gamma_S3_t2RF= new TH2F("Gamma_S3_t2RFg","Gamma_S3_t2RFg",2048,0,2048*Gbins,1024,-2048,2048);axislab(Gamma_S3_t2RF,"#gamma Energy [keV]","#Deltat [ns]");
			}	
			
			if(DS){
				ESiLi_rf2= new TH2F("ESiLi_rf2","ESiLi_rf2",2048,0,2048,1024,-2048,2048);axislab(ESiLi_rf2,"Electron Energy [keV]","Electron T-rf #Deltat [ns]");
				S3_SiLi_RFe= new TH3F("S3_SiLi_RFe","S3_SiLi_RFe",128,-1024,1024,128,-1024,1024,256,0,1024);axislab(S3_SiLi_RFe,"S3 Cfd-rf #Deltat [ns]","Electron T-rf #Deltat [ns]","Electron Energy [keV]");
				
                
                if(debug){
                    ESiLi_rfcycle=new TH2F("ESiLi_rfcycle","ESiLi_rfcycle",1024,0,2048,168,0,TRFperiodps*2);axislab(ESiLi_rfcycle,"Electron Energy [keV]","Electron T-rf #Deltat [ps]");
//                     Gamma_SiLi_SiliRF= new TH3F("Gamma_SiLi_SiliRF","Gamma_SiLi_SiliRF;gamma Energy [keV];Electron Energy [keV];Electron T-rf #Deltat [ps]",512,0,1024*Gbins,256,0,1024,168,0,TRFperiodps*2);
                }
			}

	outfile->cd();
	

	
	TH1F *eventrate,*eventratehours,*stampfail,*runtime,*runtimeraw,*timeadda,*fileentry,*filetime; 
	TH2F *runtime_sili,*runtime_silinoise,*runtime_gamma,*eventN_sili,*eventN_silinoise,*eventN_gamma,*fileN_sili,*fileN_silinoise,*fileN_gamma;
	TH3F *eventN_gammagamma ,*eventN_gammasili,*runtime_gammagamma,*runtime_gammasili;
	TH1F *monitortotal; 
	TH2F *monitor_runtime;

	TH1F *S3fragrate,*silifragrate,*gammafragrate; 

	
	outfile->mkdir("RunTime");
	outfile->cd("RunTime");
	
		float hoursrange=(float)tstamprange/(float)timestamp_hour;
	
		if(AddMonitor){
			monitortotal = new TH1F("MonitorTotal","MonitorTotal;Energy [keV]",2048,0,2048);
			monitor_runtime= new TH2F("Monitor_Runtime","Monitor_Runtime;Time [hours];Energy [keV]",1024,0,hoursrange,1024,0,2048);
		}
		
	
// 		eventrate = new TH1F("eventrate","eventrate",10000,0,hoursrange);axislab(eventrate,"Time [10 ns]");
		eventratehours = new TH1F("eventrate","eventrate",10000,0,hoursrange);axislab(eventratehours,"Time [hours]");
		runtime = new TH1F("runtime","runtime",10000,0,(long)(10000*(nentries/9999)));axislab(runtime,"Entry No.","Time [hours]");
		runtimeraw = new TH1F("runtimeraw","runtimeraw",10000,0,(long)(10000*(nentries/9999)));axislab(runtimeraw,"Entry No.","Time [hours]");
		timeadda = new TH1F("timeadda","timeadda",10000,0,(long)(10000*(nentries/9999)));axislab(timeadda,"Entry No.","Time [hours]");
		stampfail=new TH1F("stampfail","stampfail",10000,0,(long)(10000*(nentries/9999)));axislab(stampfail,"Entry No.","TStamp Fails/bin");
		
		runtime_gamma = new TH2F("runtime_gamma","runtime_gamma",1024,0,hoursrange,1024,0,2048);axislab(runtime_gamma,"Time [hours]","#gamma Energy [keV]");
// 		runtime_gammagamma = new TH3F("runtime_gammagamma","runtime_gammagamma",128,0,hoursrange,1024,0,2048,1024,0,2048);axislab(runtime_gammagamma,"Time [hours]","#gamma Energy [keV]","#gamma Energy [keV]");
		eventN_gamma = new TH2F("eventN_gamma","eventN_gamma",1024,0,(long)(1024*(nentries/1023)),1024,0,2048);axislab(eventN_gamma,"Entry No.","#gamma Energy [keV]");
// 		eventN_gammagamma = new TH3F("eventN_gammagamma","eventN_gammagamma",128,0,(long)(128*(nentries/127)),1024,0,2048,1024,0,2048);axislab(eventN_gammagamma,"Entry No.","#gamma Energy [keV]","#gamma Energy [keV]");
		fileN_gamma = new TH2F("fileN_gamma","fileN_gamma",filelist.size(),0,filelist.size(),1024,0,2048);axislab(fileN_gamma,"Entry No.","#gamma Energy [keV]");

		 
// 		int fileN=DataChain->GetListOfFiles()->GetSize();		 
		 
		if(DS){
		 runtime_sili = new TH2F("runtime_sili","runtime_sili",1024,0,hoursrange,1024,50,2098);axislab(runtime_sili,"Time [hours]","Electron Energy [keV]");
// 		 runtime_silinoise = new TH2F("runtime_silinoise","runtime_silinoise",1024,0,hoursrange,1024,0,2048);axislab(runtime_silinoise,"Time [hours]","Electron Energy [keV]");
// 		 runtime_gammasili = new TH3F("runtime_gammasili","runtime_gammasili",128,0,hoursrange,1024,0,2048,1024,0,2048);axislab(runtime_gammasili,"Time [hours]","#gamma Energy [keV]","Electron Energy [keV]");
		 eventN_sili = new TH2F("eventN_sili","eventN_sili",1024,0,(long)(1024*(nentries/1023)),1024,50,2098);axislab(eventN_sili,"Entry No.","Electron Energy [keV]");
// 		 eventN_silinoise = new TH2F("eventN_silinoise","eventN_silinoise",1024,0,(long)(1024*(nentries/1023)),1024,0,2048);axislab(eventN_silinoise,"Entry No.","Electron Energy [keV]");
// 		 eventN_gammasili = new TH3F("eventN_gammasili","eventN_gammasili",128,0,(long)(128*(nentries/127)),1024,0,2048,1024,0,2048);axislab(eventN_gammasili,"Entry No.","#gamma Energy [keV]","Electron Energy [keV]");
		 fileN_sili = new TH2F("fileN_sili","fileN_sili",filelist.size(),0,filelist.size(),1024,50,2098);axislab(fileN_sili,"File No.","Electron Energy [keV]");
// 		 fileN_silinoise = new TH2F("fileN_silinoise","fileN_silinoise",filelist.size(),0,filelist.size(),1024,0,2048);axislab(fileN_silinoise,"File No.","Electron Energy [keV]");
		}
		 
		 
		S3fragrate = new TH1F("S3fragrate","S3fragrate;Entry No.;Frags/Bin",10000,0,(long)(10000*(nentries/9999)));
		gammafragrate = new TH1F("gammafragrate","gammafragrate;Entry No.;Frags/Bin",10000,0,(long)(10000*(nentries/9999)));
		if(DS)silifragrate = new TH1F("silifragrate","silifragrate;Entry No.;Frags/Bin",10000,0,(long)(10000*(nentries/9999)));
		S3fragrate->SetBit(TH1::kIsNotW);
		gammafragrate->SetBit(TH1::kIsNotW);
		if(DS)silifragrate->SetBit(TH1::kIsNotW);
		 
		fileentry = new TH1F("fileentry","fileentry",filelist.size(),0,filelist.size());axislab(fileentry,"","EOF Entry No.");
		filetime = new TH1F("filetime","filetime",filelist.size(),0,filelist.size());axislab(filetime,"","SOF tstamp raw");
		for(unsigned int i=0;i<filelist.size();i++){
			fileentry->SetBinContent(i+1,fileentriessum[i]);
			fileentry->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			filetime->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			if(DS) fileN_sili->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
// 			if(DS) fileN_silinoise->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
			fileN_gamma->GetXaxis()->SetBinLabel(i+1,filelist[i].c_str());
		}
		
		TGraph* sorttime=new TGraph();
		TGraph* guessend=new TGraph();
	outfile->cd();

	TH3* S3IDgamma;
	TH3* S3IDsili;
	TH3* S3IDgammacoinc;
	
	vector< TH2F* >  S3particleGated;
	vector< TH2F* >  GUncorrectedring,Gcorrectedring;
	vector< TH1F* >  GammaPG,SiLiPG,GUnshifted,GUncorrected;
	vector< TH1* >  GammaPGRF,SiLiPGRF,PGmult,GammaGTime,SiLiGTime,GammaGdTime,SiLiGdTime;
	vector< TH2* >  GammaEPG,SiLiEPG,PGmultGamma,PGmultSili;
	vector< TH2F* >  GammaSiLiPG,GammaGammaPG;
	vector< TH2F* >  TigressEThetaPG;
	vector< TH3F* >  TigressEEThetaPG,TigressEEdThetaPG;
	vector<	vector< TH2F* > >  TigressDopplerAngle;
	vector< TH3F* >  TigressDopplerTheta;
	vector< TH3* >  GGGP,GSgt;
	vector< TH2F* >  GSPT,GSPt,SST;
	vector< TH2F* >  GammaCoreUnshifted;
	vector< TH2F* >  GammaCoreCorrected;
	
	vector< TH1Efficiency* > GammaPGeffcor;
	vector< TH2Efficiency* > GammaGammaPGeffcor;
	
	vector< TH2* >  RingGammaSingles;
	vector<	vector< TH1F* > >  RingGroupGammaSingles;
	vector<	vector< TH1Efficiency* > >  RingGroupGammaEff;
	vector<	TH2Efficiency* >  RingGammaEff;
	
	
	if(ParticleGate.size()>0||debug){
		outfile->mkdir("ParticleGates");
		outfile->cd("ParticleGates");
			
			if(debug){
				if(Telescope){
					S3IDgammacoinc= new TH3F("S3IDgammacoinc","S3IDgammacoinc;dE/dx [arb.];Total Energy [keV];#gamma Energy [keV]",50,0,control[S3EnergyLimit]*.4,50,0,control[S3EnergyLimit],1000,0,2000);
					S3IDgamma= new TH3F("S3IDgamma","S3IDgamma;dE/dx [arb.];Total Energy [keV];#gamma Mult.",500,0,control[S3EnergyLimit]*.4,500,0,control[S3EnergyLimit],5,0,5);
					if(DS){S3IDsili= new TH3F("S3IDsili","S3IDsili;dE/dx [arb.];Total Energy [keV];Electron Mult.",500,0,control[S3EnergyLimit]*.4,500,0,control[S3EnergyLimit],5,0,5);}
				}else{
					S3IDgammacoinc= new TH3F("S3IDgammacoinc","S3IDgammacoinc;Theta [rad.];Total Energy [keV];#gamma Energy [keV]",50,0.2,1.2,50,0,control[S3EnergyLimit],1000,0,2000);
					S3IDgamma= new TH3F("S3IDgamma","S3IDgamma;Theta [rad.];Total Energy [keV];#gamma Mult.",500,0.2,1.2,500,0,control[S3EnergyLimit],5,0,5);
					if(DS){S3IDsili= new TH3F("S3IDsili","S3IDsili;Theta [rad.];Total Energy [keV];Electron Mult.",500,0.2,1.2,500,0,control[S3EnergyLimit],5,0,5);}
				}
			}
			
			for(int n=0;n<ParticleGate.size();n++){
				string t=ParticleGate[n].title;
				string tf="ParticleGates/"+t;
				
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
					TH1F* Gammapg= new TH1F(("Gamma_"+t).c_str(),("Gamma_"+t).c_str(),2500*Gbins,0,2500*Gbins);axislab(Gammapg,"#gamma Energy [keV]");
					GammaPG.push_back(Gammapg);
					if(GammaRFgating||S3RFgating){
						Gammapg= new TH1F(("Gamma_"+t+"_RFgated").c_str(),("Gamma_"+t+"_RFgated").c_str(),2500*Gbins,0,2500*Gbins);axislab(Gammapg,"#gamma Energy [keV]");
						GammaPGRF.push_back(Gammapg);
					}
					
					TH2F* GammaEpg= new TH2F(("GammaE_"+t).c_str(),("GammaE_"+t).c_str(),1024,0,2048*Gbins,500,0,control[S3EnergyLimit]);axislab(GammaEpg,"#gamma Energy [keV]","Total Silicon Energy [keV]");
					GammaEPG.push_back(GammaEpg);
					
					TH2F* GammaGammapg= new TH2F(("GammaGamma_"+t).c_str(),("GammaGamma_"+t).c_str(),1024*Gbins,0,2048*Gbins,1024*Gbins,0,2048*Gbins);axislab(GammaGammapg,"#gamma Energy [keV]","#gamma Energy [keV]");
					GammaGammaPG.push_back(GammaGammapg);
					
					if(debugX){
						TH3* ggg = new TH3S(("GGG_"+t).c_str(),("GGG_"+t).c_str(),1024,0,2048*Gbins,512,0,2048*Gbins,512,0,2048*Gbins);axislab(ggg,"#gamma Energy [keV]","#gamma Energy [keV]","#gamma Energy [keV]");
						GGGP.push_back(ggg);
						if(DS){
                            
                            TH2F* gspt= new TH2F(("GSPT_"+t).c_str(),("GSPT_promptgamma_"+t).c_str(),1024,0,4096*Gbins,512,-512,512);axislab(gspt,"#gamma+SiLi Energy [keV]","#gamma SiLi #Deltat [ns]");
							GSPT.push_back(gspt);
                            
                            gspt= new TH2F(("GSPt_"+t).c_str(),("GSPt_promptgamma_"+t+";#gamma+SiLi Energy [keV];S3 SiLi #Deltat [ns];").c_str(),1024,0,4096*Gbins,512,-512,512);
							GSPt.push_back(gspt);   
                            
                            TH3 *gsgt= new TH3I(("GSgt_"+t).c_str(),("GSgt_promptgamma_"+t+";#gamma Energy [keV];SiLi Energy [keV];S3 SiLi #Deltat [ns];").c_str(),512,0,2048*Gbins,256,0,2048,256,-512,512);
							GSgt.push_back(gsgt);     
                            
						}
					}
					
					Gammapg= new TH1F(("GammaGTime_"+t).c_str(),("GammaGTime_"+t).c_str(),1024,-512,512);axislab(Gammapg,"#Deltat [ns]");
					GammaGTime.push_back(Gammapg);					
					Gammapg= new TH1F(("GammaGdTime_"+t).c_str(),("GammaGdTime_"+t).c_str(),1024,-512,512);axislab(Gammapg,"#Deltat [ns]");
					GammaGdTime.push_back(Gammapg);

					if(DS){
						TH1F* SiLipg= new TH1F(("SiLi_"+t).c_str(),("SiLi_"+t).c_str(),2000,0,2000);axislab(SiLipg,"Electron Energy [keV]");
						SiLiPG.push_back(SiLipg);
						
						if(SiliRFgating||S3RFgating){
							SiLipg= new TH1F(("SiLi_"+t+"_RFcycgated").c_str(),("SiLi_"+t+"_RFcycgated").c_str(),2000,0,2000);axislab(SiLipg,"Electron Energy [keV]");
							SiLiPGRF.push_back(SiLipg);
						}
						TH2F* SiLiEpg= new TH2F(("SiLiE_"+t).c_str(),("SiLiE_"+t).c_str(),1000,0,2000,500,0,control[S3EnergyLimit]);axislab(SiLiEpg,"Electron Energy [keV]","Total Silicon Energy [keV]");
						SiLiEPG.push_back(SiLiEpg);
						TH2F* GammaSilipg= new TH2F(("GammaSili_"+t).c_str(),("GammaSili_"+t).c_str(),500,0,1500,500,0,1500);axislab(GammaSilipg,"#gamma Energy [keV]","Electron Energy [keV]");
						GammaSiLiPG.push_back(GammaSilipg);
						SiLipg= new TH1F(("SiLiGTime_"+t).c_str(),("SiLiGTime_"+t).c_str(),1024,-512,512);axislab(SiLipg,"#Deltat [ns]");
						SiLiGTime.push_back(SiLipg);					
						SiLipg= new TH1F(("SiLiGdTime_"+t).c_str(),("SiLiGdTime_"+t).c_str(),1024,-512,512);axislab(SiLipg,"#Deltat [ns]");
						SiLiGdTime.push_back(SiLipg);
                        if(debugX){
                            TH2F* sst= new TH2F(("SST_"+t).c_str(),("SST_"+t).c_str(),1024,0,2048*Gbins,512,-512,512);axislab(sst,"SiLi Energy [keV]","Sili-S3 #Deltat [ns]");
							SST.push_back(sst);
                        }
					}
					
					TH2F* S3Pgtd;
					if(Telescope)S3Pgtd= new TH2F(("S3particleGated"+t).c_str(),("S3particleGated"+t+";Total Energy [keV];dE/dx [arb.]").c_str(),500,0,control[S3EnergyLimit],500,0,control[S3EnergyLimit]*.4);
					else S3Pgtd = new TH2F(("S3particleGated"+t).c_str(),("S3particleGated"+t+";Total Energy [keV];Theta [rad]").c_str(),500,0,control[S3EnergyLimit],100,0.2,1.2);
					S3particleGated.push_back(S3Pgtd);
					
					
					if(GammaEfficiency){
						stringstream histname;
						histname<<"GammaEfficiency_"<<t;
						GammaPGeffcor.push_back(new TH1Efficiency(histname.str().c_str(),(histname.str()+";#gamma Energy [keV];Counts/keV").c_str(),2500*Gbins,0,2500*Gbins,GammaEfficiencyGraph,GammaEfficiencyError));
					}
					
					if(GammaGammaEff){
						stringstream histname;
						histname<<"GammaGammaEfficiency_"<<t;
						GammaGammaPGeffcor.push_back(new TH2Efficiency(histname.str().c_str(),(histname.str()+";#gamma Energy [keV];#gamma Energy [keV]").c_str(),1024*Gbins,0,2048*Gbins,1024*Gbins,0,2048*Gbins,control[GammaGammaFrac],GammaEfficiencyGraph,GammaEfficiencyError));	
					}
					
					outfile->mkdir((tf+"/GammaAngle").c_str());
					outfile->cd((tf+"/GammaAngle").c_str());
					
						TH2F* TigressEThetapg= new TH2F(("GammaTheta_"+t).c_str(),("GammaETheta_"+t).c_str(),1024*Gbins,0,2048*Gbins,12,0,12);axislab(TigressEThetapg,"#gamma Energy [keV]","Segment Theta Ring");
						TigressEThetaPG.push_back(TigressEThetapg);
						
						TH3F* TigressEEThetapg= new TH3F(("GammaThetaGamma_"+t).c_str(),("GammaThetaGamma_"+t).c_str(),1024,0,2048*Gbins,1024,0,2048*Gbins,12,0,12);axislab(TigressEEThetapg,"Gating-#gamma Energy [keV]","#gamma Energy [keV]","Segment Theta Ring");
						TigressEEThetaPG.push_back(TigressEEThetapg);
						
						TH3F* TigressEEdThetapg= new TH3F(("GammaGammaAngSep_"+t).c_str(),("GammaGammaAngSep_"+t).c_str(),512,0,2048*Gbins,512,0,2048*Gbins,64,0,3.15);axislab(TigressEEdThetapg,"#gamma Energy [keV]","#gamma Energy [keV]","Separation [rad]");
						TigressEEdThetaPG.push_back(TigressEEdThetapg);
						
					outfile->cd(tf.c_str());

					if(MultiParticles){
						outfile->mkdir((tf+"/Mult").c_str());
						outfile->cd((tf+"/Mult").c_str());
					}
					
						TH1D* pgmult= new TH1D(("Multiplicity_"+t).c_str(),("Multiplicity_"+t+";"+t+" Multiplicity").c_str(),10,0,10);
						PGmult.push_back(pgmult);
						
						if(MultiParticles){
							TH2F* pgmultg= new TH2F(("Multiplicity_"+t+"_v_gamma").c_str(),("Multiplicity_"+t+"_v_gamma;"+t+" Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2048*Gbins,0,2048*Gbins);
							PGmultGamma.push_back(pgmultg);
							if(DS){
								TH2F* pgmults= new TH2F(("Multiplicity_"+t+"_v_sili").c_str(),("Multiplicity_"+t+"_v_sili;"+t+" Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2048*Gbins,0,2048*Gbins);
								PGmultSili.push_back(pgmults);
							}
						}
							
					outfile->cd(tf.c_str());

					TigressDopplerAngle.push_back(vector< TH2F* >());
					
					if(ParticleGate[n].use_beta){
						outfile->mkdir((tf+"/Doppler").c_str());
						outfile->cd((tf+"/Doppler").c_str());
						
							TH1F* gshift= new TH1F(("GammaUnshifted_"+t).c_str(),("GammaUnshifted_"+t).c_str(),2500*Gbins,0,2500*Gbins);axislab(gshift,"#gamma Energy [keV]");
							GUnshifted.push_back(gshift);
							
							if(debug){
								TH2F* gcorun= new TH2F(("Gamma_Core_Unshifted"+t).c_str(),("Gamma_Core_Unshifted"+t).c_str(),64,0,64,2048,0,2048*Gbins);axislab(gcorun,"Core No.","#gamma Energy [keV]");
								GammaCoreUnshifted.push_back(gcorun);
							}
							
							TH1F* guncor= new TH1F(("GammaUncorrected_"+t).c_str(),("GammaUncorrected_"+t).c_str(),2500*Gbins,0,2500*Gbins);axislab(guncor,"#gamma Energy [keV]");
							GUncorrected.push_back(guncor);
							
							if(!k)k=1;//MultiS3 variable
							
							TH2F* guncorR= new TH2F(("GammaUncorrectedRing_"+t).c_str(),("GammaUncorrectedRing_"+t).c_str(),1024,0,2048*Gbins,24*k,0,24*k);axislab(guncorR,"#gamma Energy [keV]","S3 Ring Number");
							GUncorrectedring.push_back(guncorR);

							TH2F* gcorR= new TH2F(("GammaCorrectedRing_"+t).c_str(),("GammaCorrectedRing_"+t).c_str(),1024,0,2048*Gbins,24*k,0,24*k);axislab(gcorR,"#gamma Energy [keV]","S3 Ring Number");
							Gcorrectedring.push_back(gcorR);
						
							if(debug){
								TH2F* gcorcor= new TH2F(("Gamma_Core_Corrected"+t).c_str(),("Gamma_Core_Corrected"+t).c_str(),64,0,64,2048,0,2048*Gbins);axislab(gcorcor,"Core No.","#gamma Energy [keV]");
								GammaCoreCorrected.push_back(gcorcor);
							}
							
							if(debug){
								TH3F* theg= new TH3F(("TigressDopplerAngle_"+t).c_str(),("TigressDopplerAngle_"+t+";#gamma Energy [keV];#theta_{doppler} [rad];#theta_{recoil} [rad];").c_str(),1024,0,2048*Gbins,100,0,TMath::Pi(),100,0,TMath::Pi()/2.);
								TigressDopplerTheta.push_back(theg);
								
								// for(int r=0;r<24*k;r++){
								// 	stringstream subname;subname<<"TigressDopplerAngle_"<<t<<"_Ring"<<r;
								// 	TH2F* Tigressdopplerangle= new TH2F(subname.str().c_str(),subname.str().c_str(),100,0,TMath::Pi(),2000,0,2000);axislab(Tigressdopplerangle,"#theta [rad]","#gamma Energy [keV]");
								// 	TigressDopplerAngle[n].push_back(Tigressdopplerangle);
								// }
							}


						if(DoRingGroups){
							outfile->mkdir((tf+"/RingGroups").c_str());
							outfile->cd((tf+"/RingGroups").c_str());
							
							vector< TH1F* > rggs;
							for(unsigned int r=0;r<ringgroups.size();r++){
								stringstream histname;
								histname<<"GammaCorrect_Rings_"<<ringgroups[r].first<<"-"<<ringgroups[r].second<<t;
								rggs.push_back(new TH1F(histname.str().c_str(),histname.str().c_str(),2500*Gbins,0,2500*Gbins));
								axislab(rggs[r],"#gamma Energy [keV]");
							}
							RingGroupGammaSingles.push_back(rggs);
							

							if(debug){
								int ringC=24;		
								if(MultiS3){
									ringC=0;
									for(int s=0;s<4;s++)if(s3used[s])ringC++;
									ringC*=24;
								}
								
								if(ringC){
									RingGammaSingles.push_back(new TH2F(("GammaCorrectRings_"+t).c_str(),("GammaCorrectRings_"+t+";#gamma Energy [keV];RingNumber").c_str(),1024,0,2048*Gbins,ringC,0,ringC));
									if(GammaEfficiency){
										TH2Efficiency* th2efgr =new TH2Efficiency(("GammaEfficiencyRings_"+t).c_str(),("GammaEfficiencyRings_"+t+";#gamma Energy [keV];RingNumber").c_str(),1024,0,2048*Gbins,ringC,0,ringC,1,GammaEfficiencyGraph,GammaEfficiencyError);	
										
										th2efgr->SetAsymmetric();
										RingGammaEff.push_back(th2efgr);
									}
								}
							}
							if(GammaEfficiency){
								vector< TH1Efficiency* > rgge;
								for(unsigned int r=0;r<ringgroups.size();r++){
									stringstream histname;
									histname<<"GammaEfficiency_Rings_"<<ringgroups[r].first<<"-"<<ringgroups[r].second<<t;
									rgge.push_back(new TH1Efficiency(histname.str().c_str(),(histname.str()+";#gamma Energy [keV];Counts/keV").c_str(),2500*Gbins,0,2500*Gbins,GammaEfficiencyGraph,GammaEfficiencyError));
								}
								RingGroupGammaEff.push_back(rgge);
							}
						}
						
						outfile->cd(tf.c_str());
					}
					

			}
		outfile->cd();
	}

	
	
	vector< TH1F* >  MultiPartTig,MultiPartSPICE;
	vector< TH2F* >  PhiTheta,multisA,multisB,multigA,multigB;
	vector< TH3F* >  multisAB,multigAB;
	
	if(ParticleGate.size()>1&&MultiParticles){
		outfile->cd("ParticleGates");
		
		for(int n=0;n<ParticleGate.size();n++){
			string t=ParticleGate[n].title;
			
			for(int m=n+1;m<ParticleGate.size();m++){
				string T=ParticleGate[m].title;
			
				string tf="ParticleGates/"+t+T;
				outfile->mkdir(tf.c_str());
				outfile->cd(tf.c_str());
				
				if(DS){
					TH1F* spmt= new TH1F(("SiLi_"+t+T).c_str(),("SiLi_"+t+T).c_str(),2000,0,2000);axislab(spmt,"Electron Energy [keV]");
					MultiPartSPICE.push_back(spmt);

// 					TH2F* pgmults= new TH2F(("Mult_"+t+"_sili").c_str(),("Mult_"+t+"_sili;Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2000,0,2000);
// 					multisA.push_back(pgmults);
// 					pgmults= new TH2F(("Mult_"+T+"_sili").c_str(),("Mult_"+T+"_sili;Multiplicity;Electron Energy [keV]").c_str(),5,0,5,2000,0,2000);
// 					multisB.push_back(pgmults);
					
					TH3F* pgmultss= new TH3F(("Mult_"+T+t+"_sili").c_str(),("Mult_"+T+t+"_sili;Multiplicity"+t+";Multiplicity"+T+";Electron Energy [keV]").c_str(),5,0,5,5,0,5,1000,0,2000);
					multisAB.push_back(pgmultss);
				}
				
				TH1F* tigmt= new TH1F(("Gamma_"+t+T).c_str(),("Gamma_"+t+T).c_str(),2500*Gbins,0,2500*Gbins);axislab(tigmt,"#gamma Energy [keV]");
				MultiPartTig.push_back(tigmt);
/*				
				TH2F* pgmultg= new TH2F(("Mult_"+t+"_gamma").c_str(),("Mult_"+t+"_gamma;Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2500,0,2500);
				multigA.push_back(pgmultg);
				pgmultg= new TH2F(("Mult_"+T+"_gamma").c_str(),("Mult_"+T+"_gamma;Multiplicity;#gamma Energy [keV]").c_str(),5,0,5,2500,0,2500);
				multigB.push_back(pgmultg);*/
				
				TH3F* pgmultgg= new TH3F(("Mult_"+T+t+"_gamma").c_str(),("Mult_"+T+t+"_gamma;Multiplicity"+t+";Multiplicity"+T+";#gamma Energy [keV]").c_str(),5,0,5,5,0,5,1024,0,2048*Gbins);
				multigAB.push_back(pgmultgg);
				
// 				TH2F* phth= new TH2F(("Angle_"+t+T).c_str(),("Angle_"+t+T+";#Delta#Theta;#Delta#Phi").c_str(),128,0,pi,128,0,pi);
// 				PhiTheta.push_back(phth);
			}
		}
		outfile->cd();
	}
		

	
	TH1 *ee_singles,*ee_sum,*ee_sum_gtfifty,*silirawtotal;	TH2F*ee_vs,*ee_singleVsum,*ee_singleVsum_gt50,*ee_sum_distance,*ee_sum_ring,*e_single_ring,*e_single_preamp,*e_vs_rawtotal,*ee_sum_vs_rawtotal,*ee_single_gamma,*ee_sum_gamma,*segseg;
	TH3 *e_e_y,*ee_e_y,*angmap;
	
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
				e_e_y= new TH3I("e_e_y","e_e_y",500,0,1500,500,0,1500,500,0,1500);axislab(e_e_y,"Electron Pair Single Energy [keV]","Electron Pair Single Energy [keV]","#gamma Energy [keV]");
				ee_e_y= new TH3I("ee_e_y","ee_e_y",500,0,1500,500,0,1500,500,0,1500);axislab(e_e_y,"Electron Pair Summed Energy [keV]","Electron Pair Single Energy [keV]","#gamma Energy [keV]");
				
				segseg= new TH2F("SegVSeg","SegVSeg",120,0,120,120,0,120);axislab(segseg,"Electron Pair Single Energy [keV]","Raw Electron Sum Total [keV]");
				angmap= new TH3I("Eang_map","Eang_map",200,0,pi/4.,100,-50,50,100,-50,50);axislab(angmap,"Energy Ratio","X [mm]","Y [mm]");
		outfile->cd();	
	}

	TH1 *SiLi_singles,*Gamma_singles,*GammaSiLiPlus;
	TH2 *Gamma_SiLi,*Gamma_Gamma,*GammaSiLiPlus_Gamma,*GammaSiLiPlus_SiLi,*GammaSiLiPlus_Time;
	TH3 *Gamma_Gamma_Gamma,*Gamma_Gamma_SiLi,*FeedGamma_GammaSiLi;
    
    TH1Efficiency* Gamma_singlesEff;
	TH2Efficiency* Gamma_GammaEff;
	
	Gamma_singles= new TH1D("Gamma_singles","Gamma_singles",4096*Gbins,0,2048*Gbins);axislab(Gamma_singles,"#gamma Energy [keV]");	
	Gamma_Gamma= new TH2F("Gamma_Gamma","Gamma_Gamma",2048*Gbins,0,2048*Gbins,2048*Gbins,0,2048*Gbins);axislab(Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]");
// 	Gamma_Gamma_Gamma= new TH3F("Gamma_Gamma_Gamma","Gamma_Gamma_Gamma",512,0,2048,512,0,2048,512,0,2048);axislab(Gamma_Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]","#gamma Energy [keV]");
	Gamma_Gamma_Gamma= new TH3S("Gamma_Gamma_Gamma","Gamma_Gamma_Gamma",1024,0,2048*Gbins,512,0,2048*Gbins,512,0,2048*Gbins);axislab(Gamma_Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]","#gamma Energy [keV]");
    
    if(GammaEfficiency&&debug){
        Gamma_singlesEff = new TH1Efficiency("Gamma_singlesEff","Gamma_singlesEff;#gamma Energy [keV];Counts/keV",4096*Gbins,0,2048*Gbins,GammaEfficiencyGraph,GammaEfficiencyError);
        Gamma_GammaEff =new TH2Efficiency("Gamma_GammaEff","Gamma_GammaEff;#gamma Energy [keV];#gamma Energy [keV]",2048*Gbins,0,2048*Gbins,2048*Gbins,0,2048*Gbins,control[GammaGammaFrac],GammaEfficiencyGraph,GammaEfficiencyError);
    }
    
    
	if(DS){
		SiLi_singles= new TH1D("SiLi_singles","SiLi_singles",2048,0,2048);axislab(SiLi_singles,"Electron Energy [keV]");
		Gamma_SiLi= new TH2F("Gamma_SiLi","Gamma_SiLi",2048,0,2048*Gbins,1024,0,2048);axislab(Gamma_SiLi,"#gamma Energy [keV]","Electron Energy [keV]");
		
		if(debug){
			Gamma_Gamma_SiLi= new TH3S("Gamma_Gamma_SiLi","Gamma_Gamma_SiLi",1024,0,2048*Gbins,512,0,2048*Gbins,512,0,1024);axislab(Gamma_Gamma_Gamma,"#gamma Energy [keV]","#gamma Energy [keV]","Electron Energy [keV]");

			
			outfile->mkdir("SiLi/GammaSiLiPlus");
			outfile->cd("SiLi/GammaSiLiPlus");
			GammaSiLiPlus= new TH1D("GammaSiLiPlus","GammaSiLiPlus",2048*Gbins,0,4096*Gbins);axislab(GammaSiLiPlus,"#gamma+SiLi Energy [keV]");
			GammaSiLiPlus_Gamma= new TH2F("GammaSiLiPlus_Gamma","GammaSiLiPlus_Gamma",1024,0,4096*Gbins,1024,0,2048*Gbins);axislab(GammaSiLiPlus_Gamma,"#gamma+SiLi Energy [keV]","#gamma Energy [keV]");
			GammaSiLiPlus_SiLi= new TH2F("GammaSiLiPlus_SiLi","GammaSiLiPlus_SiLi",1024,0,4096*Gbins,1024,0,2048);axislab(GammaSiLiPlus_SiLi,"#gamma+SiLi Energy [keV]","SiLi Energy [keV]");
			GammaSiLiPlus_Time= new TH2F("GammaSiLiPlus_Time","GammaSiLiPlus_Time",2048,0,4096*Gbins,1024,-512,512);axislab(GammaSiLiPlus_Time,"#gamma+SiLi Energy [keV]","#Deltat [ns]");
            
            FeedGamma_GammaSiLi=new TH3S("FeedGamma_GammaSiLi","FeedGamma_GammaSiLi;Feeding #gamma Energy [keV];#gamma+SiLi Energy [keV];#Deltat [ns]",1024,0,2048*Gbins,1024,0,4096*Gbins,512,-512,512);

            			
		}
	}
	
// 	outfile->mkdir("MeanDoppler");
// 	outfile->cd("MeanDoppler");		
// 		TH1D* SiLi_singles_dop= new TH1D("SiLi_singles_dop","SiLi_singles_dop",2000,0,2000);axislab(SiLi_singles_dop,"Electron Energy [keV]");
// 		TH1D* Gamma_singles_dop= new TH1D("Gamma_singles_dop","Gamma_singles_dop",2000,0,2000);axislab(Gamma_singles_dop,"#gamma Energy [keV]");
// 		TH2F* Gamma_SiLi_dop= new TH2F("Gamma_SiLi_dop","Gamma_SiLi_dop",1000,0,2000,1000,0,2000);axislab(Gamma_SiLi_dop,"#gamma Energy [keV]","Electron Energy [keV]");
// 		TH2F* Gamma_Gamma_dop= new TH2F("Gamma_Gamma_dop","Gamma_Gamma_dop",1000,0,2000,1000,0,2000);axislab(Gamma_Gamma_dop,"#gamma Energy [keV]","#gamma Energy [keV]");
// 	outfile->cd();

	
gROOT->cd();
