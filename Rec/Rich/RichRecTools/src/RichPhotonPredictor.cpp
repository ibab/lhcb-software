// $Id: RichPhotonPredictor.cpp,v 1.5 2004-04-19 23:06:13 jonesc Exp $

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
  : RichRecToolBase( type, name, parent ) {

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
  const double sepG = trackPixelHitSep2Global(segment, pixel);
  if ( sepG > m_maxROI2[segment->trackSegment().radiator()] ||
       sepG < m_minROI2[segment->trackSegment().radiator()] ) return false;

  return true;
}

double RichPhotonPredictor::trackPixelHitSep2Global( const RichRecSegment * segment,
                                                     const RichRecPixel * pixel ) const
{

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().y() * segment->pdPanelHitPoint().y() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->globalPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( pixel->globalPosition().x(),
                                                               -pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().x() * segment->pdPanelHitPoint().x() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->globalPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      return segment->pdPanelHitPoint().distance2( HepPoint3D( -pixel->globalPosition().x(),
                                                               pixel->globalPosition().y(),
                                                               pixel->globalPosition().z() ) );
    }

  }

  return 99999999.9;
}

double RichPhotonPredictor::trackPixelHitSep2Local( const RichRecSegment * segment,
                                                    const RichRecPixel * pixel ) const
{

  //return segment->pdPanelHitPointLocal().distance2( pixel->localPosition() );

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().y() * segment->pdPanelHitPointLocal().y() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->localPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( pixel->localPosition().x(),
                                                                    -pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->localPosition().x() * segment->pdPanelHitPointLocal().x() > 0 ) {
      return pixel->localPosition().distance2( segment->pdPanelHitPointLocal() );
    } else if ( ( pixel->localPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->localPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      return segment->pdPanelHitPointLocal().distance2( HepPoint3D( -pixel->localPosition().x(),
                                                                    pixel->localPosition().y(),
                                                                    pixel->localPosition().z() ) );
    }

  }

  return 99999999.9;
}
