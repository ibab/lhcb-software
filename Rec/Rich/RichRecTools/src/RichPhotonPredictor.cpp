// $Id: RichPhotonPredictor.cpp,v 1.6 2004-06-10 14:39:23 jonesc Exp $

// local
#include "RichPhotonPredictor.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichPhotonPredictor
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichPhotonPredictor>          s_factory ;
const        IToolFactory& RichPhotonPredictorFactory = s_factory ;

// Standard constructor
RichPhotonPredictor::RichPhotonPredictor( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_geomTool      ( 0 )
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

}

StatusCode RichPhotonPredictor::initialize()
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

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonPredictor::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichPhotonPredictor::photonPossible( RichRecSegment * segment,
                                          RichRecPixel * pixel ) const {

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Hit seperation criteria : based on global coordinates
  const double sepG = m_geomTool->trackPixelHitSep2Global(segment, pixel);
  if ( sepG > m_maxROI2[segment->trackSegment().radiator()] ||
       sepG < m_minROI2[segment->trackSegment().radiator()] ) return false;

  return true;
}

