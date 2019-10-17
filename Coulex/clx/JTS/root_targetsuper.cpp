//This script is dependent on some of my libraries that root has pre-loaded through .rootrc
	
void root_targetsuper(int AB,int AT,int Z1,int Z2,double mgcm2,double MeV,string clxfile){//Mass beam, mass target, detector angles degrees, data
		
	// holders for the cm ranges
	double *ret;
	
	/////////// hard coded variables //////////////////
	
	TFile in(clxfile.c_str());
	if(!in.IsOpen())return;
	
	if(MeV<=1)MeV=round(safe_coulex_beam(AB,Z1,AT,Z2,TMath::Pi()));	

	double partpergramm=6.022E+023/AT;
	double grammpercm=mgcm2/1000.;
	double parpercm=grammpercm*partpergramm;
	double parpermbarn=parpercm*1E-027;

	target tharget(AT,Z2,mgcm2);
	
	double detinnerrad=11.0;
	double detouterrad=35.0;
	
	stringstream outname;
	outname<<clxfile.substr(0,clxfile.find(".root"));
	outname<<"_super_"<<mgcm2<<"_mg_"<<MeV<<"_MeV.root";
	
	vector< double > fracE;
	for(int i=0;i<100;i++){
		fracE.push_back(tharget.beam_e_centre(Z1,AB,MeV,TVector3(0.0,0.0,1.0),0.01*i+0.005));
// 		cout<<endl<<fracE[i];
	}
		
	////////////////////////////////////////////
	////////////   Start Outputs 	////////////
	////////////////////////////////////////////
	
	TFile out(outname.str().c_str(),"RECREATE");// Open the file if it exists or make it
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

		TGraph xbd,xtd,xsd;
		
		//Iterate over detector positions
		for(int j=40;j>=-40;j--){
			if(j==0)j--;
			double lab_low=atan(detinnerrad/abs(j));
			double lab_high=atan(detouterrad/abs(j));
			if(j<0){lab_low=pi-lab_low;lab_high=pi-lab_high;}
			
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
				
				ret=DetectorCoMAnglesElastic(energy,AB,AT,lab_low,lab_high);
				
				
				for(int r=0;r<16;r++)ret[r]=rad2deg(ret[r]);
				
				//For the second CM ranges that may not exist the integral area is zero so no need to check/remove
				double acc=1.e-6;//1.e-12 have to set lower because of unclear error
				double	cxb=f1.Integral(ret[0],ret[1],acc)+f1.Integral(ret[2],ret[3],acc);
				double	cxt=f1.Integral(ret[4],ret[5],acc)+f1.Integral(ret[6],ret[7],acc);
				double	cxs=f1.Integral(ret[8],ret[9],acc)+f1.Integral(ret[10],ret[11],acc)+f1.Integral(ret[12],ret[13],acc)+f1.Integral(ret[14],ret[15],acc);
				
				delete ret;

				xb.SetPoint(xb.GetN(),energy,cxb);
				xt.SetPoint(xt.GetN(),energy,cxt);
				xs.SetPoint(xs.GetN(),energy,cxs);
			}
			
			double xbsum=0,xtsum=0,xssum=0;
		
			for(int e=0;e<100;e++){
				xbsum+=xb.Eval(fracE[e])*0.01;
				xtsum+=xt.Eval(fracE[e])*0.01;
				xssum+=xs.Eval(fracE[e])*0.01;
			}
			xbsum*=1000*parpermbarn;
			xtsum*=1000*parpermbarn;
			xssum*=1000*parpermbarn;

			xbd.SetPoint(xbd.GetN(),j,xbsum);
			xtd.SetPoint(xtd.GetN(),j,xtsum);
			xsd.SetPoint(xsd.GetN(),j,xssum);
			
			
		}
		
		xbd.SetTitle((level.str()+" populations per beam particle, beam detected").c_str());
		xbd.GetXaxis()->SetTitle("Detector Z [mm]");
		xbd.GetYaxis()->SetTitle("Populations per particle [p^{-1}]");

		xtd.SetTitle((level.str()+" populations per beam particle, target detected").c_str());
		xtd.GetXaxis()->SetTitle("Detector Z [mm]");
		xtd.GetYaxis()->SetTitle("Populations per particle [p^{-1}]");
		
		xsd.SetTitle((level.str()+" populations per beam particle, any detected").c_str());
		xsd.GetXaxis()->SetTitle("Detector Z [mm]");
		xsd.GetYaxis()->SetTitle("Populations per particle [p^{-1}]");
		
		out.mkdir(level.str().c_str());
		out.cd(level.str().c_str());
			xbd.Write("XsecBeam",TObject::kOverwrite);
			xtd.Write("XsecTarg",TObject::kOverwrite);
			xsd.Write("XsecBoth",TObject::kOverwrite);
		gROOT->cd();
	}
	
	///////////////////////////////////////
	/////// CM coverage
	///////////////////////////////////////
	
	vector<double> t1,t2,t3,t4,b1,b2,b3,b4,x;
	
	for(int j=40;j>=-40;j--){			
		if(j==0)j--;
		double lab_low=atan(detinnerrad/abs(j));
		double lab_high=atan(detouterrad/abs(j));
		if(j<0){lab_low=pi-lab_low;lab_high=pi-lab_high;}
		
		ret=DetectorCoMAnglesElastic(fracE[50],AB,AT,lab_low,lab_high);
		
		for(int r=0;r<16;r++)ret[r]=rad2deg(ret[r]);
				
		x.push_back(j);
		b1.push_back(ret[0]);b2.push_back(ret[1]);
		b3.push_back(ret[2]);b4.push_back(ret[3]);
		t1.push_back(ret[4]);t2.push_back(ret[5]);
		t3.push_back(ret[6]);t4.push_back(ret[7]);
	}
	
	TGraph Beam_detect;
	TGraph Targ_detect;

	for(int i=0;i<(signed)x.size();i++){
		if(b1[i]>0)Beam_detect.SetPoint(Beam_detect.GetN(),x[i],b1[i]);
		if(t1[i]>0)Targ_detect.SetPoint(Targ_detect.GetN(),x[i],t1[i]);
	}
	for(int i=(signed)x.size()-1;i>=0;i--){
		if(b1[i]>0)Beam_detect.SetPoint(Beam_detect.GetN(),x[i],b2[i]);
		if(t1[i]>0)Targ_detect.SetPoint(Targ_detect.GetN(),x[i],t2[i]);
	}
	for(int i=0;(signed)i<x.size();i++){
		if(b3[i]>0)Beam_detect.SetPoint(Beam_detect.GetN(),x[i],b3[i]);
		if(t3[i]>0)Targ_detect.SetPoint(Targ_detect.GetN(),x[i],t3[i]);
	}
	for(int i=(signed)x.size()-1;i>=0;i--){
		if(b3[i]>0)Beam_detect.SetPoint(Beam_detect.GetN(),x[i],b4[i]);
		if(t3[i]>0)Targ_detect.SetPoint(Targ_detect.GetN(),x[i],t4[i]);
	}
	Beam_detect.SetTitle("CM coverage when beam detected");
	Beam_detect.GetXaxis()->SetTitle("Detector Z [mm]");
	Beam_detect.GetYaxis()->SetTitle("Beam CoM angle [deg]");
	Targ_detect.SetTitle("CM coverage when target detected");
	Targ_detect.GetXaxis()->SetTitle("Detector Z [mm]");
	Targ_detect.GetYaxis()->SetTitle("Beam CoM angle [deg]");
	out.mkdir("Cover");
	out.cd("Cover");
		Beam_detect.Write("Beam_detect",TObject::kOverwrite);
		Targ_detect.Write("Targ_detect",TObject::kOverwrite);
	gROOT->cd();
		
	
	/////////////////////////////////////////////////////////////////////////////////////////
	////////////// Rutherford cross section into detector at this energy //////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	
	TGraph rrx,rrxb,rrxt;
	
	//Iterate over detector positions
	for(int j=40;j>=-40;j--){
		if(j==0)j--;
		double lab_low=atan(detinnerrad/abs(j));
		double lab_high=atan(detouterrad/abs(j));
		if(j<0){lab_low=pi-lab_low;lab_high=pi-lab_high;}
		
		double xsum=0;
		double xbsum=0;
		double xtsum=0;
		
		for(int i=0;i<100;i++){ //iterate through target
			double energy=fracE[i];
			
			double rxb=rutherford_crosssection_lab(AB,AT,Z1,Z2,energy,lab_low,lab_high,0);
			double rxt=rutherford_crosssection_lab(AB,AT,Z1,Z2,energy,lab_low,lab_high,1);
			double rxs=rxb+rxt;
						
			xsum+=rxs*0.01;
			xbsum+=rxb*0.01;
			xtsum+=rxt*0.01;
		}
		
		xsum*=parpermbarn;
		xbsum*=parpermbarn;
		xtsum*=parpermbarn;
		
		rrx.SetPoint(rrx.GetN(),j,xsum);
		rrxb.SetPoint(rrxb.GetN(),j,xbsum);
		rrxt.SetPoint(rrxt.GetN(),j,xtsum);
		
	}
		
	rrxb.SetTitle("Beam elastic scatters per beam particle");
	rrxb.GetXaxis()->SetTitle("Detector Z [mm]");
	rrxb.GetYaxis()->SetTitle("Scatters per particle [p^{-1}]");
	
	rrxt.SetTitle("Target elastic scatters per beam particle");
	rrxt.GetXaxis()->SetTitle("Detector Z [mm]");
	rrxt.GetYaxis()->SetTitle("Scatters per particle [p^{-1}]");
	
	rrx.SetTitle("Elastic scatters per beam particle");
	rrx.GetXaxis()->SetTitle("Detector Z [mm]");
	rrx.GetYaxis()->SetTitle("Scatters per particle [p^{-1}]");	
		
	out.cd();
		rrx.Write("Rutherford_x",TObject::kOverwrite);
		rrxb.Write("Rutherford_bx",TObject::kOverwrite);
		rrxt.Write("Rutherford_tx",TObject::kOverwrite);
	gROOT->cd();
	
	out.Close();

}
