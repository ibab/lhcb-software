// $Id$ 
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// local 
// ============================================================================
#include "FilterDecays.h"
// ============================================================================
/*  @file
 *  Implementation file for algorithm FilterDecays 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2011-05-13
 */
// ============================================================================  
/*  standard constructor 
 *  @see DVAlgorithm
 *  @see GaudiTupleAlg
 *  @see GaudiHistoAlg
 *  @see GaudiAlgorithm
 *  @see Algorithm
 *  @see AlgFactory
 *  @see IAlgFactory
 *  @param name the algorithm instance name 
 *  @param pSvc pointer to Service Locator 
 */
// ============================================================================  
FilterDecays::FilterDecays                               // standard contructor 
( const std::string& name ,                      // the algorithm instance name 
  ISvcLocator*       pSvc )                       // pointer to Service Locator
  : FilterDesktop ( name , pSvc ) 
// invalid decay finder 
  , m_finder   ( Decays::Trees::Types_<const LHCb::Particle*>::Invalid() ) 
{ }
// ============================================================================  
// destructor 
// ============================================================================  
FilterDecays::~FilterDecays () {}                             
// ============================================================================  
// finalize the algorithm 
// ============================================================================  
StatusCode FilterDecays::finalize  () 
{ 
  m_finder = Decays::Trees::Types_<const LHCb::Particle*>::Invalid() ;
  return FilterDesktop::finalize () ; 
}
// ============================================================================  
/*  the major method for filter input particles 
 *  @param input    (INPUT) the input  container of particles
 *  @param filtered (OUPUT) the output container of particles
 *  @return Status code 
 */
// ============================================================================  
StatusCode FilterDecays::filter 
( const LHCb::Particle::ConstVector& input    , 
  LHCb::Particle::ConstVector&       filtered )
{
  //
  counter ( "#decays") +=  finder().findDecay ( input , filtered ) ;
  //
  // mark & store filtered particles in DVAlgorithm local container 
  markParticles ( filtered ) ;
  //
  return StatusCode::SUCCESS ;
}
// ============================================================================
// decode the code
// ============================================================================
StatusCode FilterDecays::decodeCode () 
{
  /// get the factory
  Decays::IDecay* factory = tool<Decays::IDecay>( "LoKi::Decay" ) ;
  
  /// construct the tree 
  Decays::IDecay::Tree tree = factory->tree ( code () ) ;
  
  /// check the validity 
  if ( !tree ) 
  {
    StatusCode sc = tree.validate ( ppSvc() ) ;
    if ( sc.isFailure() ) 
    {
      return Error ( "Unable to validate the tree '" + 
                     tree.toString() + "' built from the descriptor '"
                     + code() + "'" , sc ) ;
    }
  }
  //
  if ( !tree ) 
  {
    return Error ( "Invalid tree '" + 
                   tree.toString()  + 
                   "' built from the descriptor '"
                   + code() + "'" ) ;
  }
  // reser the finder 
  m_finder = Decays::IDecay::Finder ( tree ) ;
  if ( !m_finder ) 
  {
    return Error ( "Invalid finder '" + 
                   m_finder.tree().toString()  + 
                   "' built from the descriptor '"
                   + code() + "'" ) ;
  }  
  /// release the factory ( not needed anymore) 
  release ( factory ) ;
  ///
  return StatusCode::SUCCESS ;
}

// ============================================================================
/// the factory 
DECLARE_ALGORITHM_FACTORY(FilterDecays)
// ============================================================================
// The END 
// ============================================================================


