// $Id: RichRecPhotonTool.cpp,v 1.5 2002-12-20 09:33:08 cattanem Exp $
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

  m_minROI.push_back( 130. );   // aerogel
  m_minROI.push_back( 115. );   // c4f10
  m_minROI.push_back(  55. );   // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 375. );   // aerogel
  m_maxROI.push_back( 170. );   // c4f10
  m_maxROI.push_back( 195. );   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

  m_maxCKtheta.push_back( 0.300 ); // aerogel
  m_maxCKtheta.push_back( 0.080 ); // c4f10
  m_maxCKtheta.push_back( 0.050 ); // cf4
  declareProperty( "MaxCherenkovTheta", m_maxCKtheta );

  m_minCKtheta.push_back( 0.0 );   // aerogel
  m_minCKtheta.push_back( 0.0 );   // c4f10
  m_minCKtheta.push_back( 0.0 );   // cf4
  declareProperty( "MinCherenkovTheta", m_minCKtheta );

  declareProperty( "MinPixelProbability", m_minPixelProb = 1e-15 );

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

  // Initialise some variables
  m_minROI2.push_back( m_minROI[0]*m_minROI[0] );
  m_minROI2.push_back( m_minROI[1]*m_minROI[1] );
  m_minROI2.push_back( m_minROI[2]*m_minROI[2] );
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

StatusCode RichRecPhotonTool::finalize() {
  // Release all tools
  if( m_richRecTrackTool )   toolSvc()->releaseTool( m_richRecTrackTool );
  if( m_richRecSegmentTool ) toolSvc()->releaseTool( m_richRecSegmentTool );
  if( m_richRecPixelTool )   toolSvc()->releaseTool( m_richRecPixelTool );
  if( m_richDetInterface )   toolSvc()->releaseTool( m_richDetInterface );

  return StatusCode::SUCCESS;
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

  // if (m_timing) m_chrono->chronoStart("RichRecPhotonTool:buildPhoton");

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

      // set geometrical photon information
      newPhoton->setGeomPhoton( geomPhoton );

      // Set various navigation info
      newPhoton->setRichRecSegment( segment );
      newPhoton->setRichRecPixel( pixel );

      // check photon has significant probability to be signal for any 
      // hypothesis. If not then reject and delete
      bool keepPhoton = false;
      for ( Rich::ParticleIDType hypo = Rich::ParticleIDTypeFirst;
            hypo <= Rich::ParticleIDTypeLast;
            ++hypo ) {
        if ( pixelSignalProb(newPhoton,hypo) > m_minPixelProb ) {
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
              pix != tkPixs.end();
              pix++ ) {
          RichRecPixel* pPix = *pix;
          if ( pPix == pixel ) {
            notThere = false;
            break;
          }
        }
        if ( notThere ) segment->richRecTrack()->addToRichRecPixels( pixel );

        // Add shared ring fraction code here. Cannot do this
        // until geomPhoton has reflection points

      } else {
        delete newPhoton;
        newPhoton = NULL;
      }

    } // end angles if
  } // end reconstructPhoton method if

  // Add to reference map
  m_photonDone[(int)key] = true;

  // if (m_timing) m_chrono->chronoStop("RichRecPhotonTool:buildPhoton");

    // Return pointer to this photon
  return newPhoton;

}

void RichRecPhotonTool::reconstructPhotons() {

  if ( !m_allBuilt ) {
    m_allBuilt = true;

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

          // If container was empty, skip checks on if photon already exists
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
  const Rich::RadiatorType & rad = segment->trackSegment().radiator();
  double sep = trackPixelHitSep2(segment, pixel);
  if ( sep > m_maxROI2[(int)rad] ) return false;
  if ( sep < m_minROI2[(int)rad] ) return false;

  return true;
}

double RichRecPhotonTool::trackPixelHitSep2( const RichRecSegment * segment,
                                             const RichRecPixel * pixel ) {

  const HepPoint3D & pixelPoint = pixel->globalPosition();
  const HepPoint3D & segmentPoint = segment->hpdPanelHitPoint();
  const Rich::Detector & det = segment->trackSegment().rich();

  float pixX = pixelPoint.x();
  float pixY = pixelPoint.y();
  float pixZ = pixelPoint.z();
  float segX = segmentPoint.x();
  float segY = segmentPoint.y();
  float segZ = segmentPoint.z();

  double rsep2 = 99999999.9;

  if ( Rich::Rich1 == det ) {
    if ( pixY*segY > 0 ) {
      rsep2 = (pixX-segX)*(pixX-segX) +
        (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);
    } else if ( ( pixY > 0 && segment->photonsInYPlus() ) ||
                ( pixY < 0 && segment->photonsInYMinus() ) ) {
      rsep2 = (pixX-segX)*(pixX-segX) +
        (pixY+segY)*(pixY+segY) + (pixZ-segZ)*(pixZ-segZ);
    }
  } else if ( Rich::Rich2 == det ) {
    if ( pixX*segX > 0 ) {
      rsep2 = (pixX-segX)*(pixX-segX) +
        (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);
    } else if ( ( pixX > 0 && segment->photonsInXPlus()  ) ||
                ( pixX < 0 && segment->photonsInXMinus() ) ) {
      rsep2 = (pixX+segX)*(pixX+segX) +
        (pixY-segY)*(pixY-segY) + (pixZ-segZ)*(pixZ-segZ);
    }
  }

  //rsep2 = (fabs(pixX)-fabs(segX))*(fabs(pixX)-fabs(segX)) +
  //  (fabs(pixY)-fabs(segY))*(fabs(pixY)-fabs(segY)) +
  //  (fabs(pixZ)-fabs(segZ))*(fabs(pixZ)-fabs(segZ));

  //if ( sqrt(rsep2) > 999 ) {
  //  cout << segment->key() << " " << pixel->key() << " : "
  //       << det << " " << segment->trackSegment().radiator() << " : "
  //       << segmentPoint << " : "
  //       << segment->photonsInXPlus() << segment->photonsInXMinus()
  //       << segment->photonsInYPlus() << segment->photonsInYMinus()
  //       << " : " << pixelPoint << " " << sqrt(rsep2) << endl;
  // }

  return rsep2;
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

