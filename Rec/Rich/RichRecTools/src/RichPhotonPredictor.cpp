// $Id: RichPhotonPredictor.cpp,v 1.4 2004-03-16 13:45:04 jonesc Exp $

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

StatusCode RichPhotonPredictor::initialize() {

  debug() << "Initialize" << endreq;

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

  // Informational Printout
  debug() << " Min Region of Interest       = " << m_minROI << endreq
          << " Max Region of Interest       = " << m_maxROI << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichPhotonPredictor::finalize() {

  debug() << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichPhotonPredictor::photonPossible( RichRecSegment * segment,
                                          RichRecPixel * pixel ) const {

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Hit seperation criteria
  const double sep = trackPixelHitSep2(segment, pixel);
  if ( sep > m_maxROI2[ segment->trackSegment().radiator() ] ) return false;
  if ( sep < m_minROI2[ segment->trackSegment().radiator() ] ) return false;

  return true;
}

double RichPhotonPredictor::trackPixelHitSep2( const RichRecSegment * segment,
                                               const RichRecPixel * pixel ) const {

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().y() * segment->pdPanelHitPoint().y() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->globalPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      const HepPoint3D temp( pixel->globalPosition().x(),
                             -pixel->globalPosition().y(),
                             pixel->globalPosition().z() );
      return temp.distance2( segment->pdPanelHitPoint() );
    }

  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {

    if ( pixel->globalPosition().x() * segment->pdPanelHitPoint().x() > 0 ) {
      return pixel->globalPosition().distance2( segment->pdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->globalPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      const HepPoint3D temp( -pixel->globalPosition().x(),
                             pixel->globalPosition().y(),
                             pixel->globalPosition().z() );
      return temp.distance2( segment->pdPanelHitPoint() );
    }

  }

  return 99999999.9;
}

