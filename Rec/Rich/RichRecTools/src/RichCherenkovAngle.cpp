// $Id: RichCherenkovAngle.cpp,v 1.1 2003-08-06 11:08:12 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichCherenkovAngle.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichCherenkovAngle
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichCherenkovAngle>          s_factory ;
const        IToolFactory& RichCherenkovAngleFactory = s_factory ;

// Standard constructor
RichCherenkovAngle::RichCherenkovAngle ( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichCherenkovAngle>(this);

}

StatusCode RichCherenkovAngle::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool( "RichExpectedTrackSignal", m_signal       );
  acquireTool( "RichRefractiveIndex",     m_refIndex     );
  acquireTool( "RichParticleProperties",  m_richPartProp );

  // Informational Printout
  // msg << MSG::DEBUG;

  return StatusCode::SUCCESS;
}

StatusCode RichCherenkovAngle::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_signal );
  releaseTool( m_refIndex );
  releaseTool( m_richPartProp );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichCherenkovAngle::avgCherenkovTheta( RichRecSegment * segment,
                                              const Rich::ParticleIDType id ) {

  if ( !segment->averageCKTheta().dataIsValid(id) ) {
    double angle = 0;

    // total unscattered signal
    double unscat = m_signal->nSignalPhotons( segment, id );
    if ( unscat > 0 ) {

      // which radiator
      Rich::RadiatorType rad = segment->trackSegment().radiator();

      // loop over energy bins
      RichPhotonSpectra & sigSpectra = segment->signalPhotonSpectra();
      for ( unsigned int iEnBin = 0; iEnBin < sigSpectra.energyBins(); ++iEnBin ) {
        double temp = 
          m_refIndex->refractiveIndex( rad, sigSpectra.binEnergy(iEnBin) ) * 
          m_richPartProp->beta(segment, id);
        angle += (sigSpectra.energyDist(id))[iEnBin] * ( temp>1 ? acos(1/temp) : 0 );
      }
      
      // normalise the angle
      angle /= unscat;

    }

    segment->setAverageCKTheta( id, angle );
  }

  return segment->averageCKTheta( id );
}
