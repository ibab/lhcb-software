//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
#include "TeslaMatcher.h"
//------------------------------------------------------------------------------
// Kernel
//------------------------------------------------------------------------------
#include "Kernel/HashIDs.h"

DECLARE_TOOL_FACTORY ( TeslaMatcher )


//==============================================================================
// Standard constructor
//==============================================================================
TeslaMatcher::TeslaMatcher (const std::string& type,
                            const std::string& name,
                            const IInterface* parent)
  : GaudiTool ( type, name, parent )
  , m_bestMatching ( TeslaMatcher::defaultBestMatching )
  , m_checkPid     ( true )
{
  declareProperty ( "CheckPid" , m_checkPid = true );                   
}



//==============================================================================
// Destructor
//==============================================================================
TeslaMatcher::~TeslaMatcher() 
{}


//==============================================================================
// Initialize
//==============================================================================
StatusCode TeslaMatcher::initialize()
{
  return StatusCode::SUCCESS;
}

//==============================================================================
// Matching function -- loops on candidates and output best particle
//==============================================================================
StatusCode TeslaMatcher::findBestMatch ( const Particle* inputParticle,
                                                 const Particle* &outputParticle,
                                                 const std::string& tesLocation)
                                                  const
{
  LHCb::Particles *particles = getIfExists<LHCb::Particles> ( tesLocation ); 
  outputParticle = NULL;
  
  if ( particles == NULL) 
    return StatusCode::SUCCESS;

  const LHCb::Particle *bestMatch = NULL;
  for (const LHCb::Particle* p : *particles)
  {
    if (m_checkPid 
        && inputParticle->particleID().abspid()!=p->particleID().abspid())
          continue;

      bestMatch = m_bestMatching ( inputParticle, p, bestMatch );
  }

  outputParticle = bestMatch;

  return StatusCode::SUCCESS;
}

//==============================================================================
// Default matching criterion
//==============================================================================
const Particle* TeslaMatcher::defaultBestMatching( 
                                            const LHCb::Particle* original,
                                            const LHCb::Particle* c1,
                                            const LHCb::Particle* c2)
{
  if (original == NULL) return NULL;
  if (c1    == NULL) return c2;

  std::pair<double,double> o1 = LHCb::HashIDs::overlap(original, c1);
  std::pair<double,double> o2 = LHCb::HashIDs::overlap(original, c2);

  if (std::max(o1.first,o1.second)>std::max(o2.first,o2.second))
    return c1;

  return c2;
}








