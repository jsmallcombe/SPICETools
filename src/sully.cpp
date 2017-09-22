#include "spice_cal.h"



double BiPeaks[6]={481.6935,553.8372,565.8473,975.651,1047.795,1059.805};

int Fit207BiCalSpec(TH1* hist,double& offset,double& gain){if(!hist)return 0;





}


int Fit207BiCal(TH1* hist,double& offset,double& gain){if(!hist)return 0;
	
	double range = (BiPeaks[3]-offset)/gain;

	TSpectrum sp(3);
	hist->GetXaxis()->SetRangeUser(range*0.25,range*0.75);
	sp.Search(hist,2.,"",0.06);
// 	Double_t *peak_pos=sp[i]->GetPositionX();
	double lowerpeak=sp.GetPositionX()[0];
	hist->GetXaxis()->SetRangeUser(range*0.75,range*1.5);
	sp.Search(hist,2.,"",0.06);
	double upperpeak=sp.GetPositionX()[0];

	
	gain = ((BiPeaks[3]-BiPeaks[0])/(upperpeak-lowerpeak));
	offset = BiPeaks[0]-(gain*lowerpeak);
	
	hist->GetXaxis()->SetRangeUser(1,-1);
	
	hist->Rebin(2);
	
		// An all singing all dancing function to fit N peaks that are in close enough proximity to assume constant peak parameters
	// int=1 specified to attempt to use step background int=0 uses linear
	FullFitHolder* fit=Ultrapeak::PeakFit(hist,double fLeftUser,double fRightUser,vector< jPeakDat > &fInput,int backmode=0,int peaktype=0,string sig="",string dec="",string sha="");
	
	
	
	
	
	vector<double> peaks =SullyFit(hist,offset,gain);
	if(peaks.size()<6)return 0;
	
	TGraph xy;
	for(int i=0;i<6;i++){
		xy.SetPoint(xy.GetN(),peaks[i],BiPeaks[i]);
	}
	
	TF1 line("pol1","pol1");
	xy.Fit(&line,"QN");
	offset=line.GetParameter(0);
	gain=line.GetParameter(1);
// 	xy.DrawClone("ap");
	
	return 1;
}



vector<double> SullyFit(TH1* hist,double& offset,double& gain){
	double peak_pos[2]={(BiPeaks[0]-offset)/gain,(BiPeaks[3]-offset)/gain};
	vector<double> centroids;

	//fit two biggest peaks to get fit parameters used to fit the full triplet (uses function declared above)
	for (Int_t i=0; i<2; i++){
		// declare TF1 to fit the first peak in each triplet
		TF1 *f1 = new TF1("f1", SullyPeak, 0, 2000, 5); // Creates PeakFit function with 5 parameters in the range [0, 2000]
		
		//cout << " peak " << i << " is: " << peak_pos[i] <<endl;
		hist->GetXaxis()->SetRangeUser(peak_pos[i]-10./gain, peak_pos[i]+10./gain);
		double centroid_1 = hist->GetXaxis()->GetBinCenter(hist->GetMaximumBin());
		f1->SetRange(( centroid_1)-20./gain,( centroid_1)+20./gain);
		if(hist->Integral()<50) continue;
		
		f1->SetParameter(0,hist->GetMaximum()*0.5);
		f1->SetParLimits(0, 0,hist->GetMaximum()*1.5);
		f1->SetParameter(1,centroid_1);
		f1->SetParLimits(1,( centroid_1)-10./gain,( centroid_1)+10./gain);
		f1->SetParameter(2,1.0/gain);
		f1->SetParLimits(2,0.4/gain,10./gain);
		f1->SetParameter(4,0);
		f1->SetParLimits(4,0.,hist->GetMaximum()*0.05);
		hist->GetXaxis()->SetRangeUser(( centroid_1)-30./gain,( centroid_1)+30./gain);
		hist->Fit(f1, "RQN");
		
		double centroid = f1->GetParameter(1);
		double sigma = f1->GetParameter(2);
		
		////////////////////////////////////////////////////////////////////////////fit all three peaks////////////////////////////////////////////////////////////////////// 
		//the Erfc are to fit the steps
		//the (x<[0])*[9]*(x-[0])) terms are used to fit the exponential decay on ONE side of the centroid
		TF1 *f2 = new TF1("f2", "[5]*exp(-(pow(([0]-x)/([1]*5-(x<[0])*[9]*(x-[0])),2)))+[8]*(TMath::Erfc((x-[0])/(TMath::Sqrt(2.0)*[1]))/2.)+([5]*[6])*exp(-(pow(([0]+[2]-x)/([1]*5-(x<([0]+[2]))*[9]*(x-([0]+[2]))),2)))+[8]*[6]*(TMath::Erfc((x-[0]-[2])/(TMath::Sqrt(2.0)*[1]))/2.)+([5]*[6]*[7])*exp(-(pow(([0]+[2]+[3]-x)/([1]*5-((x<([0]+[2]+[3]))*[9]*(x-([0]+[2]+[3])))),2)))+[8]*[6]*[7]*(TMath::Erfc((x-[0]-[2]-[3])/(TMath::Sqrt(2.0)*[1]))/2.) +[4]",centroid-50./gain ,centroid+150./gain );
		//set range
		hist->GetXaxis()->SetRangeUser(centroid-70./gain ,centroid+150./gain);
		if(hist->Integral()<50) continue;
		///////////parametes////////////
		//centroid
		f2->SetParameter(0,centroid); 
		f2->SetParLimits(0,centroid-5./gain,centroid+5./gain); 
		//width
		f2->SetParameter(1,sigma);
		f2->SetParLimits(1,0.4/gain,10./gain);
		//background
		f2->SetParameter(4,0);
		f2->SetParLimits(4,0,f1->GetParameter(3)*2);
		//height
		f2->SetParameter(5, hist->GetMaximum());
		//scaling factor for the second peak wrt the first peak
		f2->SetParameter(6, .333);
		f2->SetParLimits(6, 0, 0.5);
		//scaling factor for the third peak wrt the second peak
		f2->SetParameter(7, .5);
		f2->SetParLimits(7, 0, 1);
		//limit on scaling factor for the step piece of the function
		f2->SetParameter(8, f2->GetParameter(4));
		f2->SetParLimits(8, 0., f2->GetParameter(4)*3);
		//scaling factor for exponential decay
		f2->SetParameter(9, 0.5/gain);
		f2->SetParLimits(9, 0.05/gain, 10/gain);
		//how many points are drawn in the fit
		f2->SetNpx(10000);
		
		double peak2=(BiPeaks[i*3+1]-BiPeaks[i*3])/gain;
		double peak3=(BiPeaks[i*3+2]-BiPeaks[i*3+1])/gain;
		
		//where to look for the second peak in the triplet
		f2->SetParameter(2,peak2); 
		//where to look for the third peak in the triplet
		f2->SetParameter(3,peak3); 
		f2->SetParLimits(2,peak2-10./gain,peak2+10./gain); 
		f2->SetParLimits(3,peak3-5./gain,peak3+5./gain);
		
		//hist->Draw();
		hist->Fit(f2, "QR+"); 
		
		centroids.push_back(f2->GetParameter(0));
		centroids.push_back(f2->GetParameter(0)+f2->GetParameter(2));
		centroids.push_back(f2->GetParameter(0)+f2->GetParameter(2)+f2->GetParameter(3));
		
		delete f1,f2;
	}
	hist->GetXaxis()->SetRangeUser(1,-1);
	
	return centroids;
}