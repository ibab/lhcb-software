// $Id: PuVetoAlg.cpp,v 1.12 2004-03-25 08:39:48 cattanem Exp $
// Include files 
#include <math.h>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"

// from DAQEvent
#include "Event/RawBuffer.h"
// from VeloEvent
#include "Event/MCVeloFE.h"
// from L0Event
#include "Event/L0PuVeto.h"
// local
#include "PuVetoAlg.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PuVetoAlg
//
// 11/01/2002 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PuVetoAlg>          s_factory ;
const        IAlgFactory& PuVetoAlgFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PuVetoAlg::PuVetoAlg( const std::string& name,
                      ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator )
  , m_inputContainer      ( MCVeloFELocation::PuVeto  )
  , m_outputContainer     ( L0PuVetoLocation::Default )
  , m_threshold           ( 7000.     ) //electrons
  , m_lowThreshold        (    2.     )
  , m_highThreshold       (    2.     )
  , m_highPosition        (    0. *mm )
  , m_secondPosition      ( -500. *mm )
{
  declareProperty( "InputContainer"     , m_inputContainer  );
  declareProperty( "OutputContainer"    , m_outputContainer );
  declareProperty( "SignalThreshold"    , m_threshold       );
  declareProperty( "LowThreshold"       , m_lowThreshold    );
  declareProperty( "HighThreshold"      , m_highThreshold   );
  declareProperty( "HighPosition"       , m_highPosition    );
  declareProperty( "SecondPosition"     , m_secondPosition  );
  declareProperty( "BinningScenario"    , m_binningScenario = 1);
  declareProperty( "HistoBins"       , m_histoBins = 128);
  declareProperty( "Binning2Constant"       , m_bin2Constant = 6.9577);
  declareProperty( "Binning2Slope"       , m_bin2Slope = -0.00763);
  declareProperty( "MaskingWindow"       , m_maskingWindow = 3.75);
}

//=============================================================================
// Destructor
//=============================================================================
PuVetoAlg::~PuVetoAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PuVetoAlg::initialize() {

  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::DEBUG << "==> Initialise" << endreq;

  SmartDataPtr<DeVelo> velo( detSvc(), "/dd/Structure/LHCb/Velo" );
  if ( 0 == velo ) {
    logmsg << MSG::ERROR << "Unable to retrieve Velo detector element." 
           << endreq;
    return StatusCode::FAILURE;
  }
  m_velo = velo;

  int binctr;
  double lowb;
  double step;
  m_totBin = m_histoBins;
  switch (m_binningScenario) {
  case 0 : // "no binning" - equal bins
    step = 300.0/m_histoBins;
    lowb = -150.0;
    for (binctr = 0;binctr <= m_histoBins;binctr++) {
      m_lowBound.push_back(lowb * mm);
      m_step.push_back(step);
      lowb += step;
    }
    break;
  case 1:  // Nikolai's standard binning (default)
  default:
    m_totBin = 128;
    step = 1;
    lowb=-150.0;
    for (binctr = 0;binctr < 50;binctr++) { // bins 0-49 1mm wide
      m_lowBound.push_back(lowb * mm);
      m_step.push_back(step);
      lowb += step;
    }
    step = 2;
    for (binctr = 50; binctr < 75;binctr++) { //bins 50-74 2mm wide
      m_lowBound.push_back(lowb * mm);
      m_step.push_back(step);
      lowb += step;
    }
    step = 3;
    for (binctr = 75; binctr < 105;binctr++) { //bins 75-104 3mm wide
      m_lowBound.push_back(lowb * mm);
      m_step.push_back(step);
      lowb += step;
    }
    step = 5;
    for (binctr = 105; binctr < 128;binctr++) { //bins 105-127 5mm wide
      m_lowBound.push_back(lowb * mm);
      m_step.push_back(step);
      lowb += step;
    }
    break;
  case 2: //exponential binning
    double a = m_bin2Constant;
    double b = m_bin2Slope;
    double c = exp(a)*(exp(b*150.0)-exp(b*-150.0))/b;
    lowb = -150.0;
    c /= (double) m_histoBins;

    while (lowb < 150.0) {
      m_lowBound.push_back(lowb * mm);
      step = log(c*b/exp(a+b*lowb)+1)/b;
      m_step.push_back(step);
      lowb += step;
    }
    break;
  }

  std::vector<double>::const_iterator itrb;
  std::vector<double>::const_iterator itrs = m_step.begin();
  for (itrb = m_lowBound.begin();itrb != m_lowBound.end();itrb++,itrs++) {
    m_binCenter.push_back ((*itrb) + 0.5 * (*itrs));
    m_hist.push_back(0);
  }

  for ( int ks=0 ; m_velo->nbPuSensor() > ks ; ks++ ) {
    VetoInput a( m_velo->zSensor(ks+100) );
    m_input.push_back( a );
  }


  logmsg << MSG::DEBUG << "Strip threshold: " << m_threshold << endreq;
  logmsg << MSG::DEBUG << "Binning scenario: " << m_binningScenario << endreq;
  logmsg << MSG::DEBUG << "Peak low threshold: " << m_lowThreshold << endreq;
  logmsg << MSG::DEBUG << "Peak high threshold: " << m_highThreshold << endreq;
  logmsg << MSG::DEBUG << "Peak high thr. position: " << m_highPosition
         << endreq;
  logmsg << MSG::DEBUG << "Peak position cut: " << m_secondPosition << endreq;
  
  
  return StatusCode::SUCCESS;
}

  
//=============================================================================
// Main execution
//=============================================================================
StatusCode PuVetoAlg::execute() {

  MsgStream  logmsg( msgSvc(), name() );
  logmsg << MSG::DEBUG << "==> Execute" << endreq;



  int ks;
  for ( ks=0 ; m_velo->nbPuSensor() > ks ; ks++ ) {
    m_input[ks].strips()->clear();
  }
  //*** get the input data

  SmartDataPtr< MCVeloFEs > fes ( eventSvc() , m_inputContainer );
  if( 0 == fes ) {
    logmsg << MSG::ERROR << "Unable to retrieve input data container="
        << m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }

  //========================================================================
  // Apply a threshold on each strip, OR them by 4 and store the central
  // strip coordinate, to be transformed in R later.
  //========================================================================


  SmartDataPtr<RawBuffer> rawBuffer( eventSvc(), RawBufferLocation::Default );
  if ( 0 == rawBuffer ) {
    logmsg << MSG::ERROR
        << "Unable to retrieve Raw buffer from " <<RawBufferLocation::Default
        << endreq;
    return StatusCode::FAILURE;
   }
  std::vector<unsigned short int> rawpudata;
  std::vector<raw_int> rawdatavec;
  

  for ( MCVeloFEs::const_iterator itFe = fes->begin(); 
        fes->end() != itFe ; itFe++  ) {
    if ( m_threshold < (*itFe)->charge() ) {
      unsigned int sensor = (*itFe)->sensor();
      if ( 100 > sensor || 103 < sensor ) {
        logmsg << MSG::INFO << "Unexpected sensor " << sensor 
            << " in " << m_inputContainer << endreq;
        continue;
      }
      sensor -= 100;
      int fired           = 4 * ( (*itFe)->strip()/4 ) + 2;

      

      unsigned short int rawhit = rawEncode(sensor,(*itFe)->strip());
      rawpudata.push_back(rawhit);
      
 
      logmsg << MSG::VERBOSE << sensor << " " << (*itFe)->strip() << " " 
             << rawhit << endreq;

      std::vector<int>* strips = m_input[sensor].strips();
      bool toAdd = true;

      logmsg << MSG::VERBOSE << "PU Sensor " << sensor << " strip " << fired;
      
      for ( std::vector<int>::const_iterator itS = strips->begin();
            strips->end() != itS; itS++) {
        if ( (*itS) == fired ) {
          toAdd = false;
          logmsg << " exists.";
        }
      }
      if ( toAdd ) {
        strips->push_back( fired );
        logmsg << " added.";
      }
      logmsg << endreq;
    }
  }

  raw_int header = 0;
  rawVec(&rawpudata,&rawdatavec);
  rawBuffer->addBank(header,RawBuffer::L0PU,rawdatavec);
  

  fillHisto(  );

  // We have filled the 'histogram'. Search for maximum.
  double height1, sum1, pos1;
  double width;
  double height2, sum2, pos2;
  double integral;
  
  pos1 = peakValue( height1, sum1, width);
  
  logmsg << MSG::DEBUG << " Peak1 : Max " << height1 << " at z= " << pos1 
      << " integral " << sum1 << endreq;

  // If a peak was found, mask the contributing hits, fill again and find the
  // second peak. It is searched always, to have initialized values for the
  // result.

  if ( 0 < height1 ) {
    double zTol = m_maskingWindow * width;
  
    logmsg << MSG::DEBUG << " Mask around z = " << pos1 
      << " window " << zTol << endreq;
    

    maskHits( pos1, zTol );
    fillHisto( );
  }

  pos2 = peakValue( height2, sum2, width );
      
  logmsg << MSG::DEBUG << " 2nd Max " << height2 << " at z= " << pos2
      << " integral " << sum2 << endreq;
  integral = fullIntegral();
  
  // Now take the decision

  int decision;
  if ( ( m_lowThreshold > sum2 ) ||
       ( (m_highThreshold > sum2) && (m_highPosition > pos2) ) ||
       ( m_secondPosition > pos2 ) ) {
    decision = 0;
  } else {
    decision = 1;    // Multiple interaction
  }


  L0PuVeto* pileUp = new L0PuVeto();
  pileUp->setDecision( decision );
  pileUp->setHeightPeak1( height1 );
  pileUp->setSumPeak1( sum1 );
  pileUp->setZPosPeak1( pos1 );
  pileUp->setHeightPeak2( height2 );
  pileUp->setSumPeak2( sum2 );
  pileUp->setZPosPeak2( pos2 );
  //pileUp->setSTot( integral );
  int totMult = 0;
  for ( ks=0 ; m_velo->nbPuSensor() > ks ; ks++ ) {
    totMult += m_input[ks].strips()->size();
  }
  pileUp->setSTot( totMult );

  logmsg << MSG::DEBUG << "== Decision " << decision 
      << " Peak1 z,h,s " << pos1 << " " << height1 << " " << sum1
      << " Peak2 z,h,s " << pos2 << " " << height2 << " " << sum2
      << " Off peaks " << integral << " TotMult " << totMult
      << endreq;
  
  StatusCode sc = eventSvc()->registerObject( m_outputContainer, pileUp );
  if ( sc.isFailure() ) {
    delete pileUp;
    logmsg << MSG::ERROR
        << "Unable to register output to " << m_outputContainer
        << endreq;
    return StatusCode::FAILURE;
  }
  
  for ( ks=0 ; m_velo->nbPuSensor() > ks ; ks++ ) {
    m_input[ks].strips()->clear();
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PuVetoAlg::finalize() {

  MsgStream logmsg(msgSvc(), name());
  logmsg << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  Fill the histogram
//=========================================================================
void PuVetoAlg::fillHisto ( ) {
  MsgStream  logmsg( msgSvc(), name() );
  // clear histo
  for ( unsigned int j=0 ; m_hist.size() > j ; j++ ) {
    m_hist[j] = 0;
  }

  double zA, zB, z;
  double rA, rB;
  int    zoneA, zoneB;
  int    bin;

  std::vector<VetoInput>::iterator itSens = m_input.begin();
  VetoInput* sensA;
  VetoInput* sensB;
  std::vector<int>::const_iterator dA;
  std::vector<int>::const_iterator dB;
  
  for ( unsigned int i1 = 0 ; 2 > i1 ; i1++, itSens++ ) {
    sensA = &(*itSens);
    sensB = &(*(itSens+2));

    zA = sensA->zSensor();
    zB = sensB->zSensor();

    std::vector<int>* digsA = sensA->strips();
    std::vector<int>* digsB = sensB->strips() ;

    logmsg << MSG::VERBOSE << "Loop on Sensor " << i1 
        << " z = " << zA << " " << zB
        << " Mult " << digsA->size() << " and " << digsB->size() << endreq;
    for ( dA = digsA->begin() ;  digsA->end() != dA ; dA++ ) {
      if ( 10000 < (*dA) ) { continue; }
      double stripA = (double) (*dA) ;
      rA = m_velo->rOfStrip( stripA, zoneA );
      
      for ( dB = digsB->begin();  digsB->end() != dB ; dB++ ) {
        if ( 10000 < (*dB) ) { continue; }
        double stripB = (double) (*dB);
        rB = m_velo->rOfStrip( stripB, zoneB );

        // Basic Phi matching... Corresponding zones in the sensor.

        if ( !( zoneA == zoneB ) ) {  continue; }

        if ( rB < rA ) {
          z = ( zB*rA - zA*rB) / ( rA - rB );
          bin = zBin( z );
          if ( 0 <= bin ) { 
            m_hist[bin]++;

            logmsg << MSG::VERBOSE << "  A: " << rA << " B: " << rB 
                << " z " << z << " bin " << bin << endreq;
          }
        }
      }
    }
  }
}

//=========================================================================
//  Mask the hits contributing to the vertex
//=========================================================================
void PuVetoAlg::maskHits ( double zVertex,
                           double zTol   ) {
  MsgStream  logmsg( msgSvc(), name() );
  double stripA, stripB;
  double zA, zB, z;
  double rA, rB;
  int    zoneA, zoneB;

  std::vector<VetoInput>::iterator itSens = m_input.begin();
  VetoInput* sensA;
  VetoInput* sensB;
  std::vector<int>::iterator dA; 
  std::vector<int>::iterator dB; 
  
  for ( unsigned int i1 = 0 ; 2 > i1 ; i1++, itSens++ ) {
    sensA = &(*itSens);
    sensB = &(*(itSens+2));
    zA = sensA->zSensor();
    zB = sensB->zSensor();

    std::vector<int>* digsA = sensA->strips();
    std::vector<int>* digsB = sensB->strips() ;

    for ( dA = digsA->begin() ;  digsA->end() != dA ; dA++ ) {
      if ( 10000 < (*dA) ) { 
        stripA = (double) ((*dA)-10000 );
      } else {
        stripA = (double) (*dA) ;
      }
      
      rA = m_velo->rOfStrip( stripA, zoneA );
      
      for ( dB = digsB->begin() ; digsB->end() != dB ; dB++ ) {
        if ( 10000 < (*dB) ) { 
          stripB = (double) ((*dB)-10000 );
        } else {
          stripB = (double) (*dB);
        }
        rB = m_velo->rOfStrip( stripB, zoneB );

        // Basic Phi matching... Corresponding zones in the sensor.
        if ( !( zoneA == zoneB ) ) {  continue; }

        if ( rB < rA ) {
          z = ( zB*rA - zA*rB) / ( rA - rB );
          if ( zTol > fabs( zVertex-z ) ) {

            logmsg << MSG::VERBOSE << "  A: " << rA << " B: " << rB 
                << " z " << z << " MASK " << endreq;

            if ( 10000 > (*dA) ) {
              (*dA) += 10000;
            }
            if ( 10000 > (*dB) ) {
              (*dB) += 10000;
            }
          }
        }
      }
    }
  }
}

//=========================================================================
//  
//=========================================================================
double PuVetoAlg::peakValue ( double& height, double& sum, double& width) {
  double pos = -999.;
  height = 0.;
  sum    = 0.;
  width  = 0.;
  double sumt = 0.;
  int bin = -1;

  for ( int jj=0 ; jj < m_totBin ; jj++ ) {
    if ( height < m_hist[jj] ) {
      height = m_hist[jj];
      bin    = jj;
      sumt = height;
      if (jj>0) sumt += m_hist[jj-1];
      if (jj<m_totBin-1) sumt += m_hist[jj+1];
    }
    if ((height != 0.) && (height == m_hist[jj])) {
      sum = height;
      if (jj>0) sum += m_hist[jj-1];
      if (jj<m_totBin-1) sum += m_hist[jj+1];
      if (sum > sumt) bin = jj;
    }
  }

  if ( (0 < bin) && (m_totBin-1 > bin) ) {
    sum   = m_hist[bin-1] + m_hist[bin] + m_hist[bin+1];
    pos   = m_binCenter[bin-1] * m_hist[bin-1] +
            m_binCenter[bin  ] * m_hist[bin  ] +
            m_binCenter[bin+1] * m_hist[bin+1];
    pos   = pos / sum;
    m_hist[bin-1] = 0.;
    m_hist[bin  ] = 0.;
    m_hist[bin+1] = 0.;
    width = .5 * (m_binCenter[bin+1] - m_binCenter[bin-1]);
  }
  return pos;
}
//=============================================================================

unsigned short int PuVetoAlg::rawEncode(int sensor, int strip) 
{
  unsigned short int temp;
  
  temp = strip;
  temp+=sensor << 14;
  return temp;
}

void PuVetoAlg::rawVec (std::vector<unsigned short int> *vecin,
                        std::vector<raw_int> *vecout) 
{
  std::vector<unsigned short int>::iterator itraw = vecin->begin();
  unsigned short int temp1,temp2;
  raw_int tempraw;
  
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

    
    
      
