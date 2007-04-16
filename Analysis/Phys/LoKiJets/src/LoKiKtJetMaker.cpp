// $Id: LoKiKtJetMaker.cpp,v 1.6 2007-04-16 16:16:44 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.5  2007/03/27 11:21:18  ibelyaev
//  v2r3:  ugly patch to solve the proplem with copy constructor
//
// Revision 1.4  2007/03/04 16:50:53  ibelyaev
//  v2r2: improve selection of particles
//
// Revision 1.3  2006/11/12 15:13:36  ibelyaev
//  fix many bugs + add configuration files for 'standard jets'
//
// Revision 1.2  2006/10/18 12:03:31  ibelyaev
//  fix a bug, RE-tag!
//
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
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr
 *  @date   2005-03-21
 */
// ============================================================================
class LoKiKtJetMaker : public LoKi::Algo  
{
  friend class AlgFactory<LoKiKtJetMaker> ;
protected:  
  /** Standard constructor
   * 
   *  @todo The defautl values for configuration parameters 
   *        (especially for R-parameter) need to be adjusted 
   *        according Victor Coco's studies. 
   *
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
    , m_r        ( 0.7 )
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
    // define momentum combiner
    setProperty     ( "ParticleCombiners", "{ '' : 'MomentumCombiner'}" ) ;
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
        << " IParticleCombiner tobe used: "
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
  inline Jet makeJet ( const LHCb::Particle* p ) 
  {
    if ( 0 == p ) { return Jet() ; }
    const Gaudi::LorentzVector& v = p->momentum() ;
    return Jet( v.Px() , v.Py() , v.Pz() , v.E () ) ;
  } ;
  //
  struct EuclidianNorm2 : public std::unary_function<Jet,double> 
  {
    inline double operator() ( const Jet& v ) const 
    {
      return 
        v.e  () * v.e  () + 
        v.px () * v.px () + 
        v.py () * v.py () + 
        v.pz () * v.pz () ; 
    } ;
  } ;
  //
  struct SmallEuclidianNorm2 : public std::binary_function<Jet,Jet,bool>
  {
    // constructor 
    SmallEuclidianNorm2 ( const double cut ) : m_cut ( cut )  , m_norm () {}
    // the main method 
    inline bool operator() ( const Jet& v1 ,  const Jet& v2 ) const 
    {
      if ( &v1 == &v2 ) { return true ; }
      const double  value = m_cut * std::min ( m_norm ( v1 )  , m_norm ( v2 ) )  ;
      return  m_norm ( v1 - v2 ) <= value ;
    } ;
  private:
    SmallEuclidianNorm2 ( ) ;
  private:
    double             m_cut  ;
    EuclidianNorm2     m_norm ;
  } ;
  
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
  
  // Jets found 
  Jets jets ;
  
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
  const       SmallEuclidianNorm2 ok ( 1.e-5 ) ;
  typedef CompareByEuclidianNorm2 CMP ;
  
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
        std::min_element ( inputs.begin() , inputs.end() , CMP ( c ) ) ;
      if ( inputs.end() == input ) 
      { Warning ( "Constituent is not found (1)" ) ; continue ; } ;
      if ( !ok ( *input , *c ) ) 
      { Warning ( "Constituent is not found (2)" ) ; continue ; } ;
      const size_t index = input - inputs.begin() ;
      if ( index >= all.size() ) 
      { Warning ( "Illegal Constituent is found" ) ; continue ; };
      // get the particle 
      const LHCb::Particle* p = all[index] ;
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
