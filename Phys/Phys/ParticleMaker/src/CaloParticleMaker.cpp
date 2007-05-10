// $Id: CaloParticleMaker.cpp,v 1.2 2007-05-10 12:36:20 pkoppenb Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "CaloParticleMaker.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloParticleMaker
//
// 2006-08-29 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloParticleMaker );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloParticleMaker::CaloParticleMaker( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
    , m_MakerType  ()
    , m_Maker      ()
{
  declareProperty ( "Type"  , m_MakerType  = "PhotonMaker") ;
  declareInterface<IParticleMaker>(this);

}
//=============================================================================
// Destructor
//=============================================================================
CaloParticleMaker::~CaloParticleMaker() {} 

//=============================================================================
StatusCode CaloParticleMaker::initialize    ()
{
  // initialize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error(" Unable to initialize GaudiTool",sc);}

  // Retrieve tool
  m_Maker = tool< ICaloParticleMaker>( m_MakerType ,  this ) ;
  if( 0 == m_Maker ) {
    fatal() << "    Unable to locate tool "	  << endreq;
    return StatusCode::FAILURE ; 
  }
  info() << "CaloParticle Maker uses " << m_MakerType << " tools " << endreq;
  
  return StatusCode::SUCCESS ;
};
// ============================================================================

StatusCode CaloParticleMaker::finalize      ()
{
  // finalize the base class
  return GaudiTool::finalize ();
};

// ============================================================================
/** Make the particles
 *  @see IParticleMaker
 *  @param particles  vector of particles
 */
// ============================================================================
StatusCode CaloParticleMaker::makeParticles (LHCb::Particle::ConstVector & particles )
{
  LHCb::Particle::Vector parts;
  StatusCode sc = m_Maker->makeParticles(parts);
  if (!sc) return sc;
  for(LHCb::Particle::Vector::iterator ipart = parts.begin(); ipart !=parts.end(); ++ipart){
    particles.push_back( *ipart );
  }
  
 return StatusCode::SUCCESS ;
};

