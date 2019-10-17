#define S1750Selector_cxx
// The class definition in S1750Selector.h has been generated automatically
#include "S1750Selector.h"

void S1750Selector::CreateHistograms() {
	//Define Histograms
   fH1["gE"] 	= new TH1D("gE","#gamma Singles",4000,0,2000);
   fH1["aE"]	= new TH1D("aE", "Addback Singles", 4096,0,2048);
   fH1["bE"] 	= new TH1D("bE","ZeroDegre Singles",4000,0,2000);
   fH1["pE"] 	= new TH1D("pE","PACES Singles",1024,0,1024);
   
   fH1["gE_b"] = new TH1D("gE_b","#gamma #beta prompt coincidence",4096,0,2048);
   fH1["pE_b"] = new TH1D("pE_b","Paces #beta prompt coincidence",1024,0,1024);
   
   fH2["pEC"] = new TH2F("pEC","Paces E Channel",1024,0,1024,6,0,6);
   
   fH1["ggt"] 	= new TH1D("ggt","#gamma #gamma time",500,0,2000);
   fH2["gbT"] 	= new TH2F("gbT","#gamma beta time Coincidence",500,-1000,1000,2048,0,2048);
   fH2["pbT"] 	= new TH2F("pbT","Paces beta time Coincidence",500,-1000,1000,1024,0,1024);
   fH3["ggT3"] 	= new TH3F("ggt3","#gamma #gamma time",300,0,1500,500,0,1000,300,0,1500);
   fH3["gpT3"] 	= new TH3F("gpT3","#gamma paces time",300,0,1500,300,-500,500,200,0,1000);
   
   
   
	fH2["ggE"] 	= new TH2F("ggE","#gamma #gamma Coincidence",2048,0,2048,2048,0,2048);
	fH2["gpE"] 	= new TH2F("gpE","#gamma Paces Coincidence",2048,0,2048,1024,0,1024);
    
    
	fH3["gggE"] = new TH3F("gggE","#gamma #gamma #gamma",500,0,1500,500,0,1500,500,0,1500);
	fH3["ggpE"] = new TH3F("ggpE","#gamma #gamma paces",500,0,1500,500,0,1500,256,0,1024);
  
 
    

	//Send histograms to Output list to be added and written.
	for(auto it : fH1) {
		GetOutputList()->Add(it.second);
	}
	for(auto it : fH2) {
		GetOutputList()->Add(it.second);
	}
	for(auto it : fH3) {
		GetOutputList()->Add(it.second);
	}
	for(auto it : fHSparse) {
		GetOutputList()->Add(it.second);
	}
}

TF1 TimeWalkFnGamma("TimeWalkFnGamma","60+450*(exp(-x/150))",0,4000);
double GammaTimeWide(TGriffinHit *g){
   return TimeWalkFnGamma.Eval(g->GetEnergy());
}

TF1 TimeWalkFnPace("TimeWalkFnPace","45+300*(exp(-x/140))",0,4000);
double PacesTimeWide(TPacesHit *p){
   return TimeWalkFnPace.Eval(p->GetEnergy());
}

bool PromptCoincidence(TGriffinHit *g, TZeroDegreeHit *z){
    double tt=g->GetTime() - z->GetTime();
	return (tt<GammaTimeWide(g)+40&&tt>30);
}

bool PromptCoincidence(TGriffinHit *g1, TGriffinHit *g2){
    double tt=g1->GetTime() - g2->GetTime();
	return (tt<GammaTimeWide(g1)&&tt>-GammaTimeWide(g2));
}

bool PromptCoincidence(TGriffinHit *g, TPacesHit *p){
    double tt=g->GetTime() - p->GetTime();
	return (tt<GammaTimeWide(g)&&tt>-PacesTimeWide(p));
}

bool PromptCoincidence(TPacesHit *p, TZeroDegreeHit *z){
    double tt=p->GetTime() - z->GetTime();
	return (tt<PacesTimeWide(p)+40&&tt>35);
}

void S1750Selector::FillHistograms() {
    
    // Do paces singles
    vector<bool>PacesBetaPromt;
    for(auto j = 0; j < fPace->GetMultiplicity(); ++j){
        TPacesHit *p= fPace->GetPacesHit(j);
        fH1.at("pE")->Fill(p->GetEnergy());
        fH2.at("pEC")->Fill(p->GetEnergy(),p->GetDetector());
        
		bool iBetaPromt=false;
        //Check paces beta prompt
		for(auto j = 0; j < fZero->GetMultiplicity(); ++j){
            TZeroDegreeHit *z= fZero->GetZeroDegreeHit(j);
            fH2.at("pbT")->Fill(p->GetTime() - z->GetTime(),p->GetEnergy());
            
			if(PromptCoincidence(p,z)){
                fH1.at("pE_b")->Fill(p->GetEnergy());
                iBetaPromt=true;
			}
		}
		PacesBetaPromt.push_back(iBetaPromt);
    }
    
	//Loop over all Griffin Hits
    vector<bool>GammaBetaPromt;
   for(auto i = 0; i < fGrif->GetMultiplicity(); ++i){
        TGriffinHit *g1 =fGrif->GetGriffinHit(i);

        //Fill raw gamma singles Hist
        fH1.at("gE")->Fill(g1->GetEnergy());
      
        //Loop over all gamma-scintilator hits
		bool iBetaPromt=false;
		for(auto j = 0; j < fZero->GetMultiplicity(); ++j){
           
            TZeroDegreeHit *z= fZero->GetZeroDegreeHit(j);
            fH2.at("gbT")->Fill(g1->GetTime() - z->GetTime(),g1->GetEnergy());
            
			if(PromptCoincidence(g1,z)){
                fH1.at("gE_b")->Fill(g1->GetEnergy());
                iBetaPromt=true;
			}
		}
		GammaBetaPromt.push_back(iBetaPromt);
      
        //Do gamma gamma
        for(auto j = 0; j < i; ++j){
            TGriffinHit *g2 =fGrif->GetGriffinHit(j);
            double tt=std::fabs(g1->GetTime() - g2->GetTime());
            fH1.at("ggt")->Fill(tt);
            fH3.at("ggT3")->Fill(g1->GetEnergy(),tt,g2->GetEnergy());
            fH3.at("ggT3")->Fill(g2->GetEnergy(),tt,g1->GetEnergy());
            
            if(PromptCoincidence(g1,g2)){
                //Filling prompt gg coincidences
                fH2.at("ggE")->Fill(g1->GetEnergy(),g2->GetEnergy());
                fH2.at("ggE")->Fill(g2->GetEnergy(),g1->GetEnergy());
                
                //Do gamma gamma gamma
                for(auto k = 0; k < j; ++k){
                    TGriffinHit *g3 =fGrif->GetGriffinHit(k);
                    if(PromptCoincidence(g1,g3)&&PromptCoincidence(g2,g3)){
                            //Filling prompt prompt ggg coincidences
                            fH3.at("gggE")->Fill(g1->GetEnergy(),g2->GetEnergy(),g3->GetEnergy());
                            fH3.at("gggE")->Fill(g1->GetEnergy(),g3->GetEnergy(),g2->GetEnergy());
                            fH3.at("gggE")->Fill(g2->GetEnergy(),g1->GetEnergy(),g3->GetEnergy());
                            fH3.at("gggE")->Fill(g2->GetEnergy(),g3->GetEnergy(),g1->GetEnergy());
                            fH3.at("gggE")->Fill(g3->GetEnergy(),g1->GetEnergy(),g2->GetEnergy());
                            fH3.at("gggE")->Fill(g3->GetEnergy(),g2->GetEnergy(),g1->GetEnergy());
                    }
                }
            }
        }
        
        
        //Do gamma paces
        for(auto j = 0; j < fPace->GetMultiplicity(); ++j){
            TPacesHit *p= fPace->GetPacesHit(j);
            double tt=g1->GetTime() - p->GetTime();
            fH3.at("gpT3")->Fill(g1->GetEnergy(),tt,p->GetEnergy());
            
            if(PromptCoincidence(g1,p)){
                fH2.at("gpE")->Fill(g1->GetEnergy(),p->GetEnergy());
                
                for(auto m = 0; m < i; ++m){
                    TGriffinHit *g2 =fGrif->GetGriffinHit(m);
                     if(PromptCoincidence(g2,p)){
                        fH3.at("ggpE")->Fill(g1->GetEnergy(),g2->GetEnergy(),p->GetEnergy());
                        fH3.at("ggpE")->Fill(g2->GetEnergy(),g1->GetEnergy(),p->GetEnergy());
                     }
                }
            }
        }
   }
   
//  Loop over all sceptar hitsHit
    for(auto j = 0; j < fZero->GetMultiplicity(); ++j){
        TZeroDegreeHit *z= fZero->GetZeroDegreeHit(j);
        fH1.at("bE")->Fill(z->GetEnergy());
    }

    
}
