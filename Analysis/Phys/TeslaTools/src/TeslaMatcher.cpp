//------------------------------------------------------------------------------
// Local
//------------------------------------------------------------------------------
#include "TeslaMatcher.h"
//------------------------------------------------------------------------------
// Kernel
//------------------------------------------------------------------------------
#include "Kernel/HashIDs.h"

#define MATCHING_FUNC(name, original, cand, defaultCand) \
    const Particle* TeslaMatcher::name(\
                               const Particle* original,\
                               const Particle* cand,\
                               const Particle* defaultCand)

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
  declareInterface <ITeslaMatcher> (this);
  declareProperty ( "CheckPid" , m_checkPid = true );                   
  declareProperty ( "MatchingTechnique" , m_matchingTechnique = "");                   
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
  if (m_matchingTechnique == "Track")
    m_bestMatching = TeslaMatcher::trackBestMatching;
  else if (m_matchingTechnique == "TeslaHits")
    m_bestMatching = TeslaMatcher::t2bBestMatching;
  else if (m_matchingTechnique == "Momentum")
    m_bestMatching = TeslaMatcher::momentumBestMatching;
  else if (m_matchingTechnique == "")
    m_bestMatching = TeslaMatcher::defaultBestMatching;
  else 
    return Error("Unknown matching technique " + m_matchingTechnique,
                 StatusCode::FAILURE);

  return StatusCode::SUCCESS;
}

//==============================================================================
// Finalize
//==============================================================================
StatusCode TeslaMatcher::finalize()
{
  return StatusCode::SUCCESS;
}

//==============================================================================
// Matching function -- loops on candidates and output best particle
//==============================================================================
StatusCode TeslaMatcher::findBestMatch ( const Particle* inputP,
                                         const Particle* &outputP,
                                         const std::string& tesLocation)
                                         const
{
  LHCb::Particles *particles = getIfExists<LHCb::Particles> ( tesLocation ); 
  outputP = NULL;
  
  if ( particles == NULL) 
    return Warning("Failed to read TES " + tesLocation, StatusCode::SUCCESS);

  const LHCb::Particle *bestMatch = NULL;
  for (const LHCb::Particle* p : *particles)
  {
    if (m_checkPid && inputP->particleID().abspid()!=p->particleID().abspid())
      continue;

      bestMatch = m_bestMatching ( inputP, p, bestMatch );
  }

  outputP = bestMatch;

  return StatusCode::SUCCESS;
}

//==============================================================================
// Default matching criterion -- All IDs
//==============================================================================
MATCHING_FUNC(defaultBestMatching,original,c1,c2)
{
  if (original == NULL) return NULL;
  if (c1    == NULL) return c2;

  std::pair<double,double> o1 = LHCb::HashIDs::overlap(original, c1);
  std::pair<double,double> o2 = LHCb::HashIDs::overlap(original, c2);

  if (std::max(o1.first,o1.second)>std::max(o2.first,o2.second))
    return c1;

  return c2;
}

//==============================================================================
// Tesla2Brunel  matching criterion -- All IDs
//==============================================================================
MATCHING_FUNC(t2bBestMatching,original,c1,c2)
{
  if (original == NULL) return NULL;
  if (c1    == NULL) return c2;

  std::pair<double,double> o1 = LHCb::HashIDs::overlap(original, c1);
  std::pair<double,double> o2 = LHCb::HashIDs::overlap(original, c2);

  if (o1.second > o2.second)
    return c1;

  return c2;
}

//==============================================================================
// Track matching criterion
//==============================================================================
MATCHING_FUNC(trackBestMatching,original,c1,c2)
{
  if (original == NULL) return NULL;

  if (c1 && c2 && c1->proto() && c2->proto() 
      && c1->proto()->track() && c2->proto()->track())
  {
    std::pair<double,double> o1 = 
      LHCb::HashIDs::overlap(original->proto()->track(), c1->proto()->track());
    std::pair<double,double> o2 = 
      LHCb::HashIDs::overlap(original->proto()->track(), c2->proto()->track());

    if ((o1.first >= o2.first) && (o1.second >= o2.second))
      return c1;
  }

  return c2;
}

//==============================================================================
// Momentum matching criterion
//==============================================================================
MATCHING_FUNC(momentumBestMatching,original,c1,c2)
{
  if (original == NULL) return NULL;
  if (c1    == NULL) return c2;

  const Gaudi::LorentzVector m1 = c1->momentum();
  const Gaudi::LorentzVector m2 = c2->momentum();
  const Gaudi::LorentzVector mo = original->momentum();

  double fom1 = fabs(m1.x() - mo.x()) + 
                fabs(m1.y() - mo.y()) + 
                fabs(m1.z() - mo.z()) ;
  double fom2 = fabs(m2.x() - mo.x()) + 
                fabs(m2.y() - mo.y()) + 
                fabs(m2.z() - mo.z()) ;

  if (fom1 < fom2) 
    return c1;

  return c2;
}

#undef MATCHING_FUNC






