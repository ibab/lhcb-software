// $Id: ParticleMakerSeq.cpp,v 1.2 2004-07-28 14:15:24 pkoppenb Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2004/06/18 12:11:36  pkoppenb
// ParticleMakerSeq
// 
// ============================================================================
// Include files 
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h" 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// local
// ============================================================================
#include "ParticleMakerSeq.h"
// ============================================================================

/** @file 
 * 
 *  Implementation file for class : ParticleMakerSeq
 *
 *  @date 2004-04-29 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

// ============================================================================
// Declaration of the Tool Factory
// ============================================================================
static const  ToolFactory<ParticleMakerSeq>         s_Factory ;
const        IToolFactory&ParticleMakerSeqFactory = s_Factory ; 
// ============================================================================


// ============================================================================
/// constructor 
// ============================================================================
ParticleMakerSeq::ParticleMakerSeq 
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_private (   )  
    , m_public  (   )
    , m_makers  (   ) 
    , m_counter ( 0 ) 
{
  declareInterface<IParticleMaker>(this);

  declareProperty( "PrivateMakers" , m_private ) ;
  declareProperty( "PublicMakers"  , m_public  ) ;
  
};
// ============================================================================


// ============================================================================
/// destructor 
// ============================================================================
ParticleMakerSeq::~ParticleMakerSeq() {}; 
// ============================================================================

// ============================================================================
// ============================================================================
StatusCode ParticleMakerSeq::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) { return sc ; }
   
  m_makers.clear() ;
  
  { // locate private makers 
    for ( Names::const_iterator i = m_private.begin() ; 
          m_private.end() != i ; ++i ) 
    {
      IParticleMaker* maker = tool<IParticleMaker>( *i , this ) ;
      if ( 0 == maker ) { return StatusCode::FAILURE ; }
      m_makers.push_back( maker ) ;
    }
  }
  { // locate public makers 
    for ( Names::const_iterator i = m_public.begin() ; 
          m_public.end() != i ; ++i ) 
    {
      IParticleMaker* maker = tool<IParticleMaker>( *i ) ;
      if ( 0 == maker ) { return StatusCode::FAILURE ; }
      m_makers.push_back( maker ) ;
    }
  }
  
  if ( m_makers.empty() ) 
  { Warning ( " Empty list of available ParticleMakers" ) ; }
  
  return StatusCode::SUCCESS  ;
  
};
// ============================================================================

// ============================================================================
// ============================================================================
StatusCode ParticleMakerSeq::finalize()
{
  // clear list of makers 
  m_makers.clear() ;
  
  MsgStream log( msgSvc() , name() ) ;
  log << MSG::ALWAYS 
      << " Created Particles = " << m_counter 
      << " (Maker='" << name() <<"'" << endreq ;

  return GaudiTool::finalize () ;
};
// ============================================================================

// ============================================================================
// ============================================================================
StatusCode ParticleMakerSeq::makeParticles ( ParticleVector& particles )
{
  if ( m_makers.empty() ) { return StatusCode::SUCCESS ; }
  
  ParticleVector tmp ; 
  for( Makers::const_iterator imaker = m_makers.begin() ;
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
              + maker->name() + "'" , sc ) ;
      continue ;
    }
    m_counter += tmp.size() ;
    std::copy ( tmp.begin() , tmp.end() , std::back_inserter( particles ) ) ;
  }
  
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
