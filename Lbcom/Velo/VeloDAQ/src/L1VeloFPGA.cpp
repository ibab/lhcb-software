// $Id: L1VeloFPGA.cpp,v 1.1.1.1 2004/03/16 14:28:12 cattanem Exp
// Include files 
#include <vector> 

// Random numbers:
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// Velo:
#include "VeloKernel/VeloSimParams.h"
#include "Event/MCVeloFE.h"

// local
#include "L1VeloFPGA.h"
#include "L1VeloFPGACluster.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L1VeloFPGA
//
// 17/05/2002 : Niels Tuning
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L1VeloFPGA::L1VeloFPGA( ) {}

//=============================================================================
// VELO creator:
//=============================================================================
L1VeloFPGA::L1VeloFPGA( int firstStrip, std::vector<MCVeloFE*> groupOfMCVeloFE) {
  m_firstStrip      = firstStrip;
  m_groupOfMCVeloFE = groupOfMCVeloFE;
  m_nStrips         = groupOfMCVeloFE.size();
  
  setRawCharge();
  for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    m_corCharge[istrip][0] = 0.;
    m_detectHit[istrip][0] = 0 ;
    m_corCharge[istrip][1] = 0.;
    m_detectHit[istrip][1] = 0 ;
  }
  for ( int iter = 0; iter < NITER_FPGA ; iter++ ) {
    m_mean[iter]     = 0.0;
    m_slope[iter]    = 0.0;
    m_variance[iter] = 0.0;
  }
}


//=============================================================================
// Destructor
//=============================================================================
L1VeloFPGA::~L1VeloFPGA() {};

//=============================================================================
void L1VeloFPGA::setRawCharge( ) {

  MCVeloFE* theFE = NULL;
  int stripnr     = m_firstStrip;

  for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    bool stripIsHit = false;
    for ( int it = 0; it < m_nStrips ; it++ ) {
      int MCVeloStripNr = (m_groupOfMCVeloFE[it])->strip();
      if (MCVeloStripNr == stripnr){
        stripIsHit = true;
        theFE = m_groupOfMCVeloFE[it];
      }
    }
    if (stripIsHit){
      m_rawCharge[istrip] = theFE->charge();
      //std::cout << " Set Velo charge: " << istrip 
      //          << " " << m_rawCharge[istrip] << endl;
    } else {
      m_rawCharge[istrip] = 0.;
    }
    stripnr += 1;
  }
}


//=============================================================================
void L1VeloFPGA::setNoise( Rndm::Numbers& gaussDist ) {
  for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    if ( fabs(m_rawCharge[istrip]) < 1.e-6) {
      // Add random noise in whole group of 32 neighbouring strips!
      // See also: Velo/VeloAlgorithms/v*/src/VeloSim.cpp
      // 26/11/02
      // ------------------------------------------------------------
      double stripCapacitance=VeloSimParams::averageStripCapacitance;
      double noiseSigma      =stripCapacitance * VeloSimParams::noiseCapacitance
                                               + VeloSimParams::noiseConstant ;
      double noise           = noiseSigma*gaussDist();
      m_rawCharge[istrip]    = noise;
    }
  }
}



//=============================================================================
MCVeloFE*  L1VeloFPGA::fe( int istrip) {
  int stripnr = m_firstStrip + istrip;
  MCVeloFE* theFE = NULL;  
  for ( int it = 0; it < m_nStrips ; it++ ) {
    MCVeloFE* tmpFE = m_groupOfMCVeloFE[it];
    int feNr        = tmpFE->strip();
    if (feNr == stripnr){
      theFE = tmpFE;
      break;
    }
  }
  return (theFE);
}

//================================================================
void L1VeloFPGA::cmCorrection(double thresholdSigma, int doSlopeCor) {
  // Method to apply Linear Common Mode Suppression (See LHCB Note 2001-043)
  double F1  = 1./(thresholdSigma*thresholdSigma) ;

  double meanTmp[NITER_FPGA];
  double slopeTmp[NITER_FPGA];
  double varianceTmp[NITER_FPGA];
  double sigmaTmp[NITER_FPGA];
  double rawChargeTmp[NSTRIPS_FPGA];
  double corChargeTmp[NSTRIPS_FPGA][NITER_FPGA];
  int    detectHitTmp[NSTRIPS_FPGA][NITER_FPGA];

  int    countHits[NITER_FPGA];

  // Initialize :
  {for ( int iter = 0; iter < NITER_FPGA ; iter++ ) {
    meanTmp[iter]     = 0.;
    slopeTmp[iter]    = 0.;
    varianceTmp[iter] = 0.;
    countHits[iter]= 0;
  }
  }
  

  {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    rawChargeTmp[istrip] = rawCharge(istrip);
    for ( int iter = 0; iter < NITER_FPGA ; iter++ ) {
      corChargeTmp[istrip][iter] = 0.;
      detectHitTmp[istrip][iter] = 0 ;
    }
  }
  }
  
  //---------------------------------------------------------------
  
  {for ( int iter = 0; iter < NITER_FPGA ; iter++ ) {

    // Calculate mean:
    {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
      if (0 == iter || 0 == detectHitTmp[istrip][iter-1]){
        meanTmp[iter] += (1./32.)*rawChargeTmp[istrip];
      }
    }}

    if ( 1 == doSlopeCor) {
      // Calculate slope:
      for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
        if (0 == iter || 0 == detectHitTmp[istrip][iter-1]){
          slopeTmp[iter] +=(3./8192.)*(istrip-16)*(rawChargeTmp[istrip] - 
                                                   meanTmp[iter] );
        }
      }
    }
    
    // Correct charge and calculate consequent variance:
    for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
      corChargeTmp[istrip][iter] = 
        (rawChargeTmp[istrip] - meanTmp[iter]) - slopeTmp[iter]*(istrip - 16);
      if (0 == iter || 0 == detectHitTmp[istrip][iter-1]){
        varianceTmp[iter] += (1./32.)*
          corChargeTmp[istrip][iter]*corChargeTmp[istrip][iter];
      }
    }
    // sigma^2 = 1/(N-1) * Sum_i=1^i=N [ (meas_i - mean)^2 ]
    //---------------------------------------------------------
    sigmaTmp[iter] = sqrt(varianceTmp[iter]);
    
    
    // Hit detection:
    countHits[iter] = 0;
    {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
      if (   corChargeTmp[istrip][iter] > 0. &&
          F1*corChargeTmp[istrip][iter]*corChargeTmp[istrip][iter] > 
             varianceTmp[iter]){
        detectHitTmp[istrip][iter] = 1;
        countHits[iter]++;
      }
    }}
  } // End iterations linear common mode suppression
  }
  

  // Fill data members:
  {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    m_corCharge[istrip][NITER_FPGA-2] = corChargeTmp[istrip][NITER_FPGA-2];
    m_detectHit[istrip][NITER_FPGA-2] = detectHitTmp[istrip][NITER_FPGA-2];
    m_corCharge[istrip][NITER_FPGA-1] = corChargeTmp[istrip][NITER_FPGA-1];
    m_detectHit[istrip][NITER_FPGA-1] = detectHitTmp[istrip][NITER_FPGA-1];
  }
  }
  
  m_mean[ 0]    = meanTmp[ NITER_FPGA-2];
  m_mean[ 1]    = meanTmp[ NITER_FPGA-1];
  m_slope[0]    = slopeTmp[NITER_FPGA-2];
  m_slope[1]    = slopeTmp[NITER_FPGA-1];
  m_variance[0] = varianceTmp[NITER_FPGA-2];
  m_variance[1] = varianceTmp[NITER_FPGA-1];
  
}

//============================================================================
L1VeloFPGACluster L1VeloFPGA::makeCluster( std::vector<int> iDAQchannelsCluster, double totalCharge) {
  MCVeloFE* tmpfe = NULL;
  int lowestStripNr     = 9999;
  int stripNr           = 9999;
  int nStrips           = iDAQchannelsCluster.size();
  int stripNrMod32      = 9999;
  int nStripsFE         = 0;
  std::vector<MCVeloFE*> mcVeloFEinCluster; 

  mcVeloFEinCluster.clear();  

  // Find first strip of cluster:
  {for(int istrip=0 ; istrip<nStrips ; istrip++){
    stripNr = iDAQchannelsCluster[istrip];
    stripNrMod32 = stripNr%32;
    if (stripNr < lowestStripNr){
      lowestStripNr = stripNr;
    }
  }}

  // Fill vector with MCVeloFEs of cluster:
  {for(int istrip=0 ; istrip<nStrips ; istrip++){
    stripNr = iDAQchannelsCluster[istrip];
    stripNrMod32 = stripNr%32;
    tmpfe = fe( stripNrMod32 ); 
    if (tmpfe != NULL){
      nStripsFE++;
      mcVeloFEinCluster.push_back(tmpfe);
    }
  }}
   


  int sensor = (m_groupOfMCVeloFE[0])->sensor(); 
  L1VeloFPGACluster myclus(sensor, lowestStripNr, nStrips, 
                       nStripsFE, totalCharge, mcVeloFEinCluster);
  return myclus;
}

//============================================================================
void L1VeloFPGA::clustering(double thresholdSigma, double relThresholdNeighbour) {
  clustering(thresholdSigma, relThresholdNeighbour, 2);
}

//============================================================================
void L1VeloFPGA::clustering(double thresholdSigma, double relThresholdNeighbour,
                      int maxClusSize = 2) {
  //===============================================
  double F1  = 1./(thresholdSigma*thresholdSigma) ;
  //===============================================
  //int maxClusSize  = 2;
  int centralStrip = -999;
  int nextStrip    = -999;
  int nstrip       = 0;
  //
  double left1Charge = 0.0;
  double cent0Charge = 0.0;
  double righ1Charge = 0.0;
  double totalCharge = 0.0;

  std::vector<L1VeloFPGACluster> FPGAClusters;
  std::vector<int> iDAQchannelsCluster;
  std::vector<double> chargeCluster;

  int thisSideFirst = 0;

  int stripIsHit[NSTRIPS_FPGA];
  int stripIsUsed[NSTRIPS_FPGA];
  {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    stripIsUsed[istrip] = 0;
    stripIsHit[istrip] = m_detectHit[istrip][1];
  }
  }
  
  
  {for ( int istrip = 0; istrip < NSTRIPS_FPGA ; istrip++ ) {
    if ( 1 == stripIsHit[istrip]  && 0 == stripIsUsed[istrip]){
      nstrip = 1;
      
      centralStrip = istrip;
      if (0 == centralStrip) {
        left1Charge = 0.0;
      } else {
        left1Charge = corCharge( centralStrip-1, 1);
      }
      //----------------------------------------------------
      cent0Charge = corCharge( centralStrip,   1);
      //----------------------------------------------------
      if (31 == centralStrip) {
        righ1Charge = 0.0;
      } else {
        righ1Charge = corCharge( centralStrip+1, 1);
      }
      
      if (left1Charge > righ1Charge) {
        thisSideFirst = -1;
      }else{
        thisSideFirst =  1;
      }
      
      totalCharge               = cent0Charge;
      stripIsUsed[centralStrip] = 1;
      // Register strip:
      iDAQchannelsCluster.clear();
      iDAQchannelsCluster.push_back(m_firstStrip+centralStrip);
      chargeCluster.clear();
      chargeCluster.push_back(cent0Charge);
      
      //----------------------------------------------------------------------
      // Loop over neighbouring strips, add strips that:
      // o were unused
      // o have at least 20% of the charge, compared to the detectedHit
      // o are within the group of 32 channels
      //----------------------------------------------------------------------
      int onlyThisSide = 0;
      for ( int addStrip = 1; addStrip < maxClusSize  ; addStrip++ ) {
        int alternatingSign = 2*(addStrip%2)-1;
        if (0 == onlyThisSide){
          nextStrip = centralStrip +
            thisSideFirst*alternatingSign*int((addStrip+1)/2);
        }else{
          nextStrip = nextStrip + thisSideFirst*onlyThisSide;
        }

        if (nextStrip >= 0 && nextStrip < NSTRIPS_FPGA){
          double addCharge = corCharge( nextStrip, 1);
          if (addCharge > relThresholdNeighbour*cent0Charge && 
              0 == stripIsUsed[nextStrip]){
            nstrip++;
            totalCharge += addCharge;
            stripIsUsed[nextStrip] = 1;
            // Register strip:
            iDAQchannelsCluster.push_back(m_firstStrip+nextStrip);
            chargeCluster.push_back(addCharge);
          }else if( 0 == onlyThisSide ){
            onlyThisSide = -alternatingSign;
            nextStrip = centralStrip -
              thisSideFirst*alternatingSign*int((addStrip)/2);
          }else{
            break;
          }
        }else{
          break;
        }
      } // end loop over neighbouring strips.
      //----------------------------------------------------------------------

      if (F1*totalCharge*totalCharge > m_variance[1] ){
	L1VeloFPGACluster myclus = makeCluster(iDAQchannelsCluster,totalCharge);
	FPGAClusters.push_back(myclus);
      }

    } // end if hitDetected
  } // end loop over strips
  }
  
  // Save data member:
  m_FPGAClusters   = FPGAClusters;
  
}

//================================================================
std::vector<L1VeloFPGACluster> L1VeloFPGA::FPGAClusters() {
  return (m_FPGAClusters);
}
