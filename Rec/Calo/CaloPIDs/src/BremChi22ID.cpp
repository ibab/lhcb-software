// ============================================================================
// Include files 
// ============================================================================
// Local 
// ============================================================================
#include "CaloChi22ID.h"
#include "ToVector.h"
#include "CaloTrackAlg.h"
// ============================================================================
/** The preconfigured instance of class CaloChi22ID 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 */
// ============================================================================
class BremChi22ID: public CaloChi22ID 
{
  /// friend factory for instantiation 
  friend class AlgFactory<BremChi22ID>;
protected:
  /// Standard protected constructor
  BremChi22ID 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    :  CaloChi22ID ( name , pSvc ) 
  {
    using namespace LHCb::CaloAlgUtils;
    _setProperty( "Input"  , CaloIdLocation( "BremMatch", context() ) ) ;
    _setProperty( "Output" , CaloIdLocation( "BremChi2" , context() ) ) ;
    // @todo it must be in agrement with "Threshold" for BremMatchAlg 
    _setProperty( "CutOff" , "10000"  ) ; //  
    // track types:
    _setProperty( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
  } ;
  /// protected vertual destructor
  virtual ~BremChi22ID(){}; 
private:
  BremChi22ID () ;
  BremChi22ID           ( const BremChi22ID& ) ;
  BremChi22ID& operator=( const BremChi22ID& ) ;
} ;
// ============================================================================
//// declare algorithm factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY ( BremChi22ID )
// ============================================================================

// ============================================================================
// The END 
// ============================================================================







