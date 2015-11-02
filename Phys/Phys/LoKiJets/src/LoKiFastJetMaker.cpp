// $Id: LoKiFastJetMaker.cpp,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Local
// ============================================================================
#include "LoKiFastJetMaker.h"
// ============================================================================
// fastjet
// ============================================================================
#include "fastjet/ClusterSequence.hh"
#include "fastjet/config.h"
//#include "fastjet/SISConePlugin.hh"


// ============================================================================
/** @file
 *  Implementation file for class  LoKi::FastJetMaker
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @author Victor COCO   coco@lapp.in2p3.fr
 *  @date   2007-10-22
 */
// ============================================================================
/*  standard initialization of the tool
 *  @return status code
 */
// ============================================================================
StatusCode LoKi::FastJetMaker::initialize ()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  // check the parameters
  sc = check() ;
  if ( sc.isFailure() ) { return sc ; }
  //
  svc<LoKi::ILoKiSvc>( "LoKiSvc" , true ) ;
  //
  return sc ;
}
// ============================================================================
// prepare the input information
// ============================================================================
fastjet::JetDefinition LoKi::FastJetMaker::prepare
( const IJetMaker::Input&          input ,
  std::vector<fastjet::PseudoJet>& jets  ) const
{
  // input container of "particles"
  jets.reserve ( input.size() ) ;
  for ( IJetMaker::Input::const_iterator ip = input.begin() ;
        input.end() != ip ; ++ip )
  {
    const LHCb::Particle* p = *ip ;
    if ( NULL == p ) { Warning ( "Invalid input particle" ).ignore() ; continue ; }
    jets.push_back ( makeJet( p , to_user_index ( ip - input.begin() ) ) ) ;
  }

  //  if(m_type == fastjet::plugin_algorithm)
  //  {
  //     fastjet::JetDefinition::Plugin * plugin;
  //     double overlap_threshold = 0.5;
  //     plugin = new fastjet::SISConePlugin (m_r, overlap_threshold);
  //     fastjet::JetDefinition jet_def(plugin);
  //     delete plugin;
  //     return jet_def ;

  //}
  //else{

  fastjet::JetFinder           finder   = (fastjet::JetFinder)           m_type  ;
  fastjet::RecombinationScheme scheme   = (fastjet::RecombinationScheme) m_recom ;
  fastjet::Strategy            strategy = (fastjet::Strategy)            m_strat ;
  fastjet::JetDefinition jet_def
    ( finder   ,
      m_r      ,
      scheme   ,
      strategy ) ;

  if ( msgLevel ( MSG::DEBUG ) )
  {
    debug() << "fastjet::JetDefinition:" << endmsg ;
    debug() << jet_def.description()      << endmsg    ;
  }

  return jet_def ;
  //  }


}
// ===========================================================================
// find the jets
// ===========================================================================
StatusCode LoKi::FastJetMaker::makeJets( const IJetMaker::Input& input_ ,
                                         const LHCb::RecVertex& /* vtx_ */,
                                         IJetMaker::Jets& jets_ ) const
{
  makeJets(input_, jets_);
  return StatusCode::SUCCESS ;
}

StatusCode LoKi::FastJetMaker::makeJets
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const
{

  StatusCode sc = check() ;
  if ( sc.isFailure() ) { return Error ( "Invalid configuration of fastjet" ) ; }

  // input container of "particles"
  Jets_ inputs ;
  // prepare the input dat and define the jets

  fastjet::JetDefinition jetDef = prepare ( input_ , inputs ) ;

  if ( inputs.empty() )
  {
    IJetMaker::Jets output;
    output.reserve( 0 ) ;
    jets_ = output ;
    if ( statPrint() || msgLevel ( MSG::DEBUG ) )
    { counter ( "#jets" ) += output.size() ; }
    return StatusCode::SUCCESS ;
  }
  // Jets found
  Jets_ jets ;

  fastjet::ClusterSequence* clusters = new fastjet::ClusterSequence( inputs , jetDef ) ;

  switch ( m_sort )
  {
  case 3 :
    jets = sorted_by_rapidity ( clusters->inclusive_jets ( m_ptmin ) ) ;
    break ;
  case 2 :
    jets = sorted_by_pt       ( clusters->inclusive_jets ( m_ptmin ) ) ;
    break ;
  case 1 :
    jets = sorted_by_E        ( clusters->inclusive_jets ( m_ptmin ) ) ;
    break ;
  default :
    jets = sorted_by_pt       ( clusters->inclusive_jets ( m_ptmin ) ) ;
    break ;
  }

  if ( jets.empty() )
  { Warning( "No jets from fastjet::ClusterSequence",
             StatusCode::SUCCESS, 0 ).ignore() ; }

  //
  if ( NULL == m_combiner )
  { m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ; }

  IJetMaker::Jets  output  ;
  output.reserve( jets.size() ) ;

  LoKi::Point3D    point  = LoKi::Point3D( 0 , 0 , 0 ) ;

  for ( Jets_::iterator ijet = jets.begin() ; jets.end() != ijet ; ++ijet )
  {
    const Jet& jet = *ijet ;
    const Constituents& constituents = clusters->constituents ( jet ) ;
    if ( constituents.empty() ) { Warning( "Jet is 'empty'!" ).ignore() ; }

    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;

    pJet.setParticleID     (  LHCb::ParticleID( m_jetID )) ;

    pJet.setReferencePoint ( point         ) ;

    for ( Constituents::const_iterator ic = constituents.begin() ;
          constituents.end() != ic ; ++ic )
    {
      const Jet& c = *ic ;
      // find the appropriate input particle
      const int index = from_user_index ( c.user_index() ) ;
      if ( 0 > index || (int) inputs.size() <= index )
      { Warning ( "Invalid index for a constituent!" ).ignore() ;
        continue ; } // CONTINUE
      // get the appropriate particle:
      const LHCb::Particle* p = input_[index] ;
      // add the particle into the vertex
      daughters.push_back ( p ) ;
    }
    if ( daughters.empty() )
    {
      Warning ("Empty list of of daughter particles, skip it").ignore() ;
      continue ;
    }
    // use the tool
    const StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure() )
    {
      Warning ( "Error from momentum combiner, skip", sc, 0 ).ignore() ;
      continue ;
    }
    // redefine the momentum
    pJet.setMomentum
      ( Gaudi::LorentzVector ( jet.px () ,
                               jet.py () ,
                               jet.pz () ,
                               jet.e  () ) ) ;
    //
    output.push_back ( pJet.clone() ) ;
  }


  if ( statPrint() || msgLevel ( MSG::DEBUG ) )
  { counter ( "#jets" ) += output.size() ; }

  jets_ = output ;

  delete clusters ;

  return StatusCode::SUCCESS ;
}
// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,FastJetMaker)
// ============================================================================
// The END
// ============================================================================

