//This script is dependent on some of my libraries that root has pre-loaded through .rootrc

void root_parseclx(int AB,int AT,int Z1,int Z2,double energy){//Mass beam, mass target, beam energy MeV
	
	/////////// hard coded variables //////////////////
	
	ifstream in("tmp_clx_out_file.txt"); // clx data parsed by clx_mk_table_slim.awk

	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//////////// In this loop we read in the level population info to a series of TGraph ///////////
	////////////////////////////////////////////////////////////////////////////////////////////////
	
	std::vector< TGraph > data_in;
	
	double d_step=0;//angular step in degrees
	if(in.is_open()){
		string line;
		while(getline(in,line)){ //get a line of probs and xsecs for one angle
// 			cout<<line<<endl;
			stringstream ss(line);
			double entry;//data
			int i=0;//column counter
			int j=0;//level counter
			double x=0;//angle
			while(ss>>entry){//Process each column of the line
				if(i==0){//On the first column which is the angle
					x=entry;//get the angle
					if(d_step>0)d_step=d_step-x; //these two lines fetch the angular step
					if(d_step==0)d_step=x;
				}
				if(i>2){//skip Rutherford columns
					if(!(i%2)){//skip the odd numbered probability columns only take the barn xsec numbers
						if(j>=data_in.size())data_in.push_back(TGraph());//push back the level list, should only happen on row one
						data_in[j].SetPoint(data_in[j].GetN(),x,entry);//fill the relevant level for this angle
						j++;//count number of levels for this row, should always be the same
					}
				}
				i++;//iterate column counter
			}
		}
	}
	d_step=-d_step;
	
	in.close();// We have now loaded the data from each level in b/sr into a TGraph
	
	
	/////////////////////////////////////////////////////////////////////////////////////
	//////////// Convert all those TGraps from barns/str to barns/deg_theta /////////////
	/////////////////////////////////////////////////////////////////////////////////////

	// We integrate the str step and the divide by the theta coverage
	// his throws away the solid angle which is incorrect
	// but it is needed to do a simple integration over the specified angle later
	// we use a small angular step to make it more valid
	
	double r_step=(d_step/180.0)*TMath::Pi()*0.5;// half step in radians
	for(int i=0;i<data_in.size();i++){
		for(int j=0;j<data_in[i].GetN();j++){
			double X,Y;
			data_in[i].GetPoint(j,X,Y); //Get angle and barns/str
			double r=(X/180.0)*TMath::Pi();//angle in rad
			double str=0; 
			if(j>0)str+=(TMath::Cos(r-r_step)-TMath::Cos(r)); // Theta integral in two parts as the first and last angles in clx are half steps
			if(j+1<data_in[i].GetN())str+=(TMath::Cos(r)-TMath::Cos(r+r_step));
			str*=2*TMath::Pi(); // phi integral
			double perD=str*Y/d_step;//converted to barn/deg
			if(j==0||j+1==data_in[i].GetN())perD*=2;// Again adjusting for first and last angles in clx being half size steps
			data_in[i].SetPoint(j,X,perD);// Overwrite the original graph
		}	
	}
	
		
	////////////////////////////////////////////
	////////////   Start Outputs 	////////////
	////////////////////////////////////////////
	
	TFile out("temp.root","UPDATE");// Open the file if it exists or make it
	gROOT->cd();
	
	TGraph* G;
	
	for(int i=0;i<data_in.size();i++){ //iterate through the levels
		stringstream level;
		level<<"Level_"<<(i+1);//Title
		
		// Check if this level has a directory,make it if not,
		//should all be done on the first energy script call
		if(!out.GetDirectory(level.str().c_str())){
			out.mkdir(level.str().c_str());
			gROOT->cd();
		}
		
		out.cd(level.str().c_str());
			stringstream en;
			en<<energy<<"_MeV";
			data_in[i].Write(en.str().c_str(),TObject::kOverwrite);
		gROOT->cd();
	}
	
	out.Close();

}