// $Id: PuVetoAlg.cpp,v 1.29 2006-04-05 08:59:05 mtobin Exp $
// Include files
#include <fstream>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
// from DAQEvent
#include "Event/RawEvent.h"
// from VeloEvent
#include "Event/MCVeloFE.h"
// from L0Event
#include "Event/L0ProcessorData.h"
// local
#include "PuVetoAlg.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : PuVetoAlg
//
// 06/02/2006 : Marko Zupan
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PuVetoAlg );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PuVetoAlg::PuVetoAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
    , m_inputContainer      ( MCVeloFELocation::PuVeto  )
    , m_outputContainer     ( L0ProcessorDataLocation::PileUp  )
    , m_threshold           ( 7000.     ) //electrons
    , m_lowThreshold        (    3      )
    , m_maskingWindow       (    9      )
    , m_binFile             (    ""     )
{
  declareProperty( "InputContainer"     , m_inputContainer  );
  declareProperty( "OutputContainer"    , m_outputContainer );
  declareProperty( "SignalThreshold"    , m_threshold       );
  declareProperty( "PeakThreshold"      , m_lowThreshold    );
  declareProperty( "MaskingWindow"      , m_maskingWindow   );
  declareProperty( "Binning"            , m_binFile         );
 
}

//=============================================================================
// Destructor
//=============================================================================
PuVetoAlg::~PuVetoAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PuVetoAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  debug() << "==> Initialise" << endreq;

  DeVelo* m_velo = get<DeVelo>( detSvc(), "/dd/Structure/LHCb/BeforeMagnetRegion/Velo" );

  m_nbPuSensor = m_velo->numberPileUpSensors();
  m_firstPuSensor = (*(m_velo->pileUpRSensorsBegin()))->sensorNumber();
  
  unsigned int i=0;
  for (std::vector<DeVeloSensor*>::const_iterator iPU=m_velo->pileUpSensorsBegin();
       iPU != m_velo->pileUpSensorsEnd(); ++iPU, ++i) {
    m_zSensor[i] = (*iPU)->z(); 
    debug() << "Sensor " << i << " z = " << m_zSensor[i] << endreq;
  }
 
  // Initialize the binning matrix (just in case)
  for (int k=0;k<2;k++) {
    for (int i=0;i<128;i++) {
      for (int j=0;j<128;j++) {
        m_binMatrix[k][i][j] = -1;
      }
    }
  }

  m_minHistoZ = -150.0;
  m_maxHistoZ = 150.0;

  //default bin widths
  double binlens[85] = {1.25,  1.35,  1.35,  1.40,  1.40,
                        1.40,  1.40,  1.45,  1.45,  1.55,
                        1.55,  1.60,  1.60,  1.60,  1.60,
                        1.70,  1.70,  1.70,  1.80,  1.80,
                        1.80,  1.85,  1.90,  1.90,  2.00,
                        2.00,  2.00,  2.15,  2.15,  2.25,
                        2.25,  2.25,  2.30,  2.35,  2.35,
                        2.35,  2.50,  2.50,  2.65,  2.65,
                        2.80,  2.85,  2.85,  2.85,  3.15,
                        3.15,  3.15,  3.20,  3.50,  3.50,
                        3.50,  3.50,  3.95,  3.95,  3.95,
                        3.95,  4.15,  4.15,  4.25,  4.25,
                        4.85,  4.85,  4.90,  5.15,  5.15,
                        5.15,  5.55,  5.55,  5.85,  6.10,
                        6.10,  6.85,  6.85,  7.35,  7.45,
                        8.05,  8.40,  8.50,  8.95,  9.60,
                        9.60, 10.65, 10.65, 10.65, 10.65};
  
  
  // Set the bins of the histogram

  // Check first if a set of bins is passed in a text file 
  if (m_binFile != "") {
    std::ifstream binfile(m_binFile.c_str());
    if (!binfile.good()) {
      fatal() << "Corrupt binning file: " << m_binFile << endreq;
      return StatusCode::FAILURE;
    }
    
    binfile >> m_nBins;
    
    double binw;
    
    binfile >> binw;
   
    m_binStart[0] = m_minHistoZ;
    m_binCenter[0] = m_minHistoZ + binw/2;
    m_binLength[0] = binw;
   
    for (int i=1;i<m_nBins;i++) {
     
      m_binStart[i] = m_binStart[i-1] + binw;
      binfile >> binw;
      m_binCenter[i] = m_binStart[i] + binw/2;
      m_binLength[i] = binw;
    }

    binfile.close();
    
  }
  else { // otherwise use default binning
    m_nBins = 85;
    m_binStart[0] = m_minHistoZ;
    m_binCenter[0] = m_minHistoZ + binlens[0]/2;
    m_binLength[0] = binlens[0];
    for (int i=1;i<m_nBins;i++) {
      m_binStart[i] = m_binStart[i-1] + binlens[i-1];
      m_binCenter[i] = m_binStart[i] + binlens[i]/2;
      m_binLength[i] = binlens[i];
    } 
  } 


   debug() << m_nBins << " bins, starting at z =  " << m_minHistoZ << endreq;
   for (int i=0;i<m_nBins;i++) {
     debug() << "bin " << i << " starts at " << m_binStart[i] 
             << ", width " << m_binLength[i] << endreq;
   }
   
   // Set r positions of PU "clusters" (4 strips OR-ed together)
   const DeVeloRType* firstSens=(*(m_velo->pileUpRSensorsBegin()));
  for (int i=0;i<128;i++) {
    m_rCluster[i] = firstSens->rOfStrip(i*4+2); 
    debug() << "Cluster " << i << "  r = " << m_rCluster[i] << endreq;
  }

  for (int i=0;i<128;i++) {
    for (int j=0;j<128;j++) {
      for (int k=0;k<2;k++) {
        m_binMatrix[k][i][j] = (short int) 
          zBin(TrackZ(m_rCluster[i],m_zSensor[k],m_rCluster[j],m_zSensor[k+2]));
      }
    }
  }
  

  debug() << "Strip charge threshold: " << m_threshold << endreq;
  debug() << "Peak low threshold: " << m_lowThreshold << endreq;
  debug() << "Masking window: " << m_maskingWindow << endreq;
  debug() << "Binning: " << m_binFile << endreq;

  // debug() << "Peak high threshold: " << m_highThreshold << endreq;
  //debug() << "Peak high thr. position: " << m_highPosition << endreq;
  //debug() << "Peak position cut: " << m_secondPosition << endreq;
  
  return StatusCode::SUCCESS;
}
  
//=============================================================================
// Main execution
//=============================================================================
StatusCode PuVetoAlg::execute() {

  debug() << "==> Execute" << endreq;
  
  // Clear hit and masked bit patterns
  for (int k=0;k<4;k++) {
    for (int i=0;i<16;i++) {
      m_hitPattern[k][i] = 0;
      m_maskedPattern[k][i] = 0;
    }
  }

  m_totMult = 0;
 
  //*** Get the input data

 MCVeloFEs *fes =get<MCVeloFEs>( eventSvc() , m_inputContainer );
  if( 0 == fes ) {
    error() << "Unable to retrieve input data container="
            << m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }

 
  // Apply a threshold on each strip, OR them by 4 and construct the hit bit pattern
  // also store all hits in the RawEvent buffer

  RawEvent* raw = get<RawEvent>( RawEventLocation::Default );
  
  std::vector<unsigned short int> rawPuData;
  std::vector<unsigned int> rawDataVec;
  
  for ( MCVeloFEs::const_iterator itFe = fes->begin();
        fes->end() != itFe ; itFe++  ) {
    if ( m_threshold < (*itFe)->charge() ) {      
      if ( ! (*itFe)->channelID().isPileUp() ){
        info() << "Unexpected sensor " 
               << (*itFe)->channelID().sensor()
               << " in " << m_inputContainer << endreq;
        continue; 
      }

      int sensor = (*itFe)->sensor();
      unsigned int sfired           = 4 * ( (*itFe)->strip()/4 ) + 2;
      VeloChannelID fired(sensor,sfired);
      sensor-=m_firstPuSensor;
      unsigned short int rawhit = rawEncode(sensor,(*itFe)->strip());
      rawPuData.push_back(rawhit);
      
      verbose() << sensor << " " << (*itFe)->strip() << " " 
             << rawhit << endreq;

     

      verbose() << "PU Sensor " << sensor << " strip " 
                << fired.strip();
      
      short unsigned int clnum = (short unsigned int) sfired / 4;
      short unsigned int indx = (short unsigned int) clnum / 32; 
      //32 bits per (unsigned) int
      // replace with sizeof()*8 at some point

      if (!getBit(clnum%32,m_hitPattern[sensor][indx]))
      {
        setBit(clnum%32,m_hitPattern[sensor][indx]); // 32 here again!
        m_totMult++; // count total multiplicity (for multiplicity L0 component)
        verbose() << " added.";
      }
      else verbose() << " exists.";
      verbose() << endreq;
    }    
  }
  

  unsigned int header = 0;
  rawVec(&rawPuData,&rawDataVec);
  raw->addBank(header,RawBank::L0PU,1,rawDataVec);
  
  fillHisto(m_hitPattern);

  // We have filled the 'histogram'. Search for maximum.
  
  unsigned short int height1,sum1;
  unsigned short int height2, sum2;
  double pos1,pos2;
  short int bin1,bin2;
  
  
  sum1 = 0; 
  // sum1 is no longer returned/computed still present as a dummy for testing
  
  pos1 = findPeak1( height1, bin1);
  
  debug() << " Peak1 : Max " << height1 << " at z= " << pos1 
          << " integral " << sum1 << " bin " << bin1 << endreq;

  // mask the contributing hits, fill again and find the
  // second peak. 

  debug() << " Mask around z = " << bin1 
          << " window " << m_maskingWindow << endreq;
    
  maskHits(bin1,m_maskingWindow);
  fillHisto(m_maskedPattern);
  

  pos2 = findPeak2(height2,sum2,bin2);
      
  debug() << " 2nd Max " << height2 << " at z= " << pos2
          << " integral " << sum2 << " bin " << bin2 << endreq;
 
  
  // Now take the decision
  // note: LODU makes its own decision based on sum2

  int decision;
  if ( ( m_lowThreshold > sum2 )) {
    decision = 0;
  }
  else {
    decision = 1;    // Multiple interaction
  } 

  // Pass values to L0DU (untested as of 03/02/2006)

  unsigned int PuWord1 = 0x10000;
  unsigned int PuWord2 = 0x10000;
  unsigned int tmt = Saturate((unsigned int)m_totMult);


  PuWord1 = PuWord1 | ((((unsigned int) height1) << 
                        L0DUBase::L0Pu::Peak::Shift) &  
                        L0DUBase::L0Pu::Peak::Mask    );
  if (bin1 >= 0)
    PuWord1 = PuWord1 | ((((unsigned int) bin1)    << 
                          L0DUBase::L0Pu::Address::Shift) & 
                         L0DUBase::L0Pu::Address::Mask );
  PuWord2 = PuWord2 | ((((unsigned int) sum2)    << 
                        L0DUBase::L0Pu::Peak::Shift) &
                       L0DUBase::L0Pu::Peak::Mask    );
  if (bin2 >= 0)
    PuWord2 = PuWord2 | ((((unsigned int) bin2)    << 
                          L0DUBase::L0Pu::Address::Shift) &
                         L0DUBase::L0Pu::Address::Mask );
  PuWord2 = PuWord2 | (((tmt & 3) << L0DUBase::L0Pu::HitsLSB::Shift)&
                        L0DUBase::L0Pu::HitsLSB::Mask );
  PuWord2 = PuWord2 | (((tmt >> 2) << L0DUBase::L0Pu::HitsMSB::Shift) & 
                       L0DUBase::L0Pu::HitsMSB::Mask );
  
  debug() << "== Decision " << decision << endreq;
  debug() << " Peak1 z,h,s " << bin1 << " " << height1 << " " << sum1 << endreq;
  debug() << " Peak2 z,h,s " << bin2 << " " << height2 << " " << sum2 << endreq;
  debug() << " TotMult " << m_totMult << endreq;
  debug() << " PuWord1 = " << PuWord1 << " PuWord2 = " << PuWord2 << endreq;

  L0ProcessorData* l0PuData1 = new L0ProcessorData( L0DUBase::Fiber::Pu1 , PuWord1);
  L0ProcessorData* l0PuData2 = new L0ProcessorData( L0DUBase::Fiber::Pu2 , PuWord2);
  L0ProcessorDatas* l0PuDatas = new L0ProcessorDatas();
  l0PuDatas->insert( l0PuData1 ) ;
  l0PuDatas->insert( l0PuData2 ) ;

  put( l0PuDatas,  m_outputContainer );

  return StatusCode::SUCCESS;
};

//=========================================================================
//  Fill the histogram
//=========================================================================
void PuVetoAlg::fillHisto (unsigned int hp[4][16]) {
  
  // clear histo
  for ( unsigned int j=0 ; m_nBins > j ; j++ ) {
    m_hist[j] = 0;
  }

  int ia = -1;
  int ib = -1;

  for (int pia=0;pia<16;pia++) {
    for (int bia=0;bia<32;bia++) {
      ia++;
      ib = -1;
      for (int pib=0;pib<16;pib++) {
        for (int bib=0;bib<32;bib++) {
          ib++;
          if ((int)ia/128 != (int)ib/128) continue;
          for (int i=0;i<2;i++){
            short unsigned int acluster = getBit(bia,hp[i][pia]);
            short unsigned int bcluster = getBit(bib,hp[i+2][pib]);
            if (acluster && bcluster) {
              short int bin = m_binMatrix[i][ia%128][ib%128];
              if (bin > -1) {
                m_hist[bin]++;
              }
            }
          }
        }
      }
    }
  }
}




//=========================================================================
//  Mask the hits around the first peak bin, for a total of mwbins
//=========================================================================
void PuVetoAlg::maskHits ( short int bin,short int mwbins ) {

  for (int i=0;i<128;i++) {
    for (int j=0;j<128;j++) {
      for (int k=0;k<2;k++) {
        if (abs(m_binMatrix[k][i][j]-bin)<=(mwbins-1)/2) m_maskMatrix[k][i][j] = 1;
        else  m_maskMatrix[k][i][j] = 0;
      }
    }
  }
  
 int ia = -1;
 int ib = -1;
 unsigned int maskpat[4][16];
 
 
 for (int j=0;j<(int)m_nbPuSensor;j++) {
   for (int i=0;i<16;i++) {
     maskpat[j][i] = 0;
   }
 }
 
 for (int pia=0;pia<16;pia++) {
   for (int bia=0;bia<32;bia++) {
     ia++;
     ib = -1;     
     for (int pib=0;pib<16;pib++) {
       for (int bib=0;bib<32;bib++) {
         ib++;
         if ((int)ia/128 != (int)ib/128) continue;
         for (int i=0;i<2;i++){
           short unsigned int acluster = getBit(bia,m_hitPattern[i][pia]);
           short unsigned int bcluster = getBit(bib,m_hitPattern[i+2][pib]);
           if (m_maskMatrix[i][ia%128][ib%128] && acluster && bcluster) {
             setBit(bia,maskpat[i][pia]);
             setBit(bib,maskpat[i+2][pib]);
           }
         }
       }
     }
   }
 }
 
 for (int pi=0;pi<16;pi++) {
   for (int bi=0;bi<32;bi++) {
     for(int i=0;i<(int)m_nbPuSensor;i++) {
      if (getBit(bi,m_hitPattern[i][pi]) && !getBit(bi,maskpat[i][pi])) 
        setBit(bi,m_maskedPattern[i][pi]);
     }
   }
 }
 
 
}

//===========================================================================
//  Find first and second peaks, return bin, height, and for the second peak 
//  also sum.
//=========================================================================== 

double PuVetoAlg::findPeak1(unsigned short int &height, short int &bin)
{
  double pos = -999.;
  unsigned short int theight = 0;
 
  bin = -1;

  for ( int jj=0 ; jj < m_nBins ; jj++ ) {
    if ( theight < m_hist[jj] ) {
      theight = m_hist[jj];
      bin    = jj;
      pos = m_binCenter[jj];
    }
  }
  height = theight;
  return pos;  
}
//=========================================================================

double PuVetoAlg::findPeak2(unsigned short int &height, unsigned short int &sum, short int &bin)
{
  double pos = -999.;
  height = 0;
  sum    = 0;
  unsigned short int sumt = 0;
  bin = -1;

  for ( int jj=0 ; jj < m_nBins ; jj++ ) {
    sum = m_hist[jj];
    if (jj>0) sum += m_hist[jj-1];
    if (jj<m_nBins-1) sum += m_hist[jj+1];
    if ( sum > sumt) {
      sumt = sum;
      bin    = jj;
    }    
  } 
  if (bin>=0) {
    sum = sumt;
    height = m_hist[bin];
    pos = m_binCenter[bin];
  }
  return pos;  
}

//=============================================================================
// Helper methods for the encoding of RawEvent banks
//=============================================================================


unsigned short int PuVetoAlg::rawEncode(int sensor, int strip) 
{
  unsigned short int temp;
  
  temp = strip;
  temp+=sensor << 14;
  return temp;
}

void PuVetoAlg::rawVec (std::vector<unsigned short int> *vecin,
                        std::vector<unsigned int> *vecout) 
{
  std::vector<unsigned short int>::iterator itraw = vecin->begin();
  unsigned short int temp1,temp2;
  unsigned int tempraw;
  
  while (itraw != vecin->end()){
    temp1 = (*itraw);
    itraw++;
    temp2 = 0;
    if (itraw != vecin->end()) temp2 = (*itraw);
    tempraw = temp2;
    tempraw = tempraw << 16;
    tempraw += temp1;
    //std::cout << temp1 << " " << temp2 << " " << tempraw << std::endl;
    vecout->push_back(tempraw);
  }
}
