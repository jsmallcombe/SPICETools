//This script is dependent on some of my libraries that root has pre-loaded through .rootrc


void root_target(int AB,int AT,int Z1,int Z2,double mgcm2,double MeV,string detfile){//Beam, Target, input
	
	/////////// hard coded variables //////////////////
	
	TFile in(detfile.c_str());
	if(!in.IsOpen())return;
	
	if(MeV<=1)MeV=round(safe_coulex_beam(AB,Z1,AT,Z2,TMath::Pi()));	
	 
	stringstream outname;
	outname<<detfile.substr(0,detfile.find(".root"));
	outname<<mgcm2<<"_mg_"<<MeV<<"_MeV.txt";

	double partpergramm=6.022E+023/AT;
	double grammpercm=mgcm2/1000.;
	double parpercm=grammpercm*partpergramm;
	double parpermbarn=parpercm*1E-027;
	
	////////////////////////////////////////////
	////////////   Start Outputs 	////////////
	////////////////////////////////////////////
	
	ofstream out(outname.str());

	out<<"Cross-section calculations for "<<MeV<<" MeV "<<AB<<nuclear_data_ob::get_symb(Z1)<<" on "<<mgcm2<<" mg/cm2 "<<AT<<nuclear_data_ob::get_symb(Z2)<<endl;

	
	////////////////////////////////////////////
	////////////  Individual levels	////////////
	////////////////////////////////////////////
	
// 	TGraph* G;
// 	
	target tharget(AT,Z2,mgcm2);
// 	
	vector< double > fracE;
	for(int i=0;i<100;i++){
		fracE.push_back(tharget.beam_e_centre(Z1,AB,MeV,TVector3(0.0,0.0,1.0),0.01*i+0.005));
// 		cout<<endl<<fracE[i];
	}

	double a=fracE[0],b=fracE[50],c=fracE[99];
	out<<"Beam Energy "<<a<<" "<<b<<" "<<c<<" MeV"<<endl;
		
	for(int i=0;i<100;i++){ //iterate through the levels
		
		stringstream level;
		level<<"Level_"<<(i+1);//Title
		TDirectory* leveldir=in.GetDirectory(level.str().c_str());
		if(!leveldir)break;
		
		
		out<<level.str()<<endl;
	
		TGraph *xb=0,*xt=0,*xs=0;
		TGraph* data_in=0;
		leveldir->GetObject("xb",xb);//This is the only way to make the TGraph work
		leveldir->GetObject("xt",xt);
		leveldir->GetObject("xs",xs);
		if(!xb)continue;
		if(!xt)continue;
		if(!xs)continue;
		
		double xbsum=0,xtsum=0,xssum=0;
		
		for(int i=0;i<100;i++){
			xbsum+=xb->Eval(fracE[i])*0.01;
			xtsum+=xt->Eval(fracE[i])*0.01;
			xssum+=xs->Eval(fracE[i])*0.01;
		}
		xbsum*=1000;
		xtsum*=1000;
		xssum*=1000;		
		
		

// 		out<<setw(10)<<"(mbar)"<<setw(10)<<"beam"<<setw(10)<<"target"<<setw(10)<<"both"<<endl;
// 		out<<setw(10)<<"Front"<<setw(10)<<jsigfig(xb->Eval(a)*1000,4)<<setw(10)<<jsigfig(xt->Eval(a)*1000,4)<<setw(10)<<jsigfig(xs->Eval(a)*1000,4)<<endl;
// 		out<<setw(10)<<"Mid"<<setw(10)<<jsigfig(xb->Eval(b)*1000,4)<<setw(10)<<jsigfig(xt->Eval(b)*1000,4)<<setw(10)<<jsigfig(xs->Eval(b)*1000,4)<<endl;
// 		out<<setw(10)<<"Back"<<setw(10)<<jsigfig(xb->Eval(c)*1000,4)<<setw(10)<<jsigfig(xt->Eval(c)*1000,4)<<setw(10)<<jsigfig(xs->Eval(c)*1000,4)<<endl;
// 		out<<setw(10)<<"Integral"<<setw(10)<<jsigfig(xbsum,4)<<setw(10)<<jsigfig(xtsum,4)<<setw(10)<<jsigfig(xssum,4)<<endl<<endl;
// 		
// 	
// 		out<<setw(15)<<"(per particle)"<<setw(15)<<"beam"<<setw(15)<<"target"<<setw(15)<<"both"<<endl;
// 		out<<setw(15)<<"Integral"<<setw(15)<<jsigfig(xbsum*parpermbarn,4)<<setw(15)<<jsigfig(xtsum*parpermbarn,4)<<setw(15)<<jsigfig(xssum*parpermbarn,4)<<endl<<endl;
	
		out<<setw(15)<<" "<<setw(15)<<"beam"<<setw(15)<<"target"<<setw(15)<<"both"<<endl;
		out<<setw(15)<<"(mbarns)"<<setw(15)<<jsigfig(xbsum,4)<<setw(10)<<jsigfig(xtsum,4)<<setw(10)<<jsigfig(xssum,4)<<endl;
		out<<setw(15)<<"(per particle)"<<setw(15)<<jsigfig(xbsum*parpermbarn,4)<<setw(15)<<jsigfig(xtsum*parpermbarn,4)<<setw(15)<<jsigfig(xssum*parpermbarn,4)<<endl<<endl;
	}
	
	
	out.close();

}
