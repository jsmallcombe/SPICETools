#include "spice_input.h"

SPICEcmdline::SPICEcmdline(int argc, char *argv[]):SPICEcmdline(){

	vector<string> cmdinputs;	
	for(int i=1;i<argc;i++)cmdinputs.push_back(argv[i]);

	for(unsigned int i=0;i<cmdinputs.size();i++){
		string str=cmdinputs[i];
		
		if(str.find(".info")<str.size()&&!(str.find(".cal")<str.size())&&!(str.find(".root")<str.size())){
			ifstream infofile(str.c_str());
			if(infofile.good()){
				cout<<endl<<"InfoFile : "<<str<<" "<<flush;
				string middle;
				//while(infofile>>middle){infostream<<middle<<" ";}
				while(getline(infofile, middle, '\n')){
					if(!(middle[0]=='#')){//skip comments
						if(middle.find(".info")<middle.size()){
							cmdinputs.push_back(middle);
						}else{
							stringstream ss;
							ss<<middle;
							string sep;
							while(ss>>sep)store.push_back(sep);
						}
					}
				}
				infofile.close();
			}else{
				cout<<endl<<"Invalid infofile file specified : "<<str<<flush;
			}	
		}else{
			store.push_back(str);
		}   
	}
	
	Rewind();
};

void SPICEcmdline::Rewind(){
	infostream.str("");
	infostream.clear(); // Clear state flags.
	for(int i=0;i<store.size();i++)infostream<<store[i]<<" ";
	
}

string SPICEcmdline::ReturnFind(string compare){
	for(int i=0;i<store.size();i++){string str=store[i];
		if(str.find(compare)<str.size())return str;
	}	
	return "";
}

bool SPICEcmdline::IsPresent(string compare){
	return ReturnFind(compare).size();
}

double SPICEcmdline::Next(string compare){
	for(int i=0;i<store.size()-1;i++){string str=store[i];
		if(str.find(compare)<str.size()){
			stringstream ss;ss<<store[i+1];
			double ret;ss>>ret;
			return ret;
		}
	}
	return 0;
}

string SPICEcmdline::NextString(string compare){
	for(int i=0;i<store.size()-1;i++){string str=store[i];
		if(str.find(compare)<str.size()){
			return store[i+1];
		}
	}
	return "";
}


void SPICEcmdline::NextTwo(string compare,double& ret,double& retB){
	for(int i=0;i<store.size()-2;i++){
		string str=store[i];
		if(str.find(compare)<str.size()){
			stringstream ss;
			ss<<store[i+1]<<" "<<store[i+2];
			ss>>ret>>retB;
			return;
		}
	}
}

TChain* SPICEcmdline::AnalysisTree(vector<long> *entries,vector<string> *filelist){
	
	TChain* DataChain = new TChain("AnalysisTree","AnalysisTree");
	for(int i=0;i<store.size();i++){string str=store[i];
		if(str.find("analysis")<str.size()){
			TFile filetest(str.c_str());
			if(filetest.IsOpen()){
				cout<<endl<<"Added data : "<<str<<flush;
				filetest.Close();
				if(filelist)filelist->push_back(str);
				DataChain->Add(str.c_str());
				if(entries)entries->push_back(DataChain->GetEntries());
			}else{
				cout<<endl<<"Invalid data file : "<<str<<flush;
			}
		}
	}
	
	return DataChain;
}

TChain* SPICEcmdline::FragmentTree(){
	TChain* DataChain= new TChain("FragmentTree","FragmentTree");

	for(int i=0;i<store.size();i++){string str=store[i];
		if(str.find("fragment")<str.size()){
			TFile filetest(str.c_str());
			if(filetest.IsOpen()){
				cout<<endl<<"Added data : "<<str<<flush;
				filetest.Close();
				DataChain->Add(str.c_str());
			}else{
				cout<<endl<<"Invalid data file : "<<str<<flush;
			}
		}
	}
	return DataChain;
}


// TChannel::ReadCalFromTree(DataChain->GetTree());
// TChannel::ReadCalFromCurrentFile(Option_t *opt="overwrite");
// TChannel::ReadCalFromTree(TTree*,Option_t* opt="overwrite");
// TChannel::ReadCalFile(const char* filename = "");

bool SPICEcmdline::LoadCal(){

	string str=ReturnFind(".cal");
	if(str.size()){//Cal file loading
		ifstream f(str.c_str());
		if(f.good()){
			cout<<endl<<"Cal file : "<<str<<flush;
			f.close();
			cout<<endl<<"LOADING CAL FILE:"<<endl;
			TChannel::DeleteAllChannels();
			TChannel::ReadCalFile(str.c_str());
			return true;
		}else{
			cout<<endl<<"Invalid cal file specified : "<<str<<flush;
		}
	}
	return false;
}
bool SPICEcmdline::LoadCal(TChain* DataChain){
	if(DataChain){
		if(DataChain->GetEntries()){	
			if(!LoadCal()){
				cout<<endl<<"NO CAL FILE. Reading From tree."<<flush;	
				DataChain->GetEntry(1);
				if(TChannel::ReadCalFromTree(DataChain->GetTree()))return 1;
			}else{
				return 1;
			}
			cout<<endl<<"No calibration data."<<endl;
			return 0;
		}
	}
	cout<<endl<<"No Data."<<endl;
	return 0;
}



string SPICEcmdline::RootFile(string A,string B,string C){
	vector<string> exclude;
	if(A.size())exclude.push_back(A);
	if(B.size())exclude.push_back(B);
	if(C.size())exclude.push_back(C);
	return RootFile(exclude);
}

string SPICEcmdline::RootFile(vector<string>& exclude){
	exclude.push_back("analysis");
	exclude.push_back("fragment");

	string str;
	for(int i=0;i<store.size();i++){string str=store[i];
		if(str.find(".root")<str.size()){
			bool allow=true;
			for(int i=0;i<exclude.size();i++){
				if(str.find(exclude[i])<str.size())allow=false;
			}
				
			if(allow){
				return str;
			}
		}
	}

	return "";
}