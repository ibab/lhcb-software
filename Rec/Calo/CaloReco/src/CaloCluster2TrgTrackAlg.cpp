// $Id: CaloCluster2TrgTrackAlg.cpp,v 1.2 2004-12-10 17:12:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
#include "Relations/RelationWeighted2D.h"
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/TrgTrack.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CellID.h"
// ============================================================================
// CaloInterfaces 
// ============================================================================
#include "CaloInterfaces/ICaloTrackMatch.h"
// ============================================================================
// CaloDet
// ============================================================================
#include "CaloDet/DeCalorimeter.h"
// ============================================================================
// local
// ============================================================================
#include "CaloCluster2TrgTrackAlg.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class CaloCluster2TrgTrackAlg
 *  @see CaloCluster2TrgTrackAlg
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 02/11/2001
 */
// ============================================================================

// ============================================================================
/** @var CaloCluster2TrgTrackAlgFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloCluster2TrgTrackAlg>         s_Factory ;
const        IAlgFactory&CaloCluster2TrgTrackAlgFactory = s_Factory ;

// ============================================================================
/** Standard constructor
 *  @param   name   algorithm name 
 *  @param   svcloc pointer to service locator 
 */
// ============================================================================
CaloCluster2TrgTrackAlg::CaloCluster2TrgTrackAlg
( const std::string& name   ,
  ISvcLocator*       svcloc )
  : CaloAlgorithm ( name , svcloc ) 
  //
  , m_tracks      ( 1 , TrgTrackLocation::Long )  
  , m_cut         ( 100        )
  //
  , m_eTcut       ( -10 * GeV  )
  //
  , m_matchType   ( "CaloTrackMatchPhoton" ) 
  , m_matchName   ( ""         ) 
  , m_match       ( 0          ) 
{
  declareProperty ( "Tracks"      , m_tracks     ) ;
  declareProperty ( "MatchType"   , m_matchType  ) ;
  declareProperty ( "MatchName"   , m_matchName  ) ;
  declareProperty ( "Cut"         , m_cut        ) ;
  //
  declareProperty ( "EtCut"       , m_eTcut      ) ;
  // 
  // set the approproate default value for input  data 
  setInputData    ( CaloClusterLocation::  Ecal  ) ;
  // set the approproate default value for output data 
  setOutputData   ( "Rec/Calo/PhotonTrgMatch"    ) ;
  // set the appropriate defaults for detector data 
  setDetData      ( DeCalorimeterLocation:: Ecal ) ;
};
// ============================================================================

// ============================================================================
/// destructor
// ============================================================================
CaloCluster2TrgTrackAlg::~CaloCluster2TrgTrackAlg() {}; 
// ============================================================================

// ============================================================================
/** standard algorithm initialization 
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code 
 */
// ============================================================================
StatusCode CaloCluster2TrgTrackAlg::initialize()
{
  StatusCode sc = CaloAlgorithm::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  if ( 0 < m_eTcut ) 
  { Warning ( " Et Cut of " + 
              boost::lexical_cast<std::string>( m_eTcut / GeV )  +
              " GeV is applied " ) ; }
  
  if ( m_tracks.empty() ) 
  { Warning ( "No input containersa of TrgTracks are specified" ) ; } ;
  
  m_match = tool<ICaloTrackMatch>( m_matchType , m_matchName ) ;
  
  return StatusCode::SUCCESS ;
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
StatusCode CaloCluster2TrgTrackAlg::execute() 
{

  // get all clusters
  const Clusters* clusters = get<Clusters>( inputData() ) ;
  if ( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // create the relation table and register in in the store 
  Table* table = new Table ( 100 ) ;
  StatusCode sc = put ( table , outputData() ) ;
  if ( sc.isFailure() ) { return sc ; }
  
  if ( clusters-> empty() ) 
  { return Warning ( "Empty container of Clusters" , StatusCode::SUCCESS ) ; }
  
  const DeCalorimeter* det = getDet<DeCalorimeter>( detData() ) ; 
  CaloDataFunctor::EnergyTransverse<const CaloCluster*,const DeCalorimeter*> eT ( det ) ;
  
  // loop over all track containers 
  const Inputs& containers =  m_tracks ;
  for ( Inputs::const_iterator iTracks = containers.begin() ; 
        containers.end() != iTracks ; ++iTracks ) 
  {
    const Tracks* tracks = get<Tracks> ( *iTracks ) ;
    if ( 0 == tracks || tracks -> empty() ) { continue ; }
    
    for ( Clusters::const_iterator iCluster = clusters->begin() ; 
          clusters->end() != iCluster ; ++iCluster ) 
    {
      const Cluster* cluster = *iCluster ;
      
      if ( 0 == cluster                            ) { continue ; }   // CONTINUE 
      
      // use etCut
      if ( m_eTcut > 0 && eT( cluster ) <  m_eTcut ) { continue ; }   // CONTINUE

      StatusCode code = process ( table              , 
                                  cluster            ,
                                  tracks -> begin () , 
                                  tracks -> end   () ) ;
      if ( code.isFailure() ) 
      {  Error ( " Error code from process() " , sc ) ; }
    }
  };
  
  if ( msgLevel( MSG::DEBUG ) ) 
  { debug() << "Entries in the table " << table->relations().size() << endreq ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
