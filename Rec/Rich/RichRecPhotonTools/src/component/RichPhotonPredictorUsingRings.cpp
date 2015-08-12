//-----------------------------------------------------------------------------
/** @file RichPhotonPredictorUsingRings.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonPredictorUsingRings
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonPredictorUsingRings.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonPredictorUsingRings )

// Standard constructor
PhotonPredictorUsingRings::
PhotonPredictorUsingRings( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : ToolBase   ( type, name, parent ),
    m_geomTool ( NULL ),
    m_recRings ( NULL ),
    m_ringLoc  ( LHCb::RichRecRingLocation::MarkovRings )
{

  declareInterface<IPhotonPredictor>(this);

  m_minROI.push_back( 130 ); // aerogel
  m_minROI.push_back( 0 );   // rich1Gas
  m_minROI.push_back( 0 );   // rich2Gas
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 350 );   // aerogel
  m_maxROI.push_back(  90 );   // rich1Gas
  m_maxROI.push_back( 200 );   // rich2Gas
  declareProperty( "MaxTrackROI", m_maxROI );

  // location in TES of rings to use
  declareProperty( "RichRecRingLocation",  m_ringLoc );

}

StatusCode PhotonPredictorUsingRings::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecGeometry", m_geomTool );

  // Initialise some variables
  m_minROI2.push_back( m_minROI[0]*m_minROI[0] );
  m_minROI2.push_back( m_minROI[1]*m_minROI[1] );
  m_minROI2.push_back( m_minROI[2]*m_minROI[2] );
  m_maxROI2.push_back( m_maxROI[0]*m_maxROI[0] );
  m_maxROI2.push_back( m_maxROI[1]*m_maxROI[1] );
  m_maxROI2.push_back( m_maxROI[2]*m_maxROI[2] );

  // Setup incident services
  incSvc()->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return sc;
}

// fast decision on whether a photon is possible
bool
PhotonPredictorUsingRings::photonPossible( LHCb::RichRecSegment * segment,
                                           LHCb::RichRecPixel * pixel ) const
{

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Hit separation criteria : based on global coordinates
  const double sepG = m_geomTool->trackPixelHitSep2(segment, pixel);
  if ( sepG > m_maxROI2[segment->trackSegment().radiator()] ||
       sepG < m_minROI2[segment->trackSegment().radiator()] ) return false;

  // Temporary whilst no rings in RICH1
  if ( Rich::Rich1 == segment->trackSegment().rich() ) return true;

  // Run over RichRecRings and check if the current pixel "belongs"
  // to a ring associated to the current segment
  // Search could probably be made faster using stl etc...
  for ( const auto* ring : *richRings() )
  {
    if ( ring &&
         ring->richRecSegment() &&
         ring->richRecSegment()->key() == segment->key() )
    {
      for ( const auto& pix : ring->richRecPixels() )
      {
        if ( pix.pixel() && pix.pixel()->key() == pixel->key() ) { return true; }
      }

    }
  }

  // Return false if nothing found by above search
  return false;
}


// Method that handles various Gaudi "software events"
void PhotonPredictorUsingRings::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}

