// Include files 
// from Event 
#include "Event/CaloDigit.h"
// from CaloDet
#include "CaloDet/DeCalorimeter.h"
// local
#include "CaloMoniAlg.h"

using namespace LHCb; 
using namespace Gaudi::Units;

// ============================================================================
/** @file
 *
 *  Implementation file for class SpdMonitor
 *
 *  SPD Monitoring
 *
 *  @author Albert Puig apuignav@cern.ch
 *  @date   2007-15-07
 */

// ============================================================================
/** Standard constructor
 *  @param name algorithm name
 *  @param pSvc service locator
 */
// ============================================================================
class SpdMonitor : public CaloMoniAlg
{
  friend class AlgFactory<SpdMonitor>;
public:
  virtual StatusCode initialize();  
  virtual StatusCode execute();
  virtual StatusCode finalize();
  
protected:
  SpdMonitor ( const std::string& name, ISvcLocator* pSvc )
    : CaloMoniAlg ( name , pSvc )
    , digitsSpd(NULL)
    , m_nEvents(0)
    , m_detSpd(NULL)
 {
    m_split=true;  // Area splitting is the default !
  };
  virtual ~SpdMonitor() {};
  
private:   
  CaloDigits* digitsSpd;
  unsigned int m_nEvents;
  DeCalorimeter *m_detSpd;
  CaloVector<int> m_neighN; 
  
};    
DECLARE_ALGORITHM_FACTORY( SpdMonitor )
  
// ============================================================================


// ============================================================================
StatusCode SpdMonitor::initialize()
{
  StatusCode sc = CaloMoniAlg::initialize();
  if (sc.isFailure()) return sc;
  // Loads the detectors
  m_detSpd = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  // Histograms
  bookCalo2D( "1", "Spd occupancy", "Spd" );
  bookCalo2D( "2", "Spd neighbor occupancy", "Spd" );
  //hBook1( "3", "Spd occupancy", 0, 12000, 12000 );
  //hBook1( "4", "Spd neighbor occupancy" , 0, 12000, 12000 );

  // Initialize neighbor matrix
  for(unsigned int cellIt = 0; cellIt!=m_detSpd->numberOfCells(); cellIt++){
    const LHCb::CaloCellID cell = m_detSpd->cellIdByIndex(cellIt);
    const CaloNeighbors& neigh = m_detSpd->zsupNeighborCells(cell);    
    m_neighN.addEntry(neigh.size(), cell);      
  }  
  return StatusCode::SUCCESS;
}

// ============================================================================
StatusCode SpdMonitor::finalize()
{
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Finalize" << endmsg;
  info() << "Number of Events Analyzed : " << m_nEvents << endmsg;

  return CaloMoniAlg::finalize() ;
}
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode SpdMonitor::execute()
{   
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute " << endmsg;

  if( !exist<CaloDigits>( CaloDigitLocation::Spd ) ){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << "No Table container found at " << CaloDigitLocation::Spd << endmsg;
    return StatusCode::SUCCESS;
  }
  digitsSpd = get<CaloDigits>( CaloDigitLocation::Spd );  
  // Fill histos
  for(CaloDigits::iterator idig = digitsSpd->begin() ; digitsSpd->end() !=  idig ; idig++){
    if ( NULL == *idig ) continue;
    const CaloCellID cell = (*idig)->cellID();
    if ( doHisto("1") ) fillCalo2D( "1", cell, 1.0 );  
    // hFill1( cell, "3", cell.index(), 1.0 );
    const CaloNeighbors& neigh = m_detSpd->zsupNeighborCells(cell);    
    for (CaloNeighbors::const_iterator neighIt = neigh.begin(); neighIt != neigh.end(); neighIt++){
      CaloCellID neighCell = (*neighIt);
      float value = (float) 1./float(m_neighN[cell]);
      if ( doHisto("2") ) fillCalo2D( "2", neighCell , value );  
      //hFill1( neighCell, "4", cell.index(), value );
    }
  }
  m_nEvents++;
  return StatusCode::SUCCESS;
}
