
void DriftC(string file= ""){
	
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
	infile->Close();

	if(!inhist)return;		
	
	ofstream outfile("gaindrift.txt",std::ofstream::out);
	
	unsigned int N=inhist->GetNbinsX();
	TAxis *axis=inhist->GetXaxis();
	TAxis *ayis=inhist->GetYaxis();
	
	TH1* hist=inhist->ProjectionY("progY",1,1);
	
	int previoussubrun=0;
	int freeforward=0;
	
	TGraph gk2,gl2,gk1;
	
	int thres=500000;
	
	
	vector<double> maxxi;
	maxxi.push_back(0);
	
	for(int i=1;i<=N;i++){
		hist=inhist->ProjectionY("progY",i,i);
		hist->GetXaxis()->SetRangeUser(800,1200);
		maxxi.push_back(hist->GetBinCenter(hist->GetMaximumBin()));
		hist->GetXaxis()->SetRange();
		delete hist;
	}
	
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

		
		double K1=maxxi[i];	
		
		int j=1;
		bool goodback=true;
		while(i>0&&hist->Integral(hist->FindBin(800),hist->FindBin(1000))<thres){
			if(i-j>0&&goodback){
				double T1=maxxi[i-j];
				if(abs(T1-K1)<2.1||hist->Integral(hist->FindBin(800),hist->FindBin(1000))<thres*0.05){
					TH1* tmp=inhist->ProjectionY("tmpprogY",i-j,i-j);
					hist->Add(tmp);
					delete tmp;
				}else{
					goodback=false;
				}
			}
			if(j<=freeforward){
				double T1=maxxi[i+j];
				if(abs(T1-K1)<2.1){
					TH1* tmp=inhist->ProjectionY("tmpprogY",i+j,i+j);
					hist->Add(tmp);
					delete tmp;
				}else{
					freeforward=j-1;
				}
			}			
			j++;
			if(j>N)break;
		}

		hist->GetXaxis()->SetRangeUser(800,1200);
		K1=hist->GetBinCenter(hist->GetMaximumBin());
		hist->GetXaxis()->SetRange();

		double gain=K1/924.34;
		double L1=935.35*gain;
		
		vector< jPeakDat > fInput;
		fInput.push_back(jPeakDat(K1,1,4*gain));
		fInput.push_back(jPeakDat(L1-K1,1,1*gain,0.1,0.09));
		FullFitHolder* fitA=Ultrapeak::PeakFit(hist,825*gain,1015*gain,fInput,4,0,"4.5","16","0.45");
		
		if(!fitA)continue;
		
		gain=(fitA->CVal(Ultrapeak::VPC(0))+fitA->CVal(Ultrapeak::VOff))/924.34;
	
		hist->Rebin(2);
		double K2=466.78*gain;
// 		
		hist->GetXaxis()->SetRangeUser(K2-25,K2+25);
		K2=hist->GetBinCenter(hist->GetMaximumBin());
		hist->GetXaxis()->SetRange();
// 		
		gain=(K1-K2)/(924.34-466.78);
		double L2=K2+(533.66-466.78)*gain;
// 		double L2=533.66*gain;

		vector< jPeakDat > fInputB;
		fInputB.push_back(jPeakDat(K2,1,4*gain));
		fInputB.push_back(jPeakDat(L2-K2,1,1*gain,0.8,0.5));
		
		FullFitHolder* fitB=Ultrapeak::PeakFit(hist,K2-69*gain,L2+60*gain,fInputB,5,0,"3.8","12","0.005");

		if(!fitB)continue;
		
		double K1A=fitA->CVal(Ultrapeak::VPC(0))+fitA->CVal(Ultrapeak::VOff);
		double L2A=fitB->CVal(Ultrapeak::VPC(1))+fitB->CVal(Ultrapeak::VOff);
		double K2A=fitB->CVal(Ultrapeak::VPC(0))+fitB->CVal(Ultrapeak::VOff);
		

// 		hist->GetXaxis()->SetRangeUser(200,600);
// 		TCanvas* C1=new TCanvas();
// 		hist->DrawCopy();
// 		fitA->DrawCopy("same");
// 		fitB->DrawCopy("same");
// 		C1->WaitPrimitive(); 	
				
		TGraph xy;
		xy.SetPoint(0,K2A,466.78);
// 		xy.SetPoint(1,L2A,533.66);
		xy.SetPoint(1,K1A,924.34);
		TF1 line("pol1","pol1",0,1500);
		xy.Fit(&line,"0QW");
		xy.Fit(&line,"0");
		
		gk2.SetPoint(gk2.GetN(),i-0.5,K2A);
		gl2.SetPoint(gl2.GetN(),i-0.5,L2A);
		gk1.SetPoint(gk1.GetN(),i-0.5,K1A);
		
		outfile<<endl<<lab<<" "<<line.GetParameter(0)<<" "<<line.GetParameter(1);
	
		delete hist;
	}
	
	inhist->GetYaxis()->SetRangeUser(150,1200);

	
	TCanvas* C2=new TCanvas();
	inhist->DrawCopy("col");
	C2->cd();
	gPad->Update();
	gk2.SetLineWidth(3);
	gk2.DrawClone("samel");
	gl2.DrawClone("samel");
	gk1.DrawClone("samel");
	C2->SaveAs("Result.root");
	C2->WaitPrimitive();

	
	outfile.close();
	
	
	gApplication->Terminate();
	
}
