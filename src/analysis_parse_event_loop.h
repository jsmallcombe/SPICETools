
//	//Small optional code to skip the first few events of every run which are likely garbage
//	//Specifically the start of a RUN _000.root, NOT a FILE
// 	for(unsigned int i=0;i<runstartentries.size();i++){
// 		if(jentry==runstartentries[i]){
// 			if(jentry+10<nentries)jentry+=10;
// 		}
// 	}

if(jentry>fileentriessum[fileiterator]&&fileiterator<=fileentriessum.size())fileiterator++;

DataChain->GetEntry(jentry);  //start the loop
//tigress->ResetAddback(); // annoyingly important
	
///////////////////////////////////
//////// Do timestamp & RF ////////
///////////////////////////////////

long tstamp=t_stamp(rf,tigress,sili,s3);//Fetch a time stamp from whichever of these is in the event, adjusted for the tigress high low griffin grsisort mismatch


bool RFfail=true;
double trig_cfd=-1;
double rf_cfd=-1;
double rf_t=0;
double rf_phase=0;
if(rf){
	if(abs(tstamp-rf->TimeStamp())<100000){//Had to add this because of some errors when RF only PART missing
		rf_cfd=rf->GetTimeFitCfd();
		rf_t=rf->Time()/10.0;//ns->ticks
		rf_phase=rf->Phase();
		trig_cfd=rf->GetTimestampCfd()-5000;
		if(rf_cfd>0){RFfail=false;rf_cfd-=3800;}//Manually derived offset for waveform delay
	}
}


long tstampraw=tstamp;
if(tstamp>0&&tstamp<timestamp_max){//If time stamp is crazy ignore	
	//Do the short move averages of no matter what

	//First to short average of latest values
	moveshort+=tstamp;moveshort-=moveshorthold[moveshorti];//update average
	moveshorthold[moveshorti]=tstamp;//Push new entry
	moveshorti++;if(moveshorti==10)moveshorti=0;//move iterator
			
	bool good_time=true;
	
// 		cout<<endl<<tstamp<<" "<<moveshort<<" "<<movelong<<" "<<goodmoves<<" "<<overTlimit;

	//check the latest value
	if(goodmoves>128){//Cant do comparisons until the long average is refilled
		if(abs(tstamp -(movelong>>7)) > tpgap){//check if we are more than X minutes from the previous event
			good_time=false;
			overTlimit++;
		}else{
			overTlimit=0;
		}
	}		
	
	//Fill the long average
	if(good_time){//If its good (or we havent filled the averages yet)
		//Fill the averages
		movelong+=tstamp;
		if(goodmoves>=128)movelong-=movelonghold[movelongi];//Only if long is full delete old entries
		movelonghold[movelongi]=tstamp;//Add entry		
		movelongi++;if(movelongi==128)movelongi=0;//move iterator		
		goodmoves++;//If the timestamp resets within 128 events of the start, well shit
	}

	//Deal with bad values
	if(!good_time){//We either have one crazy point or a break in the time stamp. Break could be from a glitch or a run change or clock end/reset
		if(overTlimit>=12){//Once we reach 10 in a row its probably NOT just a blip
			//The small moving average should now be ALL from the changed region (as length matched)
			overTlimit=0;
			
// 			cout<<endl<<"doooooom";
// 					nentries=jentry+150;

			//A disk access intensive last ditch check to see if the time stamp REALLY changes and doesn't just glitch
			long futurestamp=0;
			if(jentry+128<nentries){
				DataChain->GetEntry(jentry+128);
				futurestamp=t_stamp(rf,tigress,sili,s3);
				DataChain->GetEntry(jentry);
			}
			if(!futurestamp || abs(futurestamp -(movelong>>7)) > tpgap){
				if(((movelong>>7)>(moveshort*0.1))||RemoveTimeGaps){
					if((movelong>>7)>timestamp_thresh)cout<<endl<<endl<<"TIME STAMP RESET"<<endl<<endl;//ERROR HANDLING
					
					if(goodmoves<256){//On the off chance it resets twice quickly this recovers the time
						timestamp_add=timestamp_recover-moveshort*0.1;
					}else{
						timestamp_add+=(movelong>>7)-moveshort*0.1;
						timestamp_recover=tstamp+timestamp_add;
					}	
				}					
				for(int i=0;i<128;i++)movelonghold[i]=0;movelong=0;// and clear the long average
				goodmoves=0;//reset counters for blank slate
			}
		}else{
			tstamp=(movelong>>7);//The first 9 we just ignore and artificially smush over
		}
	}
	
	tstamp+=timestamp_add;//Adjust time stamp to be continuous
	
}else{
	if(goodmoves>128)tstamp=(movelong>>7)+timestamp_add;
	else tstamp=(moveshort*0.1)+timestamp_add;
}//tstamp=-1;

//Now we have a timestamp fill some histograms
eventrate->Fill(tstamp);
eventratehours->Fill(tstamp/timestamp_hour);
if(!(jentry%(nentries/10000))){
	//Changed to "set bin content" rather than weighted fill, despite trying to make bins match, some double filling
	runtimeraw->SetBinContent(runtimeraw->FindBin(jentry),tstampraw);
	runtime->SetBinContent(runtimeraw->FindBin(jentry),tstamp);
	timeadda->SetBinContent(runtimeraw->FindBin(jentry),timestamp_add);
}


///////////////////////////////
//////// Do S3 singles ////////
///////////////////////////////

//Get hits for rings S3 (dE in telescope)
unsigned short apmult[4]={0,0,0,0};
for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
	TS3Hit* SR=s3->GetRingHit(i);
	unsigned short id=s3id(SR);
	apmult[id]++;
	
	double e=SR->GetEnergy();
	if(e>control[S3EnergyLimit]*0.004){//noise gate
// 		double T=SH->GetCfd();
		
		int s=SR->GetSegment();
// 		S3ring.push_back(e);//store energy dE
// 		S3ring_i.push_back(s);//GetSegment == ring number 
// 		S3ringT.push_back(T);//we're going to use the dE for time
		S3ring_sum[id]->Fill(e);//Fill some flat histograms, we will only use dE+E events for coincidence work later
		if(s>=0&&s<24){
			//S3rings[s]->Fill(e);
			S3flat[id]->Fill(s,e);
		}			

	}
}
if(MultiS3)for(unsigned short i=0;i<4;i++){if(s3used[i])S3_ringmult[s3index[i]]->Fill(apmult[i]);apmult[i]=0;}
else S3_ringmult[0]->Fill(s3->GetRingMultiplicity());


//Get hits for E S3 (sectors)
for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
	TS3Hit* SS=s3->GetSectorHit(i);
	unsigned short id=s3id(SS);
	apmult[id]++;
	
	double e=SS->GetEnergy();
	if(e>control[S3EnergyLimit]*0.004){//increased noise gate
		int s=SS->GetSegment();
		S3sector_sum[id]->Fill(e);
		if(s>=0&&s<32){
			//S3sectors[s]->Fill(e);
			S3flat[id]->Fill(s+24,e);
		}
		//
		// End of singles
		//
		
		//
		// Do raw compare
		//
		for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
			TS3Hit* SR=s3->GetRingHit(i);
			if(MultiS3)if(SS->GetArrayPosition()!=SR->GetArrayPosition())continue;
			
			double re=SR->GetEnergy();
			if(re>control[S3EnergyLimit]*0.004){//noise gate
				int r=SR->GetSegment();
				
				if(!Telescope){
					frontVback[id]->Fill(re,e);
					front_back[id]->Fill(e-re);
					if(e*control[FrontBackEnergy]<re&&re*control[FrontBackEnergy]<e){
						front_backgate[id]->Fill(e-re);
						frontVbackGated[id]->Fill(re,e);
					}
					else continue;
				}
				
				double TT=SR->GetCfd()-SS->GetCfd();
				S3RS_t[id]->Fill(TT);
				if(Telescope)S3RS_t3[id]->Fill(TT,re,e);
				if(!RFfail)S3RS_RF->Fill((SR->GetCfd()-rf_cfd)/16.0,(SS->GetCfd()-rf_cfd)/16.0);
				
				if(abs(TT)<control[FrontBackTime]){
					S3RS_tgate[id]->Fill(TT);
					if(!RFfail)S3RS_RFgated->Fill((SR->GetCfd()-rf_cfd)/16.0,(SS->GetCfd()-rf_cfd)/16.0);
					//S3sectorsout[s]->Fill(SS->GetCharge(),r);
				}
			}
		}
	}
}
if(MultiS3)for(unsigned short i=0;i<4;i++){if(s3used[i])S3_sectormult[s3index[i]]->Fill(apmult[i]);apmult[i]=0;}
else S3_sectormult[0]->Fill(s3->GetSectorMultiplicity());

//////////////////////////////////////////
//////// Make S3 complete events ////////
//////////////////////////////////////////


std::vector< TVector3 > S3pos,S3posspear;
std::vector< vector < bool > > S32D(ParticleGate.size(),vector < bool >());
std::vector< double > S3Trf;
std::vector< TS3Hit* > S3select;
std::vector< double > Vdedx;

// Build the combined hits we will use for coincidences
for(unsigned int i=0;i<s3->GetPixelMultiplicity();i++){//GetPixelMultiplicity builds the events based on pre-set settings
	TS3Hit* SH=s3->GetS3Hit(i);
	unsigned short id=s3id(SH);
	apmult[id]++;
	
	//RF gate for singles triggers
	double TT=SH->GetCfd()-rf_cfd;TT/=16.0;		
	if(!RFfail){
		S3_rf->Fill(TT);
		if(t_gate(TT,rf_S3)){//RF gate
			S3_rfgate->Fill(TT);
		}
	}
	
	double dE=SH->GetEnergy();
	double E=dE;

	TVector3 pos = SH->GetPosition(true);
//	TVector3 pos = TS3::GetPosition(S3ring_i[i],S3sec_i[j],-22.5*TMath::Pi()/180.,32.1,Telescope,false);//Get the hit vector with some random smoothing	
	S3_map[id]->Fill(pos.X(),pos.Y());//This is more for online checking of hit map
	S3_map3->Fill(pos.Z(),pos.X(),pos.Y());//This is more for online checking of hit map
	S3posspear.push_back(pos);	
	
	double theta=pos.Theta();	
	
	//cout<<endl<<pos.Phi();
	//double pp=((SH->GetSector()+6.1)/32.0)*TMath::Pi()*2;
	//pos.SetMagThetaPhi(1,theta,pp);
	//cout<<" "<<pos.Phi();
	
	if(Telescope){
		//In telescope mode need the sector energy too
		//But we use SetMultiHit(false) for Telescope so sector number can appear only once
		for(unsigned int j=0;j<s3->GetSectorMultiplicity();j++){
			TS3Hit* SS=s3->GetSectorHit(j);
			if(SS->GetSector()==SH->GetSector()){
				E+=SS->GetEnergy();
				SH->SetEnergy(E);
				break;
			}
		}
		//Noise gates
		if(dE<control[S3EnergyLimit]*0.01||(E-dE)<control[S3EnergyLimit]*.04)continue;
		
		//adjust dE for thickness -> de/dx
		dE*=TMath::Cos(pos.Theta());
	}else{
		//Noise gates
		if(E<control[S3EnergyLimit]*.004)continue;
	}

	
	if(Telescope){
		S3_dedx[id]->Fill(E,dE); //Fill the ID histogram
		S3_d3dx[id]->Fill(E,dE,theta); //Fill the ID 3D histogram
		//We are using the blurred theta so it will be lower "resolution" but not fence post
	}else{				
		dE=theta;//This line might seem a little odd but dE is only sparsely used
	}
	
	S3_theta->Fill(E,theta);	
	
	//Check for each gate of particles
	for(int g=0;g<ParticleGate.size();g++){
		
		bool goodgate=true;
		if(MultiS3&&ParticleGate[g].s3Limit){
			goodgate=false;
			for(unsigned short s=0;s<ParticleGate[g].s3.size();s++){
				if(SH->GetArrayPosition()==ParticleGate[g].s3[s]){
					goodgate=true;
					break;
				}
			}
		}		
		
		if(goodgate&&ParticleGate[g].gate.IsInside(E,dE)){
			S32D[g].push_back(true);
			S3particleGated[g]->Fill(E,dE);

		}else{
			S32D[g].push_back(false);
		}
	}	

	S3Trf.push_back(TT);
	S3select.push_back(SH);
	S3pos.push_back(SH->GetPosition(false));
	if(Telescope)Vdedx.push_back(dE);

}
int S3N=S3select.size();

if(MultiS3){
	for(unsigned short i=0;i<4;i++){if(s3used[i])S3_mult[s3index[i]]->Fill(apmult[i]);}
	if(MultiS3)S3_multot->Fill(s3->GetPixelMultiplicity());
}else S3_mult[0]->Fill(s3->GetPixelMultiplicity());


for(int g=0;g<ParticleGate.size();g++){
	int m=0;
	for(unsigned int i=0;i<S32D[g].size();i++){
		if(S32D[g][i])m++;
	}
	PGmult[g]->Fill(m);
}


//////////////////////////////////
//////// Do spice singles ////////
//////////////////////////////////

	
double rawsilisum=0;
if(UseFitCharge)sili->UseFitCharge();

// First we fill the raw stuff


if(DS)SiLi_mult->Fill(sili->GetMultiplicity());
if(DS){for(int i=0;i<sili->GetMultiplicity();i++){
	sili_hit = sili->GetSiLiHit(i);
	double e=sili_hit->GetEnergy();
	fileN_silinoise->Fill(fileiterator,e);
	eventN_silinoise->Fill(jentry,e);
	runtime_silinoise->Fill(tstamp,e);
	rawsilisum+=e;
	if(e>10&&e<4000){//noise gate and common sense gate
		SiLi_raw->Fill(e);
		TVector3 pos = sili_hit->GetPosition(true);
// 		TVector3 pos = TSiLi::GetPosition(sili_hit->GetRing(),sili_hit->GetSector(),true);
		SiLi_map->Fill(-pos.X(),pos.Y());
		
		int s=sili_hit->GetSegment();
		if(s>=0&&s<120){
			SiLiflat->Fill(s,e);
			SiLipreamp->Fill(sili_hit->GetPreamp(),e);
			silienergy[s]->Fill(e);
			int r=sili_hit->GetRing();
			SiLiring->Fill(r,e);
			SiLisector->Fill(sili_hit->GetSector(),e);
			if(r>=0&&r<10)silirings[r]->Fill(e);
			if(UseFitCharge){
				double c=sili_hit->GetFitCharge();
				if(c>0){silifitc[s]->Fill(c);
					double smirn=sili_hit->GetSmirnov();
					double sigtonoise=1./sili_hit->GetSig2Noise();
					SiLi_smirnov->Fill(smirn,e);
					SiLi_smirnov_C->Fill(smirn/c,e);
					SiLi_signoise->Fill(sigtonoise,e);
					
					if(sigtonoise>control[SiLiNoiseLimit]||(smirn/c)>control[SiLiSmirnovLimit])sili_hit->SetEnergy(0);
				}
			}
		}
	}
}}

std::vector< double > SiLiE,SiLiEdop,SiLit,SiLitRF;
std::vector< TSiLiHit* > SiLii;
std::vector< bool > SiLiRF;


//Next we do the addback selected hits that we will use for the rest
if(DS)SiLiAdd_mult->Fill(sili->GetAddbackMultiplicity());
if(DS){for(int i=0;i<sili->GetAddbackMultiplicity();i++){
	sili_hit = sili->GetAddbackHit(i);
	double e=sili_hit->GetEnergy();
	if(e>10&&e<4000){//noise gate and common sense gate
		double Nadd=sili_hit->GetAddbackSize();
		if(Nadd==1){//if its a good clean hit add it
			double ft=sili_hit->GetTimeFit();
			if(ft>50){//garbage events have early fit time
			
				SiLi_addback->Fill(e);
				
// 				double edop=sili_hit->GetDoppler(control[BetaZero]);
				
				//Calc a "cfd" time from the fit and timestamp
				double T=sili_hit->GetTimeFitCfd()-control[SiLiWaveTOffset];
				
				SiLiE.push_back(e);
				SiLiEdop.push_back(e);
				SiLii.push_back(sili_hit);
			
				SiLit.push_back(T);	
				
				SiLi_singles->Fill(e);
				runtime_sili->Fill(tstamp,e);
				fileN_sili->Fill(fileiterator,e);	
				eventN_sili->Fill(jentry,e);	

				//1D RF check appropriate for singles DAQ triggers 
				double TT=ft-rf_t-100;//else{TT=((T-rf_cfd)/16)-250;}
				
				SiLitRF.push_back(TT);
				SiLiRF.push_back(false);
				if(!RFfail){
					SiLi_rf->Fill(TT);
					SiLi_rf2->Fill(e,TT);
						
					if(t_gate(TT,rf_sili)){//RF gate
						SiLiRF[SiLiRF.size()-1]=true;
						SiLi_RFgated->Fill(e);
						SiLi_rfgate->Fill(TT);
					}else{
						SiLi_RFantigated->Fill(e);
					}					
				}
			}
			SiLi_fit_time->Fill(ft);
			SiLi_fit_timeE->Fill(ft,e);
			SiLi_fit_timeRF->Fill(ft,rf_t);
		}else{
			//For the multi hitclusters do the addback histograms
			for(int j=0;j<Nadd;j++)SiLi_rejected->Fill(sili_hit->GetAddbackEnergy(j));
			SiLi_rejected_sum->Fill(e);
		}
	}
}}
int SiLiN=SiLii.size();

//////////////////////////////////
//////// Do gammas singles ///////
//////////////////////////////////

//Quick Addback check
for(int i=0;i<tigress->GetMultiplicity();i++){
	tigress->GetTigressHit(i)->ClearTransients();
	
	Gamma_no_add->Fill(tigress->GetTigressHit(i)->GetEnergy());
    Gamma_Core->Fill(tigress->GetTigressHit(i)->GetArrayNumber(),tigress->GetTigressHit(i)->GetEnergy());
	for(int j=i+1;j<tigress->GetMultiplicity();j++){
		double TT=tigress->GetTigressHit(i)->GetCfd()-tigress->GetTigressHit(j)->GetCfd();
		if(t_gate(TT,gamma_gamma_t)){
			Gamma_Gamma_no_add->Fill(tigress->GetTigressHit(i)->GetEnergy(),tigress->GetTigressHit(j)->GetEnergy());
			Gamma_Gamma_no_add->Fill(tigress->GetTigressHit(j)->GetEnergy(),tigress->GetTigressHit(i)->GetEnergy());
		}
	}
}
std::vector< TTigressHit* > gammai;
std::vector< double > gammaE,gammaEdop,gammaTrf;
std::vector< bool > gammaRF;
std::vector< TVector3 > gammapos;

Tig_mult->Fill(tigress->GetAddbackMultiplicity());
for(int i=0;i<tigress->GetAddbackMultiplicity();i++){
	tigress_hit = tigress->GetAddbackHit(i);
	double e=tigress_hit->GetEnergy();
	
	bool suppressed=tigress_hit->BGOFired();
	
	if(suppressed)Gamma_suppressed->Fill(e);
	
	if(e>40)Gamma_singles_no_bgo->Fill(e);
	if(e>40&&!suppressed){//noise and basic BGO gate
						
		gammaE.push_back(e);
		gammaEdop.push_back(e);
		gammai.push_back(tigress_hit);
		Gamma_singles->Fill(e);
		runtime_gamma->Fill(tstamp,e);
		fileN_gamma->Fill(fileiterator,e);
		eventN_gamma->Fill(jentry,e);
		
		TVector3 pos=TTigress::GetPosition(*tigress_hit,0,true);
		double theta=pos.Theta();
		
		TigressHitMap3->Fill(pos.X(),pos.Y(),pos.Z());
		TigressHitMap->Fill(pos.Phi(),theta);
		TigressHitMapLow->Fill(pos.Phi(),theta);
		TigressETheta->Fill(e,theta);
		gammapos.push_back(tigress_hit->GetPosition());//No smear
		
		//RF check
		double TT=tigress_hit->GetCfd()-rf_cfd;TT/=16;
		gammaTrf.push_back(TT);
		gammaRF.push_back(false);
		if(!RFfail){//IF there is an RF
			Gamma_rf->Fill(TT);
			EGamma_rf2->Fill(e,TT);
			if(t_gate(TT,rf_gamma)){//RF gate
				Gamma_rfgate->Fill(TT);
				Gamma_RFgated->Fill(e);
				gammaRF[gammaRF.size()-1]=true;
			}else{
				Gamma_RFantigated->Fill(e);
			}
		}
	}
}
int gammaN=gammai.size();

////////////////////////////////////////////////////
////////////////// END OF SINGLES //////////////////
////////////////////////////////////////////////////


////////////////////////////////////////////////////
////////////////// Gammas + xxx //////////////////
////////////////////////////////////////////////////
		
for(unsigned int i=0;i<gammaN;i++){

	//////// Do gammas + SiLi ////////
	if(DS){
		for(unsigned int j=0;j<SiLiN;j++){
			double TT=gammai[i]->GetCfd()-SiLit[j];
			Gamma_SiLi_t->Fill(TT);
			Gamma_SiLi_tw->Fill(TT);
			Gamma_SiLi_t2->Fill(gammaE[i],TT);
			Gamma_SiLi_t3->Fill(gammaE[i],SiLiE[j],TT);
			
			if(t_gate(TT,gamma_sili_t)){
				Gamma_SiLi_tgate->Fill(TT);

				runtime_gammasili->Fill(tstamp,gammaE[i],SiLiE[j]);
				eventN_gammasili->Fill(jentry,gammaE[i],SiLiE[j]);
				Gamma_SiLi->Fill(gammaE[i],SiLiE[j]);
				GammaSiLiPlus->Fill(gammaE[i]+SiLiE[j]);
				GammaSiLiPlus_Gamma->Fill(gammaE[i]+SiLiE[j],gammaE[i]);
				GammaSiLiPlus_SiLi->Fill(gammaE[i]+SiLiE[j],SiLiE[j]);
				
				if(!RFfail)Gamma_SiLi_SiliRF->Fill(gammaE[i],SiLiE[j],SiLitRF[j]);
				
				if(gammaRF[i]&&SiLiRF[j]){
					SiLiGamma__RFgated->Fill(gammaE[i],SiLiE[j]);
				}
				
				Gamma_SiLi_RFgated->Fill(gammaTrf[i],SiLitRF[j]);
			}
			
			Gamma_SiLi_RF->Fill(gammaTrf[i],SiLitRF[j]);
		}
	}
	
	//////// Do gammas + gammas ////////
	for(unsigned int j=i+1;j<gammaN;j++){
		double TT=gammai[i]->GetCfd()-gammai[j]->GetCfd();
		GG_t->Fill(TT);
		if(t_gate(TT,gamma_gamma_t)){
			GG_tgate->Fill(TT);
			Gamma_Gamma->Fill(gammaE[i],gammaE[j]);
			Gamma_Gamma->Fill(gammaE[j],gammaE[i]);
			
			TigressEETheta->Fill(gammaE[i],gammaE[j],gammapos[i].Theta());
			TigressEEdTheta->Fill(gammaE[i],gammaE[j],gammapos[i].Angle(gammapos[j]));
			TigressEEdTheta->Fill(gammaE[j],gammaE[i],gammapos[i].Angle(gammapos[j]));
			
			runtime_gammagamma->Fill(tstamp,gammaE[i],gammaE[j]);
			runtime_gammagamma->Fill(tstamp,gammaE[j],gammaE[i]);
			eventN_gammagamma->Fill(jentry,gammaE[i],gammaE[j]);
			eventN_gammagamma->Fill(jentry,gammaE[j],gammaE[i]);	
			
			if(gammaRF[i]&&gammaRF[j]){
				GammaGamma_RFgated->Fill(gammaE[i],gammaE[j]);
				GammaGamma_RFgated->Fill(gammaE[j],gammaE[i]);
			}				

			//////// Do gammas + gammas + gammas ////////
			for(unsigned int k=j+1;k<gammaN;k++){
				double TTT=gammai[i]->GetCfd()-gammai[k]->GetCfd();
				if(t_gate(TTT,gamma_gamma_t)){
					Gamma_Gamma_Gamma->Fill(gammaE[i],gammaE[j],gammaE[k]);
					Gamma_Gamma_Gamma->Fill(gammaE[i],gammaE[k],gammaE[j]);
					Gamma_Gamma_Gamma->Fill(gammaE[j],gammaE[i],gammaE[k]);
					Gamma_Gamma_Gamma->Fill(gammaE[j],gammaE[k],gammaE[i]);
					Gamma_Gamma_Gamma->Fill(gammaE[k],gammaE[j],gammaE[i]);
					Gamma_Gamma_Gamma->Fill(gammaE[k],gammaE[i],gammaE[j]);
				}
			}
		}
	}		
}


////////////////////////////////////////////////////
////////////////// S3 and xxx //////////////////
////////////////////////////////////////////////////

std::vector< std::vector< unsigned short > > SiLiFirstOnly;
std::vector< std::vector< unsigned short > > GammaFirstOnly;

if(FirstOnly||MultiParticles){
	SiLiFirstOnly= std::vector< std::vector< unsigned short > > (SiLiN,std::vector< unsigned short >(S32D.size(),0));
	GammaFirstOnly= std::vector< std::vector< unsigned short > > (gammaN,std::vector< unsigned short >(S32D.size(),0));
}

for(unsigned int j=0;j<S3N;j++){
	TS3Hit* SH=S3select[j];
	
	//Stores if Sili hits are time coincident with current S3
	std::vector< bool > SiLiS3loop(SiLiN,false);		
	
	//// Do S3 and SiLi coincidence check ////
	if(DS){for(unsigned int i=0;i<SiLiN;i++){
		double TT=SiLit[i]-SH->GetCfd();
		double e=SiLiE[i];

		SiLi_S3_t->Fill(TT);
		SiLi_S3_tw->Fill(TT);
		SiLi_S3_t2->Fill(e,TT);

		bool rf2dgate=false;	
		
		if(!RFfail){
			S3_SiLi_RF->Fill(S3Trf[j],SiLitRF[i]);
			S3_SiLi_RFe->Fill(S3Trf[j],SiLitRF[i],e);	
			for(int g=0;g<s3silirf2D.size();g++)rf2dgate+=s3silirf2D[g].IsInside(SiLitRF[i],S3Trf[j]);
			if(!UseSiLiRFCoinc)rf2dgate=false;
		}
		
		//Either check SiLi-S3 coincidence time OR sili-rf S3-rf 2D gate, depending on settings
		if((t_gate(TT,s3_sili_t)&&!UseSiLiRFCoinc) || rf2dgate){
			SiLi_S3_tgate->Fill(TT);
			if(!RFfail)S3_SiLi_RFgated->Fill(S3Trf[j],SiLitRF[i]);
			SiLiS3loop[i]=true;
		}
	}}
	
	std::vector< bool > gammaS3loop(gammaN,false);

	//// Do S3 gamma coincidence check ////
	for(unsigned int i=0;i<gammaN;i++){
		double TT=gammai[i]->GetCfd()-SH->GetCfd();
		double e=gammaE[i];

		Gamma_S3_t->Fill(TT);
		Gamma_S3_t2->Fill(e,TT);
		if(t_gate(TT,s3_gamma_t)){
			Gamma_S3_tgate->Fill(TT);	
			gammaS3loop[i]=true;			
			if(!RFfail)Gamma_S3_RFgated->Fill(S3Trf[j],gammaTrf[i]);
		}
		
		if(!RFfail){Gamma_S3_RF->Fill(S3Trf[j],gammaTrf[i]);
			Gamma_S3_RFe->Fill(S3Trf[j],gammaTrf[i],e);}	
	}
	
	int mulparthist=0;
	
	//// Iterate of S3 kinematic gates, which define a particle type ////
	for(int g=0;g<S32D.size();g++){if(S32D[g][j]){
		TVector3 particlevec=S3pos[j];
		
		gate2Ddata* ggate=&ParticleGate[g];
		
		//get the calculated beta value for a particle
		double betal=0;
		if(ggate->use_beta){
			betal=control[BetaZero];
			if(ggate->use_rb){
				betal=calc_beta_KE(SH->GetEnergy()/1000.,ggate->mass);
			}else if(ggate->use_rb){
				unsigned int rrr=SH->GetRing();
				if(ggate->ring_beta.size()>rrr)betal=ggate->ring_beta[rrr];
			}else if(ggate->use_tb){
				betal=ggate->theta_beta.Eval(particlevec.Theta());
			}
		}
		
		//Convert from S3 particle to particle of interest
		if(ggate->use_tt){
			particlevec.SetMagThetaPhi(1,ggate->theta_theta.Eval(particlevec.Theta()), particlevec.Phi()+TMath::Pi());
		}
		
		//Do SiLi kinematic adjust
		if(DS){for(unsigned int i=0;i<SiLiN;i++){if(SiLiS3loop[i]){
			
			if(FirstOnly||MultiParticles){
				if(FirstOnly)if(SiLiFirstOnly[i][g]){
					SiLiS3loop[i]=false;
					continue;
				}
				SiLiFirstOnly[i][g]++;
			}
			
			double e;
			if(ggate->use_beta)e=SiLiE[i];//e=SiLii[i]->GetDoppler(betal,&particlevec);
			else e=SiLiE[i];
			SiLiEdop[i]=e;
			SiLiPG[g]->Fill(e);
			if(SiLiRF[i])SiLiPGRF[g]->Fill(e);
			SiLiEPG[g]->Fill(e,SH->GetEnergy());	
		}}}
			

		//// Do kinematic adjust gammas ////
		for(unsigned int i=0;i<gammaN;i++){if(gammaS3loop[i]){
			
			if(FirstOnly||MultiParticles){
				if(FirstOnly)if(GammaFirstOnly[i][g]){
					gammaS3loop[i]=false;
					continue;
				}
				GammaFirstOnly[i][g]++;
			}
			
			double E;
			if(ggate->use_beta)E=gammai[i]->GetDoppler(betal,&particlevec);
			else E=gammai[i]->GetEnergy();
			gammaEdop[i]=E;
			
			if(ggate->use_beta){
				double e=gammai[i]->GetEnergy();
				unsigned int R=s3r(SH);//SH->GetRing();
				
				Gcorrectedring[g]->Fill(E,R);
				
				double ang=gammapos[i].Angle(particlevec);
				TigressDopplerAngle[g][R]->Fill(ang,e);

				//Really should replace this with a precalc list
				double usangle=2*atan(pow(1-betal,0.25)/pow(1+betal,0.25));
				if(abs(ang-usangle)<0.01) GUnshifted[g]->Fill(e);
				GUncorrected[g]->Fill(e);
				GUncorrectedring[g]->Fill(e,R);
				
				for(unsigned int r=0;r<ringgroups.size();r++){
					if(R>=ringgroups[r].first&&R<=ringgroups[r].second)
						RingGroupGammaSingles[g][r]->Fill(E);
				}	
			}

			GammaPG[g]->Fill(E);
			GammaEPG[g]->Fill(E,SH->GetEnergy());
			TigressEThetaPG[g]->Fill(E,gammapos[i].Theta());
			
			if(Telescope){GammaS3dedx->Fill(SH->GetEnergy(),Vdedx[j],E);}
			
			//// Do particle gated gammas + gammas ////
			
			for(int m=i-1;m>=0;m--){
				if(gammaS3loop[m]){if(t_gate(gammai[i]->GetCfd()-gammai[m]->GetCfd(),gamma_gamma_t)){
					GammaGammaPG[g]->Fill(gammaEdop[i],gammaEdop[m]);
					GammaGammaPG[g]->Fill(gammaEdop[m],gammaEdop[i]);
					TigressEEThetaPG[g]->Fill(gammaEdop[i],gammaEdop[m],gammapos[i].Theta());
					TigressEEdThetaPG[g]->Fill(gammaEdop[i],gammaEdop[m],gammapos[i].Angle(gammapos[m]));
					TigressEEdThetaPG[g]->Fill(gammaEdop[m],gammaEdop[i],gammapos[i].Angle(gammapos[m]));
				}}
			}
			
			//// Do particle gated gammas + electron ////
			if(DS){for(unsigned int m=0;m<SiLiN;m++){
				if(SiLiS3loop[m]){if(t_gate(gammai[i]->GetCfd()-SiLit[m],gamma_sili_t)){
					GammaSiLiPG[g]->Fill(gammaEdop[i],SiLiEdop[m]);	
				}}
			}}
		}}
		

		//// Go Through Multi S3 gates ////
		if(MultiParticles)for(int k=g+1;k<S32D.size();k++){{
			for(unsigned int i=0;i<S3N;i++){
				if(i==j)continue;
				
				if(S32D[k][i]){
					double thth=S3posspear[i].Theta()+S3posspear[j].Theta();
					if(thth>pi)thth=2*pi-thth;
					PhiTheta[mulparthist]->Fill(thth,S3posspear[j].DeltaPhi(S3posspear[i]));
				}
			}
			mulparthist++;
		}}
	}}		
}



//// Iterate double S3 kinematic gates ////
if(MultiParticles){
	int mulparthist=0;
	for(int g=0;g<S32D.size();g++){
		for(int k=g+1;k<S32D.size();k++){
		
			if(DS){for(unsigned int i=0;i<SiLiN;i++){
				if(SiLiFirstOnly[i][g]&&SiLiFirstOnly[i][k])
					MultiPartSPICE[mulparthist]->Fill(SiLiE[i]);
					multisA[mulparthist]->Fill(SiLiFirstOnly[i][g],SiLiE[i]);
					multisB[mulparthist]->Fill(SiLiFirstOnly[i][k],SiLiE[i]);
			}}	
			
			for(unsigned int i=0;i<gammaN;i++){
				if(GammaFirstOnly[i][g]&&GammaFirstOnly[i][k]){
					MultiPartTig[mulparthist]->Fill(gammaE[i]);
					multigA[mulparthist]->Fill(GammaFirstOnly[i][g],gammaE[i]);
					multigB[mulparthist]->Fill(GammaFirstOnly[i][k],gammaE[i]);
				}
			}
			
			mulparthist++;
		}
		
		for(unsigned int i=0;i<gammaN;i++){
			if(GammaFirstOnly[i][g])PGmultGamma[g]->Fill(GammaFirstOnly[i][g],gammaE[i]);
		}
	}
}



////////////////////////////////
//////// Do sili + sili ////////
////////////////////////////////
// Shouldnt use the doppler adjust values any more here

if(DoDoubleElectrons&&DS){
if(rawsilisum>10&&SiLiN>0)silirawtotal->Fill(rawsilisum);
for(unsigned int i=0;i<SiLiN;i++){
	double e=SiLii[i]->GetEnergy();
	for(unsigned int j=i+1;j<SiLiN;j++){
		double ee=SiLii[j]->GetEnergy();
		double TT=SiLit[i]-SiLit[j];
		ee_t->Fill(TT);
		if(t_gate(TT,sili_sili_t)){
			ee_tgate->Fill(TT);
			int r=abs(SiLii[i]->GetRing()-SiLii[j]->GetRing());
			int s=abs(SiLii[i]->GetSector()-SiLii[j]->GetSector());
			if(r>1||(s>1&&s<11)){//if not neighbour
				ee_singles->Fill(e);
				ee_singles->Fill(ee);
				ee_sum->Fill(e+ee);
				ee_vs->Fill(e,ee);
				ee_vs->Fill(ee,e);
				ee_singleVsum->Fill(e,ee+e);
				ee_singleVsum->Fill(ee,ee+e);
				ee_sum_vs_rawtotal->Fill(e+ee,rawsilisum);
				e_vs_rawtotal->Fill(e,rawsilisum);		
				e_vs_rawtotal->Fill(ee,rawsilisum);	
						
				ee_sum_ring->Fill(e+ee,SiLii[i]->GetRing());
				ee_sum_ring->Fill(e+ee,SiLii[j]->GetRing());
				e_single_ring->Fill(e,SiLii[i]->GetRing());
				e_single_ring->Fill(ee,SiLii[j]->GetRing());
				e_single_preamp->Fill(e,SiLii[i]->GetPreamp());
				e_single_preamp->Fill(ee,SiLii[j]->GetPreamp());					
				
				TVector3 p1=TSiLi::GetPosition(SiLii[i]->GetRing(),SiLii[i]->GetSector());					
				TVector3 p2=TSiLi::GetPosition(SiLii[j]->GetRing(),SiLii[j]->GetSector());	
				double dist=(p1-p2).Mag();
				ee_sum_distance->Fill(e+ee,dist);
				if(dist>50){
					ee_singleVsum_gt50->Fill(e,ee+e);
					ee_singleVsum_gt50->Fill(ee,ee+e);
					ee_sum_gtfifty->Fill(e+ee);
				}
				
				for(unsigned int k=0;k<gammaN;k++){
					double y=gammaE[k];
					ee_single_gamma->Fill(e,y);
					ee_single_gamma->Fill(ee,y);
					ee_sum_gamma->Fill(e+ee,y);
					e_e_y->Fill(e,ee,y);
					e_e_y->Fill(ee,e,y);
					ee_e_y->Fill(e+ee,ee,y);
					ee_e_y->Fill(e+ee,e,y);
				}
			}
		}
	}
}}
		

//////////////////////////////////////////////////////////////  
//////////////////  END PROCESS OF THE CHAIN  ////////////////
////////////////// Mostly just output messages ////////////////
//////////////////////////////////////////////////////////////

if(jentry%updateinterval == 0){
	double percent=100. * (double)jentry/nentries;
	
	//Current filename
	int j=0;
	for(unsigned int i=0;i<fileentriessum.size();i++){
		j=i;
		if(jentry<fileentriessum[i])break;
	}
	
	//Estimated completion time
	double timenow=stopper.RealTime();
	stopper.Continue();
	double remaining=0;
	if(percent>0)remaining=(100.0-percent)*((timenow-currentsettime)/(percent-currentsetpercent));
	
	if(jentry%onepercent == 0){
// 			cout<<endl;
		currentsettime=nextsettime;
		currentsetpercent=nextsetpercent;	
		nextsettime=timenow;
		nextsetpercent=percent;
		sorttime->SetPoint(sorttime->GetN(),percent,timenow);
		guessend->SetPoint(guessend->GetN(),percent,remaining);
	}
	
	int hours=floor(remaining/3600.);remaining-=hours*3600;
	int minutes=floor(remaining/60.);remaining-=minutes*60;
	int seconds=round(remaining);
	
	//Screen update
	cout << setiosflags(ios::fixed) << std::setprecision(2) << percent << " % complete. File "<<filelist[j]<<", Time remaining "<<setfill('0')<<setw(2)<<hours<<":"<<setfill('0')<<setw(2)<<minutes<<":"<<setfill('0')<<setw(2)<<seconds<<"\r" << flush; 
}
