
// This function reads in your experimental data points form a plain .txt file
// Use 4 columns  Xvalue Yvalue Xuncertainty Yuncertainy 
// File should contain numbers only
TGraphErrors* read(std::string in){
	TGraphErrors* ret=0;
	std::ifstream infofile(in.c_str());
	if(infofile.good()){
			ret=new TGraphErrors();
			double x,y,dx,dy;
			while(infofile>>x>>y>>dx>>dy){
				int N=ret->GetN();
				ret->SetPoint(N,x,y);
				ret->SetPointError(N,dx,dy);
			}
			infofile.close();
	}
	return ret;
}

void ScalingTF1(){
	//Load the simulated efficiency curve
	//Change file name as needed
	TFile spiceeff("SPICEsim.root","READ");
	gROOT->cd();
	if(!spiceeff.IsOpen()){cout<<endl<<"No SPICEsim.root"<<endl;return;}
	if(!spiceeff.FindKey("SpiceEffCurve")){cout<<endl<<"No SpiceEffCurve"<<endl;return;}
	TGraph* SpiceEffGraph= (TGraph*)spiceeff.Get("SpiceEffCurve");
	
	//Load the experimental data
	//Change file name as needed
	TGraphErrors* data=read("data.txt");
	
	TF1 * f = new TF1("f",[&](double*x, double *p){ return p[0]*SpiceEffGraph->Eval(x[0]); }, 0,2000, 1);
	f->SetParameters(0.5,0);
	data->Fit(f,"+");
	
	TF1 * pol1 = new TF1("p1","pol1", 300,2000);
	pol1->SetLineColor(1);
	pol1->SetParameters(0.5,-0.0005);
	data->Fit(pol1,"R+");
	
	TF1 * pol2 = new TF1("p2","pol2", 300,2000);
	pol2->SetLineColor(8);
	pol2->SetParameters(0.5,-0.0005,0.0);
	data->Fit(pol2,"R+");
	
	TF1 * expo = new TF1("p2","expo", 300,2000);
	expo->SetLineColor(4);
	expo->SetParameters(0.5,200);
	data->Fit(expo,"R+");
	
	TF1 * FF = new TF1("f",[&](double*x, double *p){ return (p[0]+p[1]*x[0])*SpiceEffGraph->Eval(x[0]); }, 300,2000, 2);
	FF->SetLineColor(6);
	FF->SetParameters(0.8,-0.0003);
	data->Fit(FF,"R+");
	
	TCanvas* C1=new TCanvas();
	C1->cd();
	SpiceEffGraph->SetLineWidth(2);
	SpiceEffGraph->DrawClone("al");
	data->SetLineWidth(2);
	data->DrawClone("sameP");

	spiceeff.Close();
}



