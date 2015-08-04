// Include files
#include <fstream>
#include <string>

// from DAQEvent
#include "Event/RawEvent.h"

// from L0Event
#include "Event/L0ProcessorData.h"
// local
#include "PuVetoAlg.h"
#include "bit_order.h"

// histogramming
#include "GaudiUtils/Aida2ROOT.h"  
#include "TFile.h"


using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : PuVetoAlg
//
// 06/02/2006 : Marko Zupan
// 14/07/2009 : Serena Oggero (new PU banks format)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( PuVetoAlg )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PuVetoAlg::PuVetoAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
  , m_outputContainer     ( L0ProcessorDataLocation::PileUp  )
  , m_lowThreshold        (    3      )
  , m_maskingWindow       (    9      )
  , m_binFile             (    ""     )
  , m_evtNum              (    0      )
  , m_velo(NULL)
  , m_OutputFile(NULL)
  , m_PUvertices(NULL)
  , m_PUvertex1Pos(NULL)
  , m_PUvertex2Pos(NULL)
  , m_PUvertex1Height(NULL)
  , m_PUvertex2Height(NULL)
  , m_multiplicity(NULL)
{
  declareProperty( "OutputContainer"    , m_outputContainer ); 
  declareProperty( "PeakThreshold"      , m_lowThreshold    );
  declareProperty( "MaskingWindow"      , m_maskingWindow   );
  declareProperty( "Binning"            , m_binFile         );
  
  declareProperty("RawEventLocation", m_rawEventLoc=LHCb::RawEventLocation::Default);
  // comment for committing
//   declareProperty("OutputFileName", m_OutputFileName = "PUVetoAlg.root" );
//   declareProperty("MakePlots", m_enablePlots = false );
  m_minHistoZ = -150.0;
  m_maxHistoZ = 150.0;
}

//=============================================================================
// Destructor
//=============================================================================
PuVetoAlg::~PuVetoAlg() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PuVetoAlg::initialize() {

  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  if (msgLevel(MSG::DEBUG)) debug() << "==> Initialise PuVetoAlg" << endmsg;

  DeVelo* m_velo = getDet<DeVelo>( DeVeloLocation::Default );

  m_nbPuSensor = m_velo->numberPileUpSensors();
  
  unsigned int i=0;
  for (std::vector<DeVeloSensor*>::const_iterator iPU=m_velo->pileUpSensorsBegin();
       iPU != m_velo->pileUpSensorsEnd(); ++iPU, ++i) {
    m_zSensor[i] = (*iPU)->z(); 
    //if (msgLevel(MSG::DEBUG)) debug() << "Sensor " << i << " z = " << m_zSensor[i] << endmsg;
  }
 
  // Initialize the binning matrix (just in case)
  for (int k=0;k<2;k++) {
    for (int i=0;i<128;i++) {
      for (int j=0;j<128;j++) {
        m_binMatrix[k][i][j] = -1;
      }
    }
  }
  
  
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
      fatal() << "Corrupt binning file: " << m_binFile << endmsg;
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
    if (msgLevel(MSG::DEBUG)) debug() << "Use the default binning" << endmsg;
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


  //if (msgLevel(MSG::DEBUG)) debug() << m_nBins << " bins, starting at z =  " << m_minHistoZ << endmsg;
  for (int i=0;i<m_nBins;i++) {
    //if (msgLevel(MSG::DEBUG)) debug() << "bin " << i << " starts at " << m_binStart[i] 
    //        << ", width " << m_binLength[i] << endmsg;
  }
   
  // Set r positions of PU "clusters" (4 strips OR-ed together)
  const DeVeloRType* firstSens=(*(m_velo->pileUpRSensorsBegin()));
  for (int i=0;i<128;i++) {
    m_rCluster[i] = firstSens->rOfStrip(i*4+2); 
  }

  for (int i=0;i<128;i++) {
    for (int j=0;j<128;j++) {
      if( i == j ) continue; // Protect divide by zero, diagonal elements not used
      for (int k=0;k<2;k++) {
        m_binMatrix[k][i][j] = (short int) 
          zBin(TrackZ(m_rCluster[i],m_zSensor[k],m_rCluster[j],m_zSensor[k+2]));
      }
    }
  }
  

  if (msgLevel(MSG::DEBUG)) debug() << "Peak low threshold: " << m_lowThreshold << endmsg;
  if (msgLevel(MSG::DEBUG)) debug() << "Masking window: " << m_maskingWindow << endmsg;
  if (msgLevel(MSG::DEBUG)) debug() << "Binning: " << m_binFile << endmsg;

  // if (msgLevel(MSG::DEBUG)) debug() << "Peak high threshold: " << m_highThreshold << endmsg;
  //if (msgLevel(MSG::DEBUG)) debug() << "Peak high thr. position: " << m_highPosition << endmsg;
  //if (msgLevel(MSG::DEBUG)) debug() << "Peak position cut: " << m_secondPosition << endmsg;
  
  // comment in case..
  
//   if ( m_enablePlots ){
//     m_OutputFile = new TFile(m_OutputFileName.c_str(), "RECREATE");
//     m_OutputFile->cd();
//     m_PUbanksize = new TH1D("L0PUbanksize", "L0PUbanksize", 200, 0., 200.);
   // m_PUvertex1Pos = new TH1D("PrimaryVerticesPos", "PrimaryVerticesPos", 300, -200., 100.);
   // m_PUvertex2Pos = new TH1D("SecondaryVerticesPos", "SecondaryVerticesPos", 300, -200., 100.);
   // m_PUvertex1Height = new TH1D("PrimaryVerticesHeight", "PrimaryVerticesHeight", 100, 0., 100.);
   // m_PUvertex2Height = new TH1D("SecondaryVerticesHeight", "SecondaryVerticesHeight", 100, 0., 100.);
   // m_multiplicity = new TH1D("Multiplicity", "Multiplicity", 512, 0., 512.);
//   }
  
  return StatusCode::SUCCESS;
}
  
//=============================================================================
// Main execution
//=============================================================================
StatusCode PuVetoAlg::execute() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Execute: " << endmsg;
  
  // Clear hit and masked bit patterns
  for (int k=0;k<4;k++) {
    for (int i=0;i<16;i++) {
      m_PUhitmap[k][i] = 0;
      m_maskedPattern[k][i] = 0;
    }
  }

  m_totMult = 0;
  //*** Get the input data and store them in a map m_PUhitmap[sensor][beetle]
  RawEvent* raw = get<RawEvent>( m_rawEventLoc );
  const std::vector<LHCb::RawBank*>& bank = raw->banks( LHCb::RawBank::L0PU );
  if (msgLevel(MSG::DEBUG)) {
    debug() << "Get Raw Event from " << m_rawEventLoc << endmsg;
    debug() << "LHCb::RawBank::L0PU size is " << bank.size() << endmsg;
    debug() << "----- TotMult " << m_totMult<< endmsg ;
  }
  std::vector<LHCb::RawBank*>::const_iterator itBnk;
  for ( itBnk = bank.begin() ; bank.end() != itBnk ; itBnk++ ) {
    LHCb::RawBank* aBank = *itBnk;
    if( RawBank::MagicPattern != aBank->magic() ) {
      if (msgLevel(MSG::DEBUG)) debug() << "magic number is " << aBank->magic() << "; event skipped!" << endmsg;
      return Warning( "L0PU RawBank has an unexpected magic number!", StatusCode::SUCCESS, 0 );
    }
    else{
      int version = aBank->version();
      if (msgLevel(MSG::DEBUG)) debug() << "Bank version is " << version << endmsg;
    
      if ( version == 2 ){ // current bank format
        if (msgLevel(MSG::DEBUG)) debug() << "----- TotMult " << m_totMult<< endmsg ;
        
        unsigned int* data = aBank->data();
        unsigned int d = 2; 
        int wordTot = (aBank->size() / (2 * sizeof(unsigned int)));
//       if (m_enablePlots){
//         m_PUbanksize->Fill(wordTot);
//       }
        if (wordTot != 34 ) {
          if (msgLevel(MSG::DEBUG)) 
            debug() << "L0PU RawBank has an unexpected size! (size= " << wordTot << ") - event skipped" << endmsg;
	  return Warning( "L0PU RawBank has an unexpected size! Event skipped", StatusCode::SUCCESS, 0 );
        }
        else{
          fillPUmap( d, wordTot, data, 34, m_PUhitmap );
          //now check whether the words must be reversed or not
          reverseWords( m_PUhitmap );
        }
      }
      else if ( version == 1 ){ // old bank formatversion from Marko
        unsigned int* ptData = (*itBnk)->data();
        int bankSize = (*itBnk)->size()/4;  //== is in bytes...
        while ( 0 < bankSize-- ){
          unsigned int cand = (*ptData++);
          while ( 0 != cand ) {
            unsigned int data = cand & 0xFFFF;
            cand = cand >> 16;
            unsigned int sensor = data >> 14;
            unsigned int sfired = data & 0x3FFF;
  
            short unsigned int clnum = (short unsigned int) sfired / 4;
            short unsigned int indx = (short unsigned int) clnum / 32; 
            //32 bits per (unsigned) int
            // replace with sizeof()*8 at some point
            //if (msgLevel(MSG::DEBUG)) debug() << format( "Data %4x sensor%2d strip%4d clnum%3d indx%3d ",
            //                       data, sensor, sfired, clnum, indx );
  
            if ( MSG::VERBOSE <= msgLevel() ) {
              verbose() << format( "Data %4x sensor%2d strip%4d clnum%3d indx%3d ",
                                   data, sensor, sfired, clnum, indx );
            }
   
            if (!getBit(clnum%32,m_PUhitmap[sensor][indx])) {
              setBit(clnum%32,m_PUhitmap[sensor][indx]); // 32 here again!
              m_totMult++;
              //verbose() << " added.";
            } else {
              //verbose() << " exists.";
            }
            //verbose() << endmsg;
	    //if (msgLevel(MSG::DEBUG)) debug() << "m_PUhitmap[" << sensor << "][ " << indx << "] is " << binary( m_PUhitmap[sensor][indx] )<< endmsg;
          }
        }
      }
    } // magic number check

  } // loop on banks

  // now evaluate PU trigger variables...  
  measureMult( m_PUhitmap );
  
  // comment in case..
  /*
  char name[80];
  char title[80];
  
  if ( m_enablePlots){
    sprintf(name, "PU vertices-evt%d", m_evtNum);
    sprintf(title,"PU vertices-evt%d", m_evtNum);
    m_PUvertices = new TH1D(name, title, 85, 0., 85.);
  }
  */
  //
  fillHisto( m_PUhitmap );

  unsigned short int height1,sum1;
  unsigned short int height2, sum2;
  double pos1,pos2;
  short int bin1,bin2;
    
  sum1 = 0; // sum1 is no longer returned/computed still present as a dummy for testing
  pos1 = findPeak1( height1, bin1);
  
  // comment in case..
  
  // if ( m_enablePlots){
    //m_multiplicity->Fill(m_totMult);  
    //m_OutputFile->cd();
    //if ( pos1 != -999 )  
    //{
    //  m_PUvertex1Pos->Fill(pos1);
    //  m_PUvertex1Height->Fill(height1);
    //}
    //if ( pos1 != -999 && height1 > 1 ) m_PUvertices->Write(); 
    //if ( pos1 != -999 ) m_PUvertices->Write(); 
    //if ( pos1 != -999 &&  m_totMult > 30 ) m_PUvertices->Write();
  //}
  
  
  if (msgLevel(MSG::DEBUG)) debug() << " Peak1 : Max " << height1 << " at z= " << pos1 
          << " integral " << sum1 << " bin " << bin1 << endmsg;


  // mask the contributing hits, fill again and find the second peak. 
  if (msgLevel(MSG::DEBUG)) debug() << " Mask around z = " << bin1 
          << " window " << m_maskingWindow << endmsg;

  maskHits(bin1,m_maskingWindow);
  fillHisto( m_maskedPattern );
  pos2 = findPeak2(height2,sum2,bin2);
  if (msgLevel(MSG::DEBUG)) debug() << " 2nd Max " << height2 << " at z= " << pos2
          << " integral " << sum2 << " bin " << bin2 << endmsg;
  /*
  if ( m_enablePlots){  
    if ( pos2 != -999 ){  
      m_PUvertex2Pos->Fill(pos2);
      m_PUvertex2Height->Fill(height2);
    }
  }
  */
  
  // Now take the decision
  // note: LODU makes its own decision based on the multiplicity (@Jan 2010)
  // the following is the "old" decision
  int decision;
  if ( ( m_lowThreshold > sum2 )) {
    decision = 0;
  }
  else {
    decision = 1;    // Multiple interaction
  } 

  // Pass values to L0DU
  unsigned int PuWord1 = 0x10000;
  unsigned int PuWord2 = 0x10000;
  unsigned int tmt = Saturate((unsigned int)m_totMult); // temporary, to be checked

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

  if (msgLevel(MSG::DEBUG)) {
    debug() << "====== Decision " << decision << endmsg;
    debug() << " Peak1 z,h,s " << bin1 << " " << height1 << " " << sum1 << endmsg;
    debug() << " Peak2 z,h,s " << bin2 << " " << height2 << " " << sum2 << endmsg;
    debug() << " TotMult " << m_totMult << " Tmt " << tmt << endmsg;
    debug() << " PuWord1 = " << PuWord1 << " PuWord2 = " << PuWord2 << endmsg;
  }
  
  L0ProcessorData* l0PuData1 = new L0ProcessorData( L0DUBase::Fiber::Pu1 , PuWord1);
  L0ProcessorData* l0PuData2 = new L0ProcessorData( L0DUBase::Fiber::Pu2 , PuWord2);
  L0ProcessorDatas* l0PuDatas = new L0ProcessorDatas();
  l0PuDatas->insert( l0PuData1 ) ;
  l0PuDatas->insert( l0PuData2 ) ;

  put( l0PuDatas,  m_outputContainer );
  if (msgLevel(MSG::DEBUG)) debug() << "I put L0PuDatas in " << m_outputContainer << endmsg;
  m_evtNum++;
  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize 
//=============================================================================
StatusCode PuVetoAlg::finalize() 
{
  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  
//   if ( m_enablePlots ){
//     m_PUbanksize->Write();
    //m_PUvertex1Pos->Write( );
    //m_PUvertex2Pos->Write( );
    //m_PUvertex1Height->Write( );
    //m_PUvertex2Height->Write( );
    //m_multiplicity->Write( );
    //m_OutputFile->Close();
//   }
  
  return GaudiAlgorithm::finalize();  // must be called after all other actions
}

//=============================================================================
// Count multiplicity
//=============================================================================
void PuVetoAlg::measureMult (unsigned int hp[4][16]) {
  for (unsigned s=0; s<2; s++){ // sensors loops
    for (unsigned bee=0; bee<16; bee++) { // beetles loop
       for (unsigned bitIt=0; bitIt<32; bitIt++) { // bit mask
         if ( getBit(bitIt, hp[s+2][bee]) ) {
	   m_totMult++; 
	 }
         // only the front sensors are used by the hardware
       }
    }
  }
}

//=========================================================================
//  Fill the histogram
//=========================================================================
void PuVetoAlg::fillHisto (unsigned int hp[4][16]) {

  // clear histo
  if (msgLevel(MSG::DEBUG)) debug() << "fillHisto - start " << endmsg;
  for ( unsigned int j=0 ; m_nBins > j ; j++ ) {
    m_hist[j] = 0;
  }

  for (unsigned i=0;i<2;i++){ // sensor i 
    for (unsigned pia=0;pia<16;pia++) {  // beetles pia, pib 
      for (unsigned pib=0;pib<16;pib++) {
        if( (pib>>2) != (pia>>2)  ) continue; // goes on only if the hits are in the same up (bottom) halves
        for (unsigned bia=0;bia<32;bia++) {
          if (getBit(bia,hp[i][pia])==0) continue; // look for hits..
          unsigned ia=(((pia<<5)|bia)%128); // beetle index * 2^5 + channel index ==> loop over all beetle channels
          for (unsigned bib=0;bib<32;bib++) {
            if ( getBit(bib,hp[i+2][pib])==0) continue;
            unsigned ib=(((pib<<5)|bib)%128); 
            short int bin = m_binMatrix[i][ia][ib]; // association between hits combination and bin
            if (bin > -1) m_hist[bin]++;
	    /*if (bin > -1 && m_enablePlots ) m_PUvertices->Fill( bin );*/
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

  for (int k=0;k<2;k++) {
    for (int i=0;i<128;i++) {
      for (int j=0;j<128;j++) {
        if (abs(m_binMatrix[k][i][j]-bin)<=(mwbins-1)/2) m_maskMatrix[k][i][j] = 1;
        else  m_maskMatrix[k][i][j] = 0;
      }
    }
  }
  
 unsigned int maskpat[4][16];
 
 
 for (int j=0;j<(int)m_nbPuSensor;j++) {
   for (int i=0;i<16;i++) {
     maskpat[j][i] = 0;
   }
 }
 
 for (int i=0;i<2;i++){
   for (int pia=0;pia<16;pia++) {
     for (int pib=0;pib<16;pib++) {
       if( (pib>>2) != (pia>>2)  ) continue;
       for (int bia=0;bia<32;bia++) {
         if (getBit(bia,m_PUhitmap[i][pia])==0) continue;
         for (int bib=0;bib<32;bib++) {
	   if (getBit(bib,m_PUhitmap[i+2][pib])==0) continue;
           unsigned ia =((pia<<5)|bia)%128;
           unsigned ib =((pib<<5)|bib)%128;
           if (m_maskMatrix[i][ia][ib]) {
             setBit(bia,maskpat[i][pia]);
             setBit(bib,maskpat[i+2][pib]);
           }
         }
       }
     }
   }
 }
 
 for(int i=0;i<(int)m_nbPuSensor;i++) {
   for (int pi=0;pi<16;pi++) {
     for (int bi=0;bi<32;bi++) {
      if (getBit(bi,m_PUhitmap[i][pi]) && !getBit(bi,maskpat[i][pi])) 
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

//=========================================================================
void PuVetoAlg::reverseWords( unsigned int PU_hitmap[4][16] ) 
{
  for (int sens = 0; sens<4; sens++ ){
    for ( int bee = 0; bee<16; bee++ ){
           
      switch(bee){
        case 3:
	case 5:
	case 6:
	case 7:
	case 11:
	case 13:
	case 14:
	case 15:
	  reverse_bit(PU_hitmap[sens][bee]);
	  break;
	default:
	  break;
      }
    }
  }
}
//============================================================================
void PuVetoAlg::fillPUmap( int wordIt, int word_Tot, unsigned int* data_Ptr, int step, unsigned int PU_hitmap[4][16] ){
// id 1 is sensor 128
// id 0 is sensor 129
// id 3 is sensor 130
// id 2 is sensor 131
  while ( wordIt < word_Tot )
    {
      unsigned int* wordPtr = data_Ptr + wordIt;
      unsigned int* wordPtr2 = wordPtr + step;  
      switch(wordIt){
	case 2:
	 (PU_hitmap[0][2]) = *wordPtr;
	 (PU_hitmap[0][1]) =  *wordPtr2;
	 break;
	case 3:
	 (PU_hitmap[0][0]) =  *wordPtr;
	 (PU_hitmap[0][3]) =  *wordPtr2;
	 break;
	case 4:
	 (PU_hitmap[1][13]) =  *wordPtr;
	 (PU_hitmap[1][14]) =  *wordPtr2;
	 break;
	case 5:
	 (PU_hitmap[1][15]) =  *wordPtr;
	 (PU_hitmap[1][12]) =  *wordPtr2;
	 break;
	case 6:
	 (PU_hitmap[2][13]) =  *wordPtr;
	 (PU_hitmap[2][14]) =  *wordPtr2;
	 break;
	case 7:
	 (PU_hitmap[2][15]) =  *wordPtr;
	 (PU_hitmap[2][12]) =  *wordPtr2;
	 break;              
	case 8:
	 (PU_hitmap[3][2]) =  *wordPtr;
	 (PU_hitmap[3][1]) =  *wordPtr2;
	 break;     
	case 9:
	 (PU_hitmap[3][0]) =  *wordPtr;
	 (PU_hitmap[3][3]) =  *wordPtr2;  
	 break;     
	case 10:
	 (PU_hitmap[0][5]) =  *wordPtr;
	 (PU_hitmap[0][6]) =  *wordPtr2;
	 break;     
	case 11:
	 (PU_hitmap[0][7]) =  *wordPtr;
	 (PU_hitmap[0][4]) =  *wordPtr2;
	 break;     
	case 12:
	 (PU_hitmap[1][10]) =  *wordPtr;
	 (PU_hitmap[1][9])  =  *wordPtr2;
	 break;     
	case 13:
	 (PU_hitmap[1][8])  =  *wordPtr;
	 (PU_hitmap[1][11]) =  *wordPtr2;
	 break;             
	case 14:
	 (PU_hitmap[2][10]) =  *wordPtr;
	 (PU_hitmap[2][9])  =  *wordPtr2;
	 break;     
	case 15:
	 (PU_hitmap[2][8])   =  *wordPtr;
	 (PU_hitmap[2][11])  =  *wordPtr2;
	 break;     
	case 16:
	 (PU_hitmap[3][5]) =  *wordPtr;
	 (PU_hitmap[3][6]) =  *wordPtr2;
	 break;     
	case 17:
	 (PU_hitmap[3][7]) =  *wordPtr;
	 (PU_hitmap[3][4]) =  *wordPtr2;
	 break;              
	case 18:
	 (PU_hitmap[0][10]) =  *wordPtr ;
	 (PU_hitmap[0][9])  =  *wordPtr2;
	 break;     
	case 19:
	 (PU_hitmap[0][8]) =  *wordPtr;
	 (PU_hitmap[0][11]) =  *wordPtr2;
	 break;     
	case 20:
	 (PU_hitmap[1][5]) =  *wordPtr;
	 (PU_hitmap[1][6]) =  *wordPtr2;
	 break;     
	case 21:
	 (PU_hitmap[1][7]) =  *wordPtr;
	 (PU_hitmap[1][4]) =  *wordPtr2;
	 break;             
	case 22:
	 (PU_hitmap[2][5]) =  *wordPtr;
	 (PU_hitmap[2][6]) =  *wordPtr2;
	 break;     
	case 23:
	 (PU_hitmap[2][7]) =  *wordPtr;
	 (PU_hitmap[2][4]) =  *wordPtr2;
	 break;     
	case 24:
	 (PU_hitmap[3][10]) =  *wordPtr;
	 (PU_hitmap[3][9]) =  *wordPtr2;
	 break;     
	case 25:
	 (PU_hitmap[3][8]) =  *wordPtr;
	 (PU_hitmap[3][11]) =  *wordPtr2;
	 break;              
	case 26:
	 (PU_hitmap[0][13]) =  *wordPtr;
	 (PU_hitmap[0][14]) =  *wordPtr2;
	 break;     
	case 27:
	 (PU_hitmap[0][15]) =  *wordPtr;
	 (PU_hitmap[0][12]) =  *wordPtr2;
	 break;     
	case 28:
	 (PU_hitmap[1][2]) =  *wordPtr;
	 (PU_hitmap[1][1]) =  *wordPtr2;
	 break;     
	case 29:
	 (PU_hitmap[1][0]) =  *wordPtr;
	 (PU_hitmap[1][3]) =  *wordPtr2;
	 break;             
	case 30:
	 (PU_hitmap[2][2]) =  *wordPtr;
	 (PU_hitmap[2][1]) =  *wordPtr2;
	 break;     
	case 31:
	 (PU_hitmap[2][0]) =  *wordPtr;
	 (PU_hitmap[2][3]) =  *wordPtr2;
	 break;     
	case 32:
	 (PU_hitmap[3][13]) =  *wordPtr;
	 (PU_hitmap[3][14]) =  *wordPtr2;
	 break;     
	case 33:
	 (PU_hitmap[3][15]) =  *wordPtr;
	 (PU_hitmap[3][12]) =  *wordPtr2;
	 break;  
	default:
	 warning() << "BUG: This warning should never be printed" << endmsg;
      } // switch case
      wordIt++;
    } // while "beetle map"  
}
//============================================================================

// binary function with strings

std::string PuVetoAlg::binary( unsigned int number )
{
  std::string result;
  while(number)
  {
    result.insert(result.begin(), number % 2 + '0');
    number /= 2;
  }
  return result;
}

//============================================================================
