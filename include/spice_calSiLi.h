//bit to find neumonic from user input of tig10 info 
//Elizabeth O'Sullivan
//ecosulli@uwaterloo.ca
//December 2016

#ifndef spice_calsili
#define spice_calsili


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iterator>
#include <iomanip>
#include <cmath>
#include <algorithm>

#include "jlibmaster.h"

#include <iostream>
#include <TCanvas.h>
#include <TH3.h>
#include <TH2.h>
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TList.h>
#include <TMath.h>
#include <TSystem.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1.h>
#include <TString.h>
#include <TF1.h>
#include <TPad.h>
#include <TAxis.h>
#include <TSpectrum.h>
#include <TGInputDialog.h>
#include "TApplication.h"
#include "TROOT.h"
#include "TSystem.h"


#ifndef __CLING__
#ifndef __CINT__
#include "TSiLi.h"
#include "TChannel.h"
void ZeroChannel(TChannel*);
#endif
#endif

#include "spice_input.h"

using namespace std;

// extern double BiPeaks[6];

void SiLiCalParse(int,char**);
		
TH2* ChargeParse(TChain* DataChain,bool UseFitCharge);
TH2* EnergyParse(TChain* DataChain,bool UseFitCharge);

void CalSiLiBi207Single(TH2* charge,int seg);
	
void CalSiLiBi207(TH2* hist,vector<double>&offest,vector<double>&gain);

int Fit207BiCalSpec(TH1* hist,double& offset,double& gain);

int Fit207BiCal(TH1* hist,double& offset,double& gain,TCanvas* draw = 0);

void MakeSiLiBlankCal(string calname="SPICE_blank_calfile.cal");

string tig10address(unsigned int,unsigned int,unsigned int);
void write_preamp_to_file(unsigned int,unsigned int,unsigned int,unsigned int, ofstream&);
void preamp_question(unsigned int, ofstream&);

void ZeroChannel(int i);


/////////////////////////////////////////////////////////////////////////////


class ReFitMasterPanel : public TGMainFrame {
private:
	TRootEmbeddedCanvas *fCanvas1,*fCanvas2;
	TGTextButton* ftbutton1,*ftbutton2;
	TH2* Chist,*Ehist;
	TChain* DataChain;
	bool UseFitCharge;
	int pointbin;
	bool UNLOCKED;
	TH1* proj;
	
public:
	ReFitMasterPanel(TChain* Data=0,bool fit=1,TH2* Ch1=0,TH2* Eh2=0);
	virtual ~ReFitMasterPanel(){
		if(proj)delete proj;
		if(Ehist)delete Ehist;
	};
	
	void EndSave();
	void ReSort();
	void ClickedCanvas(Int_t,Int_t,Int_t,TObject*);

	ClassDef(ReFitMasterPanel, 1)
};



class silicalchan : public TGMainFrame {
private:
	TRootEmbeddedCanvas *fCanvas1;
	TGButtonGroup	*fBgroup1;
	int answer;
	bool k_mode;
	TCanvas* Can(){if(fCanvas1)return fCanvas1->GetCanvas();return 0;}
	TH1* hist;
	vector<double> Kclicks;
	double offset,gain;
	int segment;
public:
	silicalchan(TH1* h=0,int seg=0);
	~silicalchan(){if(hist)delete hist;}

	void Retry();
	void Accept();
	void Abandon();

	void ClickedCanvas(Int_t event, Int_t px, Int_t py, TObject *selected_ob);
	
	ClassDef(silicalchan, 1)
};

/////////////////////////////////////////////////////////////////////////////

void AddWave(string calfile,string wavefile);


#endif