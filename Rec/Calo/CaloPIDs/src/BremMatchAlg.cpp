// $Id: BremMatchAlg.cpp,v 1.7 2008-06-30 15:37:34 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.7 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.6  2008/05/13 12:25:51  odescham
// restore _setProperty()
//
// Revision 1.5  2008/01/24 10:22:42  vegorych
// _setProperty was removed, AddNeigbours warning was solved
//
// Revision 1.4  2007/08/24 21:25:18  odescham
// fix uncheck. StatusCodes
//
// Revision 1.3  2006/06/21 18:43:29  ibelyaev
//  fix 'Brem' problem & speed-up it a bit
//
// Revision 1.2  2006/06/20 18:17:48  odescham
// minor update to please ChargedProtoPAlg
//
// Revision 1.1  2006/06/18 18:35:27  ibelyaev
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
// ============================================================================
/** @class BremMatchAlg 
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-06-16 
 */
// ============================================================================
class BremMatchAlg : public CaloTrackMatchAlg 
{
  // friend factory for instantiation
  friend class AlgFactory<BremMatchAlg> ;
public:
  /// execution of the algorithm
  virtual StatusCode execute () ;  
protected:
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
    

    _setProperty ( "Tool"      , "CaloBremMatch/BremMatch:PUBLIC"    ) ;
    setProperty ( "Threshold" , 10000                               ).ignore() ;
    // track types:
    _setProperty ( "AcceptedType" , Gaudi::Utils::toString<int>
                   ( LHCb::Track::Velo     ,
                     LHCb::Track::Long     ,
                     LHCb::Track::Upstream ) ) ;
  }
  /// virtual and protected destrcutror
  virtual ~BremMatchAlg() {}
private:
  BremMatchAlg() ;
  BremMatchAlg           ( const BremMatchAlg& ) ;
  BremMatchAlg& operator=( const BremMatchAlg& ) ;
};
// ============================================================================
/// Declaration of the Algorithm Factory
// ============================================================================
DECLARE_ALGORITHM_FACTORY( BremMatchAlg );
// ============================================================================
/// Standard execution of the algorithm
// ============================================================================
StatusCode BremMatchAlg::execute () 
{
  //
  Assert ( !m_tracks .empty() , "No Input tracks"   ) ;
  Assert ( !m_calos  .empty() , "No Input Clusters" ) ;
  //if(sc.isFailure())warning() << "Asserting FAILED " << endreq;
  
  //
  typedef LHCb::RelationWeighted2D<LHCb::Track,LHCb::CaloHypo,float> Table ;
  BOOST_STATIC_ASSERT(INHERITS(Table,LHCb::Calo2Track::ITrHypoTable2D)) ;
  
  // create the relation table and register it in TES 
  Table* table = new Table ( 10 * 100  ) ; 
  put ( table , m_output ) ;
  
  // perform the actual jobs 
  return doTheJob<LHCb::CaloHypo,Table>( table ) ;
} ;
