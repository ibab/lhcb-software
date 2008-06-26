// $Id: LoKiKtJetMaker.cpp,v 1.12 2008-06-26 13:21:57 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
#include "Kernel/IJetMaker.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "LoKi/Kinematics.h"
#include "LoKi/Geometry.h"
// ============================================================================
// KtJet
// ============================================================================
#include "KtJet/KtLorentzVector.h"
#include "KtJet/KtEvent.h"
// ============================================================================
namespace LoKi 
{ 
  // ==========================================================================
  /** @class KtJetMaker
   *
   *  The most trivial, Kt-Jet based  implementtaion of interface IJetMaker 
   *  @see IJetMaker 
   *
   *  @warning This class will be removed rather soon, 
   *           use class LoKi::FastJetMaker or LoKi::FastJetMAkerWithArea
   *        
   *
   *  This file is a part of LoKi project - 
   *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
   *
   *  The package has been designed with the kind help from
   *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
   *  contributions and advices from G.Raven, J.van Tilburg, 
   *  A.Golutvin, P.Koppenburg have been used in the design.
   *
   *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
   *  @date   2005-03-21
   */
  class KtJetMaker 
    : public virtual IJetMaker
    , public         GaudiTool
  {
    // ========================================================================
    // the friend factory fo instantiation 
    friend class ToolFactory<LoKi::KtJetMaker> ;
    // ========================================================================
  public:
    // ========================================================================
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
    // ========================================================================
  protected:  
    // ========================================================================
    /** the standard constructor
     * 
     *  @todo The default values for configuration parameters 
     *        (especially for R-parameter) need to be adjusted 
     *        according Victor Coco's studies. 
     *
     */
    KtJetMaker
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent ) 
      : GaudiTool ( type , name , parent )
        // 
      , m_jetID        ( 98 )
        //
      , m_type         ( 4   )  
      , m_angle        ( 1   )
      , m_recom        ( 1   )
      , m_r            ( 0.6 )
        //
      , m_sort         ( 2   )
      // 
      , m_combinerName ( "MomentumCombiner"  )
      , m_combiner     ( 0   )
    { 
      //
      declareInterface <IJetMaker> ( this ) ;
      //
      declareProperty 
        ( "JetID" , m_jetID , 
          "Particle ID to be used for the constructed jets" ) ;
      //
      declareProperty
        ( "Type" , m_type , "The type of jets" ) ;
      //
      declareProperty 
        ( "AngularScheme" , m_angle , "Angular scheem to be used " ) ;
      //
      declareProperty 
        ( "Recombination"  , m_recom , 
          "Momentum recombination scheme to be used " ) ;
      //
      declareProperty 
        ( "RParameter" , m_r , "R-parameter to be used" ) ;
      //
      declareProperty 
        ( "Sort" , m_sort , "Sorting of the found jets " ) ;
      // define momentum combiner
      declareProperty 
        ( "ParticleCombiner"                   , 
          m_combinerName                       , 
          "IParticleCombiner toll to be used " ) ;
    } 
    /// destructor
    virtual ~KtJetMaker( ){}
    // ========================================================================
  public:
    // ========================================================================
    /** standard initialization of the tool
     *  @return status code 
     */
    virtual StatusCode initialize () 
    {
      StatusCode sc = GaudiTool::initialize() ;
      if ( sc.isFailure() ) { return sc ; }
      //
      Warning ( "This class is obsolete!" ) ;
      //
      if ( 4 != m_type      ) 
      { Warning ( "The 'type' is different from 'PP' [4] "  ) ; }
      if ( 0 >= m_angle || 3 < m_angle ) 
      { Warning ( "Unknown angular distance scheme   [1-3]" ) ; }
      if ( 0 >= m_recom || 5 < m_recom ) 
      { Warning ( "Unknown recombination scheme      [1-5]" ) ; }
      if ( 0 >  m_sort  || 5 < m_sort ) 
      { Warning ( "Unknown sorting  scheme           [0-5]" ) ; }
      //
      return StatusCode::SUCCESS ;
    } ;
    // ========================================================================
  private:
    // ========================================================================
    // the default constructor is disabled 
    KtJetMaker () ;
    // the copy constructor is disabled 
    KtJetMaker ( const  KtJetMaker& )  ;
    // the assignement operator is disabled
    KtJetMaker& operator=( const  KtJetMaker& )  ;
    // ========================================================================
  private:  
    // ========================================================================
    /// proposed jet ID 
    int    m_jetID     ; // proposed jet ID
    /// KtEvent flag 
    int    m_type      ; // KtEvent flag/mode
    /// Angular distance scheme  
    int    m_angle     ; // angular distance scheme
    /// Recombination scheme  
    int    m_recom     ; // recombination scheme
    /// R-parameter 
    double m_r         ; // R-parameters
    /// jet sorting criteria 
    int    m_sort      ; // jet sorting criteria
    /// combiner 
    std::string                m_combinerName ;
    mutable IParticleCombiner* m_combiner ; // combiner to be used 
    // ========================================================================
  };
  // ==========================================================================
} // End of namespace LoKi
// ============================================================================
/** @file 
 *  Implementation file for class  LoKiKtJetMaker
 *  @date  2005-03-21 
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
namespace 
{  
  // ==========================================================================
  typedef KtJet::KtLorentzVector   Jet          ;
  typedef std::vector<Jet>         Jets_        ;
  typedef std::vector<const Jet*>  Constituents ;
  // ==========================================================================
  /// trivial function which "converts" particle into the "jet"
  inline Jet makeJet ( const LHCb::Particle* p ) 
  {
    if ( 0 == p ) { return Jet() ; }
    const Gaudi::LorentzVector& v = p->momentum() ;
    return Jet( v.Px() , v.Py() , v.Pz() , v.E () ) ;
  } 
  // ==========================================================================
  struct EuclidianNorm2 : public std::unary_function<Jet,double> 
  {
    inline double operator() ( const Jet& v ) const 
    {
      return 
        v.e  () * v.e  () + 
        v.px () * v.px () + 
        v.py () * v.py () + 
        v.pz () * v.pz () ; 
    } 
  } ;
  // ==========================================================================
  struct SmallEuclidianNorm2 : public std::binary_function<Jet,Jet,bool>
  {
    // constructor 
    SmallEuclidianNorm2 ( const double cut ) : m_cut ( cut )  , m_norm () {}
    // the main method 
    inline bool operator() ( const Jet& v1 ,  const Jet& v2 ) const 
    {
      if ( &v1 == &v2 ) { return true ; }
      const double  value = m_cut * std::min ( m_norm ( v1 )  , m_norm ( v2 ) )  ;
      return  m_norm ( v1 - v2 ) <= value  ;
    } 
  private:
    SmallEuclidianNorm2 ( ) ;
  private:
    double             m_cut  ;
    EuclidianNorm2     m_norm ;
  } ;
  // ==========================================================================
  class CompareByEuclidianNorm2 : public std::binary_function<Jet,Jet,double>
  {
  public:
    // constructor 
    CompareByEuclidianNorm2 ( const Jet* v ) : m_norm () , m_v ( v ) {}
    // evaluator 
    inline double operator() ( const Jet& v1 , const Jet& v2 ) const 
    { return m_norm ( v1 - (*m_v) ) < m_norm ( v2 - (*m_v) ) ; }
  private:
    // default constructor is disabled 
    CompareByEuclidianNorm2() ;
  private:
    EuclidianNorm2 m_norm ;
    const Jet*     m_v    ;
  } ; 
} // end of anonymous namespace 
// ===========================================================================
/// find the jets 
// ===========================================================================
StatusCode LoKi::KtJetMaker::makeJets 
( const IJetMaker::Input& input_ , IJetMaker::Jets& jets_ ) const 
{
  // input container of "particles"
  Jets_ inputs ;
  inputs.reserve( input_.size() ) ;
  for ( IJetMaker::Input::const_iterator ip = input_.begin() ; input_.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) 
    { 
      Warning ( "Invalid input particle, jet history could be corrupted") ; 
      continue ; 
    }
    inputs.push_back( makeJet( p )  ) ;
  } 
  
  // Jets found 
  Jets_ jets ;
  
  if ( inputs.empty() ) 
  { Warning ( "No particles are available for jet-finding algorithm!") ; } 
  else 
  {
    // use "inclusive" jet finding 
    KtJet::KtEvent event ( inputs  ,    // input particles  
                           m_type  ,    // type
                           m_angle ,    // angular distance scheme  
                           m_recom ,    // recombination scheme
                           m_r     ) ;  // R-parameter
    
    // extract ordered jets
    switch ( m_sort ) 
    {
    case  5 : 
      jets = event.getJetsEta       () ; // sorted by pseudorapidity
      break ; 
    case 4 : 
      jets = event.getJetsRapidity  () ; // sorted by rapidity 
      break ; 
    case 3 : 
      jets = event.getJetsPt        () ; // sorted by Pt
      break ; 
    case 2 : 
      jets = event.getJetsEt        () ; // sorted by Et
      break ; 
    default: 
      jets = event.getJetsE         () ; // sorted by E      
      break ; 
    }  
    
    if ( msgLevel ( MSG::DEBUG ) )  
    {
      debug() << "KtJets: "
              << " #Jets:  "  
              << event.getNJets         () 
              << " Etot: "    
              << event.getETot          () / Gaudi::Units::GeV 
              << " Input: "   
              << event.getNConstituents () 
              << " Type: "    
              << event.getType          () 
              << " Angle: "   
              << event.getAngle         () 
              << " Recom: "   
              << event.getRecom         () 
              << " Incl: "   
              << Gaudi::Utils::toString( event.isInclusive () ) << endreq ;
    }  
  }
  
  if ( jets.empty() ) {  Warning ( "No jets from KtEvent" ) ; }
  
  //
  if ( 0 == m_combiner ) 
  { m_combiner = tool<IParticleCombiner> ( m_combinerName , this ) ; }
  
  const       SmallEuclidianNorm2 ok ( 1.e-5 ) ;
  typedef CompareByEuclidianNorm2 CMP ;
  IJetMaker::Jets ktjets ;
  ktjets.reserve( jets.size() ) ;
  
  LoKi::Point3D    point  = LoKi::Point3D( 0 , 0 , 0 ) ;
  for ( Jets_::iterator ijet = jets.begin() ; jets.end() != ijet ; ++ijet ) 
  {
    const Jet& jet = *ijet ;
    const Constituents& constituents = jet.getConstituents() ;
    if ( constituents.empty() ) { Warning ( "Jet is 'empty'!" ) ; }
    
    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;
    
    pJet.setParticleID     (  LHCb::ParticleID( m_jetID )) ;
    
    pJet.setReferencePoint ( point         ) ;
    
    for ( Constituents::const_iterator ic = constituents.begin() ; 
          constituents.end() != ic ; ++ic ) 
    {
      const Jet* c = *ic ;
      if ( 0 == c ) { continue ; }
      // find an appropriate input particle 
      Jets_::const_iterator input = 
        std::min_element ( inputs.begin() , inputs.end() , CMP ( c ) ) ;
      if ( inputs.end() == input ) 
      { Warning ( "Constituent is not found (1)" ) ; continue ; } ;
      if ( !ok ( *input , *c ) ) 
      { Warning ( "Constituent is not found (2)" ) ; continue ; } ;
      const size_t index = input - inputs.begin() ;
      if ( index >= input_.size() ) 
      { Warning ( "Illegal Constituent is found" ) ; continue ; };
      // get the particle 
      const LHCb::Particle* p = input_[index] ;
      // add the particle into the vertex 
      daughters.push_back ( p ) ;
    }
    if ( daughters.empty() ) 
    {
      Warning("Empty list of of daughter particles, skip it") ;
      continue ;
    }
    // use the tool 
    StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Error from momentum combiner, skip" , sc ) ;
      continue ;
    }
    // redefine the momentum 
    pJet.setMomentum        ( Gaudi::LorentzVector ( jet )  ) ;
    //
    ktjets.push_back( pJet.clone() ) ;
  }  
  
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#jets" ) += ktjets.size() ; }
  
  jets_ = ktjets;
  
  return StatusCode::SUCCESS ;
}
// ============================================================================
/// The factory
DECLARE_NAMESPACE_TOOL_FACTORY(LoKi,KtJetMaker)
// ============================================================================
// The END 
// ============================================================================
