// $Id: RichGeomEffDetailed.cpp,v 1.2 2003-07-03 14:46:58 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "RichGeomEffDetailed.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichGeomEffDetailed
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichGeomEffDetailed>          s_factory ;
const        IToolFactory& RichGeomEffDetailedFactory = s_factory ;

// Standard constructor
RichGeomEffDetailed::RichGeomEffDetailed ( const std::string& type,
                                           const std::string& name,
                                           const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichGeomEff>(this);

  // Define job option parameters
  declareProperty( "NPhotonsGeomEffCalc", m_nGeomEff = 100 );
  declareProperty( "NPhotonsGeomEffBailout", m_nGeomEffBailout = 20 ); 

  // randomn number distribution
  Rndm::Numbers m_uniDist;

}

StatusCode RichGeomEffDetailed::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

   // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);
  acquireTool("RichSegmentProperties", m_segProps);
  
  // randomn number service
  IRndmGenSvc * randSvc;
  if ( serviceLocator()->service( "RndmGenSvc", randSvc, true ) ) {
    if ( !m_uniDist.initialize( randSvc, Rndm::Flat(0,1) ) ) {
      msg << MSG::ERROR << "Unable to initialise randomn numbers" << endreq;
      return StatusCode::FAILURE;
    }
  } else { return StatusCode::FAILURE; }

  // Set up cached parameters for geometrical efficiency calculation
  m_hpdInc = 1.0 / ( (double)m_nGeomEff );
  m_incPhi = M_PI/2.0 + M_2PI/( (double)m_nGeomEff );
  //m_incPhi = M_2PI/( (double)m_nGeomEff );
  double ckPhi = 0.0;
  m_sinCkPhi.clear();
  m_cosCkPhi.clear();
  for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot, ckPhi += m_incPhi ) {
    m_sinCkPhi.push_back( sin(ckPhi) );
    m_cosCkPhi.push_back( cos(ckPhi) );
  }

  // Informational Printout
  msg << MSG::DEBUG
      << " Using full geometrical calculation" << endreq
      << " GeomEff Phots Max/Bailout  = " << m_nGeomEff << "/" << m_nGeomEffBailout << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichGeomEffDetailed::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Release tools and services
  m_uniDist.finalize();
  releaseTool( m_richDetInt );
  releaseTool( m_segProps );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichGeomEffDetailed::geomEfficiency ( RichRecSegment * segment,
                                             const Rich::ParticleIDType id ) {

  double eff = segment->geomEfficiency( id );
  if ( eff < -0.5 ) {

    // Cherenkov theta for this segment/hypothesis combination
    double ckTheta = m_segProps->avgCherenkovTheta( segment, id );
    if ( ckTheta <= 0 ) {
      eff = 0;
    } else {

      RichTrackSegment & trackSeg = segment->trackSegment();

      // Define rotation matrix
      HepVector3D z = trackSeg.bestMomentum().unit();
      HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                        z.cross( HepVector3D(0.,1.,0.) ) :
                        z.cross( HepVector3D(1.,0.,0.) ) );
      y.setMag(1);
      HepVector3D x = y.cross(z);
      HepRotation rotation = HepRotation();
      rotation.rotateAxes(x,y,z);

      int nDetect = 0;
      double sinCkTheta = sin(ckTheta);
      double cosCkTheta = cos(ckTheta);
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot ) {

        // Photon emission point is random between segment start and end points
        HepPoint3D emissionPt = trackSeg.bestPoint( m_uniDist() );

        // Photon direction around loop
        HepVector3D photDir = rotation*HepVector3D( sinCkTheta*m_cosCkPhi[iPhot],
                                                    sinCkTheta*m_sinCkPhi[iPhot],
                                                    cosCkTheta );

        // Ray trace through detector
        RichGeomPhoton photon;
        if ( 0 != m_richDetInt->traceToDetector( trackSeg.rich(),
                                                 emissionPt,
                                                 photDir,
                                                 photon ) ) {
          ++nDetect;
          segment->addToGeomEfficiencyPerHPD( id,
                                              (int)(photon.smartID().hpdID()),
                                              m_hpdInc );
          if ( photon.detectionPoint().x() > 0 ) {
            segment->setPhotonsInXPlus(1);
          } else {
            segment->setPhotonsInXMinus(1);
          }
          if ( photon.detectionPoint().y() > 0 ) {
            segment->setPhotonsInYPlus(1);
          } else {
            segment->setPhotonsInYMinus(1);
          }

        }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = (double)nDetect/(double)m_nGeomEff;

    } // CK theta IF

    segment->setGeomEfficiency( id, eff );
  }

  return eff;
}

double RichGeomEffDetailed::geomEfficiencyScat ( RichRecSegment * segment,
                                                 const Rich::ParticleIDType id ) {

  double eff = segment->geomEfficiencyScat( id );
  if ( eff < -0.5 ) {

    // only for aerogel
    if ( segment->trackSegment().radiator() != Rich::Aerogel ) {
      eff = 0;
    } else {

      RichTrackSegment & trackSeg = segment->trackSegment();

      // Photon emission point is end of aerogel
      HepPoint3D emissionPt = trackSeg.exitPoint();

      // Define rotation matrix
      HepVector3D z = trackSeg.bestMomentum().unit();
      HepVector3D y = ( fabs( z * HepVector3D(1.,0.,0.) ) < 1. ?
                        z.cross( HepVector3D(0.,1.,0.) ) :
                        z.cross( HepVector3D(1.,0.,0.) ) );
      y.setMag(1);
      HepVector3D x = y.cross(z);
      HepRotation rotation = HepRotation();
      rotation.rotateAxes(x,y,z);

      int nDetect = 0;
      RichGeomPhoton photon;
      for ( int iPhot = 0; iPhot < m_nGeomEff; ++iPhot ) {

        // generate randomn cos(theta)**2 distribution for thetaCk
        double ckTheta;
        do {
          ckTheta = m_uniDist()*M_PI;
        } while ( m_uniDist() > pow(cos(ckTheta),2) );

        // Photon direction around loop
        HepVector3D photDir = rotation*HepVector3D( sin(ckTheta)*m_cosCkPhi[iPhot],
                                                    sin(ckTheta)*m_sinCkPhi[iPhot],
                                                    cos(ckTheta) );

        // Ray trace through detector
        if ( 0 != m_richDetInt->traceToDetector( trackSeg.rich(),
                                                 emissionPt,
                                                 photDir,
                                                 photon ) ) { ++nDetect; }

        // Bail out if tried m_geomEffBailout times and all have failed
        if ( 0 == nDetect && iPhot >= m_nGeomEffBailout ) break;

      } // fake photon loop

      eff = (double)nDetect/(double)m_nGeomEff;

    } // CK theta IF

    // Assign this efficiency to all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return eff;
}
