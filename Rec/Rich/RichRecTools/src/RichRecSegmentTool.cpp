// $Id: RichRecSegmentTool.cpp,v 1.1.1.1 2002-07-28 10:46:20 jonesc Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IParticlePropertySvc.h"
#include "GaudiKernel/ParticleProperty.h"

// local
#include "RichRecTools/RichRecSegmentTool.h"
#include "RichRecTools/RichPhotonSpectrum.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecSegmentTool
//
// 15/03/2002 : Chris Jones
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichRecSegmentTool>          s_factory ;
const        IToolFactory& RichRecSegmentToolFactory = s_factory ;

// Standard constructor
RichRecSegmentTool::RichRecSegmentTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichRecSegmentTool>(this);

  // Get Pointer to ToolSvc
  StatusCode scTool = serviceLocator()->service("ToolSvc",
                                                m_toolService, true);
  if( scTool.isFailure() ) {
    throw GaudiException( "ToolSvc not found",
                          "RichRecSegmentToolException", scTool );
  }

  // Get pointer to Rich Detector Tool
  StatusCode scDet = m_toolService->retrieveTool( "RichDetInterface",
                                                  m_richDetInterface );
  if ( scDet.isFailure() ) {
    throw GaudiException( "RichDetInterface not found",
                          "RichRecSegmentToolException", scDet );
  }

  // Retrieve particle property service
  StatusCode pPsc = serviceLocator()->service( "ParticlePropertySvc",
                                               m_ppSvc );
  if ( pPsc.isFailure() ) {
    throw GaudiException( "ParticlePropertySvc not found",
                          "RichRecSegmentToolException", pPsc );
  }

  // Retrieve particle masses
  m_particleMass[ Rich::Electron ] = m_ppSvc->find("e+" )->mass()/GeV;
  m_particleMass[ Rich::Muon ]     = m_ppSvc->find("mu+")->mass()/GeV;
  m_particleMass[ Rich::Pion ]     = m_ppSvc->find("pi+")->mass()/GeV;
  m_particleMass[ Rich::Kaon ]     = m_ppSvc->find("K+" )->mass()/GeV;
  m_particleMass[ Rich::Proton ]   = m_ppSvc->find("p+" )->mass()/GeV;

}

double
RichRecSegmentTool::expObsPhotonSignal ( SmartRef<RichRecSegment>& segment,
                                         const Rich::ParticleIDType id ) {
  return expPhotons( segment, id ) * geomEfficiency( segment, id );
}

double RichRecSegmentTool::expPhotons ( SmartRef<RichRecSegment>& segment,
                                        const Rich::ParticleIDType id ) {

  double signal = segment->emittedPhotons( id );
  if ( signal < -0.5 ) {

    double mass = m_particleMass[id];
    double momentum = segment->trackSegment().momentum().mag();
    double beta = momentum/sqrt(momentum*momentum + mass*mass);
    double length = segment->trackSegment().pathLength();

    // Create a new photon spectrum for this segment
    //    RichPhotonSpectrum* spectrum = new RichPhotonSpectrum( 0.73 );

    // For time being hardcode a signal of 10
    segment->setEmittedPhotons( id, 10.0 );
    signal = 10.0;
  }

  return signal;
}

double
RichRecSegmentTool::geomEfficiency ( SmartRef<RichRecSegment>& segment,
                                     const Rich::ParticleIDType id ) {

  double eff = segment->geomEfficiency( id );
  if ( eff > -0.5 ) {
    return eff;
  } else {
    // Eventually this will do the full geometrical efficiency calculation.
    // for the moment 73% is returned
    segment->setGeomEfficiency( id, 0.73 );
    return 0.73;
  }

}
