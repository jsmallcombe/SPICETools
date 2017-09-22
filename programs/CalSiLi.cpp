#include "spice_calSiLi.h"

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"



int main(int argc, char *argv[]){
	int dummy=1;
	TApplication *app = new TApplication("app", &dummy, argv);
	SPICEcmdline inp(argc,argv);
	
	if(inp.IsPresent("MAKEBLANK")){
		MakeSiLiBlankCal(OrDefault("SPICE_blank_calfile.cal",inp.ReturnFind(".cal")));
	}if(inp.IsPresent("ADDWAVE")){
		if(inp.IsPresent(".cal")&&inp.IsPresent(".txt"))
			AddWave(inp.ReturnFind(".cal"),inp.ReturnFind(".txt"));
	}else{
		SiLiCalParse(argc,argv);	
		app->Run();//Dont need this if you dont want things to stay open at the end I think.	
	}

	return 0;
}