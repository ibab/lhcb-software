// $Id: LoKi_VeloClusterMC.cpp,v 1.2 2005-02-08 20:29:46 ibelyaev Exp $
// ===========================================================================
// CVS tag $Name: not supported by cvs2svn $ ; version $Revision: 1.2 $
// ===========================================================================
// $Log: not supported by cvs2svn $
// ===========================================================================
// STD & STL 
// ===========================================================================
#include <cmath>
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
 *  Simple example of "extended LoKi" - usage of LoKi facilities for 
 *  e.g. detertor performace check
 *  @date 2004-04-09 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */
// ============================================================================

// ============================================================================
LOKI_ALGORITHM( LoKi_VeloClusterMC )
{
  using namespace LoKi ;
  
  // get all VeloClusters 
  const VeloClusters* clusters = 
    get<VeloClusters>( VeloClusterLocation::Default );
  
  plot( tanh( clusters->size() / 1000  ) ,
        " tanh( #clusters / 1000 ) "    , 0 , 1.02 , 51 ) ;
  
  typedef IRelationWeighted<VeloCluster,MCParticle,double>  Table1 ;
  typedef IRelationWeighted<VeloCluster,MCVeloHit,double>   Table2 ;
  
  // get relation tables Cluster->MCParticle and Cluster->MCHit 
  const Table1* table1 = get<Table1>("Rec/Relations/VeloClusters2MCParticles");
  const Table2* table2 = get<Table2>("Rec/Relations/VeloClusters2MCHits");
  
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

    // loop over MC particles and get the sum of all weight 
    double w1 = 0 ;
    for( Table1::Range::iterator imcp = mcps.begin() ; 
         mcps.end() != imcp ; ++imcp ) 
    {
      const MCParticle* mcp    = imcp -> to     () ;
      const double      weight = imcp -> weight () ;
      
      /** do here something with MC particle and 'weight', 
       *  e,g try to accumulate the 'weight'.
       */
      if ( 0 != mcp ) { w1 += weight ; }
      
    }
    
    tuple->column( "nMCP"  , mcps.size() ) ;    
    tuple->column( "nMCPw" , w1          ) ;    

    plot(  mcps.size() , " # of MCPs " , 0 , 10 , 20  ) ;

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
    double w2 = 0 ;
    for( Table2::Range::iterator imch = mchits.begin() ; 
         mchits.end() != imch ; ++imch )
    {
      const MCVeloHit* mchit  = imch -> to     () ;
      const double     weight = imch -> weight () ;
      
      /** do here something with MC hit and weight 
       *  e,g try to accumulate the 'weight'.
       */
      if ( 0 != mchit ) { w2 += weight ; }
      
    }
    
    plot(  mchits.size() , " # of MCHs " , 0 , 10 , 20  ) ;
    
    tuple->column( "nMCH"  , mchits.size() ) ;
    tuple->column( "nMCHw" , w2            ) ;    
    
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

