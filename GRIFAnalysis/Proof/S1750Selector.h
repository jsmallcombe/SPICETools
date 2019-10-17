//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Tue Oct 25 13:18:27 2016 by ROOT version 5.34/24
// from TTree FragmentTree/FragmentTree
// found on file: fragment07844_000.root
//////////////////////////////////////////////////////////

#ifndef S1750Selector_h
#define S1750Selector_h

#include "TChain.h"
#include "TFile.h"

#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "TF1.h"
#include "THnSparse.h"

// Header file for the classes stored in the TTree if any.
#include "TGriffin.h"
#include "TSceptar.h"
#include "TZeroDegree.h"
#include "TPaces.h"
#include "TGRSISelector.h"

// Fixed size dimensions of array or collections stored in the TTree if any.

class S1750Selector : public TGRSISelector { //Must be same name as .C and .h

 public :
   TGriffin * fGrif; //Pointers to spot that events will be
//    TSceptar * fScep;
   TPaces * fPace;
   TZeroDegree * fZero;

   S1750Selector(TTree * /*tree*/ =0) : TGRSISelector(), fGrif(0), fPace(0),fZero(0) {
      SetOutputPrefix("S1750Proofed"); //Changes prefix of output file
   }
	//These functions are expected to exist
   virtual ~S1750Selector() { }
   virtual Int_t   Version() const { return 2; }
   void CreateHistograms();
   void FillHistograms();
   void InitializeBranches(TTree *tree);

   ClassDef(S1750Selector,1); //Makes ROOT happier
};

#endif

#ifdef S1750Selector_cxx
void S1750Selector::InitializeBranches(TTree* tree)
{
   if(!tree) return;
   if(tree->SetBranchAddress("TPaces", &fPace) == TTree::kMissingBranch) {
		fPace = new TPaces;
	}
   if(tree->SetBranchAddress("TZeroDegree", &fZero) == TTree::kMissingBranch) {
		fZero = new TZeroDegree;
	}
   if(tree->SetBranchAddress("TGriffin", &fGrif) == TTree::kMissingBranch) {
		fGrif = new TGriffin;
	}
//    if(tree->SetBranchAddress("TSceptar", &fScep) == TTree::kMissingBranch) {
// 		fScep = new TSceptar;
// 	}
}

#endif // #ifdef S1750Selector_cxx
