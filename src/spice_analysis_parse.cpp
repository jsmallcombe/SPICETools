#include "spice_analysis_parse.h"

///////////////////////
///////////////////////
//  DEFINE VARIABLES //
///////////////////////
///////////////////////

double TRFperiodns= 84.409;
int TRFperiodps= 84409;

typedef struct gate2Ddata{
	TGraph gate;
	string title;
	std::vector< double > ring_beta;
	bool use_rb;
	TGraph theta_beta;
	bool use_tb;
	TGraph theta_theta;
	bool use_tt;
	bool use_beta;
	std::vector< short > s3;
	bool s3Limit;
	unsigned int mass;
	bool UseEnergyBeta;
} gate2Ddata;

double pi=TMath::Pi();

enum gatenames{s3_gamma_t,s3_sili_t,gamma_gamma_t,sili_sili_t,gamma_sili_t,rf_S3,rf_sili,rf_gamma,rf_S3cyc,rf_silicyc,rf_gammacyc};
vector< string > gatetitles={"s3_gamma_t","s3_sili_t","gamma_gamma_t","sili_sili_t","gamma_sili_t","rf_S3","rf_sili","rf_gamma","rf_S3cyc","rf_silicyc","rf_gammacyc"};


vector< pair<double,double> > gates;
vector< gate2Ddata > ParticleGate;
vector< TGraph > s3silirf2D;
vector< pair<unsigned int,unsigned int> > ringgroups;

enum controlenum{BetaZero,TigressDistance,FrontBackEnergy,FrontBackTime,S3EnergyLimit,SiLiWaveTOffset,TigressTargetOffset,SiLiNoiseLimit,SiLiSmirnovLimit};
vector< string > controlnames={"BetaZero","TigressDistance","FrontBackEnergy","FrontBackTime","S3EnergyLimit","SiLiWaveTOffset","TigressTargetOffset","SiLiNoiseLimit","SiLiSmirnovLimit"};
vector< double > control={0.0,110.,0.9,75,50000,7000,-8,0.15,500};

std::vector< string > filelist;
std::vector< long > fileentriessum;
std::vector< double > fileoffset,filegain;
double FileOffset=0,FileGain=1;

bool s3used[4]={0,0,0,0};
short s3index[4]={0,0,0,0};
bool MultiS3=false;

unsigned short s3id(TS3Hit* s3hit){
	if(MultiS3){
		unsigned short i=s3hit->GetArrayPosition();
		if(i<4)return s3index[i];
	}
	return 0;
}
unsigned short s3r(TS3Hit* s3hit){
	int r=s3hit->GetRing();
	if(MultiS3){
		unsigned short i=s3hit->GetArrayPosition();
		if(i<4)return s3index[i]*24+r;
	}
	return r;
}


///////////////////////
///////////////////////
//Declare subroutines//
///////////////////////
///////////////////////
bool t_gate(double,gatenames);
void t_gateRFmake(gatenames);
bool t_gateRFcycles(double,gatenames);
long t_stamp(TRF*,TTigress*,TSiLi*,TS3*);
long t_stamp_fix(long &);
TGraph* FileTGraph(string filepath);
double calc_beta_KE(double emev,double Amass);
short TigAng(TTigressHit*);

///////////////////////
///////////////////////
// START OF MAIN FUNCTION //
///////////////////////
///////////////////////


int analysis_parse(int argc, char *argv[]){	
	
gates.clear();
ParticleGate.clear();
s3silirf2D.clear();
ringgroups.clear();

for(int z=0;z<gatetitles.size();z++)gates.push_back(pair < double,double >{-1E10,1E10});

/////////////////////////////////////////////////////////////////
////////////////// PROCESS COMMAND LINE INPUTS //////////////////
/////////////////////////////////////////////////////////////////	

SPICEcmdline inp(argc,argv);
TChain *DataChain=inp.AnalysisTree(&fileentriessum,&filelist);
if(!inp.LoadCal(DataChain)){
	cout<<"FAIL";
	return 0;
}
string outputfile=OrDefault("AparserOut.root",inp.RootFile("gate"));

bool RemoveTimeGaps=!inp.IsPresent("TimeCompressOff");//for decay work or to see DAQ problems you may want to false this
if(RemoveTimeGaps)cout<<endl<<"Time Gaps Now unsuppressed.";

bool UseSiLiRFCoinc=inp.IsPresent("UseSiLiRFCoinc");
if(UseSiLiRFCoinc)cout<<endl<<"Using S3-SiLi RF gate.";

bool UseFitCharge=inp.IsPresent("UseFitCharge");
if(UseFitCharge)cout<<endl<<"Using SiLi Waveform fit charge.";

bool DoDoubleElectrons=inp.IsPresent("DoDoubleElectrons");
if(DoDoubleElectrons)cout<<endl<<"Performing double electron analysis.";

bool Telescope=inp.IsPresent("Telescope");
if(Telescope)cout<<endl<<"Using S3 Telescope.";

bool KeepChargeShare=inp.IsPresent("KeepChargeShare");
if(KeepChargeShare)cout<<endl<<"Keeping S3 Charge Sharing Events.";

bool FirstOnly=inp.IsPresent("FirstOnly");
if(FirstOnly)cout<<endl<<"Filling particle gates only once.";

bool MultiParticles=inp.IsPresent("MultiParticles");
if(MultiParticles)cout<<endl<<"Making Multi-Particle Histograms.";

bool PreferenceSectors=inp.IsPresent("PreferenceSectors");
if(PreferenceSectors)cout<<endl<<"Preference S3 Sectors.";

bool AddMonitor=inp.IsPresent("AddMonitor");
if(AddMonitor)cout<<endl<<"Adding TGenericDet Monitor Histograms.";

bool DS=!inp.IsPresent("NoSPICE");
if(!DS)cout<<endl<<"Omitting SPICE Histograms.";

bool TigressSuppressed=inp.IsPresent("TigressSuppressed");
if(TigressSuppressed){
	cout<<endl<<"Tigress Suppressed Position.";
	control[TigressDistance]=145.0;
}
TTigress::SetArrayBackPos(!TigressSuppressed);

//Single number control parameters
for(int z=0;z<controlnames.size();z++)
	if(inp.IsPresent(controlnames[z])){
		control[z]=inp.Next(controlnames[z]);
		cout<<endl<<"Setting "<<controlnames[z]<<" "<<control[z];
	}
	

//1D gates		
for(int z=0;z<gatetitles.size();z++){
	if(inp.IsPresent(gatetitles[z])){
		inp.NextTwo(gatetitles[z],gates[z].first,gates[z].second);
		higher_jd(gates[z].first,gates[z].second);
		cout<<endl<<"Setting gate "<<gatetitles[z]<<" "<<gates[z].first<<"  "<<gates[z].second;
	}
}

bool GainDrift=inp.IsPresent("gaindrift.txt");
if(GainDrift){
	string fst=inp.ReturnFind("gaindrift.txt");
	ifstream f(fst.c_str());
	if(f.good()){
		
		string sfilen;
		double soffset,sgain;
		vector< string > Sfilen;
		vector< double > Soffset,Sgain;
		
		while(f>>sfilen>>soffset>>sgain){
			Sfilen.push_back(sfilen);
			Soffset.push_back(soffset);
			Sgain.push_back(sgain);
		}
		
		int mcount=0;
		for(unsigned int i=0;i<filelist.size();i++){
			fileoffset.push_back(0);
			filegain.push_back(0);
			
			for(unsigned int j=0;j<Sfilen.size();j++){
				if(filelist[i].find(Sfilen[j])<filelist[i].size()){
					fileoffset[i]=Soffset[j];
					filegain[i]=Sgain[j];
					mcount++;
					if(i==0){
						FileOffset=Soffset[j];
						FileGain=Sgain[j];
					}
					break;
				}
			}
		}
		
		cout<<endl<<"Loaded drift file : "<<fst<<" "<<Sfilen.size()<<" entries "<<mcount<<" files matched."<<flush;
			
		f.close();
	}else{
		cout<<endl<<"Failed to open gain drift file : "<<fst<<flush;
		GainDrift=false;
	}
}



cout<<flush;

gROOT->cd();


/////////////////////////////////////////////////////////////////
//////////////////     PROCESS GATE TYPE INPUTS      ////////////
/////////////////////////////////////////////////////////////////

TVector3 S3OffsetVector(0,0,0);
inp.Rewind();
string str;
while(inp>>str){
	
	//Set multiple S3 detectors
	if(str.find("MultiS3")<str.size()){
		unsigned short s;
		inp>>s;
		if(s<4)	s3used[s]=true;
	}
	
	if(str.find("S3Vector")<str.size()){
// 		unsigned short s;
		double x,y,z;
		inp>>x>>y>>z;
		S3OffsetVector=TVector3(x,y,z);
		cout<<endl<<"Using S3 offset "<<x<<" "<<y<<" "<<z<<" mm";
	}	
	

	//Data file loading
	if(str.find("RingGroup")<str.size()){
			pair<unsigned int,unsigned int> rg;
			inp>>rg.first>>rg.second;
			ringgroups.push_back(rg);
			cout<<endl<<"New Ring Group "<<rg.first<<"-"<<rg.second<<flush;
	}
	
	//2D gates and any associated kinematic data
	int strfi=str.find("s3_rs_2D");
	if(strfi<str.size()){
		//Get the number at the end of the name
		int X=strfi+8;
		int N=abs(atoi(str.substr(X,str.size()-X).c_str()));
		
		while(ParticleGate.size()<=N)ParticleGate.push_back(gate2Ddata());
		
		string mode;
		inp>>mode;
		
		if(mode.find("title")<mode.size()){
			inp>>ParticleGate[N].title;
		}else if(mode.find("ring")<mode.size()){
			int ring;double beta;
			inp>>ring>>beta;
			while(ParticleGate[N].ring_beta.size()<=ring)ParticleGate[N].ring_beta.push_back(0);
			ParticleGate[N].ring_beta[ring]=beta;
		}else if(mode.find("MS3")<mode.size()){
			short s;
			inp>>s;
			ParticleGate[N].s3.push_back(s);
		}else if(mode.find("UseEnergyBeta")<mode.size()){
			inp>>ParticleGate[N].mass;
			ParticleGate[N].UseEnergyBeta=true;;
		}else{
			if(mode.find("file")<mode.size()){
				string gfile;
				inp>>gfile;
				TGraph* G=FileTGraph(gfile);
				if(G){
					if(mode.find("beta")<mode.size()){
						cout<<endl<<"Setting TGraph as beta."<<flush;
						ParticleGate[N].theta_beta=*G;
					}else if(mode.find("theta")<mode.size()){
						cout<<endl<<"Setting TGraph as theta."<<flush;
						ParticleGate[N].theta_theta=*G;
					}else{
						cout<<endl<<"Setting TGraph as gate."<<flush;
						ParticleGate[N].gate=*G;
					}
					delete G;
				}
			}else{
				double x,y;
				inp>>x>>y;
				TGraph* G=0;
				
				if(mode.find("beta")<mode.size()){
					G=&ParticleGate[N].theta_beta;
				}else if(mode.find("theta")<mode.size()){
					G=&ParticleGate[N].theta_theta;
				}else{
					G=&ParticleGate[N].gate;
				}
				
				if(G)G->SetPoint(G->GetN(),x,y);
			}
		}
	}
	
	//2D gates and any associated kinematic data
	strfi=str.find("s3silirf2D");
	if(strfi<str.size()){
		//Get the number at the end of the name
		int X=strfi+10;
		int N=abs(atoi(str.substr(X,str.size()-X).c_str()));
		
		while(s3silirf2D.size()<=N)s3silirf2D.push_back(TGraph());
		
		double x,y;
		inp>>x>>y;
		s3silirf2D[N].SetPoint(s3silirf2D[N].GetN(),x,y);
	}

}
gROOT->cd();

int c=0;
stringstream cc;
cc<<"Multiple S3s position ";
for(int i=0;i<4;i++)if(s3used[i]){
	c++;cc<<i<<" ";
}

if(c>1){
	MultiS3=true;
	cout<<endl<<cc.str();
}

//
// Format the gates a bit
//
for(int y=0;y<ParticleGate.size();y++){
	if(ParticleGate[y].gate.GetN()<3){
		ParticleGate.erase(ParticleGate.begin()+y);
		y--;
		continue;
	}
	
	if(ParticleGate[y].title.size()<1){
		stringstream ss;
		ss<<"Gate"<<y;
		ParticleGate[y].title=ss.str();
	}
	string t=ParticleGate[y].title;
	ParticleGate[y].gate.SetTitle(t.c_str());
	
	ParticleGate[y].use_beta=false;
	ParticleGate[y].use_rb=true;
	ParticleGate[y].use_tb=false;
	if(ParticleGate[y].ring_beta.size()==0){
		ParticleGate[y].use_beta=(control[BetaZero]);
		ParticleGate[y].use_rb=false;
		if(ParticleGate[y].theta_beta.GetN()>1){
			ParticleGate[y].use_tb=true;
			ParticleGate[y].use_beta=true;
			ParticleGate[y].theta_beta.SetTitle((t+"beta").c_str());
		}
	}
	
	
	ParticleGate[y].use_tt=false;
	if(ParticleGate[y].theta_theta.GetN()>1){
		ParticleGate[y].use_tt=true;
		ParticleGate[y].theta_theta.SetTitle((t+"thetaVS").c_str());
	}
	
	ParticleGate[y].s3Limit=false;
	if(MultiS3&&ParticleGate[y].s3.size())ParticleGate[y].s3Limit=true;
	
	
	if(ParticleGate[y].mass&&ParticleGate[y].UseEnergyBeta){
		//If beta from energy override everything else
		ParticleGate[y].ring_beta.clear();
		ParticleGate[y].theta_beta=TGraph();
		ParticleGate[y].theta_theta=TGraph();
		ParticleGate[y].use_beta=true;
		ParticleGate[y].use_rb=false;
		ParticleGate[y].use_tb=false;
		ParticleGate[y].use_tt=false;
	}else{ParticleGate[y].UseEnergyBeta=false;}
}

for(int y=0;y<s3silirf2D.size();y++){
	if(s3silirf2D[y].GetN()<3){
		s3silirf2D.erase(s3silirf2D.begin()+y);
		y--;
		continue;
	}
}


if(gates[rf_S3cyc].first<-1E9&&gates[rf_S3cyc].second>1E9)gates[rf_S3cyc]=gates[rf_S3];
if(gates[rf_silicyc].first<-1E9&&gates[rf_silicyc].second>1E9)gates[rf_silicyc]=gates[rf_sili];
if(gates[rf_gammacyc].first<-1E9&&gates[rf_gammacyc].second>1E9)gates[rf_gammacyc]=gates[rf_gamma];
t_gateRFmake(rf_S3cyc);
t_gateRFmake(rf_silicyc);
t_gateRFmake(rf_gammacyc);

/////////////////////////////////////////////////////////////////
/////////////////  Check all needed inputs     //////////////////
/////////////////////////////////////////////////////////////////


//Generates a list of which entry is the start of a new run
std::vector< long > runstartentries;
std::vector< long > runchangeentries;
runstartentries.push_back(0);
runchangeentries.push_back(0);

string runstrprev=filelist[0].substr(filelist[0].find("analysis")+8,filelist[0].size());
runstrprev=runstrprev.substr(0,runstrprev.find("_"));

for(unsigned int i=1;i<filelist.size();i++){
	string runstr=filelist[i].substr(filelist[i].find("analysis")+8,filelist[i].size());
	runstr=runstr.substr(0,runstr.find("_"));
	
	if(filelist[i].find("_000.")<filelist[i].size()){
		runstartentries.push_back(fileentriessum[i-1]);
		runchangeentries.push_back(fileentriessum[i-1]);
	}else if(runstr!=runstrprev){
		runchangeentries.push_back(fileentriessum[i-1]);
		//Sub run 000 may be skipped for some reason (noise etc) still need to track run change
	}
	runstrprev=runstr;
}

runstartentries.push_back(DataChain->GetEntries());
runchangeentries.push_back(DataChain->GetEntries());

////////////////// SETUP THE TCHAIN //////////////////	

TSiLi *sili = 0;
TTigress *tigress = 0; 
TS3 *s3 = 0;
TRF *rf = 0;
TGenericDetector *gd = 0;
// TPaces *gd = 0;

if(DataChain->FindBranch("TSiLi"))DataChain->SetBranchAddress("TSiLi",&sili);else sili=new TSiLi();
if(DataChain->FindBranch("TTigress"))DataChain->SetBranchAddress("TTigress",&tigress);else tigress=new TTigress();
if(DataChain->FindBranch("TS3"))DataChain->SetBranchAddress("TS3",&s3);else s3=new TS3();
if(DataChain->FindBranch("TRF"))DataChain->SetBranchAddress("TRF",&rf);else rf=new TRF();
if(AddMonitor)if(DataChain->FindBranch("TGenericDetector"))DataChain->SetBranchAddress("TGenericDetector",&gd);else gd=new TGenericDetector();
// if(AddMonitor)if(DataChain->FindBranch("TPaces"))DataChain->SetBranchAddress("TPaces",&gd);else gd=new TPaces();

TSiLiHit *sili_hit;
TTigressHit *tigress_hit;
long nentries = DataChain->GetEntries();

if(!nentries)return 0;

if(UseFitCharge){
	TChannel::SetIntegration("SPI",1);
	TChannel::SetIntegration("SPE",125);
}else TChannel::SetIntegration("SP",125);
TChannel::SetIntegration("BA",125);
TChannel::SetIntegration("TI",125);
TChannel::SetIntegration("GD",125);
// TChannel::SetIntegration("PA",125);
TChannel::SetUseCalFileIntegration("BA",true);
TChannel::SetUseCalFileIntegration("SP",true);
TChannel::SetUseCalFileIntegration("TI",true);
TChannel::SetUseCalFileIntegration("GD",true);
// TChannel::SetUseCalFileIntegration("PA",true);

TTigress::SetForceCrystal();
TTigress::SetTargetOffset(control[TigressTargetOffset]);


//
// Set the control parameters used by the S3 pixel build
//

s3->SetFrontBackTime(control[FrontBackTime]);
if(Telescope){
	s3->SetMultiHit(false);
	s3->SetFrontBackEnergy(0);
}else{
	s3->SetMultiHit(true);
	s3->SetFrontBackEnergy(control[FrontBackEnergy]);
	if(PreferenceSectors)s3->PreferenceSector();//Use the sector energy mostly, worse resolution but strangely more stable
	if(KeepChargeShare)s3->SetKeepShared();
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
////////////////// PREPARE OUTPUT FILE //////////////////
//////////////////   AND HISTOGRAMS   //////////////////
//////////////////   AND TIMESTAMP   //////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////
////////////// CALCULATE ROUGH TIME STAMP RANGE //////////////////
//////////////////////////////////////////////////////////////////
cout<<endl<<endl<<"Calculating approximate run length. ";

long timestampstart=0;

long timestamp_max=0xffffffffffff;
double timestamp_hour=36E10;//double because used for division where sub integers expected
long timestamp_thresh=timestamp_max*0.99;

long tpgap=1E10;//equivalent to about 1.5 minutes

double tickspereven=0;
int validtimes=0;
long timeaveragesamples=20;
long samples=nentries/timeaveragesamples;

TH1F* timequick= new TH1F("timequick","timequick",timeaveragesamples*1000,0,timeaveragesamples*1000);

long timemax=0;

for(int i=0;i<timeaveragesamples;i++){
	TGraph timegrad;
	
	for(int jentry=0;jentry<1000;jentry++){
		long k=jentry+i*samples;
		if(k>=nentries)break;
		DataChain->GetEntry(k);
		ulong t=t_stamp(rf,tigress,sili,s3);
		if(t>0)timegrad.SetPoint(timegrad.GetN(),jentry,t);
		timequick->Fill(i*1000+jentry,t);
	}
	
	TF1 timef1grad("timef1grad","pol1",0,1000);
	timegrad.Fit(&timef1grad,"QR");//Warning in <TROOT::Append>: Replacing existing TH1: Graph (Potential memory leak).
	double chi=timef1grad.GetChisquare()/timef1grad.GetNDF();
	if(timef1grad.GetParameter(1)>0){//1E15 might seem high but is an experimentally determined limit, was 1E20 before I fixed tstamp
		//cout<<endl<<timef1grad.GetParameter(1)<<" "<<chi;
		if(chi<1E15){cout<<"+"<<flush;
			tickspereven+=timef1grad.GetParameter(1);
			validtimes++;
			if(timemax<timef1grad.Eval(1000))timemax=timef1grad.Eval(1000);
		}
	}
	
	if(i==0){if(timef1grad.GetParameter(0)>0)timestampstart=timef1grad.GetParameter(0);}
}cout<<endl;
tickspereven/=validtimes;

timequick->Sumw2(kFALSE);

//If the rate at any long point is much lower than samples, there will be more time intervals between each event
//Hence we will have underestimated the experiment length and have overflow
long tstamprange=(tickspereven*nentries*1.2);
//I've increased the number of samples to tackle this

//If we turn the compression off i.e. for a decay run, then we need a higher max 
if(!RemoveTimeGaps&&timemax>tstamprange)tstamprange=timemax*1.2;

cout<<"Average ticks between events : "<<tickspereven<<endl;
cout<<"Number of events : "<<nentries<<endl;
cout<<"Starting tick : "<<timestampstart<<endl;
cout<<"Estimated run length x1.2 safety margin : "<<tstamprange<<endl;
cout<<"Approximate Hours : "<<tstamprange/timestamp_hour<<endl;

long timestamp_recover=timestampstart;

////////////////////////////////////////////////
////////////////// HISTOGRAMS //////////////////
////////////////////////////////////////////////

cout<<endl<<"Creating output file : "<<outputfile<<flush;

gROOT->cd();
TFile *outfile = new TFile(outputfile.c_str(),"RECREATE");

#include "analysis_parse_histogram_list.h"


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  
////////////////// IMPORTANT LOOP VARIABLES //////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////	

// Timestamp tracking variables
long movelonghold[128];
for(int i=0;i<128;i++)movelonghold[i]=timestampstart;
long movelong=(timestampstart<<7);
unsigned int movelongi=0;
unsigned int overTlimit=0;
long timestamp_add=-timestampstart;

// Sorting time completion estimation
double currentsettime=0;
double nextsettime=0;
double currentsetpercent=0;
double nextsetpercent=0;
long updateinterval=10000;
long onepercent=floor(nentries/(100*updateinterval))*updateinterval;
if(onepercent<updateinterval)onepercent=updateinterval;
TStopwatch stopper;

int fileiterator=0;
int runstartrator=0;
int runiterator=0;

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  
////////////////// MAIN PROCESS OF THE CHAIN //////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////	

cout<<endl<<endl<<"Processing files"<<endl;
for(long jentry=0;jentry<nentries;jentry++){
	#include "analysis_parse_event_loop.h"
}
cout<<endl<< "100 \% COMPLETE          \n";


///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  
///////////////// DO FINAL TASKS AND SAVE FILE ////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////	


///////////////////////////////////////////////////////////////  
//////////////////// Rate Ratio Histograms ////////////////////
///////////////////////////////////////////////////////////////
gROOT->cd();
if(DS){
//These aren't saved 
TAxis* AY=eventN_gamma->GetYaxis();
TAxis* AYY=fileN_gamma->GetYaxis();

TH1D* highgamma = eventN_gamma->ProjectionX("highgamma",AY->FindBin(350),AY->FindBin(1000));
TH1D* highgammaF = fileN_gamma->ProjectionX("highgammaF",AYY->FindBin(350),AYY->FindBin(1000));

outfile->cd("RunTime");
	// Event N vs ratios
	TH1D* eventN_e_y_highratio = eventN_sili->ProjectionX("eventN_e_y_highratio",AY->FindBin(350),AY->FindBin(1000));
	eventN_e_y_highratio->SetTitle("eventN_e_y_highratio");
	axislab(eventN_e_y_highratio,"Event No.","e/y ratio");

	TH1D* eventN_sili_noiseratio = eventN_sili->ProjectionX("eventN_sili_noiseratio",1,AY->FindBin(200));
	eventN_sili_noiseratio->SetTitle("eventN_sili_noiseratio");
	axislab(eventN_sili_noiseratio,"Event No.","noise to data ratio");

	// File N vs ratios
	TH1D* fileN_e_y_highratio = fileN_sili->ProjectionX("fileN_e_y_highratio",AYY->FindBin(350),AYY->FindBin(1000));
	fileN_e_y_highratio->LabelsDeflate();
	fileN_e_y_highratio->SetTitle("fileN_e_y_highratio");
	axislab(fileN_e_y_highratio,"Event No.","e/y ratio");
	
	TH1D* fileN_sili_noiseratio = fileN_sili->ProjectionX("fileN_sili_noiseratio",1,AYY->FindBin(200));
	fileN_sili_noiseratio->LabelsDeflate();
	fileN_sili_noiseratio->SetTitle("fileN_sili_noiseratio");
	axislab(fileN_sili_noiseratio,"Event No.","noise to data ratio");
gROOT->cd();

int NN=eventN_e_y_highratio->GetNbinsX();
for(int i=1;i<NN;i++){
	double gammas=highgamma->GetBinContent(i);
	double electrons=eventN_e_y_highratio->GetBinContent(i);
	double noise=eventN_sili_noiseratio->GetBinContent(i);
	
	if(electrons>0){eventN_sili_noiseratio->SetBinContent(i,noise/electrons);
	}else eventN_sili_noiseratio->SetBinContent(i,0);
	
	if(gammas>0)eventN_e_y_highratio->SetBinContent(i,electrons/gammas);
	else eventN_e_y_highratio->SetBinContent(i,0);
}

eventN_sili_noiseratio->SetBinContent(NN,0);
eventN_e_y_highratio->SetBinContent(NN,0);

for(int i=1;i<=fileN_e_y_highratio->GetNbinsX();i++){
	double gammas=highgammaF->GetBinContent(i);
	double electrons=fileN_e_y_highratio->GetBinContent(i);
	double noise=fileN_sili_noiseratio->GetBinContent(i);
	
	if(electrons>0){fileN_sili_noiseratio->SetBinContent(i,noise/electrons);
	}else fileN_sili_noiseratio->SetBinContent(i,0);
	
	if(gammas>0)fileN_e_y_highratio->SetBinContent(i,electrons/gammas);
	else fileN_e_y_highratio->SetBinContent(i,0);
}

delete highgamma,highgammaF;

	eventN_e_y_highratio->SetMinimum(0);
	eventN_sili_noiseratio->SetMinimum(0);
	fileN_e_y_highratio->SetMinimum(0);
	fileN_sili_noiseratio->SetMinimum(0);
	
}

///////////////////////////////////////////////////////////////  
/////////////////////// WRITING OUTPUT ////////////////////////
///////////////////////////////////////////////////////////////

cout<<endl<<"WRITING FILE"<<endl;

outfile->cd();
outfile->Write();
outfile->cd("RunTime");
	timequick->Write();
	sorttime->Write("SortTime");
	guessend->Write("GuessEndTime");
	
for(int g=0;g<ParticleGate.size();g++){

	string t=ParticleGate[g].title;
	string tf="ParticleGates/"+t;
	outfile->cd(tf.c_str());
		ParticleGate[g].gate.Write(t.c_str());
		if(ParticleGate[g].use_tt){
			ParticleGate[g].theta_theta.Write((t+"thetaVS").c_str());
		}
		if(ParticleGate[g].use_tb){
			ParticleGate[g].theta_beta.Write((t+"beta").c_str());
		}else if(ParticleGate[g].use_rb){
			int N=ParticleGate[g].ring_beta.size() ;
			TH1F h((t+"beta").c_str(),(t+"beta").c_str(),N+2,0,N+2);
			for(int i=0;i<N;i++)h.SetBinContent(i+1,ParticleGate[g].ring_beta[i]);
			h.Write();
		}
}

outfile->Close();
cout<<endl<<"SAVED"<<endl<<endl;

delete DataChain;
return 0;

}	

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  
////////////////////////// END OF MAIN ////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////	



///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////  
////////////////////////// subroutines ////////////////////////
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////	




bool t_gate(double t,gatenames g){
	if(t>=gates[g].first&&t<=gates[g].second)return true;
	return false;	
}

void t_gateRFmake(gatenames g){
	
	int f=gates[g].first*10000.;
	int s=gates[g].second*10000.;
	int w=s-f;
	
	if(w>TRFperiodps){
		f+=w*0.5;
		s=f+20000;
	}
	
	f=f%TRFperiodps;
	s=s%TRFperiodps;
	
	if(f<0)f+=TRFperiodps;
	if(s<0)s+=TRFperiodps;
	
	gates[g].first=f;
	gates[g].second=s;
}

//inputs in 10s ns
bool t_gateRFcycles(double t,gatenames g){
	
	int T=t*10000.;
	T=T%TRFperiodps;
	if(T<0)T+=TRFperiodps;
	
	if(gates[g].first<gates[g].second){
		if(T>=gates[g].first&&T<=gates[g].second)return true;
	}else{
		if(T>=gates[g].first||T<=gates[g].second)return true;
	}
		
	return false;
}


long t_stamp(TRF *rf,TTigress *tigress,TSiLi *sili,TS3 *s3){
	if(tigress){
		for(unsigned int i=0;i<tigress->GetMultiplicity();i++){
			long t=tigress->GetTigressHit(i)->GetTimeStamp();
			if(t>0)return t;
		}
	}
	if(s3){
		for(unsigned int i=0;i<s3->GetRingMultiplicity();i++){
			long t=s3->GetRingHit(i)->GetTimeStamp();
			if(t>0)return t;
		}
	}
	if(sili){
		for(unsigned int i=0;i<sili->GetMultiplicity();i++){
			long t=sili->GetSiLiHit(i)->GetTimeStamp();
			if(t>0)return t;
		}
	}
	if(rf){
		long t=rf->TimeStamp();
		if(t>0)return t;
	}
	return 0;
}


long t_stamp_fix(long &time){//Fixed since grsisort 3.0.0.0
// 	long ts=time & 0x00ffffff; //the and bit mask takes just the short 24 bits
// 	time =  time>>28; //get just the long part 
// 	time =  time<<24 | ts;//bit shift gets the high where it should have been to start with and add on the short
	return time;
}

TGraph* FileTGraph(string filepath){
	TGraph* ret=0;
	cout<<endl<<"Loading TGraph from file "<<filepath<<flush;
	if(filepath.find(".root")<filepath.size()){
		TFile *f1 = new TFile(filepath.c_str(),"READ");
		gROOT->cd();
		if(f1->IsOpen()){
			TIter next(f1->GetListOfKeys());
			TKey *key;
			while ((key = (TKey*)next())){
				TClass *cl = gROOT->GetClass(key->GetClassName());
				if (cl->InheritsFrom("TGraph")){
					ret=(TGraph*)key->ReadObj()->Clone("CloneGraph");
					break;
				}
			}
			f1->Close();
		}else{
			cout<<" LOAD FAILED."<<flush;
		}
		delete f1;
	}
	return ret;
}


double calc_beta_KE(double emev,double Amass){//input particle KE in MeV and mass in amu
	double mass=931.5*Amass;
	double gamma=(emev/mass)+1;
	return sqrt(1-pow(1/gamma,2));
}


short TigAng(TTigressHit* Tseg){
	if(!Tseg)return -1;
	
	short det=Tseg->GetDetector();
	short cry=Tseg->GetCrystal();
	short seg=Tseg->GetSegment();
	
	short pos=0;
	if(det<=0||det>16)return -1;
	if(det>4)pos+=4;
	if(det>12)pos+=4;
	
	if(cry==1||cry==2){
		pos+=2;
		if(seg==1||seg==2||seg==5||seg==6)pos++;
	}else{//cry 0 3
		if(seg==2||seg==3||seg==6||seg==7)pos++;
	}

	return pos;
}