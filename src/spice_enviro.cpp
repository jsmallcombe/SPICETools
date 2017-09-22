#include "spice_enviro.h"

// int SPICEenvironment::SPICEenvironment_iterator = 0;

SPICEenvironment::SPICEenvironment():TGMainFrame(),datafile(0){}

SPICEenvironment::SPICEenvironment(TFile* infile,string path):TGMainFrame(gClient->GetRoot(),1200,500,kHorizontalFrame),datafile(infile),sumh(0),save_hover(-1),save_select(-1),unlocked(1),x1(0),x2(-1)
{
	SetWindowName("SPICE Viewer");
			
	SetCleanup(kDeepCleanup);
	
	for(int i=0;i<120;i++)singleh[i]=0;
	flath=0;
	mapflat=0;
	sumh=0;
	wavedir=0;
	waveslide=0;
	fCheck1=0;
	fCheck0=0;
	
	frequency=0;
	
	if(datafile){
		if(datafile->Get(path.c_str())){
			flath=(TH2D*)datafile->Get(path.c_str());
		
			sumh=flath->ProjectionY("sum");
			pixels=flath->ProjectionY("pixels");
			mapflat=flath->ProjectionX("mapflat");
			
			for(int i=0;i<120;i++){
				stringstream ss;
				ss<<"pixel_energy_"<<i;
				singleh[i]= flath->ProjectionY(ss.str().c_str(),i+1,i+1);
				singleh[i]->SetTitle(ss.str().c_str());
			}
		}
		wavedir=datafile->GetDirectory("waveforms");
	}
	
	if(flath){
		//Create the graphical elements
		
		buttonframe = new TGVerticalFrame(this, 0, 0, 0);
		mapframe = new TGVerticalFrame(this, 0, 0, 0);
		histframe = new TGVerticalFrame(this, 0, 0, 0);
		
		mapcanvas = new TRootEmbeddedCanvas("mapcanvas", mapframe, 450, 400);
			mapcanvas->GetCanvas()->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)", "SPICEenvironment", this,"MapSelectCanvas(Int_t,Int_t,Int_t,TObject*)");
			mapcanvas->GetCanvas()->SetFrameFillColor(1);
		histcanvas = new TRootEmbeddedCanvas("histcanvas", histframe, 450, 400);
		
		waveslide = new TGHSlider(histframe);
		waveslide->Connect("PositionChanged(Int_t)", "SPICEenvironment", this, "DrawNewWave(Int_t)");
		waveslide->SetRange(1,40);
		waveslide->SetPosition(1);//set inital value
		
		
		fCheck0 = new TGCheckButton(buttonframe,"Normalise");// A tick box with hover text belonging to a parent frame
		fCheck0->SetState(kButtonDown);
		fCheck0->SetToolTipText("Normalises map for area.");	
		fCheck0->Connect("Clicked()","SPICEenvironment",this,"FillMap()");//Link it signal to its slot
		
		fCheck1 = new TGCheckButton(buttonframe,"Fit Waves");// A tick box with hover text belonging to a parent frame
		fCheck1->SetState(kButtonUp);
		fCheck1->SetToolTipText("Show fits for waveforms.");	
		fCheck1->Connect("Clicked()","SPICEenvironment",this,"DrawNewWave()");//Link it signal to its slot
		if(!wavedir)fCheck1->SetEnabled(false);
		
		fCheck2 = new TGCheckButton(buttonframe,"ShapeFit");// A tick box with hover text belonging to a parent frame
		fCheck2->SetState(kButtonUp);
		fCheck2->SetToolTipText("Use slow TF1 shape fit.");	
		fCheck2->Connect("Clicked()","SPICEenvironment",this,"DrawNewWave()");//Link it signal to its slot
		if(!wavedir)fCheck2->SetEnabled(false);
		
		fTeh1 =  new TGTextEntry(buttonframe, new TGTextBuffer(5));//Create some text entry boxes 
		fTeh1->SetToolTipText("Oscillation Frequency");
		fTeh1->SetAlignment (kTextRight);
		// Set the data in the text buffer the text entry boxes are using
// 		sprintf(buf, "%.1f", proj->GetBinCenter(1));	fTbh1->AddText(0, buf);
		fTeh1->Connect("TextChanged(char*)", "SPICEenvironment", this,"FrequencyChange(char*)");
		
		TGButtonGroup* fBgroup1 = new TGButtonGroup(buttonframe,"Mode",kChildFrame|kVerticalFrame);//create a group of buttons belonging (point) to the parent frame
			TGRadioButton* fRButton0 = new TGRadioButton(fBgroup1,"Pixel Energy");//create buttons belonging to the group
			TGRadioButton* fRButton1 = new TGRadioButton(fBgroup1,"Pixel Waveform");
			TGRadioButton* fRButton2 = new TGRadioButton(fBgroup1,"Map Gated");
		fRButton0->SetState(kButtonDown);//Set which is pressed
		fBgroup1->Show();//Display/Add all the buttons
		fBgroup1->Connect(" Clicked(Int_t)", "SPICEenvironment", this,"ChangeMode(Int_t)");
		if(!wavedir)fRButton1->SetEnabled(false);

		fBgroup2 = new TGButtonGroup(buttonframe,"Selection",kChildFrame|kVerticalFrame);
			TGRadioButton* fRButton4 = new TGRadioButton(fBgroup2,"Single");
			TGRadioButton* fRButton5 = new TGRadioButton(fBgroup2,"Ring");
			TGRadioButton* fRButton6 = new TGRadioButton(fBgroup2,"Sector");
			TGRadioButton* fRButton7 = new TGRadioButton(fBgroup2,"Preamp");
		fRButton4->SetState(kButtonDown);//Set which is pressed
		fBgroup2->Show();//Display/Add all the buttons
		fBgroup2->Connect(" Clicked(Int_t)", "SPICEenvironment", this,"ChangeMapMode(Int_t)");		
			
		fframast = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY, 0, 0, 0, 0);
		fframastB = new TGLayoutHints(kLHintsCenterX, 3, 3, 3, 3);
		
		backsubframe = new j_gating_frame(this,flath,"1");
		backsubframe->ChangeProjection(2);
		backsubframe->Connect("OutputReady()", "SPICEenvironment", this,"FillMap()");

	//Add graphical elements to the frames
			
		buttonframe->AddFrame(fBgroup1,fframastB);
		buttonframe->AddFrame(fCheck0,fframastB);
		buttonframe->AddFrame(fCheck1,fframastB);
		buttonframe->AddFrame(fCheck2,fframastB);
		buttonframe->AddFrame(fTeh1,fframastB);
		buttonframe->AddFrame(fBgroup2,fframastB);
			AddFrame(buttonframe,new TGLayoutHints(kLHintsExpandY,3,3,3,3));
		mapframe->AddFrame(mapcanvas,fframast);
			AddFrame(mapframe,fframast);
		histframe->AddFrame(histcanvas,fframast);
		histframe->AddFrame(waveslide,new TGLayoutHints(kLHintsExpandX, 5, 5, 5, 5));
			AddFrame(histframe,fframast);
			AddFrame(backsubframe,fframast);	
		
		//Process the data
			
		//Load pointers to histogram on file
			
		TSiLi silitmp;
		shapes=silitmp.UpstreamShapes();
		shapesred=shapes;
		shapessel=shapes;

		for(int i=0;i<120;i++){
			if(i<shapes.size()){
				silimap.AddBin(shapes[i].Clone());
				shapesred[i].SetLineColor(2);
				shapesred[i].SetLineWidth(3);
				
				shapessel[i].SetFillColor(4);
				if((TSiLi::GetRing(i)+i)%2)shapessel[i].SetFillStyle(3004);
				else shapessel[i].SetFillStyle(3005);
			}
		}
		
		for(int i=0;i<500;i++){
			double a=TMath::Pi()*i/250.0;
			outline.SetPoint(outline.GetN(),TMath::Sin(a)*47,TMath::Cos(a)*47);
		}
		outline.SetLineColor(2);
		outline.SetLineWidth(3);
				
		env_mode=1;
		map_mode=1;

		//Draw the map
		silimap.SetContour(200);
		gStyle->SetPalette(53);
		silimap.SetTitle("");
		silimap.SetStats(0);
		mapcanvas->GetCanvas()->cd();
		silimap.Draw("colz");

		MapSubwindows();
		Resize(GetDefaultSize());
		MapWindow();
		HideFrame(backsubframe);
		histframe->HideFrame(waveslide);
	
		FillMap();
		
		mapcanvas->GetCanvas()->Modified();
		mapcanvas->GetCanvas()->Update();
		
		histcanvas->GetCanvas()->cd();
		if(sumh)sumh->Draw();
		histcanvas->GetCanvas()->Modified();
		histcanvas->GetCanvas()->Update();
	}
};

SPICEenvironment::~SPICEenvironment(){
   // Clean up
   Cleanup();
   if(datafile)datafile->Close();
};

int SPICEenvironment::Mapit(int i){
	if(i>-1){
		if(map_mode==2)i=TSiLi::GetRing(i);
		if(map_mode==3)i=TSiLi::GetSector(i);
		if(map_mode==4)i=TSiLi::GetPreamp(i);
	}
	return i;
}

string SPICEenvironment::MapModedTitle(int i,bool detail){
	stringstream ss;
	if(i>-1){
		if(map_mode==2)ss<<"Ring_"<<i;
		else if(map_mode==3)ss<<"Sector_"<<i;
		else if(map_mode==4){ss<<"Preamp_"<<i<<"_";if(i<8)ss<<TSiLi::fPreAmpName[i];}
		else{
			ss<<"Pixel_"<<i;
			if(detail)ss<<"_Preamp_"<<TSiLi::fPreAmpName[TSiLi::GetPreamp(i)]<<"_pin_"<<TSiLi::GetPin(i);
		}
	}
	return ss.str();
}


void SPICEenvironment::MapSelectCanvas(Int_t event, Int_t px, Int_t py, TObject *selected_ob){if(unlocked){unlocked=false;
	double x =mapcanvas->GetCanvas()->AbsPixeltoX(px);
	double y =mapcanvas->GetCanvas()->AbsPixeltoY(py);
	int current_hover=CheckMap(x,y);
	current_hover=Mapit(current_hover);
	

	//NEXT WE CALC current_hover BASED ON SETTING, RING, PREAMP, SINGLE
	if(current_hover!=save_hover){

		save_hover=current_hover;
		cout<<MapModedTitle(current_hover,true)<<endl;
		mapcanvas->GetCanvas()->cd();
		if(current_hover==-1)
			outline.Draw("samel");
		else
			mapcanvas->GetCanvas()->GetListOfPrimitives()->Remove(&outline);
		
		for(int i=0;i<shapesred.size();i++){	
			if(current_hover==Mapit(i)){
				shapesred[i].Draw("samel");
			}else{
				mapcanvas->GetCanvas()->GetListOfPrimitives()->Remove(&shapesred[i]);
			}
		}
		mapcanvas->GetCanvas()->Modified();
		mapcanvas->GetCanvas()->Update();
	}
	
	// Save_select is in the # of the current map mode
	// Mapit(segement) converts
	
	if ( event == 1 || event == 2) {//2 middle down, 12 middle up
		if(save_select!=current_hover){save_select=current_hover;
			pixels->Reset();
			pixels->SetTitle(("Energy "+MapModedTitle(save_select)).c_str());
			mapcanvas->GetCanvas()->cd();
			for(int i=0;i<shapessel.size();i++){
				if(save_select==Mapit(i)){
					shapessel[i].Draw("samelf");
					if(singleh[i])pixels->Add(singleh[i]);
				}else{
					mapcanvas->GetCanvas()->GetListOfPrimitives()->Remove(&shapessel[i]);
				}
			}
			mapcanvas->GetCanvas()->Modified();
			mapcanvas->GetCanvas()->Update();
			
			if(env_mode==2)SetupWaveWindow(save_select);
			else{
				histcanvas->GetCanvas()->cd();
				if(save_select>=0)pixels->Draw();
				else sumh->Draw();
				histcanvas->GetCanvas()->Modified();
				histcanvas->GetCanvas()->Update();
			}
		}
	}
unlocked=true;}}

int SPICEenvironment::CheckMap(double& px, double& py){
	if((px*px+py*py)<2256){//If its even in the radius
		for(int i=0;i<shapes.size();i++){
			if(shapes[i].IsInside(px,py))return i;
		}
	}
	return -1;
}

void SPICEenvironment::FillMap(){
	TH1* h=0;
	if(env_mode==3&&backsubframe->output_hist_point)h=backsubframe->output_hist_point;
	else if(mapflat)h=mapflat;

	if(h){
		for(int i=1;i<=120;i++){
			double z=h->GetBinContent(i);
			if(fCheck0->GetState())z*=TSiLi::GetSegmentArea(0)/TSiLi::GetSegmentArea(i);
			silimap.SetBinContent(i,z);
		}
		mapcanvas->GetCanvas()->Modified();
		mapcanvas->GetCanvas()->Update();
	}
}



void SPICEenvironment::ChangeMapMode(Int_t mode){
	map_mode=mode;
	MapSelectCanvas();
}

void SPICEenvironment::ChangeMode(Int_t mode){
	FillMap();//Added this to try and fix colour getting stuck in odd modes
	env_mode=mode;
	HideFrame(backsubframe);
	HideFrame(histframe);
	histframe->HideFrame(waveslide);
	fBgroup2->SetButton(1);
	fBgroup2->Hide();
	//HideFrame();
	switch (mode) {
		case 1:
			ShowFrame(histframe);
			fBgroup2->Show();
			MapSelectCanvas();
			break;
		case 2:
			fBgroup2->Show();
			SetupWaveWindow(save_select);
			break;
		default:
			ShowFrame(backsubframe);
			break;
	}
// 	gClient->NeedRedraw(buttonframe);
	gClient->NeedRedraw(this);
}


void SPICEenvironment::SetupWaveWindow(unsigned int i){
	if(wavedir && waveslide){
		int B=120;//If !(i<120) doing full detector scan so B=120
		if(i<120){
			if(!GetWaves(i))return;
			if(map_mode==2)B=12;
			else if(map_mode==3)B=10;
			else if(map_mode==4)B=15;
			else B=GetWaves(i)->GetNbinsX();
		}
		
		ShowFrame(histframe);
		histframe->ShowFrame(waveslide);
		waveslide->SetRange(1,B);
		waveslide->SetPosition(1);
		DrawNewWave(i,1);
		gClient->NeedRedraw(this);
	}
}

void SPICEenvironment::DrawNewWave(){
	if(waveslide&&env_mode==2){
		DrawNewWave(waveslide->GetPosition());
	}
}
	
void SPICEenvironment::DrawNewWave(Int_t x){
	DrawNewWave(save_select,x);
}

//"i" is map select, "x" is scroll select
void SPICEenvironment::DrawNewWave(unsigned int i,unsigned int x){
	//Scanning across the waveforms of a single pixel (with fits)
	if(map_mode==1&&GetWaves(i)){
		stringstream ss;
		ss<<"SPICE_"<<i<<"_waveforms_"<<x;
		TH1D* project=GetWaves(i)->ProjectionY(ss.str().c_str(),x,x);
		project->SetTitle(ss.str().c_str());
		histcanvas->GetCanvas()->cd();
			project->SetStats(0);
			project->DrawCopy("colz");
			if(fCheck1)if(fCheck1->GetState()){
				std::vector<short>wavedata;
				for(int x=1;x<=project->GetNbinsX();x++)wavedata.push_back(project->GetBinContent(x));
				TPulseAnalyzer pulse(wavedata,TSiLi::sili_noise_fac);
				int fitmode=1+fCheck2->GetState()+(frequency>0&&fCheck2->GetState());
				int resu=TSiLiHit::FitPulseAnalyzer(&pulse,fitmode,i);
				if(resu){
					TF1 fit=pulse.Getsilifit();
					if(resu>2)fit.SetLineColor(3);
					fit.SetNpx(1000);
					fit.DrawCopy("same");
					TText peaknum;
					peaknum.SetTextAlign(13);
					peaknum.SetTextSize(0.03);
					stringstream ss,SS;
					ss<<"Sig/Noise : "<<jsigfig(pulse.get_sig2noise(),3);
					peaknum.DrawTextNDC(0.12,0.88,ss.str().c_str());
// 					SS<<"Smirnov : "<<jsigfig(pulse.GetsiliSmirnov()/pulse.Get_wpar_amplitude(),3);
					SS<<"Smirnov : "<<jsigfig(pulse.GetsiliSmirnov()/10000.0,3);
					peaknum.DrawTextNDC(0.12,0.845,SS.str().c_str());
				}
			}	
			delete project;
		histcanvas->GetCanvas()->Modified();
		histcanvas->GetCanvas()->Update();
		return;
	}else{
		int sel=x-1;
		
		if(i<120){
			int count=0;
			for(int j=0;j<120;j++){
				if(i==Mapit(j))count++;
				if(count==x){sel=j;break;}
			}
		}
		
		//Scanning across sum histograms of pixel waveforms
		if(GetWaves(sel)){
			stringstream ss;
			ss<<"SPICE_";
			if(i<120)ss<<MapModedTitle(Mapit(sel))<<"_";
			ss<<"pixel_"<<sel<<"_waveforms";
			
			TH2* wavedata=GetWaves(sel);
			if(wavedata->GetEntries()==0)return;
			
			TH1* project;

			histcanvas->GetCanvas()->cd();
			project=wavedata->ProjectionY(ss.str().c_str(),1,1);
			project->SetMaximum(wavedata->GetMaximum());
			project->SetMinimum(wavedata->GetMinimum());
			project->SetTitle(ss.str().c_str());
			project->DrawCopy();
			
			for(int i=2;i<wavedata->GetNbinsX();i++){
				project=wavedata->ProjectionY(ss.str().c_str(),i,i);
				project->DrawCopy("same");
				
			}
			histcanvas->GetCanvas()->Modified();
			histcanvas->GetCanvas()->Update();
				
			delete project;

			return;	
		}
	}
}

TH2* SPICEenvironment::GetWaves(unsigned int i){
	if(wavedir){
		stringstream ss;
		ss<<"SPICE_waveforms_"<<i;
		if(wavedir->Get(ss.str().c_str()))return (TH2*) wavedir->Get(ss.str().c_str());		
	}

	return 0;
}


void SPICEenvironment::FrequencyChange(char* text){
	frequency=atof(text);
	TSiLi::BaseFreq=frequency;
	DrawNewWave();
}
