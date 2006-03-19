// $Id: ParticleMakerSeq.cpp,v 1.5 2006-03-19 16:33:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// from GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
// ============================================================================
// DaVinciKernel
// ============================================================================
#include "Kernel/IParticleMaker.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================

// ============================================================================
/** @file 
 * 
 *  Implementation file for class ParticleMakerSeq
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-04-29 
 *  @date 2006-03-19 
 */
// ============================================================================

// ============================================================================
/** @class ParticleMakerSeq ParticleMakerSeq.h
 *
 *  Simple particle maker which does delegates all its job to few concrete
 *  Particle makers 
 *  
 *  In principle this maker does nor provide any gain in the code ,
 *  but it results in a simplicication of job configuration, 
 *  e.g. if one need to get as an input particles of different "origin":
 *  charged particles *AND* neutral particles.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2004-04-29
 *  @date 2006-03-19 
 */
class ParticleMakerSeq : 
  public         GaudiTool      ,
  public virtual IParticleMaker 
{
  /// friend factory for instantiation
  friend class ToolFactory<ParticleMakerSeq>;
public:  
  /** make the particles
   *  @see IParticleMaker
   *  @param particles (output) vector of Particles
   *  @return status code 
   */
  virtual StatusCode makeParticles 
  ( LHCb::Particle::ConstVector & particles ) ;
  /// standard initialization of the tool
  virtual StatusCode initialize () ;
protected:
  /** Standard constructor
   *  @see GaudiTool
   *  @see AlgTool
   *  @see ToolFactory
   *  @see IToolFactory
   *  @param type tool type (?)
   *  @param name tool name 
   *  @param parent tool's parent
   */
  ParticleMakerSeq 
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) 
    : GaudiTool ( type, name , parent )
    , m_names   (   )
    , m_makers  (   ) 
  {
    declareInterface<IParticleMaker>(this);
    declareProperty ( "Makers"        , m_names ) ;
    setProperty     ( "StatPrint"     , "true"  ) ;
  };
  /// destructor 
  virtual ~ParticleMakerSeq (){};
private:
  // default constructor is disabled
  ParticleMakerSeq () ;
  // copy    constructor is disabled
  ParticleMakerSeq ( const ParticleMakerSeq& ) ;
  // assignement operator is disabled
  ParticleMakerSeq& operator=( const ParticleMakerSeq& ) ;
private:
  typedef std::vector<std::string>     Names   ;
  typedef std::vector<IParticleMaker*> Makers  ;
  // list of privat eparticle makers 
  Names   m_names   ;
  Makers  m_makers  ;
};
// ============================================================================

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
DECLARE_TOOL_FACTORY( ParticleMakerSeq ) ;
// ============================================================================

// ============================================================================
/// initialize the tool
// ============================================================================
StatusCode ParticleMakerSeq::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
  
  m_makers.clear() ;
  // locate the makers 
  for ( Names::const_iterator i = m_names.begin() ; 
        m_names.end() != i ; ++i ) 
  {
    IParticleMaker* maker = tool<IParticleMaker>( *i , this ) ;
    if ( 0 == maker ) { return StatusCode::FAILURE ; }
    m_makers.push_back( maker ) ;
  }
  if ( m_makers.empty() ) 
  { Warning ( " Empty list of available ParticleMakers" ) ; }
  //
  return StatusCode::SUCCESS  ;
};
// ============================================================================


// ============================================================================
/// the only essential method 
// ============================================================================
StatusCode ParticleMakerSeq::makeParticles 
( LHCb::Particle::ConstVector& particles )
{
  if ( m_makers.empty() ) { return Error ( "No Makers are available!" ) ; }
  
  LHCb::Particle::ConstVector tmp ; 
  for ( Makers::const_iterator imaker = m_makers.begin() ;
        m_makers.end() != imaker ; ++imaker ) 
  {
    IParticleMaker* maker = *imaker ;
    if ( 0 == maker ) { continue ; }
    tmp.clear() ; tmp.reserve(128) ;
    StatusCode sc = maker->makeParticles ( tmp ) ;
    if ( sc.isFailure() ) 
    {
      Error ( " Error from Maker='" 
              + maker->type() + "/" 
              + maker->name() + "', skip " , sc ) ;
      continue ;                                               // CONTINUE 
    }
    // use the standard counters form GaudiTool
    counter("#Particles") += tmp.size() ;
    // insert 
    particles.insert( particles.end() , tmp.begin() , tmp.end() ) ;
  }
  //  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
