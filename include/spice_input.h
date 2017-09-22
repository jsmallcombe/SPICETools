//g++ spice_parse.cpp spice_enviro.cpp -std=c++0x -I$GRSISYS/include -L$GRSISYS/libraries `grsi-config --cflags --all-libs` `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum `root-config --glibs` -o SSparser

//`grsi-config --all-libs` `root-config --cflags --libs`

// -lXMLParser -lX11 -lXpm -lXMLIO 

#ifndef spice_input
#define spice_input

#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>
#include <cstdio>
#include <sys/stat.h>
#include <sstream>

#include <TTree.h>
#include <TFile.h>
#include <TChain.h>

#ifndef __CLING__
#ifndef __CINT__
#include "TChannel.h"
#include "TFragment.h"
#endif
#endif

using namespace std;

inline string OrDefault(string def,string test){
	if(test.size())return test;
	return def;
}

class SPICEcmdline
{ 
    private:
	vector<string> store;

    public:
	stringstream infostream;
	void Rewind();
	
	SPICEcmdline(){};
	SPICEcmdline(int argc, char *argv[]);	
	~SPICEcmdline(){};
	
	SPICEcmdline( const SPICEcmdline &obj){	store=obj.store;Rewind();}//copy constructor
	SPICEcmdline& operator=(const SPICEcmdline& obj){//assignment operator
		store=obj.store;Rewind();
		return *this;
	}

	
	string ReturnFind(string compare);
	bool IsPresent(string compare);
	double Next(string compare);
	string NextString(string compare);
	void Next(string compare,double &ret){ret=Next(compare);}
	void NextTwo(string compare,double& ret,double& retB);
	TChain* AnalysisTree(vector<long> *entries=0,vector<string> *filelist=0);
	TChain* FragmentTree();
	bool LoadCal();
	bool LoadCal(TChain*);//This function acts as a double check first checking if TChain works before trying to load cal
	string RootFile(string A="",string B="",string C="");
	string RootFile(vector<string>& exclude);
};


//I'm still suprised this worked
// template <typename T>
// SPICEcmdline& operator>>(SPICEcmdline& is, T& obj){
// 	is.infostream>>obj;
// 	return is;
// }

//This new doesnt work something something
template <typename T>
stringstream& operator>>(SPICEcmdline& is, T& obj){
	is.infostream>>obj;
	return is.infostream;
}

template <typename T>
stringstream& operator<<(SPICEcmdline& is, T& obj){
	is.infostream<<obj;
	return is.infostream;
}

#endif
