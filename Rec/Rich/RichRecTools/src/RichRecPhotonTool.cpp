// $Id: RichRecPhotonTool.cpp,v 1.2 2002-11-14 13:54:24 jonrob Exp $
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
#include "RichRecTools/RichRecPhotonTool.h"


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
  declareProperty( "ChronoTiming", m_timing = false );

  m_maxROI.push_back( 99990. ); // aerogel
  m_maxROI.push_back( 70. );    // c4f10
  m_maxROI.push_back( 160. );   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

  m_minROI.push_back( 0. ); // aerogel
  m_minROI.push_back( 0. ); // c4f10
  m_minROI.push_back( 0. ); // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxCKtheta.push_back( 0.250 ); // aerogel
  m_maxCKtheta.push_back( 0.055 ); // c4f10
  m_maxCKtheta.push_back( 0.035 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  m_minCKtheta.push_back( 0.075 ); // aerogel
  m_minCKtheta.push_back( 0.0 );   // c4f10
  m_minCKtheta.push_back( 0.0 );   // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

}

StatusCode RichRecPhotonTool::initialize() {

  MsgStream log( msgSvc(), name() );
  log << MSG::DEBUG << "Initialize" << endreq;

  StatusCode sc = StatusCode::SUCCESS;

  // Get pointer to EDS
  if ( !serviceLocator()->service( "EventDataSvc", m_evtDataSvc, true ) ) {
    log << MSG::ERROR << "EventDataSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecTrack Tool
  if ( !toolSvc()->retrieveTool( "RichRecTrackTool", m_richRecTrackTool) ) {
    log << MSG::ERROR << "RichRecTrackTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecSegment Tool
  if ( !toolSvc()->retrieveTool( "RichRecSegmentTool", m_richRecSegmentTool) ) {
    log << MSG::ERROR << "RichRecSegmentTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichRecPixel Tool
  if ( !toolSvc()->retrieveTool( "RichRecPixelTool", m_richRecPixelTool) ) {
    log << MSG::ERROR << "RichRecPixelTool not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Get pointer to RichDetInterface
  if ( !toolSvc()->retrieveTool( "RichDetInterface", m_richDetInterface ) ) {
    log << MSG::ERROR << "RichDetInterface not found" << endreq;
    sc = StatusCode::FAILURE;
  }

  // Setup incident services
  IIncidentSvc * incSvc;
  if ( !serviceLocator()->service( "IncidentSvc", incSvc, true ) ) {
    log << MSG::ERROR << "IncidentSvc not found" << endreq;
    sc = StatusCode::FAILURE;
  } else {
    incSvc->addListener( this, "BeginEvent" ); // Informed of a new event
    incSvc->addListener( this, "EndEvent"   ); // Informed at the end of event
  }

  // ChronoStat timing
  if ( m_timing ) {
    if ( !serviceLocator()->service( "ChronoStatSvc", m_chrono, true ) ) {
      log << MSG::ERROR << "ChronoStatSvc not found" << endreq;
      m_timing = false;
      sc = StatusCode::FAILURE;
    }
  }

  // Initialise some variable
  m_maxROI2.push_back( m_maxROI[0]*m_maxROI[0] );
  m_maxROI2.push_back( m_maxROI[1]*m_maxROI[1] );
  m_maxROI2.push_back( m_maxROI[2]*m_maxROI[2] );

  // Temporary variables
  m_radiusCurv[ Rich::Rich1 ] = 240.0;
  m_radiusCurv[ Rich::Rich2 ] = 860.0;
  m_pixelArea = 0.2*0.2;
  m_sigmaMin[ Rich::Aerogel ] = 0.00036;
  m_sigmaMax[ Rich::Aerogel ] = 0.00126;
  m_sigmaMin[ Rich::C4F10   ] = 0.00054;
  m_sigmaMax[ Rich::C4F10   ] = 0.00076;
  m_sigmaMin[ Rich::CF4     ] = 0.00021;
  m_sigmaMax[ Rich::CF4     ] = 0.00030;
  m_theerr[0][0] = 0.00286;
  m_theerr[1][0] = 0.00206;
  m_theerr[2][0] = 0.00164;
  m_theerr[0][1] = 0.00223;
  m_theerr[1][1] = 0.00120;
  m_theerr[2][1] = 0.00093;
  m_theerr[0][2] = 0.000906;
  m_theerr[1][2] = 0.000593;
  m_theerr[2][2] = 0.000560;
  m_thebin[0][0] = 0.200;
  m_thebin[1][0] = 0.250;
  m_thebin[0][1] = 0.030;
  m_thebin[1][1] = 0.045;
  m_thebin[0][2] = 0.018;
  m_thebin[1][2] = 0.025;

  // Issue health warnings to remind me of various things
  log << MSG::WARNING;
  log << "Using hardcoded value for pixel area" << endreq;
  log << "Using hardcoded values for radii of curvature" << endreq;
  log << "using hardcoded parameterisation of scattering probability" << endreq;

  return sc;
}

// Method that handles various Gaudi "software events"
void RichRecPhotonTool::handle ( const Incident& incident ) {

  MsgStream log( msgSvc(), name() );
  log << MSG::VERBOSE << "Informed of incident '" << incident.type()
      << "' generated by '" << incident.source() << "'" << endreq;

  if ( "BeginEvent" == incident.type() ) {

    // Initialise navigation data
    m_photonDone.clear();
    m_allBuilt = false;

    SmartDataPtr<RichRecPhotons> tdsPhotons( m_evtDataSvc,
                                             m_richRecPhotonLocation );
    if ( !tdsPhotons ) {

      // Reinitialise the Photon Container
      m_photons = new RichRecPhotons();

      // Register new RichRecPhoton container to Gaudi data store
      if ( !m_evtDataSvc->registerObject(m_richRecPhotonLocation, m_photons) ) {
        log << MSG::ERROR << "Failed to register RichRecPhotons at "
            << m_richRecPhotonLocation << endreq;
      }

    } else {

      log << MSG::DEBUG
          << "Located " << tdsPhotons->size() << " RichRecPhotons at "
          << m_richRecPhotonLocation << endreq;

      // Set smartref to TES photon container
      m_photons = tdsPhotons;

      // Remake local photon reference map
      for ( RichRecPhotons::const_iterator iPhoton = tdsPhotons->begin();
            iPhoton != tdsPhotons->end();
            ++iPhoton ) {
        m_photonDone[(int)(*iPhoton)->key()] = true;
      }

    }

  } else if ( "EndEvent" == incident.type() ) {

    log << MSG::DEBUG << "Finalised with " << m_photons->size()
        << " RichRecPhotons at " << m_richRecPhotonLocation << endreq;

  }
  return;
}

RichRecPhoton*
RichRecPhotonTool::reconstructPhoton( RichRecSegment * segment,
                                      RichRecPixel * pixel ) {

  // check photon is possible before proceeding
  if ( !photonPossible(segment, pixel) ) return NULL;

  // Form the key for this photon
  m_photonKey.setSegmentNumber( segment->key() );
  m_photonKey.setPixelNumber( pixel->key() );

  // See if this photon already exists
  if ( m_photonDone[(int)m_photonKey] ) {
    return (RichRecPhoton*)(m_photons->object(m_photonKey));
  } else {
    return buildPhoton( segment, pixel );
  }

}

RichRecPhoton * RichRecPhotonTool::buildPhoton( RichRecSegment * segment,
                                                RichRecPixel * pixel ) {

  // if (m_timing) m_chrono->chronoStart("RichRecPhotonTool:buildPhoton");

  RichRecPhoton * newPhoton = NULL;

  // Reconstruct the Cherenkov angles
  RichGeomPhoton geomPhoton;
  m_richDetInterface->reconstructPhoton( segment->trackSegment(),
                                         pixel->globalPosition(),
                                         geomPhoton );

  // Form the key for this photon
  m_photonKey.setSegmentNumber( segment->key() );
  m_photonKey.setPixelNumber( pixel->key() );

  if ( ( geomPhoton.CherenkovTheta() > 0. ||
         geomPhoton.CherenkovPhi() > 0. ) &&
       geomPhoton.CherenkovTheta() <
       m_maxCKtheta[(int)segment->trackSegment().radiator()] &&
       geomPhoton.CherenkovTheta() >
       m_minCKtheta[(int)segment->trackSegment().radiator()] ) {

    newPhoton = new RichRecPhoton();

    // Form a new RichRecPhoton and give to Gaudi
    m_photons->insert( newPhoton, m_photonKey );

    // set geometrical photon information
    newPhoton->setGeomPhoton( geomPhoton );

    // Set various navigation info
    newPhoton->setRichRecSegment( segment );
    newPhoton->setRichRecPixel( pixel );
    segment->addToRichRecPixels( pixel );
    segment->addToRichRecPhotons( newPhoton );
    segment->richRecTrack()->addToRichRecPixels( pixel );
    segment->richRecTrack()->addToRichRecPhotons( newPhoton );
    pixel->addToRichRecPhotons( newPhoton );
    pixel->addToRichRecTracks( segment->richRecTrack() );

    // Add shared ring fraction code here. Cannot do this
    // until geomPhoton has reflection points
  }

  // Add to reference map
  m_photonDone[(int)m_photonKey] = true;

  // if (m_timing) m_chrono->chronoStop("RichRecPhotonTool:buildPhoton");

  // Return pointer to this photon
  return newPhoton;

}

void RichRecPhotonTool::reconstructPhotons() {

  // This function skips checking if the photons already exist,
  // so only do once per event at most
  if ( !m_allBuilt ) {
    m_allBuilt = true;

    // Iterate over pixels
    for ( RichRecPixels::iterator pixel =
            m_richRecPixelTool->richPixels()->begin();
          pixel != m_richRecPixelTool->richPixels()->end();
          ++pixel ) {

      // Iterate over all tracks
      for ( RichRecTracks::iterator track =
              m_richRecTrackTool->richTracks()->begin();
            track != m_richRecTrackTool->richTracks()->end();
            ++track ) {

        // Iterate over segments
        for ( SmartRefVector<RichRecSegment>::iterator segment =
                (*track)->richRecSegments().begin();
              segment != (*track)->richRecSegments().end();
              ++segment) {

          //if ( photonPossible(*segment, *pixel) ) buildPhoton(*segment, *pixel);
          reconstructPhoton(*segment, *pixel);

        } // segment loop
      } // track loop
    } // pixel loop

  }

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
        ++track ) { reconstructPhotons( *track, pixel ); }

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

bool RichRecPhotonTool::photonPossible( RichRecSegment * segment,
                                        RichRecPixel * pixel ) {

  // fast decision on whether a photon is possible

  const HepPoint3D & segMiddle = segment->trackSegment().middlePoint();
  const HepPoint3D & pixelPoint = pixel->globalPosition();

  // Are they in the same Rich detector.
  if ( fabs( segMiddle.z() - pixelPoint.z() ) > 500. ) return false;
  return true; // disable selection below for time being

  /*
  const HepPoint3D & segmentPoint = segment->hpdPanelHitPoint();
  const Rich::Detector & det = segment->trackSegment().rich();
  const Rich::RadiatorType & rad = segment->trackSegment().radiator();

  float pixX = pixelPoint.x();
  float pixY = pixelPoint.y();
  float pixZ = pixelPoint.z();
  float segX = segmentPoint.x();
  float segY = segmentPoint.y();
  float segZ = segmentPoint.z();

  //  cout << "rad " << rad << endl;
  // cout << " Hits " << segmentPoint << " " << pixelPoint << endl;
  // cout << " seperation " << (segmentPoint-pixelPoint).mag()
  //     << "/" << m_maxROI[(int)rad] << endl;

  // Logic copied directly from SICB. Need to work out a better solution
 double rsep2;
  if ( ( Rich::Rich2 == det && pixX*segX > 0 ) ||
       ( Rich::Rich1 == det && pixY*segY > 0 ) ) {

    rsep2 = (pixX-segX)*(pixX-segX) +
      (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);

  } else if ( ( Rich::Rich2 == det && pixX*segX < 0 ) ||
              ( Rich::Rich1 == det && pixY*segY < 0 ) ) {

    if ( Rich::Rich2 == det &&
         ( ( pixX > 0 && segment->photonsInXPlus() ) ||
           ( pixX < 0 && segment->photonsInXMinus() ) ) ) {

      rsep2 = (pixX+segX)*(pixX+segX) +
        (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);

    } else if ( Rich::Rich1 == det &&
                ( ( pixY > 0 && segment->photonsInYPlus() ) ||
                  ( pixY < 0 && segment->photonsInYMinus() ) ) ) {

      rsep2 = (pixX+segX)*(pixX+segX) +
        (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);

    } else {

      rsep2 = 9990.0;

    }

  } else {

    rsep2 = 0.0;

  }

  //  if ( Rich::C4F10 == rad ) {
  // cout << "photonPossible: " << det << " " << rad << " "
  //     << rsep2 << " " << m_maxROI2[(int)rad] << " "
  //    << (rsep2 > m_maxROI2[(int)rad]) << endl;
  // }

  if ( rsep2 > m_maxROI2[(int)rad] ) return false;

  //if ( rad == Rich::C4F10 ) cout << "Success !! " << endl;

  return true;
  */
}

double RichRecPhotonTool::pixelSignalProb( RichRecPhoton * photon,
                                           const Rich::ParticleIDType id ) {

  double prob = photon->expPixelSignalPhots( id );
  if ( prob < -0.5 ) {

    // if (m_timing) m_chrono->chronoStart("RichRecPhotonTool:pixelSignalProb");

    double signalPhots =
      m_richRecSegmentTool->signalPhotons( photon->richRecSegment(), id );
    double scattPhots =
      m_richRecSegmentTool->scatteredPhotons( photon->richRecSegment(), id );

    double Psig = this->signalProb( photon, id );
    double Pscatt = this->scatterProb( photon, id );
    prob = Psig*signalPhots + Pscatt*scattPhots;

    MsgStream log( msgSvc(), name() );
    log << MSG::VERBOSE << "pixelSignalProb : id " << id
        << ", Psig " << Psig << ", Pscatt " << Pscatt
        << ", signalPhots " << signalPhots
        << ", scattPhots " << scattPhots << ", pixelSignal " << prob << endreq;

    photon->setExpPixelSignalPhots( id, prob );

    // if (m_timing) m_chrono->chronoStop("RichRecPhotonTool:pixelSignalProb");
  }

  return prob;
}

double RichRecPhotonTool::photonResolution( RichRecPhoton * photon,
                                            const Rich::ParticleIDType id ) {

  // Expected Cherenkov theta angle
  double thetaExp =
    m_richRecSegmentTool->avgCherenkovTheta( photon->richRecSegment(), id );
  if ( thetaExp < 0.000001 ) return 0.;

  Rich::RadiatorType rad = photon->richRecSegment()->trackSegment().radiator();

  // Functional implementation
  /*
  // Refractive index
  double refIndex = m_richDetInterface-> refractiveIndex( rad );

  double resPar1 = m_sigmaMin[rad]*m_sigmaMin[rad];
  double resPar2 = ( m_sigmaMax[rad]*m_sigmaMax[rad] - resPar1 )*
    (refIndex*refIndex - 1.);
  double tanTheta = tan(thetaExp);

  return sqrt( resPar1 + resPar2/(tanTheta*tanTheta) );
  */

  // Fortran parameterised implementation
  if ( thetaExp > 0. &&  thetaExp < m_thebin[0][(int)rad] ) {
    return m_theerr[0][(int)rad];
  } else if ( thetaExp > m_thebin[0][(int)rad] &&
              thetaExp < m_thebin[1][(int)rad] ) {
    return m_theerr[1][(int)rad];
  } else if ( thetaExp > m_thebin[1][(int)rad] ) {
    return m_theerr[2][(int)rad];
  }

  return 0.;
}

double RichRecPhotonTool::signalProb( RichRecPhoton * photon,
                                      const Rich::ParticleIDType id ) {


  // Expected Cherenkov theta angle
  double thetaExp =
    m_richRecSegmentTool->avgCherenkovTheta( photon->richRecSegment(), id );
  if ( thetaExp < 0.000001 ) return 0.0;

  // Reconstructed Cherenkov theta angle
  double thetaReco = photon->geomPhoton().CherenkovTheta();

  // Which detector
  Rich::Detector det = photon->richRecSegment()->trackSegment().rich();

  // Expected Cherenkov theta angle resolution
  double thetaExpRes = this->photonResolution( photon, id );

  double thetaDiff = thetaReco-thetaExp;
  if ( fabs(thetaDiff) > 30.0*thetaExpRes ) return 0.;

  double fsig = exp( -0.5*thetaDiff*thetaDiff/(thetaExpRes*thetaExpRes))/
    ( sqrt(2.*M_PI)*M_PI*thetaExpRes);
  double farea = 2.*m_pixelArea / ( m_radiusCurv[det]*m_radiusCurv[det] *
                                    (thetaReco>0.00001 ? thetaReco : 0.00001) );

  return fsig*farea;
}

double RichRecPhotonTool::scatterProb( RichRecPhoton * photon,
                                       const Rich::ParticleIDType id ) {


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

    Rich::Detector det = photon->richRecSegment()->trackSegment().rich();
    return 8.*m_pixelArea*fbkg /
      (M_PI*M_PI*m_radiusCurv[det]*m_radiusCurv[det]*thetaRec);

  }

  return 0.;
}

RichRecPhotons * RichRecPhotonTool::richPhotons() {
  return m_photons;
}

