
void DriftB(string file= ""){
	
	TH2 *inhist=0;
	
	TFile *infile=new TFile(file.c_str(),"READ");
	gROOT->cd();
	if(infile->IsOpen()){
			cout<<endl<<"OPEN"<<endl;
			TIter next(infile->GetListOfKeys());
			TKey *key;
			while ((key = (TKey*)next())) {
				TClass *cl = gROOT->GetClass(key->GetClassName());
				if (cl->InheritsFrom("TH2")){
					inhist= (TH2*)key->ReadObj()->Clone();
					break;
				}
			}
	}

	if(!inhist)return;		
	
	ofstream outfile("gaindrift.txt",std::ofstream::out);
	
	unsigned int N=inhist->GetNbinsX();
	TAxis *axis=inhist->GetXaxis();
	TAxis *ayis=inhist->GetYaxis();
	
	TH1* hist=inhist->ProjectionY("progY",1,1);
	
	int previoussubrun=0;
	int freeforward=0;
	
	TGraph gk2,gl2,gk1;
	
	for(int i=N;i>0;i--){
		hist=inhist->ProjectionY("progY",i,i);
		string lab=axis->GetBinLabel(i);
		stringstream labsub;
		labsub<<lab.substr(lab.size()-8,3);
		int thissubrun;
		labsub>>thissubrun;
		
		if(previoussubrun>thissubrun){
			freeforward++;
		}else{
			freeforward=0;
		}
		previoussubrun=thissubrun;

		int j=1;
		while(i>0&&hist->Integral(hist->FindBin(800),hist->FindBin(1000))<40000){
			
			if(i-j>0){
				TH1* tmp=inhist->ProjectionY("tmpprogY",i-j,i-j);
				hist->Add(tmp);
				delete tmp;
			}
			if(j<=freeforward){
				TH1* tmp=inhist->ProjectionY("tmpprogY",i+j,i+j);
				hist->Add(tmp);
				delete tmp;
			}			
			j++;
			
			if(j>N)break;
		}

		hist->GetXaxis()->SetRangeUser(600,1200);
		double K1=hist->GetBinCenter(hist->GetMaximumBin());
		hist->GetXaxis()->SetRange();
		double gain=K1/924.34;
		double L1=935.35*gain;
		

		vector< jPeakDat > fInput;
		fInput.push_back(jPeakDat(K1,1,5*gain));
		fInput.push_back(jPeakDat(L1-K1,1,1*gain,0.1,0.09));
		FullFitHolder* fitA=Ultrapeak::PeakFit(hist,825*gain,1015*gain,fInput,4,0,"","15");
		
		double gain=(fitA->CVal(Ultrapeak::VPC(0))+fitA->CVal(Ultrapeak::VOff))/924.34;
	
		double K2=466.78*gain;
		double L2=533.66*gain;
		vector< jPeakDat > fInputB;
		fInputB.push_back(jPeakDat(K2,1,10*gain));
		fInputB.push_back(jPeakDat(L2-K2,1,2*gain,0.8,0.5));
		
		FullFitHolder* fitB=Ultrapeak::PeakFit(hist,415*gain,585*gain,fInputB,5,0,"","15");

		outfile<<endl<<lab<<" ";
		
		if(fitA && fitB){
				
	// 		TCanvas* C1=new TCanvas();
	// 		hist->DrawCopy();
	// 		fitA->DrawCopy("same");
	// 		fitB->DrawCopy("same");
	// 		C1->WaitPrimitive(); 	
				
			TGraph xy;
			xy.SetPoint(0,fitB->CVal(Ultrapeak::VPC(0))+fitB->CVal(Ultrapeak::VOff),K2);
			xy.SetPoint(1,fitB->CVal(Ultrapeak::VPC(1))+fitB->CVal(Ultrapeak::VOff),L2);
			xy.SetPoint(2,fitA->CVal(Ultrapeak::VPC(0))+fitA->CVal(Ultrapeak::VOff),K1);
			TF1 line("pol1","pol1",0,1500);
			xy.Fit(&line,"0QW");
			xy.Fit(&line,"0");
			double offset=line.GetParameter(0);
			double gain=line.GetParameter(1);
			
			gk2.SetPoint(gk2.GetN(),i-0.5,fitB->CVal(Ultrapeak::VPC(0))+fitB->CVal(Ultrapeak::VPCe(0)));
			gl2.SetPoint(gl2.GetN(),i-0.5,fitB->CVal(Ultrapeak::VPC(1))+fitB->CVal(Ultrapeak::VPCe(1)));
			gk1.SetPoint(gk1.GetN(),i-0.5,fitA->CVal(Ultrapeak::VPC(0))+fitA->CVal(Ultrapeak::VPCe(0)));
		
			outfile<<offset<<" "<<gain;
		}else{
			outfile<<" 0 0 0 ";
		}
			
		delete hist;
		delete fitA;
		delete fitB;
	}
	
	TCanvas* C2=new TCanvas();
	inhist->DrawCopy("col");
	C2->cd();
	gPad->Update();
	gk2.SetLineWidth(3);
	gk2.DrawClone("samel");
	gl2.DrawClone("samel");
	gk1.DrawClone("samel");
	C2->SaveAs("Result.root");
// 	C2->WaitPrimitive(); 	
	
// 	delete hist;
// 	
// 	outfits.close();
// 	outfile.close();
	
	
}
