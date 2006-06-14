// $Id: CaloCluster2TrackAlg.cpp,v 1.9 2006-06-14 16:49:22 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.9 $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.8  2006/05/30 09:42:01  odescham
// first release of the CaloReco migration
//
// Revision 1.7  2005/11/07 12:12:41  odescham
// v3r0 : adapt to the new Track Event Model
//
// Revision 1.6  2005/05/08 09:45:56  ibelyaev
//  remove all *associators*
// revision 1.7 2005/14/10 odescham
//  adapt to new track model
// ============================================================================
// Include files
// ============================================================================
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloCluster.h"
#include "Event/Track.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// CaloUtils
// ============================================================================
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloTrackAlg.h"
// ============================================================================

/** @class CaloCluster2TrackAlg CaloCluster2TrackAlg.cpp
 *  
 *  Algoriothm which performs the matching of 
 *  CaloCluster objects with reconstructed tracks.
 *  Algorithm produces the relation table between 
 *  clusters and tracks with weight = chi2 of matching.
 *  
 *  The actual matching is performed by matching tool
 *
 *  The major properties are: 
 *
 *  - "Input" 
 *   The default value is    CaloClusterLocation::Ecal 
 *   Location of input containers of clusters 
 *
 *  - "Output"
 *   The default value is    LHCb::CaloIdLocation::ClusterMatch
 *   Location of output relation table CaloCluster -> Track
 *   relation table of type RelationWeighted2D<CaloCluster,Track,float>
 *   
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @see ICaloTrackMatch 
 *  @see Associator
 *  @see AssociatorWeighed
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloCluster2TrackAlg : public CaloTrackAlg 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloCluster2TrackAlg>;  
public:
  /// standard algorithm initialization 
  virtual StatusCode initialize();
  /// standard algorithm execution 
  virtual StatusCode execute   ();
protected:
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloCluster2TrackAlg
  ( const std::string& name   , 
    ISvcLocator*       svcloc )
    : CaloTrackAlg  ( name , svcloc ) 
      , m_tracks    ( LHCb::TrackLocation::Default )
      , m_cut       ( 100        )
      , m_matchType ( "CaloTrackMatchPhoton" ) 
      , m_matchName ( ""         ) 
      , m_match     ( 0          ) 
      , m_inputData (LHCb::CaloClusterLocation::Ecal)
      , m_outputData(LHCb::CaloIdLocation::ClusterMatch )  
  {
    declareProperty ( "Tracks"         , m_tracks    ) ;
    declareProperty ( "MatchType"      , m_matchType ) ;
    declareProperty ( "MatchName"      , m_matchName ) ;
    declareProperty ( "Cut"            , m_cut       ) ;
    declareProperty ( "InputData"      , m_inputData ) ;  
    declareProperty ( "OutputData"     , m_outputData) ;  
    // 
    setProperty     ( "SkipClones"     , "true"      ) ;
    setProperty     ( "SkipInvalid"    , "true"      ) ;
    setProperty     ( "SkipBackward"   , "true"      ) ;
    setProperty     ( "CheckTracks"    , "true"      ) ;
    //
    std::vector<int> types;
    types.push_back(LHCb::Track::Long);
    types.push_back(LHCb::Track::Downstream);
    types.push_back(LHCb::Track::Ttrack);
    IntegerArrayProperty ptyp("AcceptedType", types);
    setProperty     (  ptyp ) ;
  };
  /// destructor (virtual and protected)
  virtual ~CaloCluster2TrackAlg(){};
private:
  /// default  construstor  is  private 
  CaloCluster2TrackAlg(); 
  /// copy     construstor  is  private 
  CaloCluster2TrackAlg
  ( const CaloCluster2TrackAlg& );
  /// assignement operator  is  private 
  CaloCluster2TrackAlg& operator=
  ( const CaloCluster2TrackAlg& );
private:
  /// address of Tracks 
  std::string        m_tracks    ;
  /// cut value 
  double             m_cut       ;
  // matching tool 
  std::string        m_matchType ;
  std::string        m_matchName ;
  ICaloTrackMatch*   m_match     ;
  std::string   m_inputData;
  std::string   m_outputData;
  long m_usedTracks;
  

};
// ============================================================================

// ============================================================================
/** @var CaloCluster2TrackAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloCluster2TrackAlg>         s_Factory ;
const        IAlgFactory&CaloCluster2TrackAlgFactory = s_Factory ;

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCluster2TrackAlg::initialize() 
{
  // initialize the base class 
  StatusCode sc = CaloTrackAlg::initialize();
  if ( sc.isFailure() ) { return sc ; } 
  
  // retrieve the tool from Tool service 
  m_match = tool<ICaloTrackMatch>( m_matchType , m_matchName );
  if ( 0 == m_match ) { return StatusCode::FAILURE ;}
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCluster2TrackAlg::execute() 
{
  // avoid long names 
  typedef const LHCb::CaloCluster                                    Cluster  ;
  typedef const LHCb::CaloClusters                                   Clusters ;
  typedef LHCb::RelationWeighted2D<LHCb::CaloCluster,LHCb::Track,float>  ClusterTrTable    ;

  const StatusCode OK = StatusCode::SUCCESS ;
  
  // get clusters from Transient Store  
  const Clusters* clusters = get<Clusters> ( m_inputData ) ;
  if ( 0 ==  clusters           )     { return StatusCode::FAILURE ; }
  
  // get tracks   from Transient Store  
  const LHCb::Tracks*   tracks   = get<LHCb::Tracks>   (  m_tracks   ) ;
  if ( 0 ==  tracks             )     { return StatusCode::FAILURE ; }
  
  // create relation table and register it in the store
  ClusterTrTable*    table    = new ClusterTrTable( clusters->size() * 10 ) ;
  put( table , m_outputData );
  
  if ( 0 == tracks   -> size () )
  { return Warning ( "Empty container of Tracks   " , OK ) ; }
  if ( 0 == clusters -> size () )
  { return Warning ( "Empty container of Clusters " , OK ) ; }

  m_usedTracks=0;

  
  // loop over all tracks 
  for ( LHCb::Tracks::const_iterator track = tracks->begin() ; 
        tracks->end() != track ; ++track )
  {  
    // skip NULLs 
    if ( 0 == *track       ) { continue ; }                // CONTINUE  
    
    // use track ? 
    if ( ! use( *track )   ) { continue ; }                // CONTINUE 
    m_usedTracks++;
    // loop over all clusters 
    for ( Clusters::const_iterator cluster = clusters->begin() ; 
          clusters->end() != cluster ; ++cluster )
    {
      // skip NULLs 
      if ( 0 == *cluster  ) { continue ; }               // CONTINUE 
      
      // perform matching
      double chi2 = 0 ;
      StatusCode sc = m_match -> 
        match( &((*cluster)->position()) , *track , chi2 );
      
      
      if ( sc.isFailure() )
      {
        Warning ( "Cluster/Track: matching failure, skip the pair", sc ) ;
        continue ;                                       /// CONTINUIE 
      }
      else if ( 0 <= chi2 && chi2 <=  m_cut ) 
      { table -> i_push ( *cluster , *track , chi2 ) ; } // NB: "i_push"
      
    }; // end of loop over all clusters 
  }; // end of loop over all  tracks

  // NB: call for "i_sort" is mandatory after "i_push"
  table -> i_sort() ;                                  // NB: "i_sort"
  

  debug() << "Entries in the table " << table->relations().size() << endreq ; 
  debug() << " #Tracks   ="   << tracks   -> size () << endreq;
  debug() << " #Used Tracks   ="   << m_usedTracks << endreq;
  debug() << " #Clusters ="   << clusters -> size () << endreq;
  


  return StatusCode::SUCCESS ;
};
// ============================================================================

