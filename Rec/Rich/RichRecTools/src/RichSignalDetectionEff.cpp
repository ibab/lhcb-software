// $Id: RichSignalDetectionEff.cpp,v 1.1 2003-08-06 11:08:14 jonrob Exp $

// local
#include "RichSignalDetectionEff.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichSignalDetectionEff
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSignalDetectionEff>          s_factory ;
const        IToolFactory& RichSignalDetectionEffFactory = s_factory ;

// Standard constructor
RichSignalDetectionEff::RichSignalDetectionEff ( const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichSignalDetectionEff>(this);

  // Define job option parameters

  // temporary parameters to take into acount degraded performance for robustness tests
  declareProperty( "ScalePhotonEff", m_photonEffScale = 1 );

}

StatusCode RichSignalDetectionEff::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Acquire instances of tools
  acquireTool("RichDetInterface", m_richDetInt);

  // Define mirror reflectivities and energy cutoff values.
  // Private implementation that should be removed at some later date and
  // RichDetInterface used instead
  m_detReflectorEff = 0.9*0.9;

  // Quartz window eff
  m_quartzWinEff = 0.92;

  // Digitisation pedestal loss
  m_pedLoss = 0.899999976;

  // Get the Quantum Eff
  // For time being assume only one reference curve for all HPDs
  m_referenceQE = m_richDetInt->hpdQuantumEff();
  if ( !m_referenceQE ) {
    msg << MSG::ERROR << "Failed to acquire QE function" << endreq;
    return StatusCode::FAILURE;
  }

  // Informational Printout
  msg << MSG::DEBUG
      << " Applying average mirror reflectivity = " << m_detReflectorEff << endreq
      << " Applying quartz window efficiency    = " << m_quartzWinEff << endreq
      << " Applying digitisation pedestal loss  = " << m_pedLoss << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichSignalDetectionEff::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  releaseTool( m_richDetInt );

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichSignalDetectionEff::photonDetEfficiency( double energy ) {

  return (*m_referenceQE)[energy*eV]/100 *
    m_quartzWinEff * m_detReflectorEff *
    m_pedLoss * m_photonEffScale;
  
}
