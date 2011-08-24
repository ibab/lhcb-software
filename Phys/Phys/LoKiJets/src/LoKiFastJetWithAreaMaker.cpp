// $Id: LoKiFastJetWithAreaMaker.cpp,v 1.1 2007-12-10 15:38:23 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// Local 
// ============================================================================
#include "LoKiFastJetMaker.h"
// ============================================================================
// fastjet
// ============================================================================
#include "fastjet/ClusterSequenceWithArea.hh"
#include "fastjet/ClusterSequenceActiveArea.hh"
// ============================================================================
namespace LoKi 
{ 
  // ==========================================================================
  /** @class FastJetWithAreaMaker
   *
   *  The most trivial, FastJet based  implementation of interface IJetMaker 
   *  @see IJetMaker 
   *
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @author Victor COCO   coco@lapp.in2p3.fr
   *  @date   2007-10-22
   */
  class FastJetWithAreaMaker : public LoKi::FastJetMaker
  {
    // the friend factory fo instantiation 
    friend class ToolFactory<LoKi::FastJetWithAreaMaker> ;
  public:
    /** The main method: jet-finding procedure 
     * 
     *  @code 
     *
     *  // get the tool
     *  const IJetMaker* jetMaker = tool<IJetMaker> ( .... ) ;
     *
     *  // input particles 
     *  IJetMaker::Inputs input = ... 
     *  // 1) 
     *  // const Particles* particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles->begin() , particles->end() ) ;
     *  // 2) 
     *  // LHCb::Particle::ConstVector particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles.begin() , particles.end() ) ;
     *  // 3) 
     *  // LoKi::Range particles = .... ;
     *  // // create the input container 
     *  // IJetMaker::Inputs input( particles.begin() , particles.end() ) ;
     *
     *  // placeholder for "output" jets 
     *  IJetMaker::Jets   jets ;
     *
     *  // find the jets! 
     *  StatusCode sc = jetMaker -> makeJets ( input , jets ) ;
     *
     *  // make  a loop over jets:
     *  for ( IJetMaker::Jets::const_iterator iJet = jets.begin() ; 
     *        jets.end() != iJet ; ++iJet ) 
     *    {
     *        // get the jet 
     *        LHCb::Particle* jet = *iJet ;
     *    }
     *
     *  @endcode 
     *
     *  @attention It is a responsibility of users (e.g. the algorithm) 
     *             to take care about the ownership of jets *AND* their 
     *             vertices). The tool is not intended to do it! 
     *  
     *  @param input contaainer of input particles 
     *  @param jets  container of  output jets 
     *  @return status code 
     */
    virtual StatusCode makeJets 
    ( const IJetMaker::Input& input , IJetMaker::Jets& jets ) const ;
    virtual StatusCode makeJets 
    ( const IJetMaker::Input& input, const LHCb::RecVertex& vtx  , IJetMaker::Jets& jets ) const ;
    // ========================================================================
  protected:  
    /** the standard constructor
     * 
     *  @todo The default values for configuration parameters 
     *        (especially for R-parameter) need to be adjusted 
     *        according Victor Coco's studies. 
     *
     */
    FastJetWithAreaMaker
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent ) 
      : LoKi::FastJetMaker ( type , name , parent )
      // 
      // the maximal pseudorapidity for ghosts 
      , m_ghost_etamax        ( 8       ) ///< the maximal pseudorapidity for ghosts 
      // number of repetitions for active area evaluation
      , m_active_area_repeats ( 2       ) ///< number of repetitions for active area evaluation
      // the area of the ghost 
      , m_ghost_area          ( 0.01    ) ///< the area of the ghost 
      // the scatterer parameter for the grid 
      , m_grid_scatterer      ( 1.e-4   ) ///< the scatterer parameter for the grid 
      // the scatterer parameter for kt 
      , m_kt_scatterer        ( 0.1     ) ///< the scatterer parameter for kt 
      // the kt for ghosts 
      , m_mean_ghost_kt       ( 1.e-100 ) ///< the kt for ghosts 
      // pt/area strategy 
      , m_ptarea_strategy     ( fastjet::ClusterSequenceActiveArea::median )
      // pt/area strategy 
      , m_ptarea_range        ( 2 ) ///< pt/area range
    { 
      //
      declareProperty 
        ( "GhostEtaMax"  , 
          m_ghost_etamax ,
          "The maximal pseudorapidity for active area ghosts" ) ;
      //
      declareProperty 
        ( "AreaRepeat"   , 
          m_active_area_repeats ,
          "Repeate active area evaluation" ) ;          
      //
      declareProperty 
        ( "GhostArea"   , 
          m_ghost_area  ,
          "The area, associated with single ghost" ) ;
      //
      declareProperty 
        ( "GridScatterer"   , 
          m_grid_scatterer  ,
          "The fractional random fluctiatuon of ghosts on eta-phi grid" ) ;
      //
      declareProperty 
        ( "KtScatterer"   , 
          m_kt_scatterer  ,
          "The fractional random fluctiatuon of ghosts kt" ) ;
      //
      declareProperty 
        ( "GhostKt"        , 
          m_mean_ghost_kt  ,
          "The average transverse momentum of the ghosts" ) ;
      //
      declareProperty 
        ( "PtPerAreaStrategy" , 
          m_ptarea_strategy   ,
          "The strategy for pt/unit area evaluation" ) ;
      //
      declareProperty 
        ( "PtPerAreaRange" , 
          m_ptarea_range   ,
          "The range for pt/unit area evaluation" ) ;
      //
    } ;
    /// destructor
    virtual ~FastJetWithAreaMaker( ){};
  private:
    // the default constructor is disabled 
    FastJetWithAreaMaker () ;
    // the copy constructor is disabled 
    FastJetWithAreaMaker           ( const  FastJetWithAreaMaker& )  ;
    // the assignement operator is disabled
    FastJetWithAreaMaker& operator=( const  FastJetWithAreaMaker& )  ;
  private:  
    // the maximal pseudorapidity for ghosts 
    double       m_ghost_etamax        ; ///< the maximal pseudorapidity for ghosts 
    // number of repetitions for active area evaluation
    int          m_active_area_repeats ; ///< number of repetitions for active area evaluation
    // the area of the ghost 
    double       m_ghost_area          ; ///< the area of the ghosts
    // the scatterer parameter for the grid 
    double       m_grid_scatterer      ; // the scatterer parameter for the grid 
    // the scatterer parameter for kt 
    double       m_kt_scatterer        ; ///< the scatterer parameter for kt 
    // the kt for ghosts 
    double       m_mean_ghost_kt       ; ///< the kt for ghosts
    // pt/area strategy
    unsigned int m_ptarea_strategy     ; ///< pt/area strategy
    // pt/area range 
    double       m_ptarea_range        ; ///< pt/area range
  };
} // End of namespace LoKi
// ============================================================================
/** @file 
 *  Implementation file for class  LoKi::FastJetWithAreaMaker
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @author Victor COCO   coco@lapp.in2p3.fr
 *  @date   2007-10-22
 */
// ============================================================================
// find the jets 
// ===========================================================================


StatusCode LoKi::FastJetWithAreaMaker::makeJets 
( const IJetMaker::Input& input_ , const LHCb::RecVertex& vtx_ ,  IJetMaker::Jets& jets_ ) const 
{ 
  debug()<<" could not be used with a vtx"<<endmsg;
  debug()<<" vtx: "<<vtx_<<endmsg;
  makeJets(input_, jets_);
  return StatusCode::SUCCESS ;


}



StatusCode LoKi::FastJetWithAreaMaker::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{

  StatusCode sc = check() ;
  if ( sc.isFailure() ) { return Error ( "Invalid configurtaion of fastjet") ; }
  
  // input data 
  Jets_ inputs ;
  
  // prepare the input dat and define the jets 
  fastjet::JetDefinition jet_def = prepare ( input_ , inputs ) ;
  
  // Jets found 
  Jets_ jets ;
  
  // specify the active area
  fastjet::ActiveAreaSpec active_area 
    ( m_ghost_etamax        , 
      m_active_area_repeats , 
      m_ghost_area          , 
      m_grid_scatterer      , 
      m_kt_scatterer        , 
      m_mean_ghost_kt       ) ;
  
  // clusterisation sequence 
  fastjet::ClusterSequenceActiveArea clusters  // fastjet::ClusterSequenceWithArea   clusters 
    ( inputs , jet_def , active_area ) ;
  
  switch ( m_sort ) 
  {
  case 3 : 
    jets = sorted_by_rapidity ( clusters.inclusive_jets ( m_ptmin ) ) ;
    break ;
  case 2 : 
    jets = sorted_by_pt       ( clusters.inclusive_jets ( m_ptmin ) ) ;
    break ;
  case 1 : 
    jets = sorted_by_E        ( clusters.inclusive_jets ( m_ptmin ) ) ;
    break ;
  default : 
    jets = sorted_by_pt       ( clusters.inclusive_jets ( m_ptmin ) ) ;
    break ;
  }
  
  if ( jets.empty() ) 
  {  Warning ( "No jets from fastjet::ClusterSequenceWithArea" ) ; }
  
  //
  if ( 0 == m_combiner ) 
  { m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ; }
  
  IJetMaker::Jets  output  ;
  output.reserve( jets.size() ) ;
  
  LoKi::Point3D    point  = LoKi::Point3D( 0 , 0 , 0 ) ;
  
  // get the "pt per unit area" estimate
  const double ptPerUnitArea = clusters.pt_per_unit_area 
    ( (fastjet::ClusterSequenceActiveArea::mean_pt_strategies) m_ptarea_strategy , 
      m_ptarea_range ) ;
  
  for ( Jets_::iterator ijet = jets.begin() ; jets.end() != ijet ; ++ijet ) 
  {
    const Jet& jet = *ijet ;
    const Constituents& constituents = clusters.constituents ( jet ) ;
    if ( constituents.empty() ) { Warning ( "Jet is 'empty'!" ) ; }
    
    LHCb::Particle              pJet, pJetArea ;
    LHCb::Vertex                vJet ;
    LHCb::Particle::ConstVector daughters ;
    
    pJet     .setParticleID     (  LHCb::ParticleID (      m_jetID ) ) ;
    pJetArea .setParticleID     (  LHCb::ParticleID ( -1 * m_jetID ) ) ;
    
    pJet     .setReferencePoint ( point ) ;
    pJetArea .setReferencePoint ( point ) ;
    
    // set the jet active area 
    pJet.addInfo ( LHCb::Particle::JetActiveArea      , clusters.area       ( jet ) ) ;
    // set the jet active area uncertainty  
    pJet.addInfo ( LHCb::Particle::JetActiveAreaError , clusters.area_error ( jet ) ) ;
    // get active area 4-vector
    fastjet::PseudoJet area = clusters.area_4vector ( jet ) ;
    pJet.addInfo ( LHCb::Particle::JetActiveAreaPx    , area.px ()    ) ;
    pJet.addInfo ( LHCb::Particle::JetActiveAreaPy    , area.py ()    ) ;
    pJet.addInfo ( LHCb::Particle::JetActiveAreaPz    , area.pz ()    ) ;
    pJet.addInfo ( LHCb::Particle::JetActiveAreaE     , area.e  ()    ) ;
    pJet.addInfo ( LHCb::Particle::JetPtPerUnitArea   , ptPerUnitArea ) ;
    
    for ( Constituents::const_iterator ic = constituents.begin() ; 
          constituents.end() != ic ; ++ic ) 
    {
      const Jet& c = *ic ;
      // find the appropriate input particle 
      const int index = from_user_index( c.user_index() ) ;
      if ( 0 > index || (int) inputs.size() <= index ) 
      { Warning ( "Invalid index for a constituent!" ) ; continue ; } // CONTINUE 
      // get the appropriate particle:
      const LHCb::Particle* p = input_[index] ;
      // add the particle into the vertex 
      daughters.push_back ( p ) ;
    }
    if ( daughters.empty() ) 
    {
      Warning ("Empty list of of daughter particles, skip it") ;
      continue ;
    }
    // use the tool 
    StatusCode sc = m_combiner -> combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Error from momentum combiner, skip" , sc ) ;
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
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,FastJetWithAreaMaker)
// ============================================================================
// The END 
// ============================================================================
