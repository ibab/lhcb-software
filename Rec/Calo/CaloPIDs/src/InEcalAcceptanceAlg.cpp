// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InEcalAcceptanceAlg InEcalAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InEcalAcceptanceAlg : public InCaloAcceptanceAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InEcalAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InEcalAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation("InEcal" , context() ) ) ;

    _setProperty ( "Tool"   , "InEcalAcceptance/InEcal"     ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long       ,
                     LHCb::Track::Downstream ,
                     LHCb::Track::Ttrack     ) ) ;
  } 
  /// virtual proected destructor 
  virtual ~InEcalAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InEcalAcceptanceAlg() ;
  InEcalAcceptanceAlg           ( const InEcalAcceptanceAlg& );
  InEcalAcceptanceAlg& operator=( const InEcalAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InEcalAcceptanceAlg )
// ============================================================================
// The END 
// ============================================================================
