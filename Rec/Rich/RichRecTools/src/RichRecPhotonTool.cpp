// $Id: RichRecPhotonTool.cpp,v 1.7 2003-06-27 15:14:12 cattanem Exp $
#include <cmath>

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"

// RichUtils
#include "RichUtils/RichGeomPhoton.h"

// local
#include "RichRecPhotonTool.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichRecPhotonTool
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecPhotonTool>          s_factory ;
const        IToolFactory& RichRecPhotonToolFactory = s_factory ;

// Standard constructor
RichRecPhotonTool::RichRecPhotonTool( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecPhotonTool>(this);

  // Define job option parameters
  declareProperty( "RichRecPhotonLocation",
                   m_richRecPhotonLocation = RichRecPhotonLocation::Default );

  m_minROI.push_back( 130 ); // aerogel
  m_minROI.push_back( 0 );   // c4f10
  m_minROI.push_back( 0 );   // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 350 );   // aerogel
  m_maxROI.push_back(  90 );   // c4f10
  m_maxROI.push_back( 200 );   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

  m_minCKtheta.push_back( 0.09 );   // aerogel
  m_minCKtheta.push_back( 0 );   // c4f10
  m_minCKtheta.push_back( 0 );   // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  m_maxCKtheta.push_back( 0.300 ); // aerogel
  m_maxCKtheta.push_back( 0.080 ); // c4f10
  m_maxCKtheta.push_back( 0.050 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  declareProperty( "MinPhotonProbability", m_minPhotonProb = 1e-15 );

}

StatusCode RichRecPhotonTool::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    msg << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecTrack Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool) ) {
    msg << MSG::ERROR << "RichRecTrackTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    msg << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool) ) {
    msg << MSG::ERROR << "RichRecPixelTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    msg << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    msg << MSG::ERROR << "IncidentSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    //incSvc->addListener( this, "EndEvent"   ); // Informed at the end of event
    incSvc->release();
  }

  // Initialise some variables
  m_minROI2.push_back( m_minROI[0]*m_minROI[0] );
  m_minROI2.push_back( m_minROI[1]*m_minROI[1] );
  m_minROI2.push_back( m_minROI[2]*m_minROI[2] );
  m_maxROI2.push_back( m_maxROI[0]*m_maxROI[0] );
  m_maxROI2.push_back( m_maxROI[1]*m_maxROI[1] );
  m_maxROI2.push_back( m_maxROI[2]*m_maxROI[2] );

  // Temporary variables - should be got from XML eventually
  m_radiusCurv[ Rich::Rich1 ] = 240.0;
  m_radiusCurv[ Rich::Rich2 ] = 860.0;
  m_pixelArea = 0.24*0.24;

  // Informational Printout
  // msg << MSG::DEBUG << "Tool Parameters :-" << endreq
  //    << " Min Region of Interest  = " << m_minROI << endreq
  //   << " Max Region of Interest  = " << m_maxROI << endreq
  //    << " Min Cherenkov Theta     = " << m_minCKtheta << endreq
  //    << " Max Cherenkov Theta     = " << m_maxCKtheta << endreq
  //    << " Min Photon Probability  = " << m_minPhotonProb << endreq;

  return sc;
}

StatusCode RichRecPhotonTool::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  if ( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if ( m_richRecTrackTool )   toolSvc()->releaseTool( m_richRecTrackTool );
  if ( m_richRecPixelTool )   toolSvc()->releaseTool( m_richRecPixelTool );
  if ( m_richDetInterface )   toolSvc()->releaseTool( m_richDetInterface );

  if( 0 != m_evtDataSvc ) {
    m_evtDataSvc->release();
    m_evtDataSvc = 0;
  }
  
  return StatusCode::SUCCESS;
}

// Method that handles various Gaudi "software events"
void RichRecPhotonTool::handle ( const Incident& incident ) {

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_photonDone.clear();

    SmartDataPtr<RichRecPhotons> tdsPhotons( m_evtDataSvc,
                                             m_richRecPhotonLocation );
    if ( !tdsPhotons ) {

      // Reinitialise the Photon Container
      m_photons = new RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !m_evtDataSvc->registerObject(m_richRecPhotonLocation, m_photons) ) {
        MsgStream msg( msgSvc(), name() );
        msg << MSG::ERROR << "Failed to register RichRecPhotons at "
            << m_richRecPhotonLocation << endreq;
      }

    } else {

      // Set smartref to TES photon container
      m_photons = tdsPhotons;

      // Remake local photon reference map
      for ( RichRecPhotons::const_iterator iPhoton = tdsPhotons->begin();
            iPhoton != tdsPhotons->end();
            ++iPhoton ) {
        m_photonDone[(int)(*iPhoton)->key()] = true;
      }

    }

  }
  return;
}

RichRecPhoton*
RichRecPhotonTool::reconstructPhoton( RichRecSegment * segment,
                                      RichRecPixel * pixel ) {

  // check photon is possible before proceeding
  if ( !photonPossible(segment, pixel) ) return NULL;

  // Form the key for this photon
  RichRecPhotonKey photonKey;
  photonKey.setSegmentNumber( segment->key() );
  photonKey.setPixelNumber( pixel->key() );

  // See if this photon already exists
  if ( m_photonDone[(int)photonKey] ) {
    return (RichRecPhoton*)(m_photons->object(photonKey));
  } else {
    return buildPhoton( segment, pixel, photonKey );
  }

}

RichRecPhoton * RichRecPhotonTool::buildPhoton( RichRecSegment * segment,
                                                RichRecPixel * pixel,
                                                RichRecPhotonKey & key ) {

  RichRecPhoton * newPhoton = NULL;

  // Reconstruct the Cherenkov angles
  RichGeomPhoton geomPhoton;
  if ( m_richDetInterface->reconstructPhoton( segment->trackSegment(),
                                              pixel->globalPosition(),
                                              geomPhoton ) != 0 ) {

    if ( ( geomPhoton.CherenkovTheta() > 0. ||
           geomPhoton.CherenkovPhi() > 0. ) &&
         geomPhoton.CherenkovTheta() <
         m_maxCKtheta[(int)segment->trackSegment().radiator()] &&
         geomPhoton.CherenkovTheta() >
         m_minCKtheta[(int)segment->trackSegment().radiator()] ) {

      newPhoton = new RichRecPhoton();

      // give photon same smart ID as pixel
      geomPhoton.setSmartID( pixel->smartID() );  

      // set geometrical photon information
      newPhoton->setGeomPhoton( geomPhoton );

      // Set various navigation info
      newPhoton->setRichRecSegment( segment );
      newPhoton->setRichRecTrack( segment->richRecTrack() );
      newPhoton->setRichRecPixel( pixel );

      // check photon has significant probability to be signal for any
      // hypothesis. If not then reject and delete
      bool keepPhoton = false;
      for ( int iHypo = 0; iHypo < Rich::NParticleTypes; ++iHypo ) {
        if ( predictedPixelSignal(newPhoton,
                                  (Rich::ParticleIDType)iHypo) > m_minPhotonProb ) {
          keepPhoton = true;
          break;
        }
      }

      if ( keepPhoton ) {

        m_photons->insert( newPhoton, key );

        // add this photon to pixel/segment/track references
        segment->addToRichRecPixels( pixel );
        segment->addToRichRecPhotons( newPhoton );
        segment->richRecTrack()->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecPhotons( newPhoton );
        pixel->addToRichRecTracks( segment->richRecTrack() );
        SmartRefVector<RichRecPixel> & tkPixs =
          segment->richRecTrack()->richRecPixels();
        bool notThere = true;
        for ( SmartRefVector<RichRecPixel>::iterator pix = tkPixs.begin();
              pix != tkPixs.end(); ++pix ) {
          if ( (RichRecPixel*)(*pix) == pixel ) { notThere = false; break; }
        }
        if ( notThere ) segment->richRecTrack()->addToRichRecPixels( pixel );

      } else {
        delete newPhoton;
        newPhoton = NULL;
      }

    }

  }

  // Add to reference map
  m_photonDone[(int)key] = true;

  // Return pointer to this photon
  return newPhoton;

}

void RichRecPhotonTool::reconstructPhotons() {

  bool noPhots = m_photons->empty();

  // Iterate over all tracks
  for ( RichRecTracks::iterator iTrack =
          m_richRecTrackTool->richTracks()->begin();
        iTrack != m_richRecTrackTool->richTracks()->end();
        ++iTrack ) {
    RichRecTrack * track = *iTrack;
    if ( !track->inUse() ) continue; // skip tracks not "on"

    // Iterate over pixels
    for ( RichRecPixels::iterator iPixel =
            m_richRecPixelTool->richPixels()->begin();
          iPixel != m_richRecPixelTool->richPixels()->end();
          ++iPixel ) {
      RichRecPixel * pixel = *iPixel;

      // Iterate over segments
      for ( SmartRefVector<RichRecSegment>::iterator iSegment =
              track->richRecSegments().begin();
            iSegment != track->richRecSegments().end();
            ++iSegment) {
        RichRecSegment * segment = *iSegment;

        // If container was empty, skip checks for whether photon already exists
        if ( noPhots ) {
          if ( photonPossible( segment, pixel ) ) {
            RichRecPhotonKey photonKey;
            photonKey.setSegmentNumber( segment->key() );
            photonKey.setPixelNumber( pixel->key() );
            buildPhoton( segment, pixel, photonKey );
          }
        } else {
          reconstructPhoton( segment, pixel );
        }

      } // segment loop
    } // pixel loop
  } // track loop


}

SmartRefVector<RichRecPhoton>&
RichRecPhotonTool::reconstructPhotons( RichRecTrack * track ) {

  // Iterate over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment) { reconstructPhotons( *segment ); }

  return track->richRecPhotons();
}

SmartRefVector<RichRecPhoton>&
RichRecPhotonTool::reconstructPhotons( RichRecSegment * segment ) {

  // Iterate over pixels
  for ( RichRecPixels::iterator pixel =
          m_richRecPixelTool->richPixels()->begin();
        pixel != m_richRecPixelTool->richPixels()->end();
        ++pixel ) { reconstructPhoton( segment, *pixel ); }

  return segment->richRecPhotons();
}

SmartRefVector<RichRecPhoton>&
RichRecPhotonTool::reconstructPhotons( RichRecPixel * pixel ) {

  // Iterate over tracks
  for ( RichRecTracks::iterator track =
          m_richRecTrackTool->richTracks()->begin();
        track != m_richRecTrackTool->richTracks()->end();
        ++track ) {
    if ( !(*track)->inUse() ) continue;
    reconstructPhotons( *track, pixel );
  }

  return pixel->richRecPhotons();
}

// Note to self. Need to review what this method passes back
SmartRefVector<RichRecPhoton>
RichRecPhotonTool::reconstructPhotons( RichRecTrack * track,
                                       RichRecPixel * pixel ) {

  SmartRefVector<RichRecPhoton> photons;

  // Iterate over segments
  for ( SmartRefVector<RichRecSegment>::iterator segment =
          track->richRecSegments().begin();
        segment != track->richRecSegments().end();
        ++segment ) {

    RichRecPhoton * photon = reconstructPhoton( *segment, pixel );
    if ( photon ) photons.push_back( photon );

  }

  return photons;
}

// fast decision on whether a photon is possible
bool RichRecPhotonTool::photonPossible( RichRecSegment * segment,
                                        RichRecPixel * pixel ) {

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() != pixel->detector() ) return false;

  // Hit seperation criteria
  double sep = trackPixelHitSep2(segment, pixel);
  if ( sep > m_maxROI2[ segment->trackSegment().radiator() ] ) return false;
  if ( sep < m_minROI2[ segment->trackSegment().radiator() ] ) return false;

  return true;
}

double RichRecPhotonTool::trackPixelHitSep2( const RichRecSegment * segment,
                                             const RichRecPixel * pixel ) {

  if ( Rich::Rich1 == segment->trackSegment().rich() ) {
    if ( pixel->globalPosition().y() * segment->hpdPanelHitPoint().y() > 0 ) {
      return pixel->globalPosition().distance2( segment->hpdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().y() > 0 && segment->photonsInYPlus() ) ||
                ( pixel->globalPosition().y() < 0 && segment->photonsInYMinus() ) ) {
      HepPoint3D temp( pixel->globalPosition().x(), 
                       -pixel->globalPosition().y(),
                       pixel->globalPosition().z() );
      return temp.distance2( segment->hpdPanelHitPoint() );
    }
  } else if ( Rich::Rich2 == segment->trackSegment().rich() ) {
    if ( pixel->globalPosition().x() * segment->hpdPanelHitPoint().x() > 0 ) {
      return pixel->globalPosition().distance2( segment->hpdPanelHitPoint() );
    } else if ( ( pixel->globalPosition().x() > 0 && segment->photonsInXPlus()  ) ||
                ( pixel->globalPosition().x() < 0 && segment->photonsInXMinus() ) ) {
      HepPoint3D temp( -pixel->globalPosition().x(), 
                       pixel->globalPosition().y(),
                       pixel->globalPosition().z() );
      return temp.distance2( segment->hpdPanelHitPoint() );
    }
  }

  return 99999999.9;
}

double RichRecPhotonTool::predictedPixelSignal( RichRecPhoton * photon,
                                                const Rich::ParticleIDType& id ) {

  double prob = photon->expPixelSignalPhots( id );
  if ( prob < -0.5 ) {

    prob = photon->geomPhoton().activeSegmentFraction() *
      ( ( signalProb(photon, id) * 
          m_richRecSegmentTool->nSignalPhotons(photon->richRecSegment(),id) ) +
        ( scatterProb(photon, id) * 
          m_richRecSegmentTool->nScatteredPhotons(photon->richRecSegment(),id) ) );

    photon->setExpPixelSignalPhots( id, prob );

  }

  return prob;
}

double RichRecPhotonTool::signalProb( RichRecPhoton * photon,
                                      const Rich::ParticleIDType& id ) {

  // Expected Cherenkov theta angle
  double thetaExp =
    m_richRecSegmentTool->avgCherenkovTheta( photon->richRecSegment(), id );
  if ( thetaExp < 0.000001 ) return 0.0;

  // Reconstructed Cherenkov theta angle
  double thetaReco = photon->geomPhoton().CherenkovTheta();

  // Expected Cherenkov theta angle resolution
  double thetaExpRes = 
    m_richRecSegmentTool->ckThetaResolution( photon->richRecSegment(), id );

  double thetaDiff = thetaReco-thetaExp;
  if ( fabs(thetaDiff) > 30.0*thetaExpRes ) return 0.;

  // Which detector
  Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();

  double expArg = 0.5*thetaDiff*thetaDiff/(thetaExpRes*thetaExpRes);
  expArg = ( expArg>650 ? 650 : expArg );
  return ( exp(-expArg) / ( sqrt(2.*M_PI)*M_PI*thetaExpRes ) ) *
    (2.*m_pixelArea / ( m_radiusCurv[det]*m_radiusCurv[det] *
                       (thetaReco>0.0001 ? thetaReco : 0.0001) ) );
}

double RichRecPhotonTool::scatterProb( RichRecPhoton * photon,
                                       const Rich::ParticleIDType& id ) {

  Rich::RadiatorType rad = photon->richRecSegment()->trackSegment().radiator();

  if ( rad == Rich::Aerogel ) {

    // Expected Cherenkov theta angle
    double thetaExp =
      m_richRecSegmentTool->avgCherenkovTheta( photon->richRecSegment(), id );
    if ( thetaExp < 0.000001 ) return 0.0;

    double thetaRec = photon->geomPhoton().CherenkovTheta();

    double fbkg = 0.0;
    if ( thetaRec < thetaExp ) {
      fbkg = ( exp(17.0*thetaRec) - 1.0 ) / ( exp(17.0*thetaExp) - 1.0 );
    } else if ( thetaRec < 0.5*M_PI + thetaExp - 0.04 ) {
      fbkg = cos( thetaRec - thetaExp + 0.04 );
      fbkg = fbkg*fbkg/0.9984;
    } else {
      return 0.0;
    }

    Rich::DetectorType det = photon->richRecSegment()->trackSegment().rich();
    return 8.*m_pixelArea*fbkg /
      (M_PI*M_PI*m_radiusCurv[det]*m_radiusCurv[det]*thetaRec);

  }

  return 0.;
}

RichRecPhotons * RichRecPhotonTool::richPhotons() {
  return m_photons;
}

