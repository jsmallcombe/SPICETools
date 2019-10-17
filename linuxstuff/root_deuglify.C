{
gStyle->SetCanvasColor(0);
gStyle->SetTitleBorderSize(0);
gStyle->SetTitleFillColor(0);
gStyle->SetStatColor(0);
gStyle->SetPadBorderMode(0);
gStyle->SetFrameBorderMode(0);
gStyle->SetCanvasBorderMode(0);
gStyle->SetStatBorderSize(1);
gStyle->SetPalette(1);
gStyle->SetNumberContours(100);


// gROOT->ProcessLine(".include /home/jsmallcombe/codes/library/include");
// gROOT->ProcessLine(".include ./include");
// gROOT->ProcessLine(".include /usr/include");
// gROOT->ProcessLine(".include /usr/include/gsl");
// gROOT->ProcessLine(".include /usr/include/wignerSymbols");
// gSystem->Load("/usr/lib/libwignerSymbols.so");
// gSystem->Load("/usr/lib/libgsl.so");
// gSystem->Load("~/local/lib/librange");
// gSystem->Load("librange");

gSystem->Load("libJanalysistools.so");
gSystem->Load("libjames_phys.so");jsigfig(1,1);
SetGlobalNegative(0);

}


