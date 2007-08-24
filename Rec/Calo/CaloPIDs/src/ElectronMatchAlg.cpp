// $Id: ElectronMatchAlg.cpp,v 1.3 2007-08-24 21:25:18 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/06/20 18:17:48  odescham
// minor update to please ChargedProtoPAlg
//
// Revision 1.1  2006/06/18 18:35:28  ibelyaev
//  the firstcommmit for DC06 branch
//
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
#include "ToString.h"
// ============================================================================
/** @class ElectronMatchAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-16 
 */
// ============================================================================
class ElectronMatchAlg : public CaloTrackMatchAlg 
{
  // friend factory for instantiation
  friend class AlgFactory<ElectronMatchAlg> ;
public:
  /// execution of the algorithm
  virtual StatusCode execute () ;  
protected:
  /// standard (protected) constructor 
  ElectronMatchAlg
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : CaloTrackMatchAlg ( name , pSvc ) 
  {    
    Inputs inputs = Inputs( 1 , LHCb::CaloHypoLocation::Electrons   ) ;
    //
    _setProperty ( "Calos"     , Gaudi::Utils::toString ( inputs )   ) ;
    _setProperty ( "Output"    , LHCb::CaloIdLocation::ElectronMatch ) ;
    _setProperty ( "Tool"      , "CaloElectronMatch/ElectronMatch:PUBLIC") ;
    _setProperty ( "Filter"    , LHCb::CaloIdLocation::InEcal        ) ;
    _setProperty ( "Threshold" , "10000"                             ) ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                  ( LHCb::Track::Long       ,
                    LHCb::Track::Downstream ,
                    LHCb::Track::Ttrack     ) ) ;
  }
  /// virtual and protected destrcutror
  virtual ~ElectronMatchAlg() {}
private:
  ElectronMatchAlg() ;
  ElectronMatchAlg           ( const ElectronMatchAlg& ) ;
  ElectronMatchAlg& operator=( const ElectronMatchAlg& ) ;
};
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( ElectronMatchAlg );
// ============================================================================
/// Standard execution of the algorithm
// ============================================================================
StatusCode ElectronMatchAlg::execute () 
{
  //
  Assert ( !m_tracks .empty() , "No Input tracks"   ) ;
  Assert ( !m_calos  .empty() , "No Input Clusters" ) ;
  //
  typedef LHCb::RelationWeighted2D<LHCb::Track,LHCb::CaloHypo,float> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::ITrHypoTable2D)) ;
  
  //create the relation table and register it in TES 
  Table* table = new Table ( 10 * 100  ) ; 
  put ( table , m_output ) ;
  
  //  perform the actual jobs 
  return doTheJob<LHCb::CaloHypo,Table>( table ) ;  
} ;
