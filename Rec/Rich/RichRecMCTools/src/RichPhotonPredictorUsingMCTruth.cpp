
//-----------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingMCTruth.cpp
 *
 *  Implementation file for RICH reconstruction tool : Rich::Rec::PhotonPredictorUsingMCTruth
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonPredictorUsingMCTruth.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec::MC;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonPredictorUsingMCTruth )

// Standard constructor
PhotonPredictorUsingMCTruth::
PhotonPredictorUsingMCTruth( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
  : ToolBase    ( type, name, parent ),
    m_mcRecTool ( NULL )
{
  // interface
  declareInterface<IPhotonPredictor>(this);
  // JOs
  declareProperty( "SignalFraction",     m_sigSelFrac =  999. );
  declareProperty( "BackgroundFraction", m_bkgSelFrac = -999. );
}

// Destructor
PhotonPredictorUsingMCTruth::~PhotonPredictorUsingMCTruth() {}

StatusCode PhotonPredictorUsingMCTruth::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichRecMCTruthTool", m_mcRecTool );

  // random number generator
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_rand.initialize( randSvc, Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  return sc;
}

// (not so ...) fast decision on whether a photon is possible
bool PhotonPredictorUsingMCTruth::
photonPossible( LHCb::RichRecSegment * segment,
                LHCb::RichRecPixel * pixel ) const
{
  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Check if this combination due to a true cherenkov hit using MC
  const LHCb::MCParticle * parent = m_mcRecTool->trueCherenkovPhoton(segment,pixel);
 
  const bool selected = ( parent ?
                          ( m_sigSelFrac > 1.0 || m_rand() < m_sigSelFrac ) :
                          ( m_bkgSelFrac > 0.0 && m_rand() < m_bkgSelFrac ) );

  if ( msgLevel(MSG::VERBOSE) )
    verbose() << parent << " " << selected << endmsg;

  return selected;
}

