// $Id: CaloProduceRaw.cpp,v 1.2 2003-04-17 07:55:21 ocallot Exp $
// Include files 
 // CLHEP
#include "CLHEP/Units/SystemOfUnits.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"

// Event/CaloEvent
#include "Event/CaloDigit.h"
// local
#include "CaloProduceRaw.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloProduceRaw
//
// 2003-03-21 : Olivier Callot
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<CaloProduceRaw>          s_factory ;
const        IAlgFactory& CaloProduceRawFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloProduceRaw::CaloProduceRaw( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) {

}

//=============================================================================
// Destructor
//=============================================================================
CaloProduceRaw::~CaloProduceRaw() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode CaloProduceRaw::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Initialize" << endreq;

  m_nEvents = 0.;
  for ( unsigned int kk = 0 ; 22 > kk ; kk++ ) {
    std::vector<int> dum;
    m_buffers.push_back( dum );
    m_buffers[kk].reserve(1000);
    m_size.push_back( 0. );
    m_startOfBank.push_back( 0 );
  }

  m_caloEnergyID    = 1234;
  m_caloTriggerID   = 5678;
  m_prsSpdTriggerID = 2468;
  m_magicWord       = 0xdeadbabe;

  return StatusCode::SUCCESS; 
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode CaloProduceRaw::execute() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Execute" << endreq;

  std::vector< std::vector<int> >::iterator itBuf;
  unsigned int sourceID;

  for ( itBuf = m_buffers.begin(); m_buffers.end() != itBuf; itBuf++ ) {
    (*itBuf).clear();
    sourceID =  itBuf - m_buffers.begin();
    createBankHeader( m_caloEnergyID, sourceID );
  }

  buildEnergyBuffer( CaloDigitLocation::Prs );
  buildEnergyBuffer( CaloDigitLocation::Ecal );
  buildEnergyBuffer( CaloDigitLocation::Hcal );
  
  for ( sourceID = 0; m_buffers.size() > sourceID; sourceID++ ) {
    adjustBankHeader( sourceID );
  }

  for ( sourceID = 0; m_buffers.size() > sourceID; sourceID++ ) {
    if ( 8 > sourceID ) {
      createBankHeader( m_prsSpdTriggerID, sourceID );
    } else {
      createBankHeader( m_caloTriggerID, sourceID );
    }
  }

  //buildCaloTriggerBuffer( L0CaloAdcLocation::Ecal );
  //buildCaloTriggerBuffer( L0CaloAdcLocation::Hcal );
  
  //buildPrsSpdTriggerBuffer( L0PrsSpdHitLocation::Prs );
  //buildPrsSpdTriggerBuffer( L0PrsSpdHitLocation::Spd );

  for ( sourceID = 0; m_buffers.size() > sourceID; sourceID++ ) {
    adjustBankHeader( sourceID );
  }

  m_nEvents += 1.;
  for ( unsigned int kk = 0; m_buffers.size() > kk; kk++ ){
    m_size[kk] += m_buffers[kk].size();
  }
    
  if ( MSG::DEBUG >= msg.level() ) {
    for ( itBuf = m_buffers.begin(); m_buffers.end() != itBuf; itBuf++ ) {
      msg << MSG::DEBUG 
          << format( "Buffer %2d size %8d words", 
                     itBuf- m_buffers.begin(),
                     (*itBuf).size() )
          <<endreq;
      if ( MSG::VERBOSE >= msg.level() ) {
        unsigned int kk = 0;
        while ( (*itBuf).size() > kk ) {
          unsigned int endBank = kk + (*itBuf)[kk];
          int bankClass = (*itBuf)[kk+2] & 65535;
          msg << MSG::VERBOSE 
              << format( "BANK Size %4d Magic %8x class%6d #%3d", 
                         (*itBuf)[kk],
                         (*itBuf)[kk+1],
                         (*itBuf)[kk+2] & 65535,
                         ( (*itBuf)[kk+2]>>16 ) & 65535 )
              << endreq;
          kk += 3;
          if ( m_caloEnergyID == bankClass ) {
            while ( endBank > kk ) {
              msg << MSG::VERBOSE << format( "%4d : %8x ", kk, (*itBuf)[kk] );
              int det  = ((*itBuf)[kk] >> 30 ) & 3;
              int area = ((*itBuf)[kk] >> 28 ) & 3;
              int row  = ((*itBuf)[kk] >> 22 ) & 63;
              int col  = ((*itBuf)[kk] >> 16 ) & 63;
              msg << CaloCellID( det, area, row, col ) ;
              int clusLength = (*itBuf)[kk++] & 65535 ;
              msg << format( " (%2d)", clusLength );
              for ( int j = 0 ;clusLength > j  ; ++j ) {
                msg << format( " %9.2f", .001 * (*itBuf)[kk++]) ;
            }
              msg << endreq;
            }
          } else {
            kk = endBank;
          }
        }
      }
    }
  }

  return StatusCode::SUCCESS; 
};

//=========================================================================
//  Build the Clustered Energy buffer
//=========================================================================
void CaloProduceRaw::buildEnergyBuffer ( std::string location ) {

  SmartDataPtr<CaloDigits> digits ( eventSvc() , location );
  if( 0 == digits ) { 
    MsgStream msg(msgSvc(), name());
    msg << MSG::ERROR  
        << "Unable to retrieve input data container=" 
        << location << endreq; 
    return ;
  }

  int cellIndex   ;
  int nextIndex   = -1;
  int headIndex   = -1;
  int clusLength  ;
  int cellContent ;
  int det;
  int bufIndx, prevIndx = -1;
      
  CaloDigits::const_iterator dig;
  for ( dig = digits->begin() ; digits->end() != dig ; ++dig ) {
    det  = (*dig)->cellID().calo();
    cellIndex = (*dig)->cellID().index() + (det<<14);
    cellContent = int ( (*dig)->e() / keV );
    if ( 1 == det && 0 >= cellContent ) continue; //== zSup Prs completely

    bufIndx = bufferNumber( (*dig)->cellID() );
    
    if ( bufIndx != prevIndx ) { nextIndex = -1; }
    
    if ( cellIndex != nextIndex ) {
      if ( 0 <= headIndex ) {
        m_buffers[prevIndx][headIndex] += clusLength;
      }
      clusLength = 0;
      headIndex  = m_buffers[bufIndx].size();
      m_buffers[bufIndx].push_back( cellIndex << 16 );
    }
    m_buffers[bufIndx].push_back( cellContent );
    nextIndex = cellIndex+1;
    prevIndx = bufIndx;
    clusLength++;
  }
  if ( 0 <= headIndex ) {
    m_buffers[bufIndx][headIndex] += clusLength;
  }
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode CaloProduceRaw::finalize() {
  
  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << " >>> Finalize" << endreq;
  if ( 0 < m_nEvents ) {
    double totSize = 0.;
    unsigned int kk;
    for ( kk = 0; m_size.size() > kk; kk++ ){
      m_size[kk] /= m_nEvents;
      totSize += m_size[kk];
    }
    msg << MSG::INFO 
        << format( "Average Calo event size: %8.0f words on %7.0f events", 
                   totSize, m_nEvents )
        << endreq;
    msg << format( "  Prs  %5.0f%5.0f%5.0f%10.0f | %5.0f%5.0f%5.0f%10.0f",
                   m_size[0], m_size[1], m_size[2], m_size[3],
                   m_size[4], m_size[5], m_size[6], m_size[7] )
        << endreq;
    msg << "  ECAL " 
        << format( "%5.0f%5.0f%5.0f%5.0f%5.0f | %5.0f%5.0f%5.0f%5.0f%5.0f",
                   m_size[8],  m_size[9],  m_size[10], m_size[11], m_size[12],
                   m_size[13], m_size[14], m_size[15], m_size[16], m_size[17] )
        << endreq;
    msg << format( "  HCAL %5.0f     %5.0f           | %5.0f     %5.0f",
                   m_size[18], m_size[19], m_size[20], m_size[21] )
        << endreq;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
