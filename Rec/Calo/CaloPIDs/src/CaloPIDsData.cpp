// $Id: CaloPIDsData.cpp,v 1.4 2005-11-07 12:16:09 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ , version $Revision: 1.4 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2005/05/10 11:29:21  ibelyaev
//  update for CaloPIDsData(test) algorithm
//
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/TupleObj.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Event/MCParticle.h"
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloTupleAlg.h"
// ============================================================================
// LHCbKernel/Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
// ============================================================================


/** @class CaloPIDsData CaloPIDsData.cpp
 *
 *  Simple algorithm to fill NTuple with information, 
 *  needed to obtain the reference histograms 
 *  for CaloPIDs package 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-03-17
 */
class CaloPIDsData : public CaloTupleAlg 
{
  friend class AlgFactory<CaloPIDsData> ;
public:
  /// execution     of the algorithm
  virtual StatusCode execute    () ;  
protected:  
  /** Standard constructor
   *  @param name algorithm instance name 
   *  @param isvc pointer to Service Locator 
   */
  CaloPIDsData 
  ( const std::string&  name  , 
    ISvcLocator*        isvc  ) 
    : CaloTupleAlg       ( name  , isvc ) 
    //
    , m_name_Tr2MC       ( "Rec/Relations/Track2MCP" )
    //
    , m_name_PhotonMatch ( "Rec/Calo/PhotonMatch" )
    , m_name_PrsE        ( "Rec/Calo/PrsE"        )
    , m_name_EcalChi2    ( "Rec/Calo/EcalChi2"    )
    , m_name_BremChi2    ( "Rec/Calo/BremChi2"    ) 
    , m_name_EcalE       ( "Rec/Calo/EcalE"       )
    , m_name_HcalE       ( "Rec/Calo/HcalE"       )
    //
  {
    // MC-truth info for Tracks
    declareProperty ( "Tr2MCP"    , m_name_Tr2MC       ) ;
    // Calo PID-estimators 
    declareProperty ( "PhMatch"   , m_name_PhotonMatch ) ;
    declareProperty ( "PrsE"      , m_name_PrsE     ) ;
    declareProperty ( "EcalChi2"  , m_name_EcalChi2 ) ;
    declareProperty ( "BremChi2"  , m_name_BremChi2 ) ;
    declareProperty ( "EcalE"     , m_name_EcalE    ) ;
    declareProperty ( "HcalE"     , m_name_HcalE    ) ;
    // define the appropriate default for input data  
    setInputData    ( TrackLocation::Default ) ;  
  };
  /// virtual destructor 
  virtual ~CaloPIDsData( ){};
protected:
  /// get the momentum of the track
  double momentum ( const Track* track ) const ;
private:
  // default constructor is disabled 
  CaloPIDsData();
  // copy constructor is disabled 
  CaloPIDsData( const CaloPIDsData& ) ;
  // assignement operator is disabled 
  CaloPIDsData& operator= ( const CaloPIDsData& );
private:
  std::string m_name_Tr2MC       ;
  std::string m_name_PhotonMatch ;
  std::string m_name_PrsE        ;
  std::string m_name_EcalChi2    ;
  std::string m_name_BremChi2    ;
  std::string m_name_EcalE       ;
  std::string m_name_HcalE       ;  
};

// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  Algorithm Factory
   */
  // ==========================================================================
  const  AlgFactory<CaloPIDsData>         s_Factory ;
};
// ============================================================================
/** @var CaloPIDsDataFactory
 *  Abstract Algorithm Factory
 */
// ============================================================================
const   IAlgFactory&CaloPIDsDataFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/** extract the momentum from Track
 *  @param  track pointer to the track
 *  @return the momentum of the track 
 */
// ============================================================================
double CaloPIDsData::momentum
( const Track* track ) const 
{
  if( 0 == track ) 
  { 
    Error ( "momentum(): Track* points to NULL, return -100*GeV" ) ;
    return -100 * GeV ;
  };
  
  // get the momentum from the state nearest to 0,0,0
  const State state   = track->closestState( 0.0 ) ;
  //OD if( 0 == state ) 
  //{ 
  //  Error ( "momentum(): Track has no state closest to z=0, return -100*GeV");  
  //  return -100 * GeV ;
  //}
  
  //const TrStateP* stateP = dynamic_cast<const TrStateP*>( state );
  //if( 0 == stateP ) 
  //{ 
  //  Error ( "momentum(): TrState is not TrStateP! ,return -100*GeV");  
  //  return -100 * GeV ;
  //}
  
  return state.p  () ;
}
// ============================================================================


// ============================================================================
// ============================================================================
StatusCode CaloPIDsData::execute() 
{
  using namespace Tuples ;
  
  
  /// the actual type of Track -> Eval relation table 
  typedef const IRelation<Track,float>                      Table    ;
  /// the actual type of Track -> Eval range  
  typedef Table::Range                                              Range    ;
  
  /// the actual type for Track -> MCParticle  relation table
  typedef const IRelationWeighted<Track,MCParticle,double>  TrTable  ;
  typedef TrTable::Range                                            TrRange  ;
  
  // "Photon match table"
  typedef const IRelationWeighted2D<CaloCluster,Track,float> CTable  ;
  typedef const IRelationWeighted<Track,CaloCluster,float>   CITable ;
  typedef CITable::Range                                             CIRange ;
  
  // locate input data 
  const Tracks* tracks = get<Tracks>( inputData() ) ;
  if ( 0 == tracks ) { return StatusCode::FAILURE ; }
  
  const TrTable* tr2MC    = get<TrTable>( m_name_Tr2MC    ) ;
  if ( 0 == tr2MC     ) { return Error ( " Tr2MC    table points to NULL" ) ; }
  
  const CTable* _ctable   = get<CTable> ( m_name_PhotonMatch ) ;
  if ( 0 == _ctable  ) { return Error ( " PhMatch    table points to NULL" ) ; }
  const CITable* citable  = _ctable->inverse() ;
  if ( 0 == citable  ) { return Error ( " PhMatch(I) table points to NULL" ) ; }
  
  const Table*   prsE     = get<Table>  ( m_name_PrsE     ) ;
  if ( 0 == prsE      ) { return Error ( " PrsE     table points to NULL" ) ; }
  
  const Table*   ecalChi2 = get<Table>  ( m_name_EcalChi2 ) ;
  if ( 0 == ecalChi2  ) { return Error ( " EcalChi2 table points to NULL" ) ; }
  
  const Table*   bremChi2 = get<Table>  ( m_name_BremChi2 ) ;
  if ( 0 == bremChi2  ) { return Error ( " BremChi2 table points to NULL" ) ; }
  
  const Table*   ecalE    = get<Table>  ( m_name_EcalE    ) ;
  if ( 0 == ecalE     ) { return Error ( " EcalE    table points to NULL" ) ; }
  
  const Table*   hcalE    = get<Table>  ( m_name_HcalE    ) ;
  if ( 0 == hcalE     ) { return Error ( " HcalE    table points to NULL" ) ; }
  
  Tuple tuple = nTuple( 415 , " CaloPIDs moni/test Alg=" + name() ) ;
  
  for ( Tracks::const_iterator itrack = tracks->begin() ; 
        tracks->end() != itrack  ; ++itrack ) 
  {
    const Track* track  = *itrack ;
    if ( 0 == track ) { continue ; }                                // CONTINUE
    
    const double mom = momentum ( track ) ;
    if ( mom  < 0 ) { Warning( "Negative |Momentum| is detected" ) ; }
    
    // get all MC relations from given track 
    TrRange mc = tr2MC -> relations ( track ) ;
    
    // get the relation with highest weight 
    const MCParticle* mcp = 0 ;
    if ( !mc.empty() ) { mcp = mc.back().to() ; }
    
    long pid = 0 ;
    if ( 0 != mcp ) { pid = mcp->particleID().pid() ; }
    
    double v0 = -1000 ;
    CIRange ir = citable->relations ( track ) ;
    if ( !ir.empty() ) { v0 = ir.front().weight() ; }
    
    double v1 = -1 * TeV ;
    Range r   = prsE->relations ( track ) ;
    if ( !r.empty() ) { v1 = r.front().to() ; }
    
    double v2 = -1 * TeV ;
    r         = ecalChi2 -> relations ( track ) ;
    if ( !r.empty() ) { v2 = r.front().to() ; }
    
    double v3 = -1 * TeV ;
    r         = bremChi2 -> relations ( track ) ;
    if ( !r.empty() ) { v3 = r.front().to() ; }
    
    double v4 = -1 * TeV ;
    r         = ecalE -> relations ( track ) ;
    if ( !r.empty() ) { v4 = r.front().to() ; }
    
    double v5 = -1 * TeV ;
    r         = hcalE -> relations ( track ) ;
    if ( !r.empty() ) { v5 = r.front().to() ; }
    
    tuple -> column ( "p"  , mom ) ;
    
    tuple -> column ( "PhMat"    , v0 ) ;
    tuple -> column ( "PrsE"     , v1 ) ;
    tuple -> column ( "EcalChi2" , v2 ) ;
    tuple -> column ( "BremChi2" , v3 ) ;
    tuple -> column ( "EcalE"    , v4 ) ;
    tuple -> column ( "HcalE"    , v5 ) ;
    
    tuple -> column ( "id" , pid  , -25000 , 25000 ) ;
    
    tuple -> column ( "IsLong"   , track->checkType (Track::Long) ) ;
    tuple -> column ( "IsUp"     , track->checkType (Track::Upstream) ) ;
    tuple -> column ( "IsDown"   , track->checkType (Track::Downstream) ) ;
    tuple -> column ( "IsVelo"   , track->checkType (Track::Velo) ) ;
    tuple -> column ( "IsBack"   , track-> checkFlag(Track::Backward) ) ;
    tuple -> column ( "IsT"      , track->checkType (Track::Ttrack ) ) ;
    tuple -> column ( "unique"   , track -> checkFlag(Track::Unique) ) ;
    tuple -> column ( "velo"     , track -> checkHistory(Track::PatVelo) ) ;
    tuple -> column ( "seed"     , track -> checkHistory(Track::TrackSeeding) ) ;
    tuple -> column ( "mat"      , track -> checkHistory(Track::TrackMatching) ) ;
    tuple -> column ( "forward"  , track -> checkHistory(Track::TrgForward) ) ;
    //    tuple -> column ( "follow"   , track -> follow     () ) ;
    tuple -> column ( "veloTT"   , track->checkHistory(Track::TrackVeloTT) ) ;
    tuple -> column ( "veloBack" ,track->checkType (Track::Velo) && track ->checkFlag(Track::Backward) ) ;
    tuple -> column ( "ksTrack"  , track -> checkHistory(Track::TrackKShort) ) ;

    tuple -> write () ;
    
  }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
