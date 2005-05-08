// $Id: CaloClusterMCTruth.cpp,v 1.1 2005-05-08 09:19:50 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
// ============================================================================
// CaloKernel
// ============================================================================
#include "CaloKernel/CaloAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/CaloDigit.h"
#include "Event/CaloCluster.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CaloMCTools.h"
#include "Event/MCParticle.h"
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
 *    - "Inputs"
 *      The default value is CaloClusterLocation :: Ecal  + EcalSplit 
 *      list of addresses fro CaloClusters containers 
 *
 *    - "Output"
 *      The default value "Rec/Relations/CaloClusters2MCParticles"
 *      The name of (output) relation table CaloCluster->MCParticle
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr 
 *  @date   2005-05-03
 */
class CaloClusterMCTruth: public CaloAlgorithm
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
    : CaloAlgorithm ( name , pSvc ) 
  {
    // set the appropriate default values for input data (linker)
    setInputData  ( "Rec/Relations/CaloDigits2MCParticles"   ) ;
    // set the appropriate default values for input data (clusters)
    addToInputs   ( CaloClusterLocation :: Ecal      ) ;
    addToInputs   ( CaloClusterLocation :: EcalSplit ) ;
    // set the appropriate default value  for output data
    setOutputData ( "Rec/Relations/CaloClusters2MCParticles" ) ;
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
};
// ============================================================================

// ============================================================================
/// anonymous namespace to prevent an export of concrete factory
// ============================================================================
namespace 
{
  // ==========================================================================
  /** @var s_Factory
   *  (local) concrete algorithm factory for instantiaton of 
   *   objects of type CaloClusterMCTruth 
   *  @see CaloClusterMCTruth
   *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
   *  @date 2005-05-03
   */
  // ==========================================================================
  const  AlgFactory<CaloClusterMCTruth>         s_Factory ;
  // ==========================================================================
};
// ============================================================================
/** @var CaloClusterMCTruthFactory
 *  (exported) abstract algorithm factory for instantiaton of 
 *  objects of type CaloClusterMCTruth 
 *  @see CaloClusterMCTruth
 *  @author Vanya BELYAEV Ivan.Belyaev@lapp.in2p3.fr
 *  @date 2005-05-03
 */
const   IAlgFactory&CaloClusterMCTruthFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/// algorithm execution 
// ============================================================================
StatusCode CaloClusterMCTruth::execute    () 
{
  using namespace CaloDataFunctor ;
  using namespace CaloMCTools     ;
  
  /// the actual type of Digit  
  typedef const CaloDigit                                     Digit    ;
  /// the actual type of Cluster 
  typedef const CaloCluster                                   Cluster  ;
  /// the actual type of Cluster container 
  typedef const CaloClusters                                  Clusters ;
  /// the actual type of relation table 
  typedef RelationWeighted1D<CaloCluster,MCParticle,float>    Table    ;
  /// the relation table CaloDigit->MCParticle
  typedef const IRelationWeighted<CaloDigit,MCParticle,float> DigTable ;
  /// shot cut for Calo cluster entries 
  typedef CaloCluster::Entries                                Entries  ;
  
  // create and register the relation table 
  Table* table = new Table( 1000 ) ;
  StatusCode sc = put( table , outputData() ) ;
  if ( sc.isFailure() ) { return sc ; }
  
  if ( inputs().empty() ) 
  { return Error ( "No inputs are specified!" ) ; }
  
  // get CaloDigit->MCParticle relation from TES 
  DigTable* digTable = get<DigTable> ( inputData() ) ;
  if ( 0 == digTable ) { return StatusCode::FAILURE ; }
  
  // loop over all containers of clusters 
  for ( Inputs::const_iterator input = inputs().begin() ; 
        inputs().end() != input ; ++input ) 
  {
    
    // get the container of clusters 
    Clusters* clusters = get<Clusters> ( *input ) ;
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
          const MCParticle* particle = item -> to     () ;
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
        const MCParticle* particle = imap -> first  ;
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
  
  if ( table->relations().empty() ) 
  { Warning ( " The relations table '"+outputData() + "' is empty!") ; }
  
  if ( msgLevel ( MSG::DEBUG )  ) 
  {
    debug() << " Number of established relations are #"
            << table->relations().size() << endreq ;
  }

  return StatusCode::SUCCESS ;  
};

// ============================================================================
// The END 
// ============================================================================



