// $Id: PuVetoAlg.cpp,v 1.2 2002-02-01 15:05:45 ocallot Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ObjectVector.h"

// from L1Event
#include "L1Event/L1Buffer.h"

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
  , m_inputContainer      ( "/Event/Raw/PuvDigit" )
  , m_outputContainer     ( L0PuVetoLocation::Default )
  , m_lowThreshold        (    2.     )
  , m_highThreshold       (    2.     )
  , m_highPosition        (    0. *mm )
  , m_secondPosition      ( -500. *mm )
{
  declareProperty( "InputContainer"     , m_inputContainer  );
  declareProperty( "OutputContainer"    , m_outputContainer );
  declareProperty( "LowThreshold"       , m_lowThreshold    );
  declareProperty( "HighThreshold"      , m_highThreshold   );
  declareProperty( "HighPosition"       , m_highPosition    );
  declareProperty( "SecondPosition"     , m_secondPosition  );
}

//=============================================================================
// Destructor
//=============================================================================
PuVetoAlg::~PuVetoAlg() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PuVetoAlg::initialize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Initialise" << endreq;

  SmartDataPtr<DeVelo> velo( detDataService(), "/dd/Structure/LHCb/Velo" );
  if ( 0 == velo ) {
    log << MSG::ERROR << "Unable to retrieve Velo detector element." << endreq;
    return StatusCode::FAILURE;
  }
  m_velo = velo;

  m_lowBound.push_back( -150. * mm );
  m_lowBound.push_back( -100. * mm );
  m_lowBound.push_back(  -50. * mm );
  m_lowBound.push_back(   40. * mm );
  m_lowBound.push_back(  150. * mm );

  m_step.push_back( 1. * mm );
  m_step.push_back( 2. * mm );
  m_step.push_back( 3. * mm );
  m_step.push_back( 5. * mm );

  m_nBin.push_back( 0 );
  m_nBin.push_back( zBin( m_lowBound[1]-0.001)+1 );
  m_nBin.push_back( zBin( m_lowBound[2]-0.001)+1 );
  m_nBin.push_back( zBin( m_lowBound[3]-0.001)+1 );
  m_nBin.push_back( zBin( m_lowBound[4]-0.001)+1 );

  m_totBin = m_nBin[ m_nBin.size()-1 ];

  double binCenter;
  
  for ( unsigned int k=0 ; m_step.size() > k ; k++ ) {
    binCenter = m_lowBound[k] + .5 * m_step[k];
    log << MSG::DEBUG 
        << " zone " << k 
        << " low " << m_lowBound[k]
        << " high " << m_lowBound[k+1]
        << " step " << m_step[k] 
        << " nBin " << m_nBin[k] 
        << " binCenter(beg) " << binCenter
        << endreq;
    while ( binCenter < m_lowBound[k+1] ) {
      m_hist.push_back( 0 );
      m_binCenter.push_back( binCenter );
      binCenter += m_step[k];
    }
  }
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PuVetoAlg::execute() {

  MsgStream  log( msgSvc(), name() );
  log << MSG::DEBUG << "==> Execute" << endreq;

  //*** get the input data

  SmartDataPtr< ObjectVector<L1Buffer> > digs ( eventDataService() , 
                                                m_inputContainer );
  if( 0 == digs ) {
    log << MSG::ERROR
        << "Unable to retrieve input data container="
        << m_inputContainer << endreq;
    return StatusCode::FAILURE;
  }

  fillHisto( digs );

  // We have filled the 'histogram'. Search for maximum.
  double height1, sum1, pos1;
  double width;
  double height2, sum2, pos2;
  double integral;
  
  pos1 = peakValue( height1, sum1, width);
  
  log << MSG::DEBUG << " Peak1 : Max " << height1 << " at z= " << pos1 
      << " integral " << sum1 << endreq;

  // If a peak was found, mask the contributing hits, fill again and find the
  // second peak. It is searched always, to have initialized values for the
  // result.

  if ( 0 < height1 ) {
    double zTol = 3.75 * width;
  
    log << MSG::DEBUG << " Mask around z = " << pos1 
      << " window " << zTol << endreq;
    

    maskHits( digs, pos1, zTol );
    fillHisto( digs );
  }

  pos2 = peakValue( height2, sum2, width );
      
  log << MSG::DEBUG << " 2nd Max " << height2 << " at z= " << pos2
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
  log << MSG::DEBUG << "== Decision " << decision 
      << " Peak1 z,h,s " << pos1 << " " << height1 << " " << sum1
      << " Peak2 z,h,s " << pos2 << " " << height2 << " " << sum2
      << " Integral " << integral
      << endreq;

  L0PuVeto* pileUp = new L0PuVeto();
  pileUp->setDecision( decision );
  pileUp->setHeightPeak1( height1 );
  pileUp->setSumPeak1( sum1 );
  pileUp->setZPosPeak1( pos1 );
  pileUp->setHeightPeak2( height2 );
  pileUp->setSumPeak2( sum2 );
  pileUp->setZPosPeak2( pos2 );
  pileUp->setSTot( integral );
  
  StatusCode sc = eventSvc()->registerObject( m_outputContainer, pileUp );
  if ( sc.isFailure() ) {
    delete pileUp;
    log << MSG::ERROR
        << "Unable to register output to " << m_outputContainer
        << endreq;
    return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode PuVetoAlg::finalize() {

  MsgStream log(msgSvc(), name());
  log << MSG::DEBUG << "==> Finalize" << endreq;

  return StatusCode::SUCCESS;
}


//=========================================================================
//  Fill the histogram
//=========================================================================
void PuVetoAlg::fillHisto ( ObjectVector<L1Buffer>* digs ) {
  MsgStream  log( msgSvc(), name() );
  // clear histo
  for ( unsigned int j=0 ; m_hist.size() > j ; j++ ) {
    m_hist[j] = 0;
  }

  double zA, zB, z;
  double rA, rB;
  int    zoneA, zoneB;
  int    bin;

  ObjectVector<L1Buffer>::iterator itWaf = digs->begin();
  
  for ( unsigned int i1 = 0 ; 2 > i1 ; i1++, itWaf++ ) {
    std::vector<int>* digsA = (*itWaf)->digits();
    zA = m_velo->zPuWafer( (*itWaf)->waferNumber() );
    std::vector<int>* digsB = (*(itWaf+2))->digits();
    zB = m_velo->zPuWafer( (*(itWaf+2))->waferNumber() );

    log << MSG::VERBOSE << "Loop on Wafer " << i1 
        << " z = " << zA << " " << zB
        << " Mult " << digsA->size() << " and " << digsB->size() << endreq;
    for ( std::vector<int>::const_iterator dA = digsA->begin() ; 
          digsA->end() != dA ; dA++ ) {
      if ( 10000 < (*dA) ) { continue; }
      double stripA = (double) (*dA) ;
      rA = m_velo->rOfStrip( stripA, zoneA );
      
      for ( std::vector<int>::const_iterator dB = digsB->begin() ; 
          digsB->end() != dB ; dB++ ) {
        if ( 10000 < (*dB) ) { continue; }
        double stripB = (double) (*dB);
        rB = m_velo->rOfStrip( stripB, zoneB );

        // Basic Phi matching... Corresponding zones in the wafer.

        if ( 0 == zoneA ) {
          if ( 0 != zoneB ) { continue; }
        } else if ( 1 == zoneA ) {
          if ( 1 != zoneB ) { continue; }
        } else if ( 2 == zoneA ) {
          if ( 2 < zoneB ) { continue; }
        } else if ( 3 == zoneA ) {
          if ( 3 != zoneB ) { continue; }
        } else if ( 4 == zoneA ) {
          if ( 4 != zoneB ) { continue; }
        } else if ( 5 == zoneA ) {
          if ( 3 > zoneB ) { continue; }
        }

        if ( rB < rA ) {
          z = ( zB*rA - zA*rB) / ( rA - rB );
          bin = zBin( z );
          if ( 0 <= bin ) { 
            m_hist[bin]++;

            log << MSG::VERBOSE << "  A: " << rA << " B: " << rB 
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
void PuVetoAlg::maskHits ( ObjectVector<L1Buffer>* digs, 
                           double zVertex,
                           double zTol   ) {
  MsgStream  log( msgSvc(), name() );
  double stripA, stripB;
  double zA, zB, z;
  double rA, rB;
  int    zoneA, zoneB;

  ObjectVector<L1Buffer>::iterator itWaf = digs->begin();
  
  for ( unsigned int i1 = 0 ; 2 > i1 ; i1++, itWaf++ ) {
    std::vector<int>* digsA = (*itWaf)->digits();
    zA = m_velo->zPuWafer( (*itWaf)->waferNumber() );
    std::vector<int>* digsB = (*(itWaf+2))->digits();
    zB = m_velo->zPuWafer( (*(itWaf+2))->waferNumber() );

    for ( std::vector<int>::iterator dA = digsA->begin() ; 
          digsA->end() != dA ; dA++ ) {
      if ( 10000 < (*dA) ) { 
        stripA = (double) ((*dA)-10000 );
      } else {
        stripA = (double) (*dA) ;
      }
      
      rA = m_velo->rOfStrip( stripA, zoneA );
      
      for ( std::vector<int>::iterator dB = digsB->begin() ; 
          digsB->end() != dB ; dB++ ) {
        if ( 10000 < (*dB) ) { 
          stripB = (double) ((*dB)-10000 );
        } else {
          stripB = (double) (*dB);
        }
        rB = m_velo->rOfStrip( stripB, zoneB );

        // Basic Phi matching... Corresponding zones in the wafer.

        if ( 0 == zoneA ) {
          if ( 0 != zoneB ) { continue; }
        } else if ( 1 == zoneA ) {
          if ( 1 != zoneB ) { continue; }
        } else if ( 2 == zoneA ) {
          if ( 2 < zoneB ) { continue; }
        } else if ( 3 == zoneA ) {
          if ( 3 != zoneB ) { continue; }
        } else if ( 4 == zoneA ) {
          if ( 4 != zoneB ) { continue; }
        } else if ( 5 == zoneA ) {
          if ( 3 > zoneB ) { continue; }
        }

        if ( rB < rA ) {
          z = ( zB*rA - zA*rB) / ( rA - rB );
          if ( zTol > fabs( zVertex-z ) ) {

            log << MSG::VERBOSE << "  A: " << rA << " B: " << rB 
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
  int bin = -1;
  for ( int jj=0 ; m_totBin > jj ; jj++ ) {
    if ( height < m_hist[jj] ) {
      height = m_hist[jj];
      bin    = jj;
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
