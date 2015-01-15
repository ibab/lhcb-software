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

  declareProperty("ReadXMLs", readXMLs = false, "Read condition XML files (default false)");
  declareProperty("Simulation", simulation = false, " Is simulation or data (default false, so data)");
  declareProperty("Apply_Calibration", Apply_Calibration = false, " Apply_Calibration - in xml (by now) or in DB (default false,)");
  declareProperty("Verbose", verbose = false, " Verbose, for debugging (default false,)");
  //  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/group/online/alignment/" );                               
  // declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online" );   
  declareProperty("xmlFilePath"   ,  m_xmlFilePath  = "/afs/cern.ch/user/l/lgrillo/databases_for_online/OT/results.xml" );
  declareProperty("xmlFileName"   ,  m_xmlFileName  = "results.xml" );

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

  double mtoff[] = {
    27.8546, 27.4911, 27.1707, 26.8945, 26.6638, 26.4802, 26.3445, 26.2568, 26.2233, 27.8478, 27.485, 27.1654, 26.89, 26.66,
    26.4774, 26.3425, 26.2557, 26.2183, 27.8823, 27.5192, 27.1991, 26.9231, 26.6927, 26.5092, 26.3737, 26.2874, 26.2548,
    27.8755, 27.5131, 27.1938, 26.9185, 26.6889, 26.5065, 26.3718, 26.2863, 26.2499, 28.1602, 27.7832, 27.4486, 27.1581,
    26.9129, 26.7143, 26.5635, 26.4614, 26.4162, 27.9127, 27.5628, 27.2564, 26.9949, 26.7797, 26.6116, 26.4919, 26.4203,
    26.3989, 27.948, 27.5975, 27.2905, 27.0285, 26.8123, 26.6437, 26.5232, 26.4515, 26.4309, 28.1797, 27.8039, 27.4705,
    27.1812, 26.9373, 26.7396, 26.5899, 26.4906, 26.4385, 28.1285, 27.7806, 27.4759, 27.2155, 27.0011, 26.8335, 26.7139,
    26.6417, 26.6204, 28.3566, 27.9829, 27.6521, 27.3642, 27.1219, 26.9257, 26.7768, 26.6765, 26.6236, 28.3941, 28.0204,
    27.689, 27.401, 27.158, 26.9613, 26.8118, 26.712, 26.6684, 28.1465, 27.7992, 27.4958, 27.2364, 27.0229, 26.8563,
    26.7376, 26.6672, 26.647, 28.4043, 28.0483, 27.734, 27.4635, 27.2378, 27.0581, 26.9255, 26.8403, 26.8076, 28.3955, 28.0395,
    27.7263, 27.4561, 27.2312, 27.0519, 26.9199, 26.8352, 26.7985, 28.4319, 28.0763, 27.7624, 27.4921, 27.2668, 27.0873, 26.9548,
    26.8708, 26.8392, 28.4231, 28.0676, 27.7548, 27.485, 27.2602, 27.0811, 26.9493, 26.8659, 26.8301, 29.9962, 29.659, 29.3618, 29.1064,
    28.8936, 28.7243, 28.5992, 28.5184, 28.4874, 29.9957, 29.6592, 29.3627, 29.108, 28.8956, 28.727, 28.6025, 28.5224, 28.4876,
    30.024, 29.6872, 29.3902, 29.1352, 28.9228, 28.7535, 28.6285, 28.549, 28.5191, 30.0232, 29.687, 29.3909, 29.1363, 28.9241,
    28.756, 28.6316, 28.5528, 28.5193, 30.2936, 29.9432, 29.6332, 29.3643, 29.1376, 28.9543, 28.8152, 28.7213, 28.6795,
    30.0696, 29.7449, 29.4606, 29.2186, 29.0199, 28.8649, 28.7544, 28.6883, 28.6682, 30.0984, 29.773, 29.489, 29.2466, 29.047,
    28.8915, 28.7804, 28.7145, 28.6956, 30.3194, 29.9702, 29.6607, 29.3929, 29.1675, 28.9852, 28.847, 28.7554, 28.7074,
    30.2737, 29.951, 29.6685, 29.4276, 29.2294, 29.0747, 28.9644, 28.8977, 28.8779, 30.49, 30.1432, 29.8357, 29.5695, 29.3452,
    29.1639, 29.0265, 28.9342, 28.8854, 30.5228, 30.1758, 29.8683, 29.6012, 29.3766, 29.195, 29.0571, 28.9649, 28.925, 30.2967,
    29.9749, 29.6931, 29.4533, 29.256, 29.1021, 28.9927, 28.9281, 28.9096, 30.5439, 30.2125, 29.9213, 29.6709, 29.462,
    29.2961, 29.1738, 29.0951, 29.0647, 30.5397, 30.209, 29.9183, 29.6682, 29.46, 29.2946, 29.1729, 29.0947, 29.0608, 30.5719,
    30.2407, 29.9498, 29.6997, 29.4909, 29.3252, 29.2032, 29.1256, 29.0965, 30.5675, 30.2371, 29.9467, 29.6969, 29.489,
    29.3239, 29.2022, 29.1253, 29.0926, 32.1781, 31.8639, 31.5876, 31.3502, 31.1529, 30.9959, 30.8803, 30.8056, 30.7769, 32.1733,
    31.8596, 31.5844, 31.3472, 31.1507, 30.9942, 30.879, 30.8049, 30.7727, 32.2062, 31.8923, 31.6162, 31.3791, 31.1818,
    31.0251, 30.9096, 30.8361, 30.8087, 32.2014, 31.888, 31.613, 31.3761, 31.1796, 31.0233, 30.9083, 30.8354, 30.8046, 32.468,
    32.142, 31.8531, 31.6031, 31.3927, 31.2227, 31.0939, 31.0069, 30.9683, 32.255, 31.9526, 31.6888, 31.464, 31.2794,
    31.1357, 31.0333, 30.972, 30.9534, 32.2883, 31.9859, 31.7212, 31.496, 31.311, 31.1667, 31.0638, 31.0029, 30.9857, 32.4905,
    32.1651, 31.8774, 31.6284, 31.4187, 31.2495, 31.1213, 31.0365, 30.9923, 32.4635, 32.1625, 31.8993, 31.6756, 31.4915,
    31.3481, 31.2458, 31.1838, 31.1653, 32.6667, 32.343, 32.0568, 31.809, 31.6005, 31.4325, 31.305, 31.2192, 31.1738, 32.6977,
    32.3737, 32.0871, 31.8392, 31.6305, 31.4617, 31.3338, 31.2484, 31.2115, 32.4885, 32.1884, 31.9263, 31.7032, 31.5199,
    31.3773, 31.2756, 31.2157, 31.1986, 32.7279, 32.419, 32.1478, 31.9149, 31.7209, 31.5668, 31.4532, 31.3801, 31.3518, 32.7255,
    32.4177, 32.1464, 31.9141, 31.7206, 31.567, 31.4538, 31.3813, 31.3496, 32.7558, 32.4473, 32.1763, 31.9435, 31.7497,
    31.5959, 31.4825, 31.4105, 31.3837, 32.7534, 32.446, 32.175, 31.9429, 31.7497, 31.5962, 31.4832, 31.4118, 31.3815
  };

  double test[3][4][4][9]; memset(test, 0, sizeof(test));
  double t0s[3][4][4][9]; memset(t0s, 0, sizeof(t0s));

  //Here you need to read t0s?
  if(readXMLs) readCondXMLs(t0s);
  else readCondDB(t0s);

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

  TFile* f = new TFile("clbr_hists.root");
  
  if( false == f->IsZombie() )
    {   
      unsigned int xmlVersion;

     // To store HPD occupancies
     std::vector<std::pair<unsigned int, double>> Rich1_OccsVec, Rich2_OccsVec;
     std::pair<unsigned int, double> HpdOcc;

     //To Book Monitoring histograms
     std::cout << "Book hists ..." << std::endl;
     TH1D* hdt0 = new TH1D("hdt0", "", 432, 0, 432);
     TH1D* ht0 = new TH1D("ht0", "", 432, 0, 432);

     //Loop over the station layer quarter module
     std::cout << "Loop ..." << std::endl;
     for(int s = 0; s < 3; s++) 
       for(int l = 0; l < 4; l++) 
	 for(int q = 0; q < 4; q++)
	   {
	     double t0_ = 0.0;
	     double dt0_ = 0.0;
	     double dt0err_ = 0.1;
	     for(int m = 8; m >= 0; m--)
	       {
		 std::string histName = "OTModuleClbrMon/" + stationNames[s] + "/" + layerNames[l] + "/" + quarterNames[q] + "/" + moduleNames[m] + "/driftTimeResidual";
		 TH1D* hist = (TH1D*)f->Get(histName.c_str());
		 
		 int modulen = m + 9 * (q + 4 * (l + 4 * s));
	
		 if(hist == 0 || hist->GetEntries() < 1000 || (s == 0 && m == 0))
		   {
		     if(m == 8) t0_ = -mtoff[modulen];
		     if(hist != 0 && !(s == 0 && m == 0)) std::cout << histName << " :: N = " << hist->GetEntries() << std::endl;
		     t0s[s][l][q][m] = t0_ + mtoff[modulen];
		     hdt0->SetBinContent(hdt0->FindBin(modulen), dt0_);
		     hdt0->SetBinError(hdt0->FindBin(modulen), dt0err_);
		     ht0->SetBinContent(ht0->FindBin(modulen), t0_ + (28.0 + 2.0 * s));
		     ht0->SetBinError(ht0->FindBin(modulen), dt0err_);
		     continue;
		   }
		 double left = hist->GetXaxis()->GetXmin();
		 double right = hist->GetXaxis()->GetXmax();
		 for(int i = 0; i < 5; i++)
		   {
		     hist->Fit("gaus", "Q0R", "", left, right);
		     left = hist->GetFunction("gaus")->GetParameter(1) - 2.0 * hist->GetFunction("gaus")->GetParameter(2);
		     right = hist->GetFunction("gaus")->GetParameter(1) + 2.0 * hist->GetFunction("gaus")->GetParameter(2);
		   }

		 std::cout<<"Now fitting the 4 sub contributions"<<std::endl;

		 double residual_01L=0.0;
		 double residual_01R=0.0;
		 double residual_23L=0.0;
		 double residual_23R=0.0;

		 StatusCode sc_01L = fit_single_hist(m_histModuleDriftTimeResidual01L[s][l][q][m],s,l,q, m, residual_01L);
		 StatusCode sc_01R = fit_single_hist(m_histModuleDriftTimeResidual01R[s][l][q][m],s,l,q, m, residual_01R);
		 StatusCode sc_23L = fit_single_hist(m_histModuleDriftTimeResidual23L[s][l][q][m],s,l,q, m, residual_23L);
		 StatusCode sc_23R = fit_single_hist(m_histModuleDriftTimeResidual23R[s][l][q][m],s,l,q, m, residual_23R);

		 test[s][l][q][m] = 0.25*(residual_01L + residual_01R + residual_23L+ residual_23R);

		 double dt0 =  test[s][l][q][m];

		 /*
		 double dt0 = 0.25 * (fit(file, "driftTimeResidual01L", s, l, q, m) +
				      fit(file, "driftTimeResidual01R", s, l, q, m) +
				      fit(file, "driftTimeResidual23L", s, l, q, m) +
				      fit(file, "driftTimeResidual23R", s, l, q, m));
		 */

		 //      dt0 = hist->GetFunction("gaus")->GetParameter(1);                                                                                                      
		 double dt0err = hist->GetFunction("gaus")->GetParError(1);

		 double t0 = t0s[s][l][q][m] + test[s][l][q][m];
		 //double t0 = t0s[s][l][q][m] = t0s[s][l][q][m] + dt0;
		 //      t0 = t0s[s][l][q][m] = mtoff[modulen] - (28.0 + 2.0 * s);                                                                                              
		 t0 -= mtoff[modulen];

		 hdt0->SetBinContent(hdt0->FindBin(modulen), dt0);
		 hdt0->SetBinError(hdt0->FindBin(modulen), dt0err);

		 ht0->SetBinContent(ht0->FindBin(modulen), t0 + (28.0 + 2.0 * s));
		 ht0->SetBinError(ht0->FindBin(modulen), dt0err);

		 t0_ = t0;
		 dt0_ = dt0;
		 dt0err_ = dt0err;

		 if(fabs(dt0) > 1) std::cout << histName << " :: dt0 = " << dt0 << std::endl;

		 if(Apply_Calibration){
		   t0s[s][l][q][m]= t0s[s][l][q][m] + test[s][l][q][m];
		 }
		 else
		   t0s[s][l][q][m]= t0s[s][l][q][m] + 0.0;
		 
	       }
	   }


     //Here you need to write t0s or after closing f?

  /*
     // Loop over HPD
     for(int hpdID = m_minHPDID; hpdID< m_maxHPDID; ++hpdID)
       {
	 // HPD copy number
	 const Rich::DAQ::HPDCopyNumber copyNumber =  Rich::DAQ::HPDCopyNumber( hpdID );
	 // SmartID
	 const LHCb::RichSmartID smartID = m_RichSys->richSmartID( copyNumber );
	 if ( msgLevel(MSG::DEBUG) ) debug() << "HPD ID " << hpdID
					     << ", smartID "  << smartID.toString()
					     << ", smartID.key() " << smartID.key()
					     << endmsg;
	 // Get condition
	 Condition* siAlign = getSiSensorAlignment( copyNumber );
	 
	 // Get image histogram
	 std::string imageHistName = m_HistBase + "Rich_HPD_" + std::to_string( hpdID ) + "_Image";
	 TH2D* imageHist = (TH2D*)f->Get( imageHistName.c_str() );
	 
	 // Get occupancy histogram
	 std::string occpHistName = m_OccpHistBase + "CopyNum-" + std::to_string( hpdID ) ;
	 TH1D* occpHist = (TH1D*)f->Get( occpHistName.c_str() );
	 
	 if( !imageHist )  // check image hist is there
	   {
	     warning() << "Failed to get HPD image histogram: " << imageHistName
		       << endmsg ;
	   }
	 else if( !occpHist ) // check occupancy hist is there
	   {
	     // set occupancy to 0
	     HpdOcc.first  = smartID.key() ;
	     HpdOcc.second = 0 ;
	     
	     if( smartID.rich() == Rich::DetectorType::Rich1 ) Rich1_OccsVec.push_back( HpdOcc );
	     if( smartID.rich() == Rich::DetectorType::Rich2 ) Rich2_OccsVec.push_back( HpdOcc );
	     
	     warning() << "Failed to get HPD image occupancy histogram: " << occpHistName
		       << endmsg ;
	   } 
	 else if( imageHist->GetEntries() < m_minEntries )
	   {
	     warning() << "Not enough HPD image entries: " << imageHist->GetEntries() << endmsg ;
	   }
	 else // both hists are there, and enough entries
	   { 
	     // Do the fit
	     const HPDFit::Result result = m_fitter.fit( *imageHist, m_params, occpHist->GetEntries() );
	     if ( msgLevel(MSG::DEBUG) ) debug() << "In Fit, shift of " << hpdID
						 << ": (" << result.x() << ", "  << result.y() << ")"
						 << ", Radius " << result.radInMM()
						 << endmsg;       
	     
	     // Update condition
	     std::string paramName = "dPosXYZ" ;
	     std::vector<double> dPosVec = siAlign->paramAsDoubleVect( paramName );
	     
	     if ( msgLevel(MSG::DEBUG) ) debug() << "In DB, shift of " << hpdID
						 << ": " <<  dPosVec
						 << endmsg ;
	     
	     dPosVec[0] = result.x();
	     dPosVec[1] = result.y();
	     dPosVec[2] = 0 ;
	     
	     siAlign->addParam( paramName, dPosVec, "");
	     
	     if ( msgLevel(MSG::DEBUG) ) debug() << "Condition to write out " << siAlign->toXml() 
						 << endmsg;
	     
	     // Get occupancy
	     HpdOcc.first  = smartID.key() ;
	     HpdOcc.second = occpHist->GetMean() ;
	     
	     if( smartID.rich() == Rich::DetectorType::Rich1 ) Rich1_OccsVec.push_back( HpdOcc );
	     if( smartID.rich() == Rich::DetectorType::Rich2 ) Rich2_OccsVec.push_back( HpdOcc );
	     
	   }   // Now write out xml file
	 xmlVersion = getVersion( m_mergedRun.first, copyNumber );
	 if( !xmlWriter( siAlign, copyNumber, xmlVersion ) ) warning() << "Failed to write xml file for HPD " << hpdID << endmsg;
	 
       }  //Loop over HPD
     
     // Now write out HPD occupancies to xml file
     if( !writeHpdOccXml( Rich1_OccsVec, "Rich1",   m_mergedRun.first ) )
       warning() << "Failed to write xml file for Rich1 Occupancies" << endmsg;
     
     if( !writeHpdOccXml( Rich2_OccsVec, "Rich2",  m_mergedRun.first ) )
       warning() << "Failed to write xml file for Rich2 Occupancies" << endmsg;       
  */
     
     // empty vectors, just for safety to-do: ddo you want to move to vectors?
     //  Rich1_OccsVec.clear();
     //  Rich2_OccsVec.clear();
     
     f->Close();
   
    } else {
   
    //warning() << m_mergedRun.second.c_str() << " is zombie "
    warning() << "file is zombie "
	     << endmsg;
  }
  
  writeCondXMLs(t0s);
  writeCondDBXMLs(t0s);
  
  delete f;
  
  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::readCondDB(double read_t0s[3][4][4][9])
{
  std::string subDet = "OT";

  double Module_t0;

  for(int s = 0; s < 3; s++) 
    for(int l = 0; l < 4; l++) 
      for(int q = 0; q < 4; q++)
	{
	  for(int m = 8; m >= 0; m--){
	    
	    std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];

	    std::string alignLoc ="/dd/Conditions/Calibration/OT/CalibrationModules"+stationNames[s]+layerNames[l]+quarterNames[q]+"/"+stationNames[s]+layerNames[l]+ quarterNames[q]+moduleNames[m];
	    
	    Condition *myCond = get<Condition>( detSvc(), alignLoc );
	    if(simulation){
	      std::vector<double> TZeroVec = myCond->paramAsDoubleVect( "TZero" );
	      for(int i = 0; i<TZeroVec.size();i++){
		//std::cout << "t0 per straw = "<<TZeroVec.at(i)<<std::endl; // for check     
		Module_t0 +=TZeroVec.at(i);
	      }
	      read_t0s[s][l][q][m] = Module_t0/(TZeroVec.size()*1.0);//in simcond the t0 are on straw bases, I make an average - for now - to make it module basis   
	    }
	    else{
	      read_t0s[s][l][q][m] = myCond->paramAsDouble( "TZero" );
	    }

	  }
	}


  return StatusCode::SUCCESS;
}

StatusCode OTt0OnlineClbr::readCondXMLs(double t0s[3][4][4][9])
{
  std::string prefix = "CalibrationModules";

  for(int s = 0; s < 3; s++) 
    for(int l = 0; l < 4; l++) 
      for(int q = 0; q < 4; q++)
	{
	  std::string quarterId = stationNames[s] + layerNames[l] + quarterNames[q];
	  
	  std::string fileName = quarterId + "@" + prefix + ".xml";
	  
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
  boost::filesystem::path full_path(m_xmlFilePath);

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

StatusCode OTt0OnlineClbr::fit_single_hist(TH1D* hist, int s, int l, int q, int m, double& result)
{
  if(hist == 0 || hist->GetEntries() < 100)
    {
      if(!(q % 2 == 0 && m == 8))
	{
	  std::cout << "FAIL: " << s<< " "<<l <<" "<<q<< " "<<m << std::endl;
        }
      return 0.0;
    }
  double left = hist->GetXaxis()->GetXmin();
  double right = hist->GetXaxis()->GetXmax();
  for(int i = 0; i < 5; i++)
    {
      hist->Fit("gaus", "QRLL", "", left, right);
      left = hist->GetFunction("gaus")->GetParameter(1) - 1.0 * hist->GetFunction("gaus")->GetParameter(2);
      right = hist->GetFunction("gaus")->GetParameter(1) + 1.0 * hist->GetFunction("gaus")->GetParameter(2);
    }
  std::cout << "FITTING: " << s<< " "<<l <<" "<<q<< " "<<m <<" "<<hist->GetFunction("gaus")->GetParameter(1)<< " "<< hist->GetFunction("gaus")->GetParameter(2)<<std::endl;

  result =  hist->GetFunction("gaus")->GetParameter(1);

  return StatusCode::SUCCESS;
}


//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTt0OnlineClbr::finalize() {
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;
  
  return AnalysisTask::finalize();  // must be called after all other actions
}
