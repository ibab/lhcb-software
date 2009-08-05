// $Id: BremMatchAlg.cpp,v 1.9 2009-08-05 17:35:33 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloHypo.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/static_assert.hpp"
// ============================================================================
// local
// ============================================================================
#include "CaloTrackMatchAlg.h"
// ============================================================================
/** @class BremMatchAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-16 
 */
// ============================================================================
class BremMatchAlg : public CaloTrackMatchAlg 
{
  // ==========================================================================
  /// friend factory for instantiation
  friend class AlgFactory<BremMatchAlg> ;
  // ==========================================================================
public:
  // ==========================================================================
  /// execution of the algorithm
  virtual StatusCode execute () ;  
  // ==========================================================================
protected:
  // ==========================================================================
  /// standard (protected) constructor 
  BremMatchAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrackMatchAlg ( name , pSvc ) 
  {
    if( "HLT" == context() ){
      Inputs inputs = Inputs( 1 , LHCb::CaloHypoLocation::PhotonsHlt   ) ;
      _setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )   ) ;
      _setProperty ( "Output"    , LHCb::CaloIdLocation::BremMatchHlt  ) ;
      _setProperty ( "Filter"    , LHCb::CaloIdLocation::InBremHlt     ) ;
    }
    else{
      Inputs inputs = Inputs( 1 , LHCb::CaloHypoLocation::Photons     ) ;
      _setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )   ) ;
      _setProperty ( "Output"    , LHCb::CaloIdLocation::BremMatch     ) ;
      _setProperty ( "Filter"    , LHCb::CaloIdLocation::InBrem        ) ;
    }
    
    _setProperty ( "Tool"      , "CaloBremMatch/BremMatch"    ) ;
    setProperty ( "Threshold" , 10000                               ).ignore() ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
  }
  /// virtual and protected destrcutror
  virtual ~BremMatchAlg() {}
  // ==========================================================================
private:
  // ==========================================================================
  BremMatchAlg() ;
  BremMatchAlg           ( const BremMatchAlg& ) ;
  BremMatchAlg& operator=( const BremMatchAlg& ) ;
  // ==========================================================================
};
// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( BremMatchAlg );
// ============================================================================
// Standard execution of the algorithm
// ============================================================================
StatusCode BremMatchAlg::execute () 
{
  //
  Assert ( !m_tracks .empty() , "No Input tracks"   ) ;
  Assert ( !m_calos  .empty() , "No Input Clusters" ) ;
  
  //
  typedef LHCb::RelationWeighted2D<LHCb::Track,LHCb::CaloHypo,float> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::ITrHypoTable2D)) ;
  
  // create the relation table and register it in TES 
  Table* table = new Table ( 10 * 100  ) ; 
  put ( table , m_output ) ;
  
  // perform the actual jobs 
  return doTheJob<LHCb::CaloHypo,Table>( table ) ;
} 
// ============================================================================
// The END 
// ============================================================================
