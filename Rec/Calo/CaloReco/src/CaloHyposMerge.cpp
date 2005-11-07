// $Id: CaloHyposMerge.cpp,v 1.5 2005-11-07 12:12:42 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.4  2004/02/17 12:08:08  ibelyaev
//  update for new CaloKernel and CaloInterfaces
//
// Revision 1.3  2003/04/08 08:36:08  ibelyaev
//  CaloHyposMerge: add SplitPhotons to the default list
//
// Revision 1.2  2002/11/14 14:28:38  ibelyaev
//  bug fix for Win2K
//
// Revision 1.1.1.1  2002/11/13 20:46:40  ibelyaev
// new package 
//
// ============================================================================
// Include files
#include <map>
#include <algorithm>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// CaloEvent 
#include "Event/CaloParticle.h"
#include "Event/CaloHypo.h"
// local
#include "CaloHyposMerge.h"

// ============================================================================
/** @file
 * 
 *  Implementation file for class : CaloHyposMerge
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-11-10
 */
// ============================================================================

// ============================================================================
/** @var  CaloHyposMergeFactory
 *  Declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<CaloHyposMerge>          s_Factory ;
const        IAlgFactory&CaloHyposMergeFactory  = s_Factory ; 
// ============================================================================


// ============================================================================
/** Standard constructor
 *  @param name algorithm name 
 *  @param pSvc service locator 
 */
// ============================================================================
CaloHyposMerge::CaloHyposMerge( const std::string& name ,
                                ISvcLocator*       pSvc )
  : CaloAlgorithm ( name , pSvc ) 
  , m_hypos       ()
{
  // set the appropriate defaults for hypos 
  m_hypos.push_back ( CaloHypoLocation::Photons      ) ;
  m_hypos.push_back ( CaloHypoLocation::Electrons    ) ;  
  m_hypos.push_back ( CaloHypoLocation::MergedPi0s   ) ;
  m_hypos.push_back ( CaloHypoLocation::SplitPhotons ) ;
  /// addresses for input hypos  
  declareProperty   ( "InputHypos" , m_hypos         ) ;
  /// set the default value for input data 
  setInputData      ( CaloClusterLocation::  Ecal    ) ;
  /// set the default value for output container 
  setOutputData     ( CaloParticleLocation:: Default ) ;
};
// ============================================================================


// ============================================================================
/// Destructor
// ============================================================================
CaloHyposMerge::~CaloHyposMerge() {}; 
// ============================================================================


// ============================================================================
/**  standard Algorithm  initialization
 *   @see CaloAlgorithm
 *   @see     Algorithm
 *   @see    IAlgorithm
 *   @return status code 
 */
// ============================================================================
StatusCode CaloHyposMerge::initialize()
{
  /// initialize the base class 
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
  { return Error("Base class CaloAlgorithm could not be initialized",sc); }
  /// input data?
  if( m_hypos.empty() ) { Warning("No input data is specified!"); }
  
  // eliminate duplicates 
  std::sort     ( m_hypos.begin ()  , m_hypos.end () );
  Addresses::iterator it = std::unique ( m_hypos.begin () , m_hypos.end () );
  m_hypos.erase ( it                , m_hypos.end () ) ;
  
  return sc ;
};
// ============================================================================

// ============================================================================
/**  standard Algorithm execution
 *   @see     Algorithm
 *   @see    IAlgorithm
 *   @return status code 
 */
// ============================================================================
StatusCode CaloHyposMerge::execute() 
{
  
  StatusCode sc( StatusCode::SUCCESS );  
  
  // avoid long names 
  typedef CaloParticle       Particle  ;
  typedef CaloParticles      Particles ;  
  typedef CaloHypo           Hypo      ;
  typedef CaloHypos          Hypos     ;
  typedef CaloCluster        Cluster   ;
  typedef CaloClusters       Clusters  ;
  typedef CaloHypo::Clusters Clusts    ;
  
  // create and register in Gaudi Event Transient Store the output container 
  CaloParticles* particles = new CaloParticles() ;
  sc = put( particles , outputData() );
  if( sc.isFailure() ) { return StatusCode::FAILURE ; }
  
  // get the clusters from the store 
  const Clusters* clusters = get<Clusters>( inputData() );
  if( 0 == clusters ) { return StatusCode::FAILURE ; }
  
  // the intermediate map for relations between hypos and clusters  
  typedef std::multimap<const CaloCluster*,const CaloHypo*> TheMap ;
  typedef TheMap::const_iterator                            IT     ;
  typedef std::pair<IT,IT>                                  IP     ;
  TheMap                                                    theMap ;
  
  // build the intermediate table of hypo <---> relations 
  for( Addresses::const_iterator address = m_hypos.begin() ; 
       m_hypos.end() != address ; ++address )
  {
    const Hypos* hypos = get<Hypos>( *address );
    if( 0 == hypos ) { return StatusCode::FAILURE ; }
    for( Hypos::const_iterator hypo = hypos->begin() ; 
         hypos->end() != hypo ; ++hypo )
    {
      // skip nulls 
      if( 0 == *hypo ) { continue ; }
      const Hypo::Clusters& clusters = (*hypo)->clusters();
      for( Hypo::Clusters::const_iterator cluster = clusters.begin() ; 
           clusters.end() != cluster ; ++cluster )
      {
        if( *cluster == 0 ) { continue ; }
        theMap.insert( std::make_pair( *cluster , *hypo ) );     
      }
    }
  }
  
  // loop over all clusters 
  for( Clusters::const_iterator cluster = clusters->begin() ; 
       clusters->end() != cluster ; ++cluster ) 
  {
    // skip NULLS 
    if( 0 == *cluster ) { continue ; }
    // for each valid cluster create CaloParticle 
    Particle* particle = new CaloParticle();
    // add newly created particle to the container of particles 
    particles->insert( particle );
    // get all connected hypos 
    IP related = theMap.equal_range( *cluster );
    for( IT it = related.first ; it != related.second ; ++it )
    {
      const Hypo* hypo = it->second ;
      // add valid hypo to the selected particle
      if( 0 != hypo ) { particle->addToHypos( hypo ); }
    }
  }
  
  // clear the map 
  theMap.clear();
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
