// $Id: RichTabulatedRefractiveIndex.cpp,v 1.1 2003-08-26 14:40:21 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichTabulatedRefractiveIndex.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichTabulatedRefractiveIndex
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedRefractiveIndex>          s_factory ;
const        IToolFactory& RichTabulatedRefractiveIndexFactory = s_factory ;

// Standard constructor
RichTabulatedRefractiveIndex::RichTabulatedRefractiveIndex ( const std::string& type,
                                                             const std::string& name,
                                                             const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichRefractiveIndex>(this);

  // Define job option parameters
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/AerogelIdealRIndexPT");
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/C4F10SellParamRIndexPT");
  m_refLocations.push_back("/dd/Materials/RichMaterialTabProperties/CF4RIndexPT");
  declareProperty( "RefIndexLocations", m_refLocations );
}

StatusCode RichTabulatedRefractiveIndex::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);

  // Get the Quantum Eff
  // For time being assume only one reference curve for all HPDs
  m_referenceQE = m_richDetInt->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  // release det interface
  releaseTool( m_richDetInt );

  // Get the refractive indices from the XML
  SmartDataPtr<TabulatedProperty> tabAero( detSvc(), m_refLocations[Rich::Aerogel] );
  if ( !tabAero ) {
    msg << MSG::ERROR << "Failed to load refractive index from "
        << m_refLocations[Rich::Aerogel] << endreq;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<TabulatedProperty> tabR1Gas( detSvc(), m_refLocations[Rich::Rich1Gas] );
  if ( !tabR1Gas ) {
    msg << MSG::ERROR << "Failed to load refractive index from "
        << m_refLocations[Rich::Rich1Gas] << endreq;
    return StatusCode::FAILURE;
  }
  SmartDataPtr<TabulatedProperty> tabR2Gas( detSvc(), m_refLocations[Rich::Rich2Gas] );
  if ( !tabR2Gas ) {
    msg << MSG::ERROR << "Failed to load refractive index from "
        << m_refLocations[Rich::Rich2Gas] << endreq;
    return StatusCode::FAILURE;
  }

  // Create tabulated property objects from XML information
  m_refIndex[Rich::Aerogel]  = new Rich1DTabProperty( tabAero  );
  m_refIndex[Rich::Rich1Gas] = new Rich1DTabProperty( tabR1Gas );
  m_refIndex[Rich::Rich2Gas] = new Rich1DTabProperty( tabR2Gas );

  // Informational Printout
  msg << MSG::DEBUG
      << " Using XML tabulated implimentation" << endreq
      << " Aerogel refractive index     = " << m_refLocations[Rich::Aerogel] << endreq
      << " Rich1Gas refractive index    = " << m_refLocations[Rich::Rich1Gas] << endreq
      << " Rich2Gas refractive index    = " << m_refLocations[Rich::Rich2Gas] << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedRefractiveIndex::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Delete tabulated properties
  for ( RefractiveIndices::iterator iRef = m_refIndex.begin();
        iRef != m_refIndex.end(); ++iRef ) {
    if ( *iRef ) { delete *iRef; *iRef = 0; }
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType radiator,
                                                      double energy ) {
  return (*m_refIndex[radiator])[energy*eV];
}

double RichTabulatedRefractiveIndex::refractiveIndex( RichRecSegment * segment ) {
  return refractiveIndex( segment->trackSegment().radiator() );
}

double RichTabulatedRefractiveIndex::refractiveIndex( const Rich::RadiatorType radiator ) {

  double meanEnergy = m_referenceQE->meanX( m_referenceQE->minX(),
                                            m_referenceQE->maxX() ) / eV;
  return refractiveIndex( radiator, meanEnergy );
}
