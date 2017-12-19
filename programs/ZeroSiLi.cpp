#include "TSiLi.h"
#include "TSiLiHit.h"
#include "TChannel.h"
#include "spice_input.h"

int main(int argc, char *argv[]){
	SPICEcmdline inp(argc,argv);
	
	if(!inp.LoadCal()){
		cout<<"No cal file provided";
		return 1;
	}
	
	inp.Rewind();
	std::string str;
	while(inp>>str){
		if(str.find(".cal")<str.size()){
			continue;
		}
		
		unsigned int seg=std::stoul(str);
		
		if(seg<120){
			std::cout<<seg<<" ";
			TChannel *chan=TSiLiHit::GetSiLiHitChannel(seg);
			if(chan){
				chan->DestroyENGCal();
				chan->AddENGCoefficient(0);
				chan->AddENGCoefficient(-1);
			}
		}
	}
	std::cout<<std::endl;
	
	TChannel::WriteCalFile("SiLiChannelsZeroed.cal");
	return 1;
}