//g++ spice_parse.cpp spice_enviro.cpp -std=c++0x -I$GRSISYS/include -L$GRSISYS/libraries `grsi-config --cflags --all-libs` `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum `root-config --glibs` -o SSparser


//`grsi-config --all-libs` `root-config --cflags --libs`

// -lXMLParser -lX11 -lXpm -lXMLIO 

#ifndef spice_enviro
#define spice_enviro

#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>
#include <cstdio>
#include <sys/stat.h>
#include <sstream>

#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TStyle.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeIndex.h"
#include "TVirtualIndex.h"
#include "TFile.h"
#include "TList.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH2Poly.h"
#include "TMultiGraph.h"
#include "TLine.h"
#include "TGraph.h"


#include "TCanvas.h"
#include "TMath.h"
#include "TGButton.h"
#include "TRootEmbeddedCanvas.h"
#include "TGLayout.h"
#include "TFrame.h"
#include "TGTextEntry.h"
#include "TGSlider.h"
#include "TGTripleSlider.h"
#include "TH2F.h"
#include "TGButtonGroup.h"
#include "TGButton.h"
#include "THashList.h"



#include <jlibmaster.h>
// #include "james_gating_frame.h"

#ifndef __CLING__
#ifndef __CINT__
#include "TSiLi.h"
#endif
#endif

class SPICEenvironment : public TGMainFrame
{ 
    public:
	    
	////////////// Public Access Environment variables ///////////////////   
	
	///////// Constructors copy etc //////////////
	
	SPICEenvironment();
	SPICEenvironment(TFile*,string="SPICE_energy_");
	virtual ~SPICEenvironment();
	
	//These need to be implemented properly, what with all the pointers etc
// 	SPICEenvironment( const SPICEenvironment &obj)= default;//copy constructor
// 	SPICEenvironment& operator=(const SPICEenvironment&)= default;//assignment operator
// 	SPICEenvironment(SPICEenvironment&&) = default;                    // Move constructor
// 	SPICEenvironment& operator=(SPICEenvironment&&) & = default;       // Move assignment operator

	
	///////////////////////////////////////////////////////////////////////////
	//////// Methods for General Use including Command Line ////////
	///////////////////////////////////////////////////////////////////////////

	
	///////////////////////////////////////////////////////////////////////////
	//////// Methods For The Graphical Interface ////////
	///////////////////////////////////////////////////////////////////////////
	
	void MapSelectCanvas(Int_t=1, Int_t=0, Int_t=0, TObject* =0);
	void FillMap();
	void ChangeMode(Int_t);
	void ChangeMapMode(Int_t);
	void SetupWaveWindow(unsigned int=0);
	void DrawNewWave();
	void DrawNewWave(Int_t);
	void DrawNewWave(unsigned int,unsigned int);
	void FrequencyChange(char*);
	
	TH2* GetWaves(unsigned int=0);

   
    private:
	// Members
	TFile* datafile;
	TH1D* singleh[120],*sumh,*mapflat,*pixels;
	TH2D* flath;
	TGVerticalFrame *buttonframe,*mapframe,*histframe;
	TRootEmbeddedCanvas *mapcanvas,*histcanvas;
	TGLayoutHints* fframast,*fframastB;
	std::vector< TGraph > shapes,shapesred,shapessel;
	TGraph outline;
	TH2Poly silimap;
	TMultiGraph hover,select;
	int save_hover,save_select;
	bool unlocked;
	TGCheckButton *fCheck0,*fCheck1,*fCheck2;
	int x1,x2;
	j_gating_frame* backsubframe;		
	int env_mode,map_mode;
	TGButtonGroup* fBgroup2;
	TDirectory* wavedir;
	TGHSlider* waveslide;
	TGTextEntry  *fTeh1;
	double frequency;
	
	
	// Internal Methods, not called directly or by GUI
	int CheckMap(double&, double&);
	int Mapit(int);
	string MapModedTitle(int,bool=false);
	
// 	static int SPICEenvironment_iterator;
	ClassDef(SPICEenvironment, 1);
};





#endif
