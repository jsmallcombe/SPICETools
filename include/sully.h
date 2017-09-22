//Elizabeth O'Sullivan
//ecosulli@uwaterloo.ca
//December 2016

#ifndef spice_cal
#define spice_cal

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

#include "james_utility.h"

#include <iostream>
#include <TCanvas.h>
#include <TH3.h>
#include <TH2.h>
#include <TMatrixDSym.h>
#include <TFitResult.h>
#include <TList.h>
#include <TMath.h>
#include <TSystem.h>
#include <TFile.h>
#include <TH1.h>
#include <TString.h>
#include <TF1.h>
#include <TPad.h>
#include <TAxis.h>
#include <TSpectrum.h>


using namespace std;

extern double BiPeaks[6];

Double_t SullyPeak(Double_t *dim, Double_t *par);
int Fit207BiCal(TH1*,double&,double&);
vector<double>  SullyFit(TH1*,double&,double&);

#endif