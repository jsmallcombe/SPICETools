	
//////////////////////////////////////
//////// Do input file checks ////////
//////////////////////////////////////

//runstartentries && fileentriessum give the jevent before the new file
bool newrun=false;
if(jentry+1>runstartentries[runstartrator]&&runstartrator+1<runstartentries.size()){
	runstartrator++;
	newrun=true;
	//Small optional code to skip the first few events of every run which are likely garbage
	//Specifically the start of a RUN _000.root, NOT a FILE
	//if(jentry+32<nentries)jentry+=32;
}
if(jentry+1>runchangeentries[runiterator]&&runiterator+1<runchangeentries.size()){
	runiterator++;
	newrun=true;
}

// if(newrun){cout<<" Event "<<jentry<<" is a new run."<<endl;}

//fileentriessum is the number of events to the EOF N
bool newfile=false;
if(jentry+1>fileentriessum[fileiterator]&&fileiterator<fileentriessum.size()){
	if(fileiterator+1<fileentriessum.size())fileiterator++;
	newfile=true;
}
if(jentry==0)newfile=true;

if(GainDrift&&newfile){
	FileOffset=fileoffset[fileiterator];
	FileGain=filegain[fileiterator];
}
//
//Branch check on new run
//
if(newrun){
	cout<<endl;//This is just useful for the sort progress status message
	
// 	DataChain->ResetBranchAddresses(); //Tried various things to get rid of the error messages, but we're stuck with them
// 					   //At least now they dont matter
	
	DataChain->GetEntry(jentry);  //This is crucial to actually load the next file of the chain
				      //Otherwise we are actually checking the previous file for the branches
	
	// If you only set the branch address once runs, a branch which only appears in some files could be lost entirely
	// Also can cause multiple filling when branch is not present so uses last values
	if(DS){if(DataChain->FindBranch("TSiLi"))DataChain->SetBranchAddress("TSiLi",&sili);else sili=new TSiLi();}
	if(DataChain->FindBranch("TTigress"))DataChain->SetBranchAddress("TTigress",&tigress);else tigress=new TTigress();
	if(DataChain->FindBranch("TS3"))DataChain->SetBranchAddress("TS3",&s3);else s3=new TS3();
	if(DataChain->FindBranch("TRF"))DataChain->SetBranchAddress("TRF",&rf);else rf=new TRF();
	if(AddMonitor){if(DataChain->FindBranch("TGenericDetector"))DataChain->SetBranchAddress("TGenericDetector",&gd);else gd=new TGenericDetector();}
	// Could possible just Reset() the TTree owned detector classes when branch is missing,
	// but this way works and only creates a few loose ends of memory	
}



/////////////////////////////////////////////////////////
///////////// LOAD THE DATA FOR THIS LOOP ////////////////
/////////////////////////////////////////////////////////
DataChain->GetEntry(jentry);  
//tigress->ResetAddback(); // Was annoyingly important
/////////////////////////////////////////////////////////
///////////////////////////////////
//////// Do timestamp & RF ////////
///////////////////////////////////

long tstamp=t_stamp(rf,tigress,sili,s3);//Fetch a time stamp from whichever of these is in the event, adjusted for the tigress high low griffin grsisort mismatch
if(tstamp==-1)stampfail->Fill(jentry);
if(newfile)filetime->SetBinContent(fileiterator+1,tstamp);

bool RFfail=true;
// double trig_cfd=-1;
// double rf_cfd=-1;
double rf_ns=-1;
// double rf_t=0;
double rf_phase=0;
if(rf){
	if(abs(tstamp-rf->TimeStamp())<100000){//Had to add this because of some errors when RF only PART missing
// 		rf_cfd=rf->GetTimeFitCfd();
		rf_ns=rf->GetTimeFitns();
// 		rf_t=rf->Time()/10.0;//ns->ticks
		rf_phase=rf->Phase();
// 		trig_cfd=rf->GetTimestampCfd()-5000;
// 		if(rf_cfd>0){RFfail=false;rf_cfd-=3800;}//Manually derived offset for waveform delay
		if(rf_ns>0){RFfail=false;rf_ns-=2375;}//Manually derived offset for waveform delay
	}
}


// We wish to have a more or less continuous timestamps.
// This means adjusting for missing time if any subruns are removed
// and adjusting for the timestamp resets at the start of a new run.
//
// Additionally timestamp has (occasional) zero values and erroneously high values.
// Problems particularly occur at start and end of runs
//
// The basic premiss is as follows:
// We assume rates of >>100 per minute.
// A long (128) moving average stores the approximate current timestamp.
// The long average is only added to if the time stamp is "good" 
// The long average is reset when we accept a major time stamp change.
//
// If latest time is far (~2 minutes) from long average this registers as a "bad" time stamp
// 
// Whenever the time stamp is "bad" the previous "good" time is used in its place
//
// If the timestamp is "bad" for several (16) events it is probably a true time change
// The long average is reset to the new "timeadd" value is adjusted.
//
// We also (currently) reset the timestamp at the start of all subruns
// This may be overkill but as the most common reset are missing subruns and new runs its easiest


long tstampraw=tstamp;
if(tstamp>0&&tstamp<timestamp_max){//If time stamp is crazy ignore	
	
	bool good_time=true;

	//check if the  latest value is "good"
	//check if we are more than X minutes from average previous events
	if(abs(tstamp-(movelong>>7)) > tpgap){//Time IS BAD
		good_time=false;
		overTlimit++;
		//Never reset near end of run, just stay in "bad" state.
		if(jentry+128>fileentriessum[fileiterator]){
			overTlimit=0;
		}
		
		//Just use last good value
		tstamp=movelonghold[movelongi];
	}else{// Time IS good
		overTlimit=0;
		
		//Fill the long averages
		movelongi++;if(movelongi==128)movelongi=0;//move iterator
		movelong+=tstamp;movelong-=movelonghold[movelongi];//update average
		movelonghold[movelongi]=tstamp;//Add entry
	}
	
	// Once we are in time "bad" state 16 times in a row its probably NOT just a blip
	if(overTlimit>=16){
		//A disk access intensive last ditch check to see if the time stamp REALLY changes
		// and doesn't just glitch
		long futurestamp=0;
		if(jentry+128<nentries){
			DataChain->GetEntry(jentry+128);
			futurestamp=t_stamp(rf,tigress,sili,s3);
			DataChain->GetEntry(jentry);
			
			if(abs(futurestamp-(movelong>>7)) < tpgap)overTlimit=0;
		}
	}
	
	// If we have been in bad for a long time or new run
	// Accept the new times and adjust accordingly
	if(overTlimit>=16||newfile){
		overTlimit=0;
		tstamp=tstampraw;
		
		if((movelonghold[movelongi]>tstamp)||RemoveTimeGaps){
			timestamp_add+=movelonghold[movelongi]-tstamp;
		}		
		
		//Reset the averages etc
		for(int i=0;i<128;i++)movelonghold[i]=tstamp;
		movelong=(tstamp<<7);

	}
	
	tstamp+=timestamp_add;//Adjust time stamp to be continuous

}else{
	tstamp=movelonghold[movelongi]+timestamp_add;
}

//Now we have a timestamp fill some histograms
// eventrate->Fill(tstamp);

float tstamphour=tstamp/(float)timestamp_hour;

eventratehours->Fill(tstamphour);
if(nentries>5000){//Trying to fix a segfault from very small runs
	if(!(jentry%(nentries/10000))){
		//Changed to "set bin content" rather than weighted fill, despite trying to make bins match, some double filling
		int bin=runtimeraw->FindBin(jentry);
		runtimeraw->SetBinContent(bin,tstampraw/(float)timestamp_hour);
		runtime->SetBinContent(bin,tstamphour);
		timeadda->SetBinContent(bin,timestamp_add/(float)timestamp_hour);
	}
}

//// check the rawrest fragment count we can in the analysis tree

S3fragrate->Fill(jentry,s3->GetRingMultiplicity()+s3->GetSectorMultiplicity());
eventsS3ring+=s3->GetRingMultiplicity();
eventsS3sector+=s3->GetSectorMultiplicity();

if(DS){
	silifragrate->Fill(jentry,sili->GetMultiplicity());
	eventssili+=sili->GetMultiplicity();
}
gammafragrate->Fill(jentry,tigress->GetMultiplicity()+tigress->GetBGOMultiplicity());

eventstigress+=tigress->GetMultiplicity();
eventsbgotigress+=tigress->GetBGOMultiplicity();


//////////////////////////////////////////////////////////
//////// Do the TGenericDetector / monitor singles ///////
//////////////////////////////////////////////////////////

if(AddMonitor){
	double sumenergy=0;//currently single clover
	for(unsigned int i=0;i<gd->GetMultiplicity();i++){
		sumenergy+=gd->GetHit(i)->GetEnergy();
	}
	if(sumenergy){
		monitortotal->Fill(sumenergy);
		monitor_runtime->Fill(tstamphour,sumenergy);
	}
}

///////////////////////////////
//////// Do S3 singles ////////
///////////////////////////////


//Get hits for rings S3 (dE in telescope)
unsigned short apmult[4]={0,0,0,0};
for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
	TS3Hit* SR=s3->GetRingHit(i);
	unsigned short id=s3id(SR);
	if(MultiS3)if(id<0)continue;
	apmult[id]++;
	int s=SR->GetSegment();
	chanhits[id]->Fill(s);
	
	double e=SR->GetEnergy();
	if(e>control[S3EnergyLimit]*0.004){//noise gate
// 		double T=SH->GetTime()*1.6;
		
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
if(MultiS3)for(unsigned short i=0;i<4;i++){
	if(s3used[i]){
		S3_ringmult[s3index[i]]->Fill(apmult[s3index[i]]);
		apmult[s3index[i]]=0;
	}
}
else S3_ringmult[0]->Fill(s3->GetRingMultiplicity());


//Get hits for E S3 (sectors)
for(unsigned int i=0;i<s3->GetSectorMultiplicity();i++){
	TS3Hit* SS=s3->GetSectorHit(i);
	unsigned short id=s3id(SS);
	if(MultiS3)if(id<0)continue;
	apmult[id]++;
	
	int s=SS->GetSegment();
	chanhits[id]->Fill(s+24);
	
	double e=SS->GetEnergy();
	if(e>control[S3EnergyLimit]*0.004){//increased noise gate
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
		for(unsigned int j=0;j<s3->GetRingMultiplicity();j++){
			TS3Hit* SR=s3->GetRingHit(j);
			if(MultiS3)if(SS->GetArrayPosition()!=SR->GetArrayPosition())continue;
			
			double re=SR->GetEnergy();
			if(re>control[S3EnergyLimit]*0.004){//noise gate
				int r=SR->GetSegment();
				
				double TT=SR->GetTime()-SS->GetTime();
				bool gT=(abs(TT)<control[FrontBackTime]);
				
				S3RS_t[id]->Fill(TT);
				if(gT)S3RS_tgate[id]->Fill(TT);

				if(!Telescope){
					frontVback[id]->Fill(re,e);
					front_back[id]->Fill(e-re);
					// Note this MUST mirror the constraint in GRSISort TS3.cxx BuildPixels() or these histograms are meaningless
					if((e-control[FrontBackOffset])*control[FrontBackEnergy]<re&&(re-control[FrontBackOffset])*control[FrontBackEnergy]<e){
						front_backgated[id]->Fill(e-re);
						frontVbackGated[id]->Fill(re,e);
						fb_time[id]->Fill(TT);
						if(gT)fb_timegated[id]->Fill(TT);
					}
					else continue;
				}
				
				if(Telescope)S3RS_t3[id]->Fill(TT,re,e);
// 				if(!RFfail)S3RS_RF->Fill((SR->GetTime()-rf_ns),(SS->GetTime()-rf_ns));
				
// 				if(gT){
// 					if(!RFfail)S3RS_RFgated->Fill((SR->GetTime()-rf_ns),(SS->GetTime()-rf_ns));
// 					//S3sectorsout[s]->Fill(SS->GetCharge(),r);
// 				}
			}
		}
	}
}
if(MultiS3)for(unsigned short i=0;i<4;i++){
	if(s3used[i]){
		S3_sectormult[s3index[i]]->Fill(apmult[s3index[i]]);
		apmult[s3index[i]]=0;
	}	
}
else S3_sectormult[0]->Fill(s3->GetSectorMultiplicity());

//////////////////////////////////////////
//////// Make S3 complete events ////////
//////////////////////////////////////////


std::vector< TVector3 > S3pos,S3possmear;
std::vector< vector < bool > > S32D(ParticleGate.size(),vector < bool >());
std::vector< double > S3Trf;
std::vector< TS3Hit* > S3select;
std::vector< double > Vdedx;

// Build the combined hits we will use for coincidences
for(unsigned int i=0;i<s3->GetPixelMultiplicity();i++){//GetPixelMultiplicity builds the events based on pre-set settings
	TS3Hit* SH=s3->GetS3Hit(i);
	unsigned short id=s3id(SH);
	if(MultiS3)if(id<0)continue;
	apmult[id]++;
	
	//RF gate for singles triggers
	double TT=SH->GetTime()-rf_ns;		
// 	if(!RFfail){
// 		S3_rf->Fill(TT);
// 		if(t_gate(TT,rf_S3)){//RF gate
// 			S3_rfgate->Fill(TT);
// 			if(t_gateRFcycles(TT,rfcyc_S3)){//New RF cycle gate
// 				S3_rfgatecyc->Fill(TT);
// 			}
// 		}
// 	}
	S3Trf.push_back(TT);
	
	double dE=SH->GetEnergy();
	double E=dE;

	TVector3 pos = SH->GetPosition(true)+S3OffsetVector;
//	TVector3 pos = TS3::GetPosition(S3ring_i[i],S3sec_i[j],-22.5*TMath::Pi()/180.,32.1,Telescope,false);//Get the hit vector with some random smoothing	
	S3_map[id]->Fill(pos.X(),pos.Y());//This is more for online checking of hit map
	S3_map3->Fill(pos.Z(),pos.X(),pos.Y());//This is more for online checking of hit map
	S3possmear.push_back(pos);	
	
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
		
		// This loop checks if (when there are multiple S3s) the gate is set for only a specific S3
		if(MultiS3&&ParticleGate[g].s3Limit){
			goodgate=false;
			for(unsigned short s=0;s<ParticleGate[g].s3.size();s++){
				if(SH->GetArrayPosition()==ParticleGate[g].s3[s]){
					goodgate=true;
					break;
				}
			}
		}		
		
		// If this is a valid S3 check the gate
		if(goodgate&&ParticleGate[g].gate.IsInside(E,dE)){
			S32D[g].push_back(true);
			S3particleGated[g]->Fill(E,dE);
		}else{
			S32D[g].push_back(false);
		}
	}	

	S3select.push_back(SH);
	S3pos.push_back(SH->GetPosition(false)+S3OffsetVector);
	if(Telescope)Vdedx.push_back(dE);

}
int S3N=S3select.size();

// Do the multiplicity hits for the S3 detectors separately if there is more than one.
if(MultiS3){
	for(unsigned short i=0;i<4;i++){if(s3used[i])S3_mult[s3index[i]]->Fill(apmult[s3index[i]]);}
	S3_multot->Fill(s3->GetPixelMultiplicity());
}else S3_mult[0]->Fill(s3->GetPixelMultiplicity());


// Count the multiplicity of each S3 kinematic/particle gate
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
	if(GainDrift)e=e*FileGain+FileOffset;
	
	
// 	fileN_silinoise->Fill(fileiterator,e);
// 	eventN_silinoise->Fill(jentry,e);
// 	runtime_silinoise->Fill(tstamphour,e);
	
	//Moved from addback section so noise and mess is underflow bin
	runtime_sili->Fill(tstamphour,e);
	fileN_sili->Fill(fileiterator,e);	
	eventN_sili->Fill(jentry,e);	
	
	rawsilisum+=e;
	if(e>10&&e<4000){//noise gate and common sense gate
		int s=sili_hit->GetSegment();
		if(s>=0&&s<120){
			SiLi_raw->Fill(e);
			if(sili->GetMultiplicity()==1)SiLi_rawm1->Fill(e);
			TVector3 pos = sili_hit->GetPosition(true);
	// 		TVector3 pos = TSiLi::GetPosition(sili_hit->GetRing(),sili_hit->GetSector(),true);
			SiLi_map->Fill(-pos.X(),pos.Y());
		
			SiLiflat->Fill(s,e);
			SiLipreamp->Fill(sili_hit->GetPreamp(),e);
			silienergy[s]->Fill(e);
			int r=sili_hit->GetRing();
			SiLiring->Fill(r,e);
			SiLisector->Fill(sili_hit->GetSector(),e);
			//if(r>=0&&r<10)silirings[r]->Fill(e);
		}
	}
}}

std::vector< double > SiLiE,SiLiEdop,SiLit,SiLitRF;
std::vector< TSiLiHit* > SiLii;
// std::vector< bool > SiLiRF;
std::vector< bool > SiLiRFcyc;


//Next we do the addback selected hits that we will use for the rest
if(DS)SiLiAdd_mult->Fill(sili->GetAddbackMultiplicity());
if(DS){for(int i=0;i<sili->GetAddbackMultiplicity();i++){
	sili_hit = sili->GetAddbackHit(i);
	double e=sili_hit->GetEnergy();
	
	// Moved here as we dont want to zero energy and before the addback neighbour check.
	// If we every to actual addback this may need changing somehow
	if(UseFitCharge){
		double c=sili_hit->GetFitCharge();
		if(c>0){
			//silifitc[s]->Fill(c);
			double smirn=sili_hit->GetSmirnov();
			double sigtonoise=1./sili_hit->GetSig2Noise();
			SiLi_smirnov->Fill(smirn,e);
			SiLi_smirnov_C->Fill(smirn/c,e);
			SiLi_signoise->Fill(sigtonoise,e);
			
			short noisegood=0;
			if(sigtonoise<control[SiLiNoiseLimit]){
				SiLi_signoisecut->Fill(sigtonoise,e);
				noisegood++;
			}
			if((smirn/c)<control[SiLiSmirnovLimit]){
				SiLi_smirnov_Ccut->Fill(smirn/c,e);
				noisegood++;
			}
			
			if(noisegood<2){
				sili_hit->SetEnergy(0);
				e=0;
				continue;
			}
		}
	}
	
	
	if(e>10&&e<4000){//noise gate and common sense gate
		
		if(GainDrift)e=e*FileGain+FileOffset;
		
		double Nadd=sili_hit->GetAddbackSize();
		if(Nadd==1){//if its a good clean hit add it
			double ft=sili_hit->GetTimeFit();
			if(ft>50){//garbage events have early fit time
			
			
				if(sili_hit->MagnetShadow()){
					SiLi_magshad->Fill(e);
					SiLi_shadRing->Fill(sili_hit->GetRing(),e);
				}else{
					SiLi_nagshad->Fill(e);
					SiLi_nagshadRing->Fill(sili_hit->GetRing(),e);
				}
				
// 				double edop=sili_hit->GetDoppler(control[BetaZero]);
				
				//Calc a "cfd" time from the fit and timestamp
// 				double T=sili_hit->GetTimeFitCfd()-control[SiLiWaveTOffset];
				double T=sili_hit->GetTimeFitns()-control[SiLiWaveTOffset];
				
				SiLiE.push_back(e);
				SiLiEdop.push_back(e);
				SiLii.push_back(sili_hit);
			
				SiLit.push_back(T);	
				
				SiLi_singles->Fill(e);

				//1D RF check appropriate for singles DAQ triggers 
				double TT=T-rf_ns;//else{TT=((T-rf_cfd)/16)-250;}
				
				SiLitRF.push_back(TT);
// 				SiLiRF.push_back(false);
				SiLiRFcyc.push_back(false);
				if(!RFfail){
					SiLi_rf->Fill(TT);
					SiLi_rf2->Fill(e,TT);
						
					if(t_gate(TT,rf_sili)){//RF gate
// 						SiLiRF[SiLiRF.size()-1]=true;
// 						SiLi_RFgated->Fill(e);
// 						SiLi_rfgate->Fill(TT);
						
						if(t_gateRFcycles(TT,rfcyc_sili)){//New RF cycle gate
							SiLi_rfgatecyc->Fill(TT);
							SiLi_RFgatedcyc->Fill(e);
							SiLiRFcyc[SiLiRFcyc.size()-1]=true;
						}else{
							SiLi_RFantigatedcyc->Fill(e);
						}
						
					}else{
// 						SiLi_RFantigated->Fill(e);
					}
					

					
				}
			}
			SiLi_fit_time->Fill(ft);
			SiLi_fit_timeS->Fill(ft,sili_hit->GetSegment());
			SiLi_fit_timeE->Fill(ft,e);
		}else{
			//For the multi hitclusters do the addback histograms
			for(int j=0;j<Nadd;j++)SiLi_unaddback->Fill(sili_hit->GetAddbackEnergy(j));
			SiLi_addback->Fill(e);
		}
	}
}}
int SiLiN=SiLii.size();

if(DS){
	double rejsum=0;
	for(int i=0;i<sili->GetRejectMultiplicity();i++){
		sili_hit = sili->GetRejectHit(i);
		double e=sili_hit->GetEnergy();
		if(e>10)SiLi_rejected->Fill(e);
		rejsum+=e;
	}
	if(rejsum>10)SiLi_rejected_sum->Fill(rejsum);
}

//////////////////////////////////
//////// Do gammas singles ///////
//////////////////////////////////

//Quick Addback check
Tig_rawmult->Fill(tigress->GetMultiplicity());
Tig_bgomult->Fill(tigress->GetBGOMultiplicity());
for(int i=0;i<tigress->GetMultiplicity();i++){
	tigress->GetTigressHit(i)->ClearTransients();
	
	Gamma_no_add->Fill(tigress->GetTigressHit(i)->GetEnergy());
	Gamma_Core->Fill(tigress->GetTigressHit(i)->GetArrayNumber(),tigress->GetTigressHit(i)->GetEnergy());
	Gamma_Core_Charge->Fill(tigress->GetTigressHit(i)->GetArrayNumber(),tigress->GetTigressHit(i)->GetCharge());
	for(int j=i+1;j<tigress->GetMultiplicity();j++){
		double TT=tigress->GetTigressHit(i)->GetTime()-tigress->GetTigressHit(j)->GetTime();
		if(t_gate(TT,gamma_gamma_t)){
			Gamma_Gamma_no_add->Fill(tigress->GetTigressHit(i)->GetEnergy(),tigress->GetTigressHit(j)->GetEnergy());
			Gamma_Gamma_no_add->Fill(tigress->GetTigressHit(j)->GetEnergy(),tigress->GetTigressHit(i)->GetEnergy());
		}
	}
}
std::vector< TTigressHit* > gammai;
std::vector< double > gammaE,gammaEdop,gammaTrf;
std::vector< short > gammaAng;
// std::vector< bool > gammaRF;
std::vector< bool > gammaRFcyc;
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
		runtime_gamma->Fill(tstamphour,e);
		fileN_gamma->Fill(fileiterator,e);
		eventN_gamma->Fill(jentry,e);
		
		TVector3 pos=TTigress::GetPosition(tigress_hit->GetDetector(), tigress_hit->GetCrystal(), 0, 0, true);
		if(e>200){//extra theshold for these plots
			TigressHitMapLow->Fill(pos.Phi(),pos.Theta());
			TigressHitMap3->Fill(pos.X(),pos.Y(),pos.Z());
		}
		
		pos=tigress_hit->GetPosition();//No smear
		gammapos.push_back(pos);
		if(e>200)TigressHitMap->Fill(pos.Phi(),pos.Theta());
		
		short tigang=TigAng(tigress_hit);
		gammaAng.push_back(tigang);
		TigressETheta->Fill(e,tigang);
		
		//RF check
		double TT=tigress_hit->GetTime()-rf_ns;
		gammaTrf.push_back(TT);
// 		gammaRF.push_back(false);
		gammaRFcyc.push_back(false);
		if(!RFfail){//IF there is an RF
			Gamma_rf->Fill(TT);
			EGamma_rf2->Fill(e,TT);
			if(t_gate(TT,rf_gamma)){//RF gate
// 				Gamma_rfgate->Fill(TT);
// 				Gamma_RFgated->Fill(e);
// 				gammaRF[gammaRF.size()-1]=true;
				
				if(t_gateRFcycles(TT,rfcyc_gamma)){//New RF cycle gate
					Gamma_rfgatecyc->Fill(TT);
					Gamma_RFgatedcyc->Fill(e);
					gammaRFcyc[gammaRFcyc.size()-1]=true;
				}else{
					Gamma_RFantigatedcyc->Fill(e);
				}				
				
			}else{
// 				Gamma_RFantigated->Fill(e);
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
			double TT=gammai[i]->GetTime()-SiLit[j];
			Gamma_SiLi_t->Fill(TT);
			Gamma_SiLi_twide->Fill(TT);
			Gamma_SiLi_t2->Fill(gammaE[i],TT);
			Gamma_SiLi_t3->Fill(gammaE[i],SiLiE[j],TT);
			
			if(t_gate(TT,gamma_sili_t)){
				Gamma_SiLi_tgate->Fill(TT);

// 				runtime_gammasili->Fill(tstamphour,gammaE[i],SiLiE[j]);
// 				eventN_gammasili->Fill(jentry,gammaE[i],SiLiE[j]);
				Gamma_SiLi->Fill(gammaE[i],SiLiE[j]);
				if(!SiLii[j]->MagnetShadow())SiLiGamma_nagshad->Fill(gammaE[i],SiLiE[j]);
				GammaSiLiPlus->Fill(gammaE[i]+SiLiE[j]);
				GammaSiLiPlus_Gamma->Fill(gammaE[i]+SiLiE[j],gammaE[i]);
				GammaSiLiPlus_SiLi->Fill(gammaE[i]+SiLiE[j],SiLiE[j]);
				
				if(!RFfail)Gamma_SiLi_SiliRF->Fill(gammaE[i],SiLiE[j],SiLitRF[j]);
				
// 				if(gammaRF[i]&&SiLiRF[j]){
// 					SiLiGamma__RFgated->Fill(gammaE[i],SiLiE[j]);
// 				}
				
				if(gammaRFcyc[i]&&SiLiRFcyc[j]){
					SiLiGamma__RFgatedcyc->Fill(gammaE[i],SiLiE[j]);
				}
				
				Gamma_SiLi_RFgated->Fill(gammaTrf[i],SiLitRF[j]);
			}
			
			Gamma_SiLi_RF->Fill(gammaTrf[i],SiLitRF[j]);
		}
	}
	
	//////// Do gammas + gammas ////////
	for(unsigned int j=i+1;j<gammaN;j++){
		double TT=gammai[i]->GetTime()-gammai[j]->GetTime();
		GG_t->Fill(TT);
		
		Gamma_Gamma_t3->Fill(gammaE[i],gammaE[j],TT);
		Gamma_Gamma_t3->Fill(gammaE[j],gammaE[i],TT);		
		
		if(t_gate(TT,gamma_gamma_t)){
			GG_tgate->Fill(TT);
			
			Gamma_Gamma->Fill(gammaE[i],gammaE[j]);
			Gamma_Gamma->Fill(gammaE[j],gammaE[i]);

			
			TigressEETheta->Fill(gammaE[i],gammaE[j],gammaAng[j]);
			TigressEETheta->Fill(gammaE[j],gammaE[i],gammaAng[i]);
			
			TigressEEdTheta->Fill(gammaE[i],gammaE[j],gammapos[i].Angle(gammapos[j]));
			TigressEEdTheta->Fill(gammaE[j],gammaE[i],gammapos[i].Angle(gammapos[j]));
			
// 			runtime_gammagamma->Fill(tstamphour,gammaE[i],gammaE[j]);
// 			runtime_gammagamma->Fill(tstamphour,gammaE[j],gammaE[i]);
// 			eventN_gammagamma->Fill(jentry,gammaE[i],gammaE[j]);
// 			eventN_gammagamma->Fill(jentry,gammaE[j],gammaE[i]);	
			
// 			if(gammaRF[i]&&gammaRF[j]){
// 				GammaGamma_RFgated->Fill(gammaE[i],gammaE[j]);
// 				GammaGamma_RFgated->Fill(gammaE[j],gammaE[i]);
// 			}
			if(gammaRFcyc[i]&&gammaRFcyc[j]){
				GammaGamma_RFgatedcyc->Fill(gammaE[i],gammaE[j]);
				GammaGamma_RFgatedcyc->Fill(gammaE[j],gammaE[i]);
			}					

			//////// Do gammas + gammas + gammas ////////
			for(unsigned int k=j+1;k<gammaN;k++){
				double TTT=gammai[i]->GetTime()-gammai[k]->GetTime();
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


// matrix of size: Number-of-Gammas x Number-of-S3-gates-definitions
// store the multiplicity of the hits in the S3 particle gates coinident with each gamma ray
// Useful for many particle evaporation channels and/or when one wants to avoid multiple filling

std::vector< std::vector< unsigned short > > SiLiHitPGMulti;
std::vector< std::vector< unsigned short > > GammaHitPGMulti;

if(FirstOnly||MultiParticles){
	SiLiHitPGMulti= std::vector< std::vector< unsigned short > > (SiLiN,std::vector< unsigned short >(S32D.size(),0));
	GammaHitPGMulti= std::vector< std::vector< unsigned short > > (gammaN,std::vector< unsigned short >(S32D.size(),0));
}

//
// Entire loop goes over each S3 hit only once
// But over electrons/gammas a few times
//
for(unsigned int j=0;j<S3N;j++){
	TS3Hit* SH=S3select[j];
	
	// Vector stores if SiLi hit is time-coincident with this S3 hit
	std::vector< double > SiLiS3loop(SiLiN,0);		
	
	// Do S3 and SiLi coincidence check
	// Fill histograms that only depend on an S3 hit, not on particle gates
	if(DS){for(unsigned int i=0;i<SiLiN;i++){
		double TT=SiLit[i]-SH->GetTime();
		double e=SiLiE[i];

		SiLi_S3_t->Fill(TT);
		SiLi_S3_twide->Fill(TT);
		SiLi_S3_t2->Fill(e,TT);
		if(e>180)SiLiSegT->Fill(SiLii[i]->GetSegment(),TT);
		SiLiS3loop[i]=TT;
		
		if(!RFfail){//If there is RF data
			S3_SiLi_RF->Fill(S3Trf[j],SiLitRF[i]);
			S3_SiLi_RFe->Fill(S3Trf[j],SiLitRF[i],e);
		}
		
		if(t_gate(TT,s3_sili_t)){
			SiLi_S3_tgate->Fill(TT);
			if(!RFfail)S3_SiLi_RFgated->Fill(S3Trf[j],SiLitRF[i]);
		}
	}}
	
	// As SiLi
	//// Do S3 gamma coincidence check ////
	std::vector< double > gammaS3loop(gammaN,false);
	for(unsigned int i=0;i<gammaN;i++){
		double TT=gammai[i]->GetTime()-SH->GetTime();
		Tig_S3_t->Fill(TT,gammai[i]->GetArrayNumber());
		double e=gammaE[i];
		Gamma_S3_t->Fill(TT);
		Gamma_S3_t2->Fill(e,TT);
		gammaS3loop[i]=TT;
		if(!RFfail){
			Gamma_S3_RF->Fill(S3Trf[j],gammaTrf[i]);
			Gamma_S3_RFe->Fill(S3Trf[j],gammaTrf[i],e);
		}
		if(t_gate(TT,s3_gamma_t)){
			Gamma_S3_tgate->Fill(TT);		
			if(!RFfail)Gamma_S3_RFgated->Fill(S3Trf[j],gammaTrf[i]);
		}	
	}
	
	int mulparthist=0;//Counter for multi particle gates near the end of the next loop
	
	//// Iterate over all possible S3 kinematic/particle gates, for this S3 hit
	for(int g=0;g<S32D.size();g++){
	if(S32D[g][j]){// If S3 hit j is in particle gate g.
		// Any complexities for multiple physical S3s are dealt with at the S3 singles stage
		
		TVector3 particlevec=S3pos[j];
		
		gate2Ddata* ggate=&ParticleGate[g];//The gate settings (kinematics etc) for gate [g]
		
		// Get/Calculate beta value for a particle
		// Depends on S3Hit and Gate, no advantage to pre-calculating
		double betal=0;
		if(ggate->use_beta){
			betal=control[BetaZero];
			if(ggate->UseEnergyBeta){
				betal=calc_beta_KE(SH->GetEnergy()/1000.,ggate->mass);
			}else if(ggate->use_rb){
				unsigned int rrr=SH->GetRing();
				if(ggate->ring_beta.size()>rrr)betal=ggate->ring_beta[rrr];
			}else if(ggate->use_tb){
				betal=ggate->theta_beta.Eval(particlevec.Theta());
			}
		}
		
		// In a binary reaction when gating on the secondary particle in the S3
		// convert position vector from S3 particle to particle of interest
		// beta above should already be given correctly for particle of interest
		if(ggate->use_tt){
			particlevec.SetMagThetaPhi(1,ggate->theta_theta.Eval(particlevec.Theta()), particlevec.Phi()+TMath::Pi());
		}
		
		///////////////////////////////////////////
		// Do SiLi S3-Particle-Gate Coincidences //
		///////////////////////////////////////////
		std::vector< bool > SiLiGateGood(SiLiN,false);
		if(DS){for(unsigned int i=0;i<SiLiN;i++){
			double TS=SiLiS3loop[i];
			SiLiGTime[g]->Fill(TS);
		if(t_gate(TS,ggate->stgate)){//If SiLiHit i is time coincident with this S3hit 
			SiLiGdTime[g]->Fill(TS);
			SiLiGateGood[i]=true;
			
			// If we are tracking gate multiplicity
			if(FirstOnly||MultiParticles){
				SiLiHitPGMulti[i][g]++;
				
				if(FirstOnly)if(SiLiHitPGMulti[i][g]>1){
					// If FirstOnly. we are only filling histograms for each particle gate
					// once per SiLiHit, even if following S3Hits also satisfy the gate
					// Good for avoiding double counting, but not for multiple kinematic solutions
					SiLiEdop[i]=0;
					continue;
				}
			}
			
			//kinematic adjust
			double e;
			if(ggate->use_beta)e=SiLiE[i];//e=SiLii[i]->GetDoppler(betal,&particlevec,SiLiE[i]);
			else e=SiLiE[i];
			SiLiEdop[i]=e;
			SiLiPG[g]->Fill(e);
			if(SiLiRFcyc[i])SiLiPGRFcyc[g]->Fill(e);
			SiLiEPG[g]->Fill(e,SH->GetEnergy());	
		}}}
			

		////////////////////////////////////////////
		// Do Gamma S3-Particle-Gate Coincidences //
		////////////////////////////////////////////
		std::vector< bool > GammaGateGood(gammaN,false);
		for(unsigned int i=0;i<gammaN;i++){
			double TG=gammaS3loop[i];
			GammaGTime[g]->Fill(TG);
		if(t_gate(TG,ggate->gtgate)){//If GammaHit i is time coincident with this S3hit 
			GammaGdTime[g]->Fill(TG);
			GammaGateGood[i]=true;
			
			// If we are tracking gate multiplicity
			if(FirstOnly||MultiParticles){
				GammaHitPGMulti[i][g]++;
				if(FirstOnly)if(GammaHitPGMulti[i][g]>1){
					// If FirstOnly. we are only filling histograms for each particle gate
					// once per SiLiHit, even if following S3Hits also satisfy the gate
					// Good for avoiding double counting, but not for multiple kinematic solutions
					gammaEdop[i]=0;
					continue;
				}
			}
			
			double E;
			//kinematic adjust
			if(ggate->use_beta)E=gammai[i]->GetDoppler(betal,&particlevec);
			else E=gammai[i]->GetEnergy();
			gammaEdop[i]=E;
			
			// Fill a specific set of histograms only in use when particle gates have
			// Significant doppler shift for which correction must be optimised
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
			TigressEThetaPG[g]->Fill(E,gammaAng[i]);
			if(gammaRFcyc[i])GammaPGRFcyc[g]->Fill(E);
			
			if(Telescope){GammaS3dedx->Fill(SH->GetEnergy(),Vdedx[j],E);}
			
			//////////////////////////////////////////////////
			// Do Gamma-Gamma S3-Particle-Gate Coincidences //
			//////////////////////////////////////////////////
			
			// Goes backward as must use the tigress events which have already been checked
			// against this current S3Hit gate and kinematically corrected
			for(int m=i-1;m>=0;m--){
				if(GammaGateGood[m]&&gammaEdop[m]){
				double TT=gammai[m]->GetTime()-gammai[i]->GetTime();
				if(t_gate(TT,gamma_gamma_t)){
					// check GammaHit m is also coincident with this S3Hit & valid & time valid
					// In the case FirstOnly is being used gammaEdop[m]==0 if it was "used" 
					// by an earlier S3Hit that also matched this gate
					
					GammaGammaPG[g]->Fill(gammaEdop[i],gammaEdop[m]);
					GammaGammaPG[g]->Fill(gammaEdop[m],gammaEdop[i]);
					
					TigressEEThetaPG[g]->Fill(gammaEdop[i],gammaEdop[m],gammaAng[m]);
					TigressEEThetaPG[g]->Fill(gammaEdop[m],gammaEdop[i],gammaAng[i]);

					double ang=gammapos[i].Angle(gammapos[m]);
					TigressEEdThetaPG[g]->Fill(gammaEdop[i],gammaEdop[m],ang);
					TigressEEdThetaPG[g]->Fill(gammaEdop[m],gammaEdop[i],ang);
				}}
			}
			
			/////////////////////////////////////////////////
			// Do Gamma-SiLi S3-Particle-Gate Coincidences //
			/////////////////////////////////////////////////
			
			if(DS){for(unsigned int m=0;m<SiLiN;m++){
				if(SiLiGateGood[m]&&SiLiEdop[m]){
				if(t_gate((gammai[i]->GetTime()-SiLit[m]),gamma_sili_t)){
					GammaSiLiPG[g]->Fill(gammaEdop[i],SiLiEdop[m]);	
				}}
			}}
		}}//End of "Gamma S3-Particle-Gate Coincidences"
		// Still in particle-gate Loop inside S3Hit loop

		
		////////////////////////////////////////////
		// Do S3-Multi-Particle-Gate Coincidences //
		////////////////////////////////////////////
		if(MultiParticles)for(int k=g+1;k<S32D.size();k++){{//Iterate over all SUBSEQUENT gates
			for(unsigned int i=0;i<S3N;i++){//Iterate over ALL S3 hits
				if(i==j)continue;//Skip self hits
				//This covers all S3Hit and gate combinations without double counting
				
				if(S32D[k][i]){//If other hit is in other gate
					double thth=S3possmear[i].Theta()+S3possmear[j].Theta();//Angle difference
					if(thth>pi)thth=2*pi-thth;
					PhiTheta[mulparthist]->Fill(thth,S3possmear[j].DeltaPhi(S3possmear[i]));
				}
			}
			mulparthist++;
		}}
	}}//End of particle-gate loop
}//End of S3Hit loop


		
///////////////////////////////////////////////////
// Do S3-Multi-Particle-Gate + XXXX Coincidences //
///////////////////////////////////////////////////
if(MultiParticles){
	int mulparthist=0;
	for(int g=0;g<S32D.size();g++){//iterate over particle gates
		for(int k=g+1;k<S32D.size();k++){//iterate over other particle gates
		
			if(DS){for(unsigned int i=0;i<SiLiN;i++){//Iterate over SiLiHits
				if(SiLiHitPGMulti[i][g]&&SiLiHitPGMulti[i][k]){//If SiLiHit coincident with at least one S3Hit in both gates
					MultiPartSPICE[mulparthist]->Fill(SiLiE[i]);
					//multisA[mulparthist]->Fill(SiLiHitPGMulti[i][g],SiLiE[i]);
					//multisB[mulparthist]->Fill(SiLiHitPGMulti[i][k],SiLiE[i]);
					multisAB[mulparthist]->Fill(SiLiHitPGMulti[i][g],SiLiHitPGMulti[i][k],SiLiE[i]);
			}}}
			
			// Note: We are using NON-kinematically corrected energies here, as those are dependant on a single gate.
			
			for(unsigned int i=0;i<gammaN;i++){//Iterate over GammaHit
				if(GammaHitPGMulti[i][g]&&GammaHitPGMulti[i][k]){//If GammaHit coincident with at least one S3Hit in both gates
					MultiPartTig[mulparthist]->Fill(gammaE[i]);
					//multigA[mulparthist]->Fill(GammaHitPGMulti[i][g],gammaE[i]);
					//multigB[mulparthist]->Fill(GammaHitPGMulti[i][k],gammaE[i]);
					multigAB[mulparthist]->Fill(GammaHitPGMulti[i][g],GammaHitPGMulti[i][k],gammaE[i]);
				}
			}
			
			mulparthist++;
		}
		
		//
		// Only here after iterating over all S3Hits can we say how many particles of each gate
		// A given gamma/SiLiHit was in coincidence with
		//
		
		for(unsigned int i=0;i<gammaN;i++){
			if(GammaHitPGMulti[i][g])PGmultGamma[g]->Fill(GammaHitPGMulti[i][g],gammaE[i]);
		}

		if(DS)for(unsigned int i=0;i<SiLiN;i++){
			if(SiLiHitPGMulti[i][g])PGmultSili[g]->Fill(SiLiHitPGMulti[i][g],SiLiE[i]);
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
	double e=SiLiE[i];
	int segA=SiLii[i]->GetSegment();
	
	for(unsigned int j=i+1;j<SiLiN;j++){
		double ee=SiLiE[j];
		double TT=SiLit[i]-SiLit[j];
		ee_t->Fill(TT);
		if(abs(TT)<control[SiLiCoincidenceT]){
			ee_tgate->Fill(TT);
			int r=abs(SiLii[i]->GetRing()-SiLii[j]->GetRing());
			int s=abs(SiLii[i]->GetSector()-SiLii[j]->GetSector());
			
			int segB=SiLii[j]->GetSegment();
			
			if(r>1||(s>1&&s<11)){//if not neighbour
				
				SiLieeT->Fill(segA,TT);
				SiLieeT->Fill(segB,-TT);
				
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
				
				
				segseg->Fill(segA,segB);
				segseg->Fill(segB,segA);
				
				double eang=atan(e/ee);
				if(eang>(pi/4.))eang=(pi/2.)-eang;
				TVector3 A=SiLii[i]->GetPosition(true);
				TVector3 B=SiLii[j]->GetPosition(true);
				angmap->Fill(eang,A.X(),A.Y());
				angmap->Fill(eang,B.X(),B.Y());
				
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
	cout << setiosflags(ios::fixed) << std::setprecision(2) << percent << " % complete. File "<<filelist[fileiterator]<<", Time remaining "<<setfill('0')<<setw(2)<<hours<<":"<<setfill('0')<<setw(2)<<minutes<<":"<<setfill('0')<<setw(2)<<seconds<<"\r" << flush; 
}
