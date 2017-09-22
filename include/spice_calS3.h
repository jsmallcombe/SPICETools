//g++ spice_parse.cpp spice_enviro.cpp -std=c++0x -I$GRSISYS/include -L$GRSISYS/libraries `grsi-config --cflags --all-libs` `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum `root-config --glibs` -o SSparser


//`grsi-config --all-libs` `root-config --cflags --libs`

// -lXMLParser -lX11 -lXpm -lXMLIO 

#ifndef spice_cals3
#define spice_cals3

#include <iostream>
#include <iomanip>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>
#include <cstdio>
#include <sys/stat.h>
#include <sstream>

#include "TROOT.h"
#include "TSystem.h"
#include "TChain.h"
#include "TTree.h"
#include "TBranch.h"
#include "TTreeIndex.h"
#include "TSpectrum.h"
#include "TGraphErrors.h"
#include "TVirtualIndex.h"
#include "TFile.h"
#include "TList.h"
#include "TF1.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "TH2Poly.h"
#include "TMultiGraph.h"
#include "TStopwatch.h"
#include "TApplication.h"

#include "TCanvas.h"
#include "TMath.h"

#include <jlibmaster.h>

#include "spice_input.h"

#ifndef __CLING__
#ifndef __CINT__
#include "TTigress.h"
#include "TSiLi.h"
#include "TS3.h"
#include "TRF.h"
#include "TChannel.h"
#include "TFragment.h"
#endif
#endif

using namespace std;

int S3CalParse(int,char**,TApplication* app=0);

void S3CalParseChain(TChain* DataChain,string outputcalfile,TApplication* app=0);

void MakeBlankS3cal(string mnemonic, string calout);

#endif