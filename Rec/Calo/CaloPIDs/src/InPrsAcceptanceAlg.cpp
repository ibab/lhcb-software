// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InPrsAcceptanceAlg InPrsAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
class InPrsAcceptanceAlg : public InCaloAcceptanceAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InPrsAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InPrsAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation("InPrs" , context() ) ) ;

    //
    _setProperty ( "Tool"   , "InPrsAcceptance/InPrs" ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Long       ,
                     LHCb::Track::Downstream ,
                     LHCb::Track::Ttrack     ) ) ;
  } 
  /// virtual proected destructor 
  virtual ~InPrsAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InPrsAcceptanceAlg() ;
  InPrsAcceptanceAlg           ( const InPrsAcceptanceAlg& );
  InPrsAcceptanceAlg& operator=( const InPrsAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InPrsAcceptanceAlg )
// ============================================================================
// The END 
// ============================================================================
