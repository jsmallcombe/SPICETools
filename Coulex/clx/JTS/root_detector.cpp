//This script is dependent on some of my libraries that root has pre-loaded through .rootrc
	
// holders for the cm ranges
double u[4]={-1,-1,-1,-1},l[4]={-1,-1,-1,-1};
double su[4],sl[4];

void repopcmranges(int AB,int AT,double lower, double upper, double energy);

void root_detector(int AB,int AT,int Z1,int Z2,double lab_low,double lab_high,string clxfile){//Mass beam, mass target, detector angles degrees, data
	
	/////////// hard coded variables //////////////////
	
	TFile in(clxfile.c_str());
	if(!in.IsOpen())return;
	
	
	stringstream outname;
	outname<<clxfile.substr(0,clxfile.find(".root"));
	outname<<"_det_"<<lab_low<<"_"<<lab_high<<".root";
	
	/////////////////////////////////////////////////////////////////////////////////////
	//////////// Convert all those TGraps from barns/str to barns/deg_theta /////////////
	/////////////////////////////////////////////////////////////////////////////////////

	// We integrate the str step and the divide by the theta coverage
	// his throws away the solid angle which is incorrect
	// but it is needed to do a simple integration over the specified angle later
	// we use a small angular step to make it more valid
	
	////////////////////////////////////////////
	////////////   Start Outputs 	////////////
	////////////////////////////////////////////
	
	TFile out(outname.str().c_str(),"RECREATE");// Open the file if it exists or make it
	gROOT->cd();

	
	repopcmranges(AB,AT,lab_low,lab_high,AB*4);
	// Make the director Angles and fill it with the 1D CM coverages
	// These change a TINY bit with angle, and we store that info too, but we just store the 1D stuff once
	out.mkdir("Angles");
	TGraph b_cm,t_cm,d_cm;
	TGraph* c=&b_cm;
	b_cm.SetPoint(b_cm.GetN(),0,0);
	t_cm.SetPoint(t_cm.GetN(),0,0);
	for(int i=0;i<4;i++){
		if(l[i]>0&&u[i]>0){
			c->SetPoint(c->GetN(),l[i],0);
			c->SetPoint(c->GetN(),l[i],1);
			c->SetPoint(c->GetN(),u[i],1);
			c->SetPoint(c->GetN(),u[i],0);
		}
		if(i==1)c=&t_cm;
	}
	b_cm.SetPoint(b_cm.GetN(),180,0);
	t_cm.SetPoint(t_cm.GetN(),180,0);


	d_cm.SetPoint(d_cm.GetN(),0,0);
	for(int i=0;i<4;i++){
		if(sl[i]>=0&&su[i]>0){
			d_cm.SetPoint(d_cm.GetN(),sl[i],0);
			d_cm.SetPoint(d_cm.GetN(),sl[i],1);
			d_cm.SetPoint(d_cm.GetN(),su[i],1);
			d_cm.SetPoint(d_cm.GetN(),su[i],0);
		}
	}
	d_cm.SetPoint(d_cm.GetN(),180,0);	
	out.cd("Angles");		
		b_cm.Write("b_cm",TObject::kOverwrite);
		t_cm.Write("t_cm",TObject::kOverwrite);
		d_cm.Write("d_cm",TObject::kOverwrite);
	gROOT->cd();

	
	////////////////////////////////////////////
	////////////  Individual levels	////////////
	////////////////////////////////////////////
	
	TGraph* G;
	
	for(int i=0;i<100;i++){ //iterate through the levels
		stringstream level;
		level<<"Level_"<<(i+1);//Title
		TDirectory* leveldir=in.GetDirectory(level.str().c_str());
		if(!leveldir)break;

		TGraph xb,xt,xs;
		
		//Iterate through energies of this level
		TIter next(leveldir->GetListOfKeys());
		TObject* object = 0;
		while ((object = next()))
		{
			string name=object->GetName();
			int nn=name.find("_MeV");
			if(!(nn<name.size()))continue;
			stringstream E;
			E<<name.substr(0,nn);
			double energy;
			E>>energy;
			
			TGraph* data_in=0;
			leveldir->GetObject(name.c_str(),data_in);//This is the only way to make the TGraph work
			if(!data_in)continue;
		
			//Because integrals of TGraphs are weird we reference it into a TF1
			TF1 f1("f",[&](double *x, double *){ return data_in->Eval(x[0]); },0,180,0);
			
			repopcmranges(AB,AT,lab_low,lab_high,energy);
					
			//For the second CM ranges that may not exist the integral area is zero so no need to check/remove
			double acc=1.e-6;//1.e-12 have to set lower because of unclear error
			double	cxb=f1.Integral(l[0],u[0],acc)+f1.Integral(l[1],u[1],acc);
			double	cxt=f1.Integral(l[2],u[2],acc)+f1.Integral(l[3],u[3],acc);
			double	cxs=f1.Integral(sl[0],su[0],acc)+f1.Integral(sl[1],su[1],acc)+f1.Integral(sl[2],su[2],acc)+f1.Integral(sl[3],su[3],acc);

			xb.SetPoint(xb.GetN(),energy,cxb);
			xt.SetPoint(xt.GetN(),energy,cxt);
			xs.SetPoint(xs.GetN(),energy,cxs);
		}
		
		out.mkdir(level.str().c_str());
		out.cd(level.str().c_str());
			xb.Write("xb",TObject::kOverwrite);
			xt.Write("xt",TObject::kOverwrite);
			xs.Write("xs",TObject::kOverwrite);
		gROOT->cd();
	}
	
	
	//////////////////////////////////////////////////////////////////////
	////////////// safe coulex angle as a function of energy /////////////
	//////////////////////////////////////////////////////////////////////
	
	TGraph safe;
	
	for(int i=0;i<100;i++){ //iterate through the levels
		double energy=((double)AB/10.0)*i;
		
		double safecm=safe_coulex_angle(AB,Z1,AT,Z2,energy); //[Inputs(A1,Z1,A2,Z2,E_lab)]	
		safe.SetPoint(safe.GetN(),energy,rad2deg(safecm));
	}
	
	out.cd();
		safe.Write("Safe_beam_CM",TObject::kOverwrite);
	gROOT->cd();
	
	/////////////////////////////////////////////////////////////////////////////////////////
	////////////// Rutherford cross section into detector at this energy //////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	
	TGraph rrxb,rrxt,rrxs;
	for(int i=10;i<100;i++){ //iterate through the levels
		double energy=((double)AB/10.0)*i;
		repopcmranges(AB,AT,lab_low,lab_high,energy);
	
		double ecm=get_com_KE(get_rel_mom(energy,AB),AB,AT);

		double	rxb=0,rxt=0,rxs=0;
		if(l[0]>0.0001)rxb+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(l[0]),deg2rad(u[0]));
		if(l[1]>0.0001)rxb+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(l[1]),deg2rad(u[1]));
		
		if(l[2]>0.0001)rxt+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(l[2]),deg2rad(u[2]));
		if(l[3]>0.0001)rxt+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(l[3]),deg2rad(u[3]));
		
		if(sl[0]>0.0001)rxs+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(sl[0]),deg2rad(su[0]));
		if(sl[1]>0.0001)rxs+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(sl[1]),deg2rad(su[1]));
		if(sl[2]>0.0001)rxs+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(sl[2]),deg2rad(su[2]));
		if(sl[3]>0.0001)rxs+=0.001*rutherford_crosssection(Z1,Z2,ecm,deg2rad(sl[3]),deg2rad(su[3]));
	
		rrxb.SetPoint(rrxb.GetN(),energy,rxb);
		rrxt.SetPoint(rrxt.GetN(),energy,rxt);
		rrxs.SetPoint(rrxs.GetN(),energy,rxs);
	}
		
	out.cd();
		rrxb.Write("Rutherford_x_beam",TObject::kOverwrite);
		rrxt.Write("Rutherford_x_targ",TObject::kOverwrite);
		rrxs.Write("Rutherford_x_comb",TObject::kOverwrite);
	gROOT->cd();
	
	/////////////////////////////////////////////////////
	////////////// beta of the particles  ///////////////
	/////////////////////////////////////////////////////
	
	double mid=deg2rad(lab_low+((lab_high-lab_low)*0.5));
	
	TGraph b1,b2;
	
	for(int i=1;i<100;i++){ //iterate through the levels
		double energy=((double)AB/10.0)*i;
		double* mom=kinetic_lab_calcs_elastic_P(energy,AB,AT,mid);
		if(mid<mom[2]){
			
			double bz_b=get_beta_mom(cos(mid)*mom[8],AB);
			double bz_t=get_beta_mom(cos(mom[5])*mom[9],AT);
				
			b1.SetPoint(b1.GetN(),energy,bz_b);
			b2.SetPoint(b2.GetN(),energy,bz_t);
			
		}
		delete mom;
	}
	
	out.cd();
		b1.Write("Beta_Z_beam_mid_det_foward_beam_hit",TObject::kOverwrite);
		b2.Write("Beta_Z_targ_mid_det_foward_beam_hit",TObject::kOverwrite);
	gROOT->cd();
	
	out.Close();

}


void repopcmranges(int AB,int AT,double lower, double upper, double energy){
	
	for(int i=0;i<4;i++){u[i]=-1;l[i]=-1;}
	
	/////////////////////////////////////////////////////////
	//////////// Calc the CM angles from the lab/////////////
	/////////////////////////////////////////////////////////
	
	// Fetch and convert to rad the detector angles
	higher_jd(lower,upper);//idiot check
	lower*=pi/180.0;
	upper*=pi/180.0;

	
	////// Beam ///////
	// Determine the possible CM angles for a given beam lab angle
	double* ret=kinetic_lab_calcs_elastic_P(energy,AB,AT,lower);
// 	kinetic_lab_calcs_readout(ret);
	
	if(ret[2]>lower){// ret[2] is the largest lab angle available to the beam, important in inverse kinematics
			// this checks the beam can ever reach the detector, before continuing
		
		l[0]=ret[6];l[1]=ret[12];// Save the two possible CM angles, if there is only one the second will be 0
		if(ret[2]<upper){// check if the beam can cover the whole detector
			u[1]=l[0];l[0]=-1;// in the case where it cannot there will be two CM solutions for lower angle
					  // and the CM range between the two covers the lab angles from lower to max
		}else{
			delete ret; //If it can reach the outer edge then fetch the one or two solutions for the upper in CM
			ret=kinetic_lab_calcs_elastic_P(energy,AB,AT,upper);
// 			kinetic_lab_calcs_readout(ret);
			u[0]=ret[6];u[1]=ret[12];// in the case of two u[] and l[] will be correctly paired up as both are given smallest CM angle first by "kinetic_lab_calcs_elastic_P"
		}
	}
	delete ret;
	
	
	////// Target ///////
	// Slightly different because we want to check angle of target for detection
	// But we want to save the CM angle of beam, because that is how the xsec distribution is given
	
	ret=kinetic_lab_calcs_P(energy,AB,AT,lower,AT,AB);//Switched beam and target on output of the "reaction" so we can give target angle
// 	kinetic_lab_calcs_readout(ret);
	
	if(ret[2]>lower){
		l[2]=ret[7];l[3]=ret[13];// Note different ret[] taken because we ask the CM angle of the opposite particle to the Lab angle input
		if(ret[2]<upper){
			u[3]=l[2];l[2]=-1;
		}else{
			delete ret;
			ret=kinetic_lab_calcs_P(energy,AB,AT,upper,AT,AB);
// 			kinetic_lab_calcs_readout(ret);
			u[2]=ret[7];u[3]=ret[13];
		}
	}
	delete ret;
	
	// Convert to degrees
	// Quickly sort the ordering as for a second CM solution the "lower" will have a larger CM angle than the "upper"
	for(int i=0;i<4;i++){
		l[i]*=180.0/pi;
		u[i]*=180.0/pi;
		higher_jd(l[i],u[i]);
	}
	
	
	////// EITHER ///////
	////// B or T //////
	////// SUM    //////
	
	// This is a little more involved because we dont want to just add the two together as that may double count xsec
	// We need to check for overlap in their CM angular ranges
	
	for(int i=0;i<4;i++){;su[i]=u[i];sl[i]=l[i];} //copy over the range delimiters 

	for(int i=0;i<4;i++){
		for(int j=i+1;j<4;j++){// compare each pair to all above it
			if((su[i]>sl[j])&&(sl[i]<su[j])){// If there is any overlap
				higher_jd(su[i],su[j]);// copy the outer limits to one entry
				higher_jd(sl[j],sl[i]);
				su[i]=-1;// delete the other
				sl[i]=-1;
				j=4;
			}
		}		
	}
	// Now we have dealt with any overlaps
	
	// It is possible any remaining separate ranges are not ordered
	// This is a problem for one of the following drawing commands
	for(int i=0;i<4;i++){
		for(int j=i+1;j<4;j++){
			if(sl[j]<sl[i]){
				swap_jd(sl[j],sl[i]);
				swap_jd(su[j],su[i]);
			}	
		}
	}

}