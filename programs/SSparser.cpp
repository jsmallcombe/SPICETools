#include "spice_parse.h"
#include "spice_enviro.h"

int main(int argc, char *argv[]){
	
	bool parsed=SpiceParse(argc,argv);//This will parse and produce an output file if there are frags or analysis trees
	
	SPICEcmdline inp(argc,argv);

////////////////// OVERARCHING CONTROL VARIABLES //////////////////	

	string outputfile=OrDefault("spice_check.root",inp.RootFile());
	string path="energy2D";

	TFile *viewfile = new TFile(outputfile.c_str(),"READ");
	if(viewfile->IsOpen()&&(parsed||inp.LoadCal())){
			
		if(inp.IsPresent("Path")){
			cout<<endl<<"Please give energy/chan TH2 path/name within file: ";
			cin>>path;
		}
		
		int dummy=1;
		TApplication *app = new TApplication("app", &dummy, argv);
		SPICEenvironment* ssev= new SPICEenvironment(viewfile,path);cout<<"Showing data from file "<<outputfile<<" hist "<<path<<endl;
		ssev->Connect("Destroyed()", "TApplication", app, "Terminate()");
		app->Run();
	}else{
		if(!viewfile->IsOpen())cout<<endl<<"No sorted data file to view."<<endl;
		else cout<<endl<<"No Calibration file."<<endl;
	}

	return 0;
}
