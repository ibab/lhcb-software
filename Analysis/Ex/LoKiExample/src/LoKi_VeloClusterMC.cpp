// $Id: LoKi_VeloClusterMC.cpp,v 1.1 2004-04-09 07:23:44 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ===========================================================================
// $Log: not supported by cvs2svn $ 
// ===========================================================================
// Include files 
// ===========================================================================
#include "LoKi/LoKi.h"
// ===========================================================================
// Relations
// ===========================================================================
#include "Relations/IAssociatorWeighted.h"
// ===========================================================================
// Event
// ===========================================================================
#include "Event/VeloCluster.h"
#include "Event/MCVeloHit.h"
#include "Event/MCParticle.h"
// ===========================================================================

// ============================================================================
/** @file 
 *  Simple exampel of "extended LoKi" - usage of LoKi facilities for 
 *  e.g. detercor performace check
 *  @date 2004-04-09 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
class LoKi_VeloClusterMC: public LoKi::Algo
{
  // friend factory for instantiation
  friend class AlgFactory<LoKi_VeloClusterMC>;
public:
  virtual StatusCode initialize() ;
  virtual StatusCode analyse   () ;
protected:
  LoKi_VeloClusterMC( const std::string& name , 
                      ISvcLocator*       svc  )
    : LoKi::Algo( name , svc )
    , m_asct1 ( 0 )
    , m_asct2 ( 0 ) {};
  virtual ~LoKi_VeloClusterMC(){};
private:
  
  typedef IAssociatorWeighted<VeloCluster,MCParticle,double> IAsct1;
  typedef IAssociatorWeighted<VeloCluster,MCVeloHit,double>  IAsct2;
  typedef IAsct1::DirectType Table1 ;
  typedef IAsct2::DirectType Table2 ;
  
  IAsct1* m_asct1 ;
  IAsct2* m_asct2 ;
  
} ;
// ============================================================================

// ============================================================================
// Declaration of the Algorithm Factory
// ============================================================================
static const  AlgFactory<LoKi_VeloClusterMC>         s_Factory ;
const        IAlgFactory&LoKi_VeloClusterMCFactory = s_Factory ; 
// ============================================================================

// ============================================================================
/// initialize: locate associators
// ============================================================================
StatusCode LoKi_VeloClusterMC::initialize() 
{
  StatusCode sc = LoKi::Algo::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  m_asct1 = tool<IAsct1> ( "VeloCluster2MCParticleAsct" ) ;
  m_asct2 = tool<IAsct2> ( "VeloCluster2MCHitAsct"      ) ;

  return StatusCode::SUCCESS ;
} ;
// ============================================================================

// ============================================================================
// ============================================================================
StatusCode LoKi_VeloClusterMC::analyse() 
{
  using namespace LoKi ;
  
  // get all VeloClusters 
  const VeloClusters* clusters = 
    get<VeloClusters>( VeloClusterLocation::Default );
  
  // get relation tables Cluster->MCParticle and Cluster->MCHit 
  const Table1* table1 = m_asct1->direct() ;
  const Table2* table2 = m_asct2->direct() ;
  if ( 0 == table1 ) 
  { return Error ( "Clusters -> MCParticles table points to NULL" ) ; }
  if ( 0 == table2 ) 
  { return Error ( "Clusters -> MCHits      table points to NULL" ) ; }
  

  Tuple tuple = nTuple( "VeloCluster -> MC ");
  
    
  for ( VeloClusters::const_iterator ic = clusters->begin() ; 
        clusters -> end() != ic ; ++ic ) 
  {
    const VeloCluster* cluster = *ic ;
    if ( 0 == cluster ) { continue ; } 
  
    /** do here something with VeloClusters */
    tuple -> column ( "NC" , clusters->size() ) ;
    tuple -> column ( "IC" , ic - clusters->begin() );
    tuple -> column ( "ClSize" , cluster->size() );
    tuple -> column ( "ClSens" , cluster->sensor() );
    

    // get all associated MC particles 
    Table1::Range mcps   = table1->relations ( cluster ) ;
    // loop over MC particles 
    for( Table1::Range::iterator imcp = mcps.begin() ; 
         mcps.end() != imcp ; ++imcp ) 
    {
      const MCParticle* mcp = imcp->to() ;
      double weight = imcp->weight() ;

      /** do here something with MC particle and 'weight' */

    }

    tuple->column( "nMCP" , mcps.size() ) ;
    if( !mcps.empty() ) 
    {
      tuple->column( "mcpMinw" , mcps.front().weight() ) ;
      tuple->column( "mcpMaxw" , mcps.back().weight()  ) ;
      }
    else 
    {
      tuple->column( "mcpMinw" , -1000000.0 ) ;
      tuple->column( "mcpMaxw" , -1000000.0 ) ;
    }
    
    // get all associated MC hits 
    Table2::Range mchits = table2->relations ( cluster ) ;
    // loop over MC hits 
    for( Table2::Range::iterator imch = mchits.begin() ; 
         mchits.end() != imch ; ++imch )
    {
      const MCVeloHit* mchit = imch->to() ;
      double weight = imch->weight() ;
      
      /** do here something with MC hit and weight */

    }

    tuple->column( "nMCH" , mchits.size() ) ;
    
    if( !mchits.empty() ) 
    {
      tuple->column( "mchMinw" , mchits.front().weight() ) ;
      tuple->column( "mchMaxw" , mchits.back().weight()  ) ;
    }
    else 
    {
      tuple->column( "mchMinw" , -1000000.0 ) ;
      tuple->column( "mchMaxw" , -1000000.0 ) ;
    }
   
    tuple->write() ;
    
  }
  
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================

