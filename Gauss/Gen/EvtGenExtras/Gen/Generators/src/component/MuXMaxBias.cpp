// $Id: MuXMaxBias.cpp,v 1.1.1.1 2009-01-20 13:27:50 wreece Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.1.1.1 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2007/01/12 15:17:39  ranjard
// v7r0 - use GAUDI v19r0
//
// Revision 1.1  2006/05/08 15:42:43  ibelyaev
//  really add new tool
// 
// ============================================================================
// Include files 
// ============================================================================
// STD&STD
// ============================================================================
#include <vector>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/DeclareFactoryEntries.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// Phys/LoKiCore 
// ============================================================================
#include "LoKi/ILoKiSvc.h"
// ============================================================================
// Phys/LoKiGen 
// ============================================================================
#include "LoKi/GenExtract.h"
#include "LoKi/GenKinematics.h"
#include "LoKi/GenParticleCuts.h"
// ============================================================================
// Gen/Generators 
// ============================================================================
#include "Generators/IFullGenEventCutTool.h"
// ============================================================================

// ============================================================================
/** @file 
 *
 *  Implemenation of "Generator Cuts" for generator selection of 
 *  "maximum-bias" sample for Bs -> mu+ mu- background.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-02 
 */
// ============================================================================
/** @class MuXMaxBias 
 *
 * "Generator Cuts" for selection of dangerouse 
 * backround events fro Bs- >mu+ mu- background 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-02 
 */
// ============================================================================

using namespace LoKi::Cuts   ;
using namespace LoKi::Types  ;

class MuXMaxBias 
  : public GaudiTool
  , public virtual IFullGenEventCutTool
{
  /// friend factory for instantiation
  friend class ToolFactory<MuXMaxBias> ;
public:
  /// standard initialization of the tool 
  virtual StatusCode initialize() ;
  /** Apply the cut on a event.
   *  @param[in] theEvents      Container of all interactions in the event.
   *  @param[in] theCollisions  Container of hard process informations of each 
   *                            pile-up interactions of the event.
   *  @return    true  if the full event passes the cut.
   *  @see IFullGnEventCutTool 
   */  
  virtual bool studyFullEvent
  ( LHCb::HepMCEvents*    theEvents     ,
    LHCb::GenCollisions * theCollisions ) const ;
protected:
  /** standard constructor 
   *  @param type   tool type (?)
   *  @param name   tool name 
   *  @param parent tool owner 
   *  @see GaudiTool 
   */
  MuXMaxBias 
  ( const std::string& type   , ///< tool type 
    const std::string& name   , ///< tool instance name 
    const IInterface*  parent ) ///< tool owner 
    : GaudiTool( type , name , parent ) 
    // "Theta  Min" cut value 
    , m_cutThetaMin  ( 0.018  )  ///< "Theta  Min" cut value 
    // "Theta  Max" cut value 
    , m_cutThetaMax  ( 0.300  )  ///< "Theta  Max" cut value 
    // "ThetaY Max" cut value 
    , m_cutThetaYMax ( 0.250  )  ///< "ThetaY Max" cut value 
    // "Min P" cut value 
    , m_cutPMin      ( 5   * CLHEP::GeV )  ///< "Min P" cut value 
    // "Min PT" cut value 
    , m_cutPtMin     ( 1.3 * CLHEP::GeV )  ///< "Min PT" cut value
    // "Min Mass" cut value
    , m_cutMassMin   ( 4   * CLHEP::GeV )  ///< "Min Mass" cut value
    // "Max Mass" cut value 
    , m_cutMassMax   ( 7   * CLHEP::GeV )  ///< "Max Mass" cut value
    // list of "other" particles 
    , m_names ()
    , m_other ( LoKi::Cuts::GNONE )
    , m_first ("mu+")
    , m_muon  ( LoKi::Cuts::GNONE )
  {
    declareInterface<IFullGenEventCutTool>(this);
    // 
    setProperty( "StatPrint"       , "true" ) ;
    //
    declareProperty ( "ThetaMin"  , m_cutThetaMin  ) ;
    declareProperty ( "ThetaMax"  , m_cutThetaMax  ) ;
    declareProperty ( "ThetaYMax" , m_cutThetaYMax ) ;
    declareProperty ( "PMin"      , m_cutPMin      ) ;
    declareProperty ( "PtMin"     , m_cutPtMin     ) ;
    declareProperty ( "MassMin"   , m_cutMassMin   ) ;
    declareProperty ( "MassMax"   , m_cutMassMax   ) ;
    //
    m_names.push_back ( "mu+" ) ;
    m_names.push_back ( "e+"  ) ;
    m_names.push_back ( "pi+" ) ;
    m_names.push_back ( "K+"  ) ;
    m_names.push_back ( "p+"  ) ;
    
    declareProperty ( "Other"  , m_names ) ;
    declareProperty ( "First"  , m_first ) ;
  };
  /// virtual and protected destructor  
  virtual ~MuXMaxBias() {} ;  ///< virtual and protected destructor  
private:
  // the defautl constructor is disabled 
  MuXMaxBias() ;
  // the defautl constructor is disabled 
  MuXMaxBias( const  MuXMaxBias& ) ;
  // the assignement operator is disabled 
  MuXMaxBias& operator=( const  MuXMaxBias& ) ;
private:
  // "Theta  Min" cut value 
  double m_cutThetaMin  ;  ///< "Theta  Min" cut value 
  // "Theta  Max" cut value 
  double m_cutThetaMax  ;  ///< "Theta  Max" cut value 
  // "ThetaY Max" cut value 
  double m_cutThetaYMax ;  ///< "ThetaY Max" cut value 
  // "Min P" cut value 
  double m_cutPMin      ; ///<   "Min P" cut value 
  // "Min PT" cut value 
  double m_cutPtMin     ; ///<   "Min PT" cut value
  // "Min Mass" cut value
  double m_cutMassMin   ; ///<   "Min Mass" cut value
  // "Max Mass" cut value
  double m_cutMassMax   ; ///<   "Max Mass" cut value
private:
  typedef std::vector<std::string> Names ;
  Names             m_names ;
  LoKi::Types::GCut m_other ;
  std::string       m_first ;
  LoKi::Types::GCut m_muon  ;  
} ;
// ============================================================================
/// Tool factory, needed for instantiation 
// ============================================================================
DECLARE_TOOL_FACTORY(MuXMaxBias);
// ============================================================================
/// standard initialization of the tool 
// ============================================================================
StatusCode MuXMaxBias::initialize() 
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  if ( m_names.empty() ) 
  { return Error("The ID of the second particle is not specified!") ;}
  
  svc<LoKi::ILoKiSvc>("LoKiSvc",true) ;
  
  // particle in the acceptance!
  GCut inAcceptance = 
    GTHETA < m_cutThetaMax && 
    GTHETA > m_cutThetaMin && 
    abs(atan2(GPY,GPZ)) < m_cutThetaYMax ;
  
  // fast particle ! 
  GCut fast = GPT > m_cutPtMin && GP > m_cutPMin ;
  
  // all selection cuts for the first particle 
  m_muon  = fast && inAcceptance && ( m_first == GABSID ) ;
  
  m_other = m_names.front() == GABSID ;
  for ( size_t i = 1 ; i < m_names.size() ; ++i ) 
  { m_other = m_other || ( m_names[i] == GABSID ) ; }
  
  // all selection cuts for the second  particle 
  m_other = fast && inAcceptance && m_other ;
  
  { // some printout 
    MsgStream& log = info() ;
    log << "The first '" << m_first << 
      "' particle will be selected with: " << endreq ;
    log << m_muon << endreq ;
    
    log << " The other [" ;
    for ( Names::const_iterator ic = m_names.begin() ; 
          m_names.end() != ic ; ++ic ) 
    {
      if ( m_names.begin() != ic ) { log << "," ; }
      log << "'" << (*ic) << "'" ;
    }
    log << "] will be selected with: " << endreq ;
    log << m_other << endreq ;
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

// ============================================================================
/** Apply the cut on a event.
 *  @param[in] theEvents      Container of all interactions in the event.
 *  @param[in] theCollisions  Container of hard process informations of each 
 *                            pile-up interactions of the event.
 *  @return    true  if the full event passes the cut.
 *  @see IFullGnEventCutTool 
 */  
// ============================================================================
bool MuXMaxBias::studyFullEvent
( LHCb::HepMCEvents*    theEvents     ,
  LHCb::GenCollisions * theCollisions ) const
{
  
  StatEntity& ok = counter("Accepted") ;
  
  typedef std::vector<const HepMC::GenParticle*> PARTICLES ;
  typedef PARTICLES::const_iterator              iterator  ;
  
  // trivial check 
  if ( 0 == theEvents || 0 == theCollisions ) 
  { ok += 0 ; return false ; }                              // RETURN
  
  PARTICLES muons ;
  PARTICLES other ;
  
  // select fast muons in acceptance particles 
  LoKi::Extract::genParticles 
    ( theEvents , std::back_inserter ( muons ) , m_muon ) ;
  if ( muons.empty() ) 
  { ok += 0 ; return false ; }                              // RETURN 
  
  // select fast good particles in acceptance
  LoKi::Extract::genParticles 
    ( theEvents , std::back_inserter ( other ) , m_other ) ;
  if ( other.empty() ) 
  { ok += 0 ; return false ; }                              // RETURN 
  
  bool good = false ;
  for ( iterator ip1 = muons.begin() ; muons.end() != ip1 ; ++ip1 ) 
  {
    for ( iterator ip2 = other.begin() ; other.end() != ip2 ; ++ip2 ) 
    {
      // skip the combinations of the same charge 
      if ( 0 < G3Q(*ip1)*G3Q(*ip2) ) { continue ; }         // CONTINUE 
      const double m12 = LoKi::Kinematics::mass ( *ip1 , *ip2 ) ;
      if ( m_cutMassMin > m12      ) { continue ; }         // CONTINUE
      if ( m_cutMassMax < m12      ) { continue ; }         // CONTINUE   
      good = true ;     
      break ;                                               // BREAK     
    }
  }
  
  ok += good ;
  
  return good ;
  
} ;  
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
