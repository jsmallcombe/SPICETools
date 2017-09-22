#include "spice_calS3.h"
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"

int main(int argc, char *argv[]){
	int dummy=1;
	TApplication *app = new TApplication("app", &dummy, argv);
	
	SPICEcmdline inp(argc,argv);

	if(inp.IsPresent("MAKEBLANK")){
		MakeBlankS3cal(OrDefault("SPE02F",inp.ReturnFind("SPE0")),OrDefault("S3cal.cal",inp.ReturnFind(".cal")));
	}else{
		S3CalParse(argc,argv,app);	
		app->Run();//Dont need this if you dont want things to stay open at the end I think.
	}

	return 0;
}
