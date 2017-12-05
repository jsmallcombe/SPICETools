void Drift(string file= ""){
	
	TH2 *inhist=0;
	
	if(file.size()>5){
	
		TFile *infile=new TFile(file.c_str(),"READ");
		gROOT->cd();
		if(infile->IsOpen()){
			cout<<endl<<"OPEN"<<endl;
			if(infile->Get("RunTime/fileN_sili")){
				cout<<endl<<"Found"<<endl;
				inhist=(TH2*)infile->Get("RunTime/fileN_sili");
			}
		}
	}
	
	if(!inhist)return;		
	
	double peak1=902.2;
	double peak2=449.1;
	double peak3=513.8;

	double gate=70;
	
	
	vector< jPeakDat > fInput;
	fInput.push_back(jPeakDat(peak1,1,20));
	
	vector< jPeakDat > fInputB;
	fInputB.push_back(jPeakDat(peak2,1,20));
	fInputB.push_back(jPeakDat(peak3-peak2,1,10,0.9,0.3));

	stringstream ss;
	ss<<8<<"+"<<5;

	ofstream outfits("gaindriftfits.txt",std::ofstream::out);
	ofstream outfile("gaindrift.txt",std::ofstream::out);
	
	unsigned int N=inhist->GetNbinsX();
	TAxis *axis=inhist->GetXaxis();
	TAxis *ayis=inhist->GetYaxis();
	TCanvas c1;
	
	TH1* hist=inhist->ProjectionY("progY",1,1);
	
	for(int i=N;i>0;i--){
		hist->Reset();
		
		vector< string > label;
				
		while(i>0&&hist->Integral(ayis->FindBin(peak1-gate),ayis->FindBin(peak1+gate))<1500){
			TH1* tmp=inhist->ProjectionY("tmpprogY",i,i);
			hist->Add(tmp);
			delete tmp;
				
			string lab=axis->GetBinLabel(i);
			if(lab.find("analysis")<lab.size())lab=lab.substr(lab.find("analysis")+8,lab.size());
			if(lab.find(".root")<lab.size())lab=lab.substr(0,lab.find(".root"));
			label.push_back(lab);
			i--;
		}
		if(!label.size())continue;
		i++;
		
		FullFitHolder* fitA=Ultrapeak::PeakFit(hist,peak1-gate,peak1+gate,fInput,3,0,ss.str());
		FullFitHolder* fitB=Ultrapeak::PeakFit(hist,peak2-gate,peak3+gate,fInputB,3,0,ss.str());
		
		for(unsigned int j=0;j<label.size();j++){
			outfits<<endl<<label[j]<<" ";
			outfile<<endl<<label[j]<<" ";
			if(fitA && fitB){
// 				hist->DrawCopy();
// 				fitA->DrawCopy("same");
// 				fitB->DrawCopy("same");
// 				c1.WaitPrimitive();
				
				outfits<<fitB->CVal(Ultrapeak::VPC(0))<<" ";
				outfits<<fitB->CVal(Ultrapeak::VPC(1))<<" ";
				outfits<<fitA->CVal(Ultrapeak::VPC(0));
				
				TGraph xy;
				xy.SetPoint(0,fitB->CVal(Ultrapeak::VPC(0)),peak2);
				xy.SetPoint(1,fitB->CVal(Ultrapeak::VPC(1)),peak3);
				xy.SetPoint(2,fitA->CVal(Ultrapeak::VPC(0)),peak1);
				
				TF1 line("pol1","pol1",0,1500);
				xy.Fit(&line,"0QW");
				xy.Fit(&line,"0");
				double offset=line.GetParameter(0);
				double gain=line.GetParameter(1);
				
				outfile<<offset<<" "<<gain;
			}else{
				outfits<<" 0 0 0 ";
				outfile<<" 0 0 0 ";
			}
		}
	}
	
	delete hist;
	
	outfits.close();
	outfile.close();
	
	// An all singing all dancing function to fit N peaks that are in close enough proximity to assume constant peak parameters
	// int=1 specified to attempt to use step background int=0 uses linear
// 	static FullFitHolder* PeakFit(TH1* fHist,double fLeftUser,double fRightUser,vector< jPeakDat > &fInput,int backmode=0,int peaktype=0,string sig="",string dec="",string sha="");
	
	// "pol0+step",0
	// "pol1 fixed",1
	// "pol1",2
	// "pol1+step",3
	// "pol2",4
	// "pol2+step",5   
	
}
