// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// local
#include "OTt0OnlineClbr.h"

using namespace LHCb;

#include <DetDesc/Condition.h>

#include <iostream>
#include <fstream>

#include <boost/preprocessor/seq.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <boost/filesystem.hpp>

#include <boost/assign.hpp>
using namespace boost::assign;

#define PHOENIX_LIMIT 6
#include <boost/spirit/home/classic.hpp>
#include <boost/spirit/home/phoenix.hpp>

//-----------------------------------------------------------------------------
// Implementation file for class : OTt0OnlineClbr
//
// 2015-01-08 : Lucia Grillo
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTt0OnlineClbr )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTt0OnlineClbr::OTt0OnlineClbr( const std::string& name, ISvcLocator* pSvcLocator): AnalysisTask ( name , pSvcLocator )
{

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || __cplusplus >= 201103L
  std::vector<std::string> sNames= {"T1","T2","T3"};
  //std::vector<std::string> sNames= list_of("T1")("T2")("T3");
  std::vector<std::string> lNames= list_of("X1")("U")("V")("X2");
  std::vector<std::string> qNames= list_of("Q0")("Q1")("Q2")("Q3");
  std::vector<std::string> mNames= list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
  stationNames = sNames;
  layerNames = lNames;
  quarterNames = qNames;
  moduleNames = mNames;
#else
  stationNames = {"T1","T2","T3"};
  //stationNames = list_of("T1")("T2")("T3");
  layerNames = list_of("X1")("U")("V")("X2");
  quarterNames = list_of("Q0")("Q1")("Q2")("Q3");
  moduleNames = list_of("M1")("M2")("M3")("M4")("M5")("M6")("M7")("M8")("M9");
#endif

  detector = 0;

  declareProperty("ReadXMLs", readXMLs = false, "Read LOCAL condition XML files (default false)");
  declareProperty("Simulation", simulation = false, " Is simulation or data (default false, so data)");
  declareProperty("Apply_Calibration", Apply_Calibration = false, " Apply_Calibration - in xml (by now) or in DB (default false,)");
  declareProperty("Verbose", verbose = false, " Verbose, for debugging (default false,)");
  declareProperty("Save_Fits", save_fits = false, " Save fitted gaussian (default false,)");//currently broken: doesnt do anything
  declareProperty("Fit_module_contributions", fit_module_contributions = true, " Fit 4 contributions each module (default true)");
  //declareProperty("Fit_module_2D", fit_module_2d = false, " Fit 2d histogram time residual versus modulen)"); //will be an option, now we do anyhow
  declareProperty("GetMean_instead_of_Fit", getmean_instead_of_fit = false, "GetMean instead of fit the distributions ");

  declareProperty("Calibrate_only_GlobalT0", calibrate_only_globalt0 = true, "Calculates only the new Global t0 - NO per Module ");
  declareProperty("read_global_t0_from_db", read_global_t0_from_db = true, "Reads Global t0 from db. at the moment is an option beacuse of implementation ");

  //  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/group/online/alignment/" );                               
  // declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online" );   
  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online/OT/" );
  declareProperty("xmlFileName"   ,  m_xmlFileName  = "results.xml" );

  //inputs
  declareProperty("InputFileName"   ,  m_InputFileName  = "clbr_hists_109.root" );
  declareProperty("InputFileName_2d"   ,  m_InputFileName_2d  = "histog_2d_109.root" );

  /*
  declareProperty("EoRSignal" ,  m_EoRSignal = "-EOR" ); // End of Run Signal 

  declareProperty("HistBase"      ,  m_HistBase     = "RICH/RichHPDImageSummary/" );   // Histogram location
  declareProperty("OccpHistBase"  ,  m_OccpHistBase = "RICH/HPDHitsMoni/PDs/NumHits/" );   // Occupancy Histogram location
  declareProperty("minHPDID"   , m_minHPDID =   0 );
  declareProperty("maxHPDID"   , m_maxHPDID = 484 );
  declareProperty("HPDFitType" , m_params.type = "Sobel" );
  declareProperty("minEntries" , m_minEntries = 10000 );
 
 declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/group/online/alignment/" );
  declareProperty("xmlFileName"   ,  m_xmlFileName  = "results.xml" );
  declareProperty("xmlVersionLog" , m_xmlVersionLog = "version.txt" );
  */
}
//=============================================================================
// Destructor
//=============================================================================
OTt0OnlineClbr::~OTt0OnlineClbr() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode OTt0OnlineClbr::initialize() {

  // Init things for this task
  // must be run here
  
  detector = getDet<DeOTDetector>(DeOTDetectorLocation::Default);
  if(detector == 0) return StatusCode::FAILURE;

  StatusCode sc = AnalysisTask::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by AnalysisTask
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  
  /*
  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));
  if(readXMLs) statusCode = readCondXMLs(t0s);
  else statusCode = readCondDB(t0s);

  return statusCode;
  */

  return StatusCode::SUCCESS;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTt0OnlineClbr::execute() {
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;

  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::analyze (std::string& SaveSet,
                                     std::string Task)
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Analyze" << endmsg;

  // Mandatory: call default analyze method
  StatusCode sc = AnalysisTask::analyze(SaveSet, Task);
  if ( sc.isFailure() ) return sc;

  //std::string fileName = boost::filesystem::path( SaveSet.c_str() ).stem().string();
  std::string fileName = boost::filesystem::path( SaveSet.c_str() ).string();

  if ( msgLevel(MSG::DEBUG) ) debug() << "Extracted file name: " << fileName
				// << ", Run: " << getRunNumber( fileName )
				      << endmsg;

  //double mtoff[3][4][4][9]; memset(mtoff, 0, sizeof(mtoff));

  double test[3][4][4][9]; memset(test, 0, sizeof(test));
  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));
  
  double global_t0 = 0.0;
 
  //readCondXMLs(mtoff, true);

  //Here you need to read t0s?
  if(!calibrate_only_globalt0 || !read_global_t0_from_db){    
    if(readXMLs) readCondXMLs(t0s);
    else readCondDB(t0s);
  }

  double read_global_t0 = 0.0;

  if(read_global_t0_from_db){
    readCondDB_Globalt0(read_global_t0);
  }
  else{
    //temporary way to get the global t0: 
    debug() << "Temporary way to GET THE GLOBAL t0 : calculating from the module contributions in DB"<< endmsg;
    for(int s = 0; s < 3; s++){
      for(int l = 0; l < 4; l++){
	for(int q = 0; q < 4; q++){
	  for(int m = 8; m >= 0; m--){
	    read_global_t0 += t0s[s][l][q][m];
	  }
	}
      }
    }
    read_global_t0 = read_global_t0/432.0;
  }

  debug() << "GLOBAL t0 = " << read_global_t0 << endmsg;

  //temporary output to compare the t0 values and uncertainties
  std::string t0Studies_fileName = "t0_studies.txt";
  std::ofstream t0Studies_file(t0Studies_fileName.c_str());

  //+++++++++++++++++++//
  //read file by Jibo
  //+++++++++++++++++++//
  /*
  if( fileName.find( m_EoRSignal ) != std::string::npos ) {
   
    m_mergedRun = std::pair<unsigned int, std::string>( getRunNumber( fileName ), SaveSet );
    m_lastRun   = std::pair<unsigned int, std::string>( 0, "") ;

  } // "-EOR" inside the file name 
  else
    {
      // if it is the first time to read a file without "-EOR"
      // or still the same run
      if( m_lastRun.first ==0 || getRunNumber( fileName ) ==  m_lastRun.first ) 
	{
	  m_lastRun = std::pair<unsigned int, std::string>( getRunNumber( fileName ), SaveSet );
	  
	  return StatusCode::SUCCESS;
	} // wait for new file
      else if( getRunNumber( fileName ) !=  m_lastRun.first ) // new Run, use previous file
	{
	  m_mergedRun = m_lastRun;
	  m_lastRun   = std::pair<unsigned int, std::string>( 0, "") ;
	}   
    }
  */
  // Open Saveset
  //TFile *f = new TFile(m_mergedRun.second.c_str(),"READ");

  //TFile* f = new TFile("clbr_hists.root");
  TFile* f = new TFile((m_InputFileName).c_str());

  //TFile* f_2d = new TFile("histog_2d.root") 
  TFile* f_2d = new TFile((m_InputFileName_2d).c_str());

  TFile * outFile = new TFile("calibration_monitoring.root", "RECREATE");

  //To Book Monitoring histograms
  std::cout << "Book hists ..." << std::endl;
  TH1D* hdt0 = new TH1D("hdt0", "hdt0", 432, 0, 432);
  TH1D* ht0 = new TH1D("ht0", "ht0", 432, 0, 432);

  //To Book the 2D histogram
  // TH2D* twod_residual = new TH2D("twod_residual", "twod_residual", 432, 0, 432, 50, -25, 25);

  if( false == f->IsZombie() )
      {   
      unsigned int xmlVersion;

     // To store HPD occupancies
     //std::vector<std::pair<unsigned int, double>> Rich1_OccsVec, Rich2_OccsVec;
     //std::pair<unsigned int, double> HpdOcc;


     //Global t0 from a single distribution
     
     std::string histName_s1 = "OTModuleClbrMon/" + stationNames[0] + "/driftTimeResidual";
     std::string histName_s2 = "OTModuleClbrMon/" + stationNames[1] + "/driftTimeResidual";
     std::string histName_s3 = "OTModuleClbrMon/" + stationNames[2] + "/driftTimeResidual";

     TH1D* hist_s1 = (TH1D*)f->Get(histName_s1.c_str());
     TH1D* hist_s2 = (TH1D*)f->Get(histName_s2.c_str());
     TH1D* hist_s3 = (TH1D*)f->Get(histName_s3.c_str());

     TH1D* hist_global = (TH1D*)hist_s1->Clone();
     hist_global->Reset("ICES");
     hist_global->Add(hist_s1, hist_s2, 1.0, 1.0);
     hist_global->Add(hist_s3, 1.0);

     double residual_global = 0.0;
     double residual_global_err = 0.0;

     if(getmean_instead_of_fit){
       residual_global = hist_global->GetMean();
       residual_global_err = hist_global->GetMeanError();
     }
     else{
       StatusCode sc_global_t0 = fit_single_hist(hist_global, -1, -1, -1, -1, residual_global, residual_global_err, "hist_global", outFile);
     }

     global_t0 = read_global_t0 + residual_global;
 
     debug() << " GLOBAL t0 : " << global_t0 << endmsg;   
     debug() << " GLOBAL dt0 : " << residual_global  << endmsg;   
     debug() << " GLOBAL dt0_err : " << residual_global_err << endmsg;   


     if(!calibrate_only_globalt0){
  
       //Module t0 from 2d histogram
       TH2D* twod_residual = (TH2D*)f_2d->Get("twod_residual");
       
       twod_residual->FitSlicesY();
       
       TH1D *twod_residual_1 = (TH1D*)gDirectory->Get("twod_residual_1");
       TH1D *twod_residual_2 = (TH1D*)gDirectory->Get("twod_residual_2");
       
       double average = 0.0;
       for(int i = 0 ; i<  twod_residual_1->GetNbinsX();i++){
	 average += twod_residual_1->GetBinContent(i);
       }
       
       average = average/twod_residual_1->GetNbinsX();
       
       double average_sigma = 0.0;
       for(int i = 0 ; i<  twod_residual_2->GetNbinsX();i++){
	 average_sigma += twod_residual_2->GetBinContent(i);
       }
       
       average_sigma = average_sigma/twod_residual_2->GetNbinsX();
       
       //TF1* f1 = new TF1("f1", "gaus", (average-2.0*average_sigma),(average+2.0*average_sigma));
       TF1* f1 = new TF1("f1", "gaus", -4.0, 4.0);
       
       TObjArray aSlices;
       twod_residual->FitSlicesY(f1, 0, -1, 0,"QNR", &aSlices);
       
       TH1D *twod_residual_refit_1 = (TH1D*)gDirectory->Get("twod_residual_1");
       
       TH1D* hdt0proj_from2d = new TH1D("hdt0proj_from2d", "", 100, -10, 10);
       for(int i = 0; i < 432; i++) if(twod_residual_refit_1->GetBinContent(i+1) != 0) hdt0proj_from2d->Fill(twod_residual_refit_1->GetBinContent(i+1));
       
       debug()<< "n modules "<< twod_residual_1->GetNbinsX() << " average t0 "<< average << endmsg;
       
       outFile->cd();
       twod_residual->Write();
       twod_residual_1->Write();
       hdt0proj_from2d->Write();
       
       
       //Loop over the station layer quarter module
       std::cout << "Loop ..." << std::endl;
       for(int s = 0; s < 3; s++) 
	 for(int l = 0; l < 4; l++) 
	   for(int q = 0; q < 4; q++)
	     {
	       /* // used only for Alex's monitorning
		  double t0_ = 0.0;
		  double dt0_ = 0.0;
		  double dt0err_ = 0.1;
	       */
	       for(int m = 8; m >= 0; m--)
		 {
		   std::string histName = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual";
		   std::string histName01L = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual01L";
		   std::string histName01R = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual01R";
		   std::string histName23L = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual23L";
		   std::string histName23R = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual23R";
		   
		   TH1D* hist = (TH1D*)f->Get(histName.c_str());
		   TH1D* hist01L = (TH1D*)f->Get(histName01L.c_str());
		   TH1D* hist01R = (TH1D*)f->Get(histName01R.c_str());
		   TH1D* hist23L = (TH1D*)f->Get(histName23L.c_str());
		   TH1D* hist23R = (TH1D*)f->Get(histName23R.c_str());
		   
		   int modulen = m + 9 * (q + 4 * (l + 4 * s));
		   
		   if(hist == 0 || hist->GetEntries() < 1000 || (s == 0 && m == 0))
		     {
		       //if(m == 8) t0_ = -mtoff[s][l][q][m]; //
		       //Here Alex's old monitoring: does it make sense? in the meanwhile, I implement a simple new simple one
		       //if(m == 8) t0_ = -t0s[s][l][q][m]; //why you assume m = 8 different from the others?
		       if(hist != 0 && !(s == 0 && m == 0)) std::cout << histName << " :: N = " << hist->GetEntries() << std::endl;
		       //t0s[s][l][q][m] = t0_ + mtoff[s][l][q][m];
		       /*
			 t0s[s][l][q][m] = t0_ + t0s[s][l][q][m];
			 hdt0->SetBinContent(hdt0->FindBin(modulen), dt0_);
			 hdt0->SetBinError(hdt0->FindBin(modulen), dt0err_);
			 ht0->SetBinContent(ht0->FindBin(modulen), t0_ + (28.0 + 2.0 * s));
			 ht0->SetBinError(ht0->FindBin(modulen), dt0err_);
		       */
		       
		       hdt0->SetBinContent(hdt0->FindBin(modulen), 0.0);
		       hdt0->SetBinError(hdt0->FindBin(modulen), 0.0);
		       ht0->SetBinContent(ht0->FindBin(modulen), t0s[s][l][q][m]);
		       ht0->SetBinError(ht0->FindBin(modulen), 0.0);
		       continue;
		     }
		   
		   //what was this good for?
		   /*
		     double left = hist->GetXaxis()->GetXmin();
		     double right = hist->GetXaxis()->GetXmax();
		     for(int i = 0; i < 5; i++)
		     {
		     hist->Fit("gaus", "Q0R", "", left, right);
		     left = hist->GetFunction("gaus")->GetParameter(1) - 2.0 * hist->GetFunction("gaus")->GetParameter(2);
		     right = hist->GetFunction("gaus")->GetParameter(1) + 2.0 * hist->GetFunction("gaus")->GetParameter(2);
		     }
		   */
		   
		   double dt0err = 0.0;
		   
		   if(!fit_module_contributions){
		     
		     if(getmean_instead_of_fit){
		       test[s][l][q][m] = hist->GetMean();
		       dt0err = hist->GetMeanError();
		     }
		     else{
		       double residual_per_module = 0.0;
		       double residual_per_module_err = 0.0;
		       
		       StatusCode t0_per_module = fit_single_hist(hist,s,l,q, m, residual_per_module, residual_per_module_err, histName, outFile);
		       
		       test[s][l][q][m] = residual_per_module;
		       
		       dt0err = residual_per_module_err;
		     }
		   }
		   else{
		     std::cout<<"Now fitting the 4 sub contributions"<<std::endl;
		     
		     double residual_01L=0.0;
		     double residual_01R=0.0;
		     double residual_23L=0.0;
		     double residual_23R=0.0;
		     
		     double residual_01L_err=0.0;
		     double residual_01R_err=0.0;
		     double residual_23L_err=0.0;
		     double residual_23R_err=0.0;
		     
		     if(getmean_instead_of_fit){
		       residual_01L = hist01L->GetMean();
		       residual_01R = hist01R->GetMean();
		       residual_23L = hist23L->GetMean();
		       residual_23R = hist23R->GetMean();
		       
		       residual_01L_err = hist01L->GetMeanError();
		       residual_01R_err = hist01R->GetMeanError();
		       residual_23L_err = hist23L->GetMeanError();
		       residual_23R_err = hist23R->GetMeanError();
		     }
		     else{
		       StatusCode sc_01L = fit_single_hist(hist01L,s,l,q, m, residual_01L, residual_01L_err, histName01L , outFile);
		       StatusCode sc_01R = fit_single_hist(hist01R,s,l,q, m, residual_01R, residual_01R_err, histName01R , outFile);
		       StatusCode sc_23L = fit_single_hist(hist23L,s,l,q, m, residual_23L, residual_23L_err, histName23L , outFile);
		       StatusCode sc_23R = fit_single_hist(hist23R,s,l,q, m, residual_23R, residual_23R_err, histName23R , outFile);
		     }
		     
		     if( m==8 && (q == 0 || q == 2) && hist23L->GetEntries()==0 && hist23R->GetEntries()==0){ //only 2 half monlayer contributions 
		       test[s][l][q][m] = 0.5*(residual_01L + residual_01R);
		       
		       dt0err = 0.2*(sqrt((residual_01L_err*residual_01L_err)+(residual_01R_err*residual_01R_err)));
		       
		     }
		     else{
		       test[s][l][q][m] = 0.25*(residual_01L + residual_01R + residual_23L+ residual_23R);
		       
		       
		       dt0err = 0.25*(sqrt((residual_01L_err*residual_01L_err)+(residual_01R_err*residual_01R_err)
					   +(residual_23L_err*residual_23L_err)+(residual_23R_err*residual_23R_err)));
		     }
		     
		   }
		   
		   double dt0 =  test[s][l][q][m];
		   
		   //suspect: the fit above was only for  monitoring :(
		   //      dt0 = hist->GetFunction("gaus")->GetParameter(1);                                                                                                      
		   double t0 = t0s[s][l][q][m] + test[s][l][q][m];
		   
		   //double t0 = t0s[s][l][q][m] = t0s[s][l][q][m] + dt0;
		   //      t0 = t0s[s][l][q][m] = mtoff[modulen] - (28.0 + 2.0 * s);                                                                                              
		   //t0 -= mtoff[s][l][q][m];
		   //again weird monitorning 
		   /*
		     t0 -= t0s[s][l][q][m];
		     
		     hdt0->SetBinContent(hdt0->FindBin(modulen), dt0);
		     hdt0->SetBinError(hdt0->FindBin(modulen), dt0err);
		     
		     ht0->SetBinContent(ht0->FindBin(modulen), t0 + (28.0 + 2.0 * s));
		     ht0->SetBinError(ht0->FindBin(modulen), dt0err);
		     
		     t0_ = t0;
		     dt0_ = dt0;
		     dt0err_ = dt0err;
		   */
		   
		   
		   hdt0->SetBinContent(hdt0->FindBin(modulen),  dt0);
		   hdt0->SetBinError(hdt0->FindBin(modulen), dt0err);
		   ht0->SetBinContent(ht0->FindBin(modulen), t0);
		   ht0->SetBinError(ht0->FindBin(modulen), dt0err);
		   
		   
		   if(fabs(dt0) > 1) std::cout << histName << " :: dt0 = " << dt0 << std::endl;
		   
		   t0Studies_file << histName << " dt0 = " << dt0 << " +/- " << dt0err << "\n";
		   
		   
		   if(Apply_Calibration){
		     t0s[s][l][q][m]= t0s[s][l][q][m] + test[s][l][q][m];
		   }
		   else
		     t0s[s][l][q][m]= t0s[s][l][q][m] + 0.0;
		   
		 }
	     }   
     }  

     f->Close();
      } else {
    
    warning() << "file is zombie "
	      << endmsg;
  }

  if(!calibrate_only_globalt0){     
    writeCondXMLs(t0s);
    writeCondDBXMLs(t0s);
    
    t0Studies_file.flush();
    t0Studies_file.close();
    
    
    if(!ht0){
      Error("Histogram not found " );
    }
    
    TH1D* hdt0proj = new TH1D("hdt0proj", "", 100, -10, 10);
    for(int i = 0; i < 432; i++) if(hdt0->GetBinContent(i+1) != 0) hdt0proj->Fill(hdt0->GetBinContent(i+1));

    if(save_fits){
      outFile->cd();
      hdt0->Write();
      ht0->Write();
      hdt0proj->Write();
    }
  }
   
  write_Globalt0_XML(global_t0);
  
  //outFile->Flush();
  outFile->Close();
  
  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::readCondDB(double read_t0s[3][4][4][9])
{
  std::string subDet = "OT";

  double Module_t0(0);

  for(int s = 0; s < 3; s++) 
    for(int l = 0; l < 4; l++) 
      for(int q = 0; q < 4; q++)
	{
	  for(int m = 8; m >= 0; m--){
	    
	    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

	    std::string alignLoc ="/dd/Conditions/Calibration/OT/CalibrationModules"+stationNames[s]+
        layerNames[l]+quarterNames[q]+"/"+stationNames[s]+layerNames[l]+ quarterNames[q]+moduleNames[m];
	    
	    Condition *myCond = get<Condition>( detSvc(), alignLoc );
	    
	    debug() << "CAFFE : requiring doublevect " << endmsg;
        
	    std::vector<double> TZeroVec = myCond->paramAsDoubleVect( "TZero" );
	    
	    Module_t0 = 0; // Reset 
	    for(int i = 0; i<TZeroVec.size();i++){
	      debug() << "t0 per straw = "<<TZeroVec.at(i)<< endmsg; // for check     
	      Module_t0 +=TZeroVec.at(i);
	    }
	    read_t0s[s][l][q][m] = Module_t0/(TZeroVec.size()*1.0);
	    //in simcond the t0 are on straw bases, I make an average - for now - to make it module basis
	    //also in LHCb cond the t0s are stored in a vector, using only one entry->same code
	    
	  }
	}


  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::readCondXMLs(double t0s[3][4][4][9], bool TestBeam)
{

  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) 
    for(int l = 0; l < 4; l++) 
      for(int q = 0; q < 4; q++)
	{
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];
	  
	  std::string fileName = quarterId + "@" + prefix + ".xml";

	  if(TestBeam)
	    std::string fileName = "TestBeam-t0s-inXML" + quarterId + "@" + prefix + ".xml";
	  
	  // load file with condition for specific station-layer-quarter                                                                                             
	  std::ifstream file(fileName.c_str());
	  if(file.fail())
	    {
	      warning() << "Can't open file: '" << fileName << "'" << endmsg;
	      continue;
	    }
	  
	  std::string xml;
	  
	  // read file to string                                                                                                                                     
	  file.seekg(0, std::ios_base::end);
	  xml.resize(file.tellg());
	  file.seekg(0, std::ios_base::beg);
	  file.read(&xml.at(0), xml.size());
	  file.close();
	  
	  std::vector<double> cRt[9], cRtErr[9], cT0[9], cWalk[9];
	  {
	    using namespace boost::spirit::classic;
	    using namespace boost::phoenix;
	    using namespace boost::phoenix::arg_names;

	    int m; std::string param;
	    rule<phrase_scanner_t> xmlRule =
	      !("<?xml" >> *(anychar_p - '>') >> '>') // header: <?xml ... >                                                       
	      >> !("<!DOCTYPE" >> *(anychar_p - '>') >> '>') // header <!DOCTYPE ... >                                        
	      >> "<DDDB>"
	      >> "<catalog" >> *(anychar_p - '>') >> '>' // <catalog ... >                              
	      >> +(
              lexeme_d[
                  "<condition"
                  >> *(
		       (
			" name" >> *space_p >> '=' >> *space_p >> '\"'
			>> *(anychar_p - 'M') >> 'M' >> int_p[boost::phoenix::ref(m) = arg1 - 1]
			>> '\"'
			)
		       | (anychar_p - '>')
		       )
                  >> '>' // <condition ... name="...M{m + 1}" ... >                                                                          
              ]
              >> +(
                  lexeme_d[
                      "<paramVector"
                      >> *(
			   (
			    " name" >> *space_p >> '=' >> *space_p >> '\"'
			    >> (+(anychar_p - '\"'))[boost::phoenix::ref(param) = construct<std::string>(arg1, arg2)]
			    >> '\"'
			    )
			   | (anychar_p - '>')
			   ) >> '>' // <paramVector ... name="{param}" ... >                                                          
                  ]
                  >> +real_p // vector of values                               
                     [
		      if_(boost::phoenix::ref(param) == "TRParameters")
		      [ boost::phoenix::push_back(boost::phoenix::ref(cRt)   [boost::phoenix::ref(m)], arg1) ],
		      if_(boost::phoenix::ref(param) == "STParameters")
		      [ boost::phoenix::push_back(boost::phoenix::ref(cRtErr)[boost::phoenix::ref(m)], arg1) ],
		      if_(boost::phoenix::ref(param) == "TZero")
		      [ boost::phoenix::push_back(boost::phoenix::ref(cT0)   [boost::phoenix::ref(m)], arg1) ],
		      if_(boost::phoenix::ref(param) == "WalkParameters")
		      [ boost::phoenix::push_back(boost::phoenix::ref(cWalk) [boost::phoenix::ref(m)], arg1) ]
                      ]
                  >> "</paramVector>"
		   )[boost::phoenix::ref(param) = ""]
              >> "</condition>"
		   )[boost::phoenix::ref(m) = -1]
	      >> "</catalog>"
	      >> "</DDDB>"
	      >> end_p;
	    if(parse(xml.c_str(), xmlRule, space_p).full != true) return StatusCode::FAILURE;
	  }

	  for(int m = 0; m < 9; m++)
	    {
	      LHCb::OTChannelID id(s + 1, l, q, m + 1, 1);
	      DeOTModule* module = detector->findModule(id);
	      module->setStrawT0s(cT0[m]);
	      OTDet::RtRelation rt(2.45, cRt[m], cRtErr[m], 50);
	      module->setRtRelation(rt);
	      if(cWalk[m].size() == 4)
		{
		  OTDet::WalkRelation walk(cWalk[m][0], cWalk[m][1], cWalk[m][2], cWalk[m][3]);
		  module->setWalkRelation(walk);
		}
	      else std::cout << "skip walk initialization: npars != 4" << std::endl;
	    }


	  for(int m = 0; m < 9; m++)
	    {
	      double t0 = cT0[m][0];
	      t0s[s][l][q][m] = t0;
	      std::cout<< "t0 for module " << s <<" "<< l << " "<< q << " "<< m <<" = "<< t0s[s][l][q][m] << std::endl;
	    }

	}
  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::writeCondXMLs(double t0s[3][4][4][9])
{
  std::string prefix = "CalibrationModules";

  std::cout<< "WRITING XMLs" <<std::endl;

  for(int s = 0; s < 3; s++)
    for(int l = 0; l < 4; l++)
      for(int q = 0; q < 4; q++)
        {
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

	  std::string fileName = quarterId + "@" + prefix + ".xml";

	  std::ofstream file(fileName.c_str());
          if(file.fail())
            {
              printf("Can't open file: '%s'\n", fileName.c_str());

              continue;
            }

          file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
          file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
          file << "\n";
          file << "<DDDB>\n";
          file << "<catalog name=\"" << prefix << quarterId << "\">\n";
	  if(simulation){
            for(int m = 0; m < 9; m++)
              {
                if(verbose)
		  std::cout<< "WRITING m = "<< m<< "t0 = "<< t0s[s][l][q][m]<< "written num"<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<<std::endl;

		std::string moduleId = quarterId + moduleNames[m];

                file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

                file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
		if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
                else      file << "     " << 2.6 << " " << 0 << " " << 4.0 * 0.15 << "\n";
		//else      file << "     " << 2.6 << " " << (3.2 - 2.6 - 4.0 * 0.15) << " " << 4.0 * 0.15 << "\n";                                            
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
                file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
		//file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";                             
		for(int a = 0; a<16;a++){
		  file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< " "<< 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5)<< "\n";
		}
		file << "    </paramVector>\n";
                //              file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";                               
                //              file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";                                                       
                //file << "    </paramVector>\n";                                                                                                                
                file << "  </condition>\n";
              }
          }
          else{
	    for(int m = 0; m < 9; m++)
              {
		std::string moduleId = quarterId + moduleNames[m];

                file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

                file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
                if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
                else      file << "     " << 2.6 << " " << 0  << " " << 4.0 * 0.15 << "\n";
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
                file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
                file << "    </paramVector>\n";

                file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
                file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
                file << "    </paramVector>\n";
                file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
                file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
                file << "    </paramVector>\n";

                file << "  </condition>\n";
              }
          }

          file << "</catalog>\n";
          file << "</DDDB>\n";

          file.flush();
          file.close();
        }

  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::writeCondDBXMLs(double t0s[3][4][4][9])
{

  std::string prefix = "CalibrationModules";
  boost::filesystem::path full_path(m_xmlFilePath+m_xmlFileName);

  if ( msgLevel(MSG::DEBUG) ) debug() << "Writing new XML for online to " << full_path << endmsg ;

  std::ofstream file;
  file.open( full_path.string().c_str(), std::ios::app ) ;  // always in append mode  

  for(int s = 0; s < 3; s++)
    for(int l = 0; l < 4; l++)
      for(int q = 0; q < 4; q++)
        {
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

          file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
          file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
          file << "\n";
          file << "<DDDB>\n";
          file << "<catalog name=\"" << prefix << quarterId << "\">\n";
	  for(int m = 0; m < 9; m++)
            {
	      std::string moduleId = quarterId + moduleNames[m];

              file << "  <condition classID=\"5\" name=\"" << moduleId << "\">\n";

              file << "    <paramVector name=\"STParameters\" type=\"double\" comment=\"SigmaT parameters in ns\">\n";
              if(m < 7) file << "     " << 2.7 << " " << (3.7 - 2.7) << "\n";
              else      file << "     " << 2.6 << " " << 0  << " " << 4.0 * 0.15 << "\n";
              file << "    </paramVector>\n";

              file << "    <paramVector name=\"TRParameters\" type=\"double\" comment=\"RT parameters in ns\">\n";
              file << "     " << 0 << " " << (35.5 - 4.0 * 3.6) << " " << (4.0 * 3.6) << "\n";
              file << "    </paramVector>\n";

              file << "    <paramVector name=\"TZero\" type=\"double\" comment=\"T0s of straws in module\">\n";
              file << "      " << 0.001 * (int)(1000.0 * t0s[s][l][q][m] + 0.5) << "\n";
              file << "    </paramVector>\n";
              file << "    <paramVector name=\"WalkParameters\" type=\"double\" comment=\"Walk parameters\">\n";
              file << "      " << 0 << " " << 1.10 << " " << 400 << " " << 0.15 << "\n";
              file << "    </paramVector>\n";

              file << "  </condition>\n";
            }
        }

  file << "</DDDB>\n";

  //logging << "<condition classID=\"6\" name=\"AverageHPDOccupancies\">\n";                                                                                      
  //logging << "<paramVector name=\"Occupancies\" type=\"std::string\" comment=\"Average HPD occupancy\">"                                                        
  //          << occS << "</paramVector>\n";                                                                                                                      
  //logging << "</condition>\n";                                                                                                                                  
  //logging << "\n" ;                                                                                                                                             

  file.close();


  return StatusCode::SUCCESS;
}


StatusCode OTt0OnlineClbr::readCondDB_Globalt0(double& read_t0)
{
  std::string subDet = "OT";

  std::string alignLoc ="/dd/Conditions/Calibration/OT/CalibrationGlobal";

  Condition *myCond = get<Condition>( detSvc(), alignLoc );
  
  read_t0 = myCond->paramAsDouble( "TZero" );
	    
  debug()<< "reading GLOBAL t0 " << read_t0<< endmsg;

  return StatusCode::SUCCESS;
}



StatusCode OTt0OnlineClbr::write_Globalt0_XML(double global_t0)
{

  std::string prefix = "CalibrationModules";
  boost::filesystem::path full_path(m_xmlFilePath+m_xmlFileName);

  if ( msgLevel(MSG::DEBUG) ) debug() << "Writing the GLOBAL t0 XML for online to " << full_path << endmsg ;

  std::ofstream file;
  //file.open( full_path.string().c_str(), std::ios::app ) ;  // always in append mode  
  file.open( full_path.string().c_str()) ;  // do we want it in append mode? 

  file << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n";
  file << "<!DOCTYPE DDDB SYSTEM \"conddb:/DTD/structure.dtd\">\n";
  file << "\n";
  file << "<DDDB>\n";
  file << "  <condition classID=\"5\" name=\"" << "CalibrationGlobal" << "\">\n";
  file << "    <param name=\"TZero\" type=\"double\" comment=\"Global t0 of OT\">\n";
  //file << "      " << 0.001 * (int)(1000.0 * global_t0 + 0.5) << "\n";
  file << "      " <<  global_t0  << "\n";
  file << "    </param>\n";
  file << "  </condition>\n";
  file << "</DDDB>\n";

  // this is how the file should look like
  //<?xml version="1.0" encoding="ISO-8859-1"?>
  //<!DOCTYPE DDDB SYSTEM "../../../DTD/structure.dtd">
  //
  //<DDDB>
  //  <condition classID="5" name="CalibrationGlobal">
  //    <param name="TZero" type="double" comment="Global t0 of OT">
  //      0.001
  //    </param>
  //  </condition>
  //</DDDB>
  //

  file.close();


  return StatusCode::SUCCESS;

}

StatusCode OTt0OnlineClbr::fit_single_hist(TH1D* hist, int s, int l, int q, int m, double& result, double& result_error, std::string name, TFile* outFile)
{
  debug() << hist << " " << s << " " << l << " " << q << " "<< m << endmsg;
  
  if(!hist){
    Error("Histogram not found " );
    return StatusCode::FAILURE;
  }else if(hist->GetEntries() < 100)
  {
    if(!(q % 2 == 0 && m == 8))
    {
      std::cout << "FAIL: " << s<< " "<<l <<" "<<q<< " "<<m << std::endl;
    }
    return StatusCode::FAILURE;
  }
  
  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();


  for(int i = 0; i < 5; i++)
  {
    hist->Fit("gaus", "QRLL", "", left, right);
    left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
    right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
  }
  debug() << "FITTING: " << s<< " "<<l <<" "<<q<< " "<<m <<" "<<hist->GetFunction("gaus")->GetParameter(1)<< " "<< hist->GetFunction("gaus")->GetParameter(2)<< endmsg ;
  result =  hist->GetFunction("gaus")->GetParameter(1);
  result_error =  hist->GetFunction("gaus")->GetParError(1);
  

  //only to save stuff

  if(save_fits){
    outFile->cd();

    std::string hist_name;

    if(s==-1 && l==-1 && q==-1 && m==-1)
      hist_name = "Global_hist";
    else
      hist_name = stationNames[s] + "_" + layerNames[l] + "_" + quarterNames[q] + "_"+ moduleNames[m];

    hist->SetName(hist_name.c_str());
    hist->Write();

    //c->SetName(canvas_name.c_str());
    //c->Write();
    //c->SaveAs((canvas_name +".pdf").c_str());
    //c->Clear();
    
  }

  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTt0OnlineClbr::finalize() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  return AnalysisTask::finalize();  // must be called after all other actions
}
