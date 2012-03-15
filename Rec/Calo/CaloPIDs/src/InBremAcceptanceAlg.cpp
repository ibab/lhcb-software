// ============================================================================
// Local
// ============================================================================
#include "InCaloAcceptanceAlg.h"
// ============================================================================
/** @class InBremAcceptanceAlg InBremAcceptanceAlg.cpp
 *  the preconfigured instance of InCaloAcceptanceAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date   2006-06-17
 */
// ============================================================================
#include "GaudiKernel/System.h"
class InBremAcceptanceAlg : public InCaloAcceptanceAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<InBremAcceptanceAlg>;
  // ==========================================================================
protected:
  // ==========================================================================
  /// Standard protected constructor
  InBremAcceptanceAlg 
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : InCaloAcceptanceAlg ( name , pSvc ) 
  {

    using namespace LHCb::CaloAlgUtils;
    _setProperty ( "Output" , CaloIdLocation("InBrem" , context() ) ) ;
    
    _setProperty ( "Tool"   , "InBremAcceptance/InBrem" ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
    //
  } 
  /// virtual proected destructor 
  virtual ~InBremAcceptanceAlg() {} 
  // ==========================================================================
private:
  // ==========================================================================
  InBremAcceptanceAlg() ;
  InBremAcceptanceAlg           ( const InBremAcceptanceAlg& );
  InBremAcceptanceAlg& operator=( const InBremAcceptanceAlg& );
  // ==========================================================================
} ;
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( InBremAcceptanceAlg )
// ============================================================================
// The END 
// ============================================================================
