// $Id: RichPhotonPredictorUsingRings.cpp,v 1.1 2004-06-29 20:05:55 jonesc Exp $

// local
#include "RichPhotonPredictorUsingRings.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonPredictorUsingRings
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonPredictorUsingRings>          s_factory;
const        IToolFactory& RichPhotonPredictorUsingRingsFactory = s_factory;

// Standard constructor
RichPhotonPredictorUsingRings::RichPhotonPredictorUsingRings( const std::string& type,
                                                              const std::string& name,
                                                              const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_geomTool      ( 0 ),
    m_recRings      ( 0 ),
    m_ringLoc       ( RichRecRingLocation::MarkovRings )
{

  declareInterface<IRichPhotonPredictor>(this);

  m_minROI.push_back( 130 ); // aerogel
  m_minROI.push_back( 0 );   // c4f10
  m_minROI.push_back( 0 );   // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 350 );   // aerogel
  m_maxROI.push_back(  90 );   // c4f10
  m_maxROI.push_back( 200 );   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

  declareProperty( "RichRecRingLocation",  m_ringLoc );

}

StatusCode RichPhotonPredictorUsingRings::initialize()
{

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecGeomTool", m_geomTool );

  // Initialise some variables
  m_minROI2.push_back( m_minROI[0]*m_minROI[0] );
  m_minROI2.push_back( m_minROI[1]*m_minROI[1] );
  m_minROI2.push_back( m_minROI[2]*m_minROI[2] );
  m_maxROI2.push_back( m_maxROI[0]*m_maxROI[0] );
  m_maxROI2.push_back( m_maxROI[1]*m_maxROI[1] );
  m_maxROI2.push_back( m_maxROI[2]*m_maxROI[2] );

  // Setup incident services
  IIncidentSvc * incSvc = svc<IIncidentSvc>( "IncidentSvc", true );
  incSvc->addListener( this, IncidentType::BeginEvent );

  // Make sure we are ready for a new event
  InitNewEvent();

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonPredictorUsingRings::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichPhotonPredictorUsingRings::photonPossible( RichRecSegment * segment,
                                                    RichRecPixel * pixel ) const {

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Hit separation criteria : based on global coordinates
  const double sepG = m_geomTool->trackPixelHitSep2Global(segment, pixel);
  if ( sepG > m_maxROI2[segment->trackSegment().radiator()] ||
       sepG < m_minROI2[segment->trackSegment().radiator()] ) return false;


  // Temporary whilst no rings in RICH1
  if ( Rich::Rich1 == segment->trackSegment().rich() ) return true;


  // Run over RichRecRings and check if the current pixel "belongs"
  // to a ring associated to the current segment
  for (RichRecRings::iterator iRing = richRings()->begin(); iRing != richRings()->end(); ++iRing ) {
    if ( *iRing && (*iRing)->richRecSegment() && (*iRing)->richRecSegment()->key() == segment->key() ) {
      for (SmartRefVector<RichRecPixel>::iterator iPix = (*iRing)->richRecPixels().begin();
           iPix != (*iRing)->richRecPixels().end(); ++iPix) {
        if ( *iPix && (*iPix)->key() == pixel->key() ) { return true; }
      }
    }
  }


  return false;
}


// Method that handles various Gaudi "software events"
void RichPhotonPredictorUsingRings::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) InitNewEvent();
}


RichRecRings *
RichPhotonPredictorUsingRings::richRings() const
{
  if ( !m_recRings ) {
    // Fetch them
    m_recRings = get<RichRecRings>( m_ringLoc );
    debug() << "Located " << m_recRings->size()
            << " RichRecRings at " << m_ringLoc << endreq;
  }

  return m_recRings;
}
