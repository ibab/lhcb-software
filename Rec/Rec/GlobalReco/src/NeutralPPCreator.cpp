// $Id: NeutralPPCreator.cpp,v 1.2 2002-07-18 17:57:42 gcorti Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/07/15 19:27:08  ibelyaev
//  add new algorithm for creation on Neutral ProtoParticles
// 
// ============================================================================
// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h"
// PhysEvent
#include "Event/ProtoParticle.h"
// CaloEvent 
#include "Event/CaloHypo.h"
// CaloUtils
#include "CaloUtils/ClusterFunctors.h"
// local
#include "NeutralPPCreator.h"

/** @file 
 * 
 *  Implementation file for class : NeutralPPCreator
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date 2002-07-15 
 */

// ============================================================================
/** @var  NeutralPPCreatorFactory
 *  mandatory declaration of the Algorithm Factory
 */
// ============================================================================
static const  AlgFactory<NeutralPPCreator>          s_factory ;
const        IAlgFactory& NeutralPPCreatorFactory = s_factory ;
// ============================================================================

// ============================================================================
/** Standard constructor  
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @param name the name of the algorithm instance
 *  @param svcLoc pointer to Service Locator 
 */
// ============================================================================
NeutralPPCreator::NeutralPPCreator( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : CaloAlgorithm ( name , pSvcLocator ) 
  , m_inputs    ()
  /// matching with tracks 
  , m_matchName ( "AssociatorWeighted<CaloCluster,TrStoredTrack,float>" 
                  + std::string("/PhotonMatch") )
  , m_match     ( 0 )
  /// spd/prs ID
  , m_spdPrsName ( "CaloSingleGammaTool/SpdPrsID" )
  , m_spdPrs    ( 0 )
  /// 
  , m_ecal      ("Ecal")
{
  declareProperty ( "OtherInputs" , m_inputs                        ) ;
  declareProperty ( "Matching"    , m_matchName                     ) ;
  declareProperty ( "SpdPrs"      , m_spdPrsName                    ) ;  
  setProperty     ( "Output"      , ProtoParticleLocation::Neutrals ) ;
};
// ============================================================================

// ============================================================================
/** destructor 
 */
// ============================================================================
NeutralPPCreator::~NeutralPPCreator() {}; 
// ============================================================================

// ============================================================================
/** standard initialization method
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return ctatus code 
 */
// ============================================================================
StatusCode NeutralPPCreator::initialize() 
{
  StatusCode sc = CaloAlgorithm::initialize();
  if( sc.isFailure() ) 
    { return Error("Could not initialize the base class CaloAlgoriothm ",sc); }
  
  /// matching associatior 
  m_match  = tool( m_matchName , m_match );
  if( 0 == m_match ) { return StatusCode::FAILURE ; }
  
  /// Spd/Prs ID 
  m_spdPrs = tool( m_spdPrsName , m_spdPrs );
  if( 0 == m_match ) { return StatusCode::FAILURE ; }
  
  return StatusCode::SUCCESS;
};
// ============================================================================

// ============================================================================
/** process of one calo hypo into the protoparticle
 *  @param hypo CloHypo object 
 *  @param particle Protopareticle to be updated 
 *  @param associator table 
 *  @return statsu code 
 */
// ============================================================================
StatusCode NeutralPPCreator::process
( const CaloHypo* hypo                 , 
  ProtoParticle*  particle             , 
  const NeutralPPCreator::Table* table ) const
{
  if( 0 == hypo || 0 == particle ) return StatusCode::FAILURE ;
  
  // add the current  hypo to ProtoParticle 
  particle->addToCalo( hypo );
  
  
  { // get track matching chi2
    // get Ecal cluster 
    const Clusters& clusters = hypo->clusters();
    Clusters::const_iterator cluster =
      std::find_if( clusters.begin() , clusters.end() , m_ecal );
    if( clusters.end() != cluster ) 
      {
        // get all related tracks 
        Range range = table->relations( *cluster ) ;
        if( !range.empty() ) 
          {
            const double chi2 = range.begin()->weight();
            particle->
              pIDDetectors().push_back( std::make_pair( 100 , chi2 ) );
          }
      }
  }
  //        // get Spd/Prs ID 
  const double spdPrsID    = m_spdPrs->likelyhood( hypo ) ;
  particle->
    pIDDetectors().push_back( std::make_pair( 101 , spdPrsID   ) );
  
  const CaloPosition* position = hypo->position();
  // shower shape 
  if( 0 != position ) 
    {
      const double showerShape = 
        position->spread()( 1 , 1 )+ position->spread()( 2 , 2 ) ;
      particle->
        pIDDetectors().push_back( std::make_pair( 102 , showerShape ) );
    }
  
  const CaloMomentum* momentum = hypo->momentum ();
  // shower mass
  const double showerMass  = momentum->momentum().m() ;
  particle->
    pIDDetectors().push_back( std::make_pair( 103 , showerMass ) );

  return StatusCode::SUCCESS ;  
};

// ============================================================================

// ============================================================================
/** standard execution  method
 *  @see CaloAlgorithm 
 *  @see     Algorithm 
 *  @see    IAlgorithm
 *  @return ctatus code 
 */
// ============================================================================
StatusCode NeutralPPCreator::execute() 
{
  /// create and register the output container 
  ProtoParticles* particles = new ProtoParticles();
  StatusCode sc = put( particles , outputData() ) ;
  if( sc.isFailure() ) { return StatusCode::FAILURE ; }
  
  /// locate "main" input data 
  const Hypos* hypos = get( eventSvc() , inputData() , hypos );
  if( 0 == hypos     ) { return StatusCode::FAILURE ; }

  if( 0 == hypos->size() ) 
    { Warning("'Main' input container '"+inputData()+"' is empty!"); }

  /// check the table from associator 
  Table* table = m_match -> direct () ;
  if( 0 == table ) { return Error("'Table' from Associator points to NULL!");}
  
  for( Hypos::const_iterator hypo = hypos->begin() ;
       hypos->end() != hypo ; ++hypo )
    {
      // skip the artificial NULLs 
      if( 0 == *hypo     ) { continue ; }
      // valid momentum information ? 
      const CaloMomentum* momentum = (*hypo)->momentum ();
      if( 0 ==  momentum ) { continue ; }
      
      // create new protoparticle 
      ProtoParticle* particle = new ProtoParticle() ;
      // and immediately add it into the container 
      particles->insert( particle );
      
      process( *hypo , particle , table );
    }
  
  // add 'additional' hypos
  for( Inputs::const_iterator input = m_inputs.begin() ; 
       m_inputs.end() != input ; ++input ) 
    {
      /// get the input data
      const Hypos* hypos = get( eventSvc() , *input , hypos );
      
      // loop over all 'new' hypos 
      for( Hypos::const_iterator hypo = hypos->begin() ; 
           hypos->end() != hypo ; ++hypo ) 
        {
          // skip NULLS 
          if( 0 == *hypo ) { continue ; }
          
          // loop over all existing protoparticles
          ProtoParticles::iterator pp = particles->begin();
          for( ; particles->end() != pp ; ++pp )
            {
              // skip NULLS 
              if( 0 == *pp               ) { continue ; }
              if( overlap( *hypo , *pp ) ) break ;
            }
          
          // unique hypo? 
          if( particles->end() == pp ) 
            {
              // valid momentum information ? 
              const CaloMomentum* momentum = (*hypo)->momentum ();
              if( 0 ==  momentum ) { continue ; }
              // create the new particle 
              // and immediately add it into the container 
              ProtoParticle* particle = new ProtoParticle();
              particles->insert( particle );
              process( *hypo , particle , table );
              // go to next hypo 
              continue ;                                   /// CONTINUE
            }
          // non unique particle
          
          // update existing particle 
          ProtoParticle* particle = *pp ;
          if( 0 == particle ) { continue ; }
          
          process( *hypo , particle , table );
          
        } 
    }
  
  return StatusCode::SUCCESS;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
