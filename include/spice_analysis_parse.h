
//g++ analysis_parse.cpp -std=c++0x -I$GRSISYS/include -L$GRSISYS/libraries `grsi-config --cflags --all-libs` `root-config --cflags --libs` -lTreePlayer -lMathMore -lSpectrum -o Aparser


//`grsi-config --all-libs` `root-config --cflags --libs`

// -lXMLParser -lX11 -lXpm -lXMLIO 

#ifndef spice_analparse
#define spice_analparse

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
#include "TCutG.h"
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
#include "TCanvas.h"
#include "TMath.h"
#include "TFile.h"
#include "TDirectory.h"
#include "TStopwatch.h"


#ifndef __CLING__
#ifndef __CINT__
#include "TFragment.h"
#include "TTigress.h"
#include "TSiLi.h"
#include "TS3.h"
#include "TGenericDetector.h"
#include "TPaces.h"
#include "TRF.h"
#include "TChannel.h"
#endif
#endif

#include "spice_input.h"

#include <jlibmaster.h>
// #include <jphysmaster.h>
using namespace std;

int analysis_parse(int argc, char *argv[]);

#endif