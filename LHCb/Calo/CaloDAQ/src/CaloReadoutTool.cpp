// $Id: CaloReadoutTool.cpp,v 1.5 2006-11-23 13:38:32 cattanem Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 
#include "DetDesc/Condition.h"

// local
#include "CaloDAQ/CaloReadoutTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloReadoutTool
//
// 2005-08-25 : Olivier Callot
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( CaloReadoutTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloReadoutTool::CaloReadoutTool( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<CaloReadoutTool>(this);
  declareProperty( "DetectorName", m_detectorName );
  if ( "ToolSvc.PrsReadoutTool"  == name ) m_detectorName = "Prs";
  if ( "ToolSvc.EcalReadoutTool" == name ) m_detectorName = "Ecal";
  if ( "ToolSvc.HcalReadoutTool" == name ) m_detectorName = "Hcal";
  declareProperty( "CheckingCards", m_checkingCards = false );
}
//=============================================================================
// Destructor
//=============================================================================
CaloReadoutTool::~CaloReadoutTool() {}; 


//=========================================================================
//  
//=========================================================================
StatusCode CaloReadoutTool::initialize ( ) {
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  if( "Ecal" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  }
  else if( "Hcal" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  }
  else if( "Prs" == m_detectorName ) {
    m_calo = getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  }
  else {
    return( Error( "Invalid detector name: " + m_detectorName ) );
  }

  unsigned int caloID = CaloCellCode::CaloNumFromName( m_detectorName );
  bool hasFault = false;

  //== First get the FE card description
  Condition* cond = m_calo->condition( "CellsToCards" );
  if ( 0 == cond ) {
    return Error( "Condition 'CellsToCards' not found." );
  } 

  //== Cards
  if ( cond->exists( "cards" ) ) {
    LHCb::CaloCellID dummy( 0, 0, 0, 0 );
    std::vector<int> temp = cond->paramAsIntVect( "cards" );
    debug() << "The calorimeter has " << temp.size()/8 << " front end cards." << endreq;
    for ( unsigned int kk = 0; temp.size()/8 > kk  ; ++kk ) {
      int ll = 8*kk;
      CaloFECard  card( temp[ll], temp[ll+6], temp[ll+7] );
      for ( int row = temp[ll+3]; temp[ll+5] >= row; ++row ) {
        for ( int col = temp[ll+2]; temp[ll+4] >= col; ++col ) {
          LHCb::CaloCellID id( caloID, temp[ll+1], row, col );
          if ( m_calo->valid( id ) ) {
            card.addID( id );
          } else {
            card.addID( dummy );
          }
        }
      }
      m_cards.push_back( card );

      //== Sanity checks
      if ( m_checkingCards ) {
        info() << format( "card %3d A=%1d cf%3d rf%3d cl%3d rl%3d crate%3d slot%3d",
                          temp[ll  ],  temp[ll+1], temp[ll+2],  temp[ll+3],
                          temp[ll+4],  temp[ll+5], temp[ll+6],  temp[ll+7] )
               << endreq;
        int diffRow = temp[ll+5] - temp[ll+3];
        
        if ( ( temp[ll+4] - temp[ll+2] != 7 ) ||
             ( diffRow != 7 && diffRow != 3 ) ) {
          info() << "************ Wrong size for row/col ****" << endreq;
          hasFault = true;
        }
        if ( int(kk) != temp[ll] ) {
          info() << "*********** wrong card number, found " << temp[ll]
                 << " expect " << kk << endreq;
          hasFault = true;
        }
      }
    } 
  }
  if ( hasFault ) return StatusCode::FAILURE;

  //== Tell1 boards

  if ( cond->exists( "Tell1" ) ) {
    std::vector<int> temp = cond->paramAsIntVect( "Tell1" );
    std::vector<int>::iterator it = temp.begin();
    while( temp.end() > it ) {
      unsigned int num = (*it++);
      if ( num != m_tell1.size() ) {
        info() << "TELL1 number not in sequence: Found " << num 
               << " expect " << m_tell1.size() << endreq;
        hasFault = true;
      }
      int nb  = (*it++);
      CaloTell1 tell1( num );
      while ( 0 < nb-- ) {
        unsigned int feCard = *it++;
        if ( m_cards.size() < feCard ) {
          info() << "**** TELL1 " << num << " reads unknown card " << feCard << endreq;
          hasFault = true;
        }
        tell1.addFeCard( feCard );
      }
      m_tell1.push_back( tell1 );

      if ( m_checkingCards ) {
        std::vector<int> fes = tell1.feCards();
        info() << "  Added TELL1 card " << num 
               << " with " << fes.size() << " cards : ";
        for ( std::vector<int>::iterator itL = fes.begin(); fes.end() != itL; ++itL ) {
          info() << *itL << " ";
        }
        info() << endreq;
      }
    }    
  }

  if ( hasFault ) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;
  
}
//=============================================================================
