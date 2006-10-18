// $Id: LoKiKtJetMaker.cpp,v 1.2 2006-10-18 12:03:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.2 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// LoKiAlgo
// ============================================================================
#include "LoKi/Algo.h"
#include "LoKi/ParticleCuts.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/IParticleCombiner.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// KtJet
// ============================================================================
#include "KtJet/KtLorentzVector.h"
#include "KtJet/KtEvent.h"
// ============================================================================
/** @class LoKiKtJetMaker KtJetMaker.cpp
 *  
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  "No Vanya's lines are allowed in LHCb/Gaudi software."
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2005-03-21
 */
// ============================================================================
class LoKiKtJetMaker : public LoKi::Algo  
{
  friend class AlgFactory<LoKiKtJetMaker> ;
protected:  
  /** Standard constructor
   *  @param name instance name 
   *  @param pSvc pointer to Service Locator 
   */
  LoKiKtJetMaker
  ( const std::string& name ,
    ISvcLocator*       pSvc ) 
    : LoKi::Algo( name , pSvc )
    // 
    , m_jetID    ( 98 )
    //
    , m_type     ( 4   )  
    , m_angle    ( 1   )
    , m_recom    ( 1   )
    , m_r        ( 1.0 )
    //
    , m_sort     ( 2   )
    // 
    , m_combiner ( 0   )
  { 
    declareProperty ( "JetID"          , m_jetID  ) ;
    //
    declareProperty ( "Type"           , m_type   ) ;
    declareProperty ( "AngularScheme"  , m_angle  ) ;
    declareProperty ( "Recombination"  , m_recom  ) ;
    declareProperty ( "RParameter"     , m_r      ) ;
    //
    declareProperty ( "Sort"           , m_sort   ) ;
  } ;
  /// destructor
  virtual ~LoKiKtJetMaker( ){};
public:
  /** standard initialization of the algorithm 
   *  @see LoKi::Algo
   *  @return status code 
   */
  virtual StatusCode initialize () 
  {
    StatusCode sc = LoKi::Algo::initialize() ;
    if ( sc.isFailure() ) { return sc ; }
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
    m_combiner = particleCombiner() ;
    if ( 0 == m_combiner ) 
    { return Error ( "No particle combiner is defined!" );  }
    else 
    {
      info()
        << " IParticleCombiner available type/name:"
        << "'" << m_combiner->type() << "'/'" 
        << m_combiner->name() <<"'" << endreq;
    }
    //
    return StatusCode::SUCCESS ;
  } ;
  /** standard execution of the algorithm 
   *  @see LoKi::Algo 
   *  @return status code 
   */
  virtual StatusCode analyse   () ;
private:
  // the default constructor is disabled 
  LoKiKtJetMaker () ;
  // the copy constructor is disabled 
  LoKiKtJetMaker ( const  LoKiKtJetMaker& )  ;
  // the assignement operator is disabled 
  LoKiKtJetMaker& operator=( const  LoKiKtJetMaker& )  ;
private:  
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
  // jet sorting criteria 
  int    m_sort      ; ///< jet sorting criteria
  // combiner 
  const IParticleCombiner* m_combiner ; ///< combiner to be used 
};
// ============================================================================
/** @file 
 *  Implementation file for class  LoKiKtJetMaker
 *  @date  2005-03-21 
 *  @author Vanya BELYAEV  belyaev@lapp.in2p3.fr
 */
// ============================================================================
DECLARE_ALGORITHM_FACTORY( LoKiKtJetMaker );
// ============================================================================
namespace 
{  
  typedef KtJet::KtLorentzVector   Jet          ;
  typedef std::vector<Jet>         Jets         ;
  typedef std::vector<const Jet*>  Constituents ;
  /// trivial function which "converts" particle into the "jet"
  inline KtJet::KtLorentzVector makeJet 
  ( const LHCb::Particle* p ) 
  {
    if ( 0 == p ) { return KtJet::KtLorentzVector() ; }
    //
    const Gaudi::LorentzVector& v = p->momentum() ;
    //
    return KtJet::KtLorentzVector ( v.Px() , v.Py() , v.Pz() , v.E () ) ;
  };
}
// ===========================================================================
/** standard execution of the algorithm 
 *  @see LoKi::Algo 
 *  @return status code 
 */
// ===========================================================================
StatusCode LoKiKtJetMaker::analyse   () 
{
  using namespace LoKi        ;
  using namespace LoKi::Types ;
  // select all input particles
  Range all = select ( "all" , LoKi::Cuts::PALL ) ;
  // input container of "particles"
  Jets inputs ;
  inputs.reserve( all.size() ) ;
  for ( Range::iterator ip = all.begin() ; all.end() != ip ; ++ip ) 
  {
    const LHCb::Particle* p = *ip ;
    if ( 0 == p ) 
    { 
      Warning ( "Invalid input particle, jet history could be corrupted") ; 
      continue ; 
    }
    inputs.push_back( makeJet( p )  ) ;
  } ;
  if ( inputs.empty() ) 
  { Warning ( "No particles are available for jet-finding algorithm!") ; } ;
  
  // use "inclusive" jet finding 
  KtJet::KtEvent event ( inputs  ,    // input particles  
                         m_type  ,    // type
                         m_angle ,    // angular distance scheme  
                         m_recom ,    // recombination scheme
                         m_r     ) ;  // R-parameter
  
  // get jets in sorted order 
  Jets jets = 
    ( 5 == m_sort ) ? event.getJetsEta       () : // sorted by pseudorapidity
    ( 4 == m_sort ) ? event.getJetsRapidity  () : // sorted by rapidity 
    ( 3 == m_sort ) ? event.getJetsPt        () : // sorted by Pt 
    ( 2 == m_sort ) ? event.getJetsEt        () : // sorted by Et 
    ( 1 == m_sort ) ? event.getJetsE         () : // sorted by E  
    ( 0 == m_sort ) ? event.getJets          () : // unsorted 
    event.getJets () ;                           // unsorted 
  
  if ( msgLevel( MSG::DEBUG ) )  
  {
    debug() << "KtJets: "
            << " #Jets:  "  << event.getNJets         () 
            << " Etot: "    << event.getETot          () / Gaudi::Units::GeV 
            << " Input: "   << event.getNConstituents () 
            << " Type: "    << event.getType          () 
            << " Angle: "   << event.getAngle         () 
            << " Recom: "   << event.getRecom         () 
            << " Incl: "    << event.isInclusive      () << endreq ;
  };
  
  LoKi::Point3D    point  = LoKi::Point3D( 0 , 0 , 0 ) ;
  for ( Jets::iterator ijet = jets.begin() ; jets.end() != ijet ; ++ijet ) 
  {
    const Jet& jet = *ijet ;
    const Constituents& constituents = jet.getConstituents() ;
    if ( constituents.empty() ) { Warning ( "Jet is 'empty'!" ) ; }
    
    LHCb::Particle              pJet ;
    LHCb::Vertex                vJet      ;
    LHCb::Particle::ConstVector daughters ;
    
    pJet.setParticleID     ( m_jetID       ) ;
    
    pJet.setReferencePoint ( point         ) ;
    
    for ( Constituents::const_iterator ic = constituents.begin() ; 
          constituents.end() != ic ; ++ic ) 
    {
      const Jet* c = *ic ;
      if ( 0 == c ) { continue ; }
      // find an appropriate input particle 
      Jets::const_iterator input = 
        std::find ( inputs.begin () , inputs.end   ()   , *c ) ;
      if ( inputs.end() == input ) 
      { Warning ( "Constituent is not found" ) ; continue ; };
      const size_t index = input - inputs.begin() ;
      if ( index >= all.size() ) 
      { Warning ( "Illegal Constituent is found" ) ; continue ; };
      // get the particle 
      const LHCb::Particle* p = all[index] ;
      // add the particle into the vertex 
      daughters.push_back ( p ) ;
    }
    // use the tool 
    StatusCode sc = m_combiner->combine ( daughters , pJet , vJet ) ;
    if ( sc.isFailure() ) 
    {
      Error ( "Error form momentum combiner, skip" , sc ) ;
      continue ;
    }
    // redefine the momentum 
    pJet.setMomentum        ( Gaudi::LorentzVector ( jet )  ) ;
    //
    save ( "jet" , &pJet ) ;  
  };  
  // some decoration 
  Range ktjets = selected("jet") ;
  if ( ktjets.empty() ) { Warning ( "No Kt-Jets are found!" ) ; }
  if ( statPrint() || msgLevel ( MSG::DEBUG ) ) 
  { counter ( "#jets" ) += ktjets.size() ; }
  
  setFilterPassed ( true ) ;
  
  return StatusCode::SUCCESS ;
};
// ===========================================================================

// ============================================================================
// The END 
// ============================================================================
