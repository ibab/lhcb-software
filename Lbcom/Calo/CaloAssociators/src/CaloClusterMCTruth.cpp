// $Id: CaloClusterMCTruth.cpp,v 1.3 2006-02-21 11:17:17 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2006/02/21 10:04:45  odescham
// update for new Event Model
//
// ============================================================================
// Include files 
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/RelationWeighted1D.h"
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiAlg/GaudiAlgorithm.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/MCParticle.h"
// Local
#include "CaloMCTools.h"
// ============================================================================

/** @class CaloClusterMCTruth CaloClusterMCTruth.cpp
 *  
 *  Simple class to create CaloCluster->MCParticle 
 *  relation table basing on information from 
 *  CaloDigit->MCParticle relations table 
 * 
 *  The important  properties of algorithm:
 *  
 *    - "Input"
 *      The default value is "Rec/Relations/CaloDigits2MCParticles" 
 *      The name of relation table CaloDigit->MCParticle 
 *
 *    - "Output"
 *      The default value "Rec/Relations/CaloClusters2MCParticles"
 *      The name of (output) relation table CaloCluster->MCParticle
 *
 *    - "Clusters"
 *      The default value is CaloClusterLocation :: Ecal  + EcalSplit 
 *      list of addresses fro CaloClusters containers 
 *
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
 *  @date   2005-05-03
 */
class CaloClusterMCTruth: public GaudiAlgorithm
{
  friend class AlgFactory<CaloClusterMCTruth> ;
public:
  /// algorithm execution 
  virtual StatusCode execute    () ;
protected:
  /** standard constructor 
   *  @param name algorithm instance name 
   *  @param pSvc pointer to service locator
   */
  CaloClusterMCTruth
  ( const std::string& name , 
    ISvcLocator*       pSvc ) 
    : GaudiAlgorithm ( name , pSvc ) 
    , m_inputRelations    ( "Rec/Relations/CaloDigits2MCParticles"   )
    , m_outputRelations   ( "Rec/Relations/CaloClusters2MCParticles" )
    , m_clusterContainers ()
  {    // set the appropriate default values for input data (linker)
    declareProperty ( "Clustes", m_clusterContainers);
    declareProperty ( "Input"  , m_inputRelations  ) ;
    declareProperty ( "Output" , m_outputRelations ) ;
    m_clusterContainers.push_back( LHCb::CaloClusterLocation :: Ecal      ) ; 
    m_clusterContainers.push_back( LHCb::CaloClusterLocation :: EcalSplit ) ;    
  };
  /// virtual destructor (protected)
  virtual ~CaloClusterMCTruth() {}
private:
  // default constructor  is disabled 
  CaloClusterMCTruth ();
  // copy    constructor  is disabled  
  CaloClusterMCTruth ( const CaloClusterMCTruth& ) ;
  // assignement operator is disabled  
  CaloClusterMCTruth& operator=( const CaloClusterMCTruth& ) ;
private:
  // set the appropriate default values for input data 
  std::string m_inputRelations ;
  std::string m_outputRelations ;
  typedef std::vector<std::string> Inputs ;
  Inputs      m_clusterContainers; 
};
// ============================================================================

// ============================================================================
DECLARE_ALGORITHM_FACTORY(CaloClusterMCTruth);
// ============================================================================

// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloClusterMCTruth::execute    () 
{
  using namespace LHCb::CaloDataFunctor ;
  using namespace CaloMCTools     ;
  
  /// the actual type of Digit  
  typedef const LHCb::CaloDigit                               Digit    ;
  /// the actual type of Cluster 
  typedef const LHCb::CaloCluster                             Cluster  ;
  /// the actual type of Cluster container 
  typedef const LHCb::CaloClusters                            Clusters ;
  /// the actual type of relation table 
  typedef LHCb::RelationWeighted1D<LHCb::CaloCluster,LHCb::MCParticle,float> Table ;
  /// the relation table CaloDigit->MCParticle
  typedef const IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float> DigTable ;
  /// shot cut for Calo cluster entries 
  typedef Cluster::Entries                                    Entries  ;
  
  // create and register the relation table 
  Table* table = new Table( 1000 ) ;
  StatusCode sc = put( table , m_outputRelations ) ;
  if ( sc.isFailure() ) { return sc ; }
  
  
  if ( m_inputRelations.empty() ) 
  { return Error ( "No inputs are specified!" ) ; }
  
  // get CaloDigit->MCParticle relation from TES 
  DigTable* digTable = get<DigTable> ( m_inputRelations ) ;
  if ( 0 == digTable ) { return StatusCode::FAILURE ; }
  
  // loop over all containers of clusters 
  for ( Inputs::const_iterator container = m_clusterContainers.begin() ; 
        m_clusterContainers.end() != container ; ++container ) 
  {
    
    // get the container of clusters 
    Clusters* clusters = get<Clusters> ( *container ) ;
    if ( 0 == clusters ) { return StatusCode::FAILURE ; }
    
    // loop over all clusters in the container  
    for ( Clusters::const_iterator icluster = clusters->begin() ; 
          clusters->end() != icluster ; ++icluster ) 
    {
      const Cluster* cluster = *icluster ;
      if ( 0 == cluster ) { continue ; }
      
      // auxillary container to collect all links from individual digits 
      CaloMCMap mcMap ;
      
      // loop over all digits in the cluster and collect 
      // the links from individual  digits
      const Entries& entries = cluster->entries() ;
      for ( Entries::const_iterator entry = entries.begin() ; 
            entries.end() != entry ; ++entry ) 
      {
        const Digit* digit = entry->digit() ;
        if ( 0 == digit ) { continue ; }
        
        // get all MC relations from this digit
        DigTable::Range range = digTable->relations ( digit ) ;
        // loop over all MC entries and collect the enegy 
        for ( DigTable::iterator item = range.begin() ; 
              range.end() != item ; ++item ) 
        {
          const LHCb::MCParticle* particle = item -> to     () ;
          const double      energy   = item -> weight () ;
          // accumulate the energy from the same particle 
          mcMap[particle] += energy ;
        }
      }; // end of loop over all entries in Cluster
      
      if ( mcMap.empty() ) 
      { Warning ( "No MC information for the cluster is found" ) ; }
      
      // loop over auxillary container of merged depositions
      // and fill relation table entries for given cluster 
      for ( CaloMCMap::iterator imap = mcMap.begin() ; 
            mcMap.end() != imap ; ++imap ) 
      {
        // MC particle  
        const LHCb::MCParticle* particle = imap -> first  ;
        if ( 0 == particle  ) { continue ; }
        // its cumulative energy deposition to the cluster 
        const double      energy   = imap -> second ;
        // fill the relation table:         ATTENTION "i_push" is used!
        table->i_push ( cluster , particle , energy ) ; // NB: "i_push"
      } ;
      
    } ; // end of loop over clusters
    
  } ; // end of loop over containers of clusters 
  
  // mandatory after "i_push" ;
  table->i_sort()  ;                                     // NB: "i_sort"

  
  // count number of links 
  counter ( "#CC2MC links" ) + table->relations().size() ;
  
  if ( table->relations().empty() ) 
  { Warning ( " The relations table '"+ m_outputRelations + "' is empty!") ; }
  
  if ( msgLevel ( MSG::DEBUG )  ) 
  { debug() << " Number of established relations are #"
            << table->relations().size() << endreq ; }

  return StatusCode::SUCCESS ;  
};

// ============================================================================
// The END 
// ============================================================================



