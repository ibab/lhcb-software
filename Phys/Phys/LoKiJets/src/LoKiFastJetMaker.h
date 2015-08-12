// $Id: LoKiFastJetMaker.h,v 1.2 2009-12-14 12:34:33 cocov Exp $
// ============================================================================
#ifndef LOKIFASTJETMAKER_H
#define LOKIFASTJETMAKER_H 1
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
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
// FastJet
// ============================================================================
#include "fastjet/PseudoJet.hh"
#include "fastjet/JetDefinition.hh"
// ============================================================================
namespace LoKi
{
  // ==========================================================================
  /** @class FastJetMaker
   *
   *  The most trivial, FastJet based  implementaion of interface IJetMaker
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
  class FastJetMaker
    : public virtual IJetMaker
    , public         GaudiTool
  {
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
    /** the standard constructor
     *
     *  @todo The default values for configuration parameters
     *        (especially for R-parameter) need to be adjusted
     *        according Victor Coco's studies.
     *
     */
    FastJetMaker
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent )
      : GaudiTool ( type , name , parent )
      //
      , m_jetID        ( 98  )
      //
      , m_type         ( 0   )  
      , m_recom        ( fastjet::E_scheme   )
      , m_r            ( 0.7 )
      , m_ptmin        ( 0   )
      //
      , m_sort         ( 2   )
      //
      , m_strat        ( fastjet::Best  )
      //
      , m_combinerName ( "MomentumCombiner"  )
      , m_combiner     ( 0   )
    {
      //
      declareInterface <IJetMaker> ( this ) ;
      //
      declareProperty
        ( "JetID"          ,
          m_jetID          ,
          "Particle ID for the Jet") ;
      //
      declareProperty
        ( "Type"           ,
          m_type           ,
          "The type of the algorithm [0:kt,1:Cambridge,2:anti-kt,default:kt]") ;
      declareProperty
        ( "Recombination"  ,
          m_recom          ,
          "Recombination scheme: fastjet::RecombinationScheme") ;
      declareProperty ( "RParameter"     , m_r      ) ;
      declareProperty ( "PtMin"          , m_ptmin  ) ;
      //
      declareProperty
        ( "Sort"             ,
          m_sort             ,
          "Sorting Criteria for jets [3:eta,2:pt,1:E,default:pt]" ) ;
      declareProperty
        ( "Strategy"         ,
          m_strat            ,
          "JetFinding strategy, see fastjet::Strategy " ) ;
      // define momentum combiner
      declareProperty ( "ParticleCombiner", m_combinerName ) ;
    }
    /// destructor
    virtual ~FastJetMaker( ){}
  public:
    /** standard initialization of the tool
     *  @return status code
     */
    virtual StatusCode initialize () ;
  protected:
    /// make the detailed check of all parameters
    inline StatusCode  check() const
    {
      // verify the algorithm type
      switch ( m_type )
      {
      case fastjet::kt_algorithm        : break ;
      case fastjet::cambridge_algorithm : break ;
      case fastjet::antikt_algorithm : break ;
      case fastjet::genkt_algorithm : break ;
      case fastjet::cambridge_for_passive_algorithm : break ;
      case fastjet::genkt_for_passive_algorithm : break ;
      case fastjet::plugin_algorithm : break ;
      default:
        return Error ( "Invalid JetFinder-algorithm is specified") ;
      }
      // verify the recombination sceme
      switch ( m_recom )
      {
      case fastjet::E_scheme     : break ;
      case fastjet::pt_scheme    : break ;
      case fastjet::pt2_scheme   : break ;
      case fastjet::Et_scheme    : break ;
      case fastjet::Et2_scheme   : break ;
      case fastjet::BIpt_scheme  : break ;
      case fastjet::BIpt2_scheme : break ;
      default:
        return Error ( "Invalid RecombinationScheme is specified") ;        
      }
      // verify the strategy
      switch ( m_strat )
      {  
        /// experimental ...
      case fastjet::N2MinHeapTiled  : break ;
        /// fastest from about 50..10^4
      case fastjet::N2Tiled         : break ;
        /// legacy
      case fastjet::N2PoorTiled     : break ;
        /// fastest below 50
      case fastjet::N2Plain         : break ;
        /// worse even than the usual N^3 algorithms
      case fastjet::N3Dumb          : break ;
        /// automatic selection of the best (based on N)
      case fastjet::Best            : break ;
        /// best of the NlnN variants -- best overall for N>10^4
      case fastjet::NlnN            : break ;
        /// legacy N ln N using 3pi coverage of cylinder
      case fastjet::NlnN3pi         : break ;
        /// legacy N ln N using 4pi coverage of cylinder
      case fastjet::NlnN4pi         : break ;
        /// Chan's closest pair method (in a variant with 4pi coverage),
        /// for use exclusively wit h the Cambridge algorithm
      case fastjet::NlnNCam4pi      : break ;
      case fastjet::NlnNCam2pi2R    : break ;
      case fastjet::NlnNCam         : break ;  // 2piMultD
        /// the plugin has been used...
      case fastjet::plugin_strategy : break ;
      default:
        return Error ( "Invalid Strategy is specified") ;        
      }
      // check the minimum momentum
      if ( 0 > m_ptmin ) { Warning ( "PtMin is negative " ) ; }
      //
      return StatusCode( StatusCode::SUCCESS , true ) ;
    }
  protected:
    // prepare the input information
    fastjet::JetDefinition prepare
    ( const IJetMaker::Input&          input ,
      std::vector<fastjet::PseudoJet>& jets  ) const ;
    //
    int to_user_index   ( const int index ) const { return index + 10000 ; }
    int from_user_index ( const int index ) const { return index - 10000 ; }
  private:
    // the default constructor is disabled
    FastJetMaker () ;
    // the copy constructor is disabled
    FastJetMaker           ( const  FastJetMaker& )  ;
    // the assignement operator is disabled
    FastJetMaker& operator=( const  FastJetMaker& )  ;
  protected:
    // proposed jet ID
    int    m_jetID     ; ///< proposed jet ID
    // KtEvent flag
    int    m_type      ; ///< KtEvent flag/mode
    // Angular distance scheme  
    int    m_angle     ; ///< angular distance scheme
    // Recombination scheme  
    int    m_recom     ; ///< recombination scheme
    // R-parameter
    double m_r         ; ///< R-parameters
    // ptMin-parameter
    double m_ptmin     ; ///< pt-min parameter
    // jet sorting criteria
    int    m_sort      ; ///< jet sorting criteria
    // jet finding strategy
    int    m_strat    ; ///< jet finding strategy
    // combiner
    std::string                m_combinerName ;
    mutable IParticleCombiner* m_combiner ; ///< combiner to be used
  };
  // ==========================================================================
  typedef fastjet::PseudoJet       Jet          ;
  typedef std::vector<Jet>         Jets_        ;
  typedef std::vector<Jet>         Constituents ;
  /// trivial function which "converts" particle into the "jet"
  inline Jet makeJet
  ( const LHCb::Particle* p ,
    const int index         )
  {
    if ( 0 == p ) { return Jet() ; }
    const Gaudi::LorentzVector& v = p->momentum() ;
    Jet jet ( v.Px() , v.Py() , v.Pz() , v.E () ) ;
    jet.set_user_index ( index ) ;
    return jet ;
  }
} // End of namespace LoKi
// ============================================================================
// The END
// ============================================================================
#endif // LOKIFASTJETMAKER_H
// ============================================================================

  
