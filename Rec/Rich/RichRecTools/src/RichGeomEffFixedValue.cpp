// $Id: RichGeomEffFixedValue.cpp,v 1.2 2003-08-06 11:08:12 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichGeomEffFixedValue.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichGeomEffFixedValue
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichGeomEffFixedValue>          s_factory ;
const        IToolFactory& RichGeomEffFixedValueFactory = s_factory ;

// Standard constructor
RichGeomEffFixedValue::RichGeomEffFixedValue ( const std::string& type,
                                               const std::string& name,
                                               const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichGeomEff>(this);

  declareProperty( "FixedSignalEfficiency", m_fixedValue = 0.73 );
  declareProperty( "FixedScatterEfficiency", m_fixedScatValue = 0.73 );

}

StatusCode RichGeomEffFixedValue::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Informational Printout
  msg << MSG::DEBUG
      << " Using fixed value calculation" << endreq;

  return StatusCode::SUCCESS;
}

StatusCode RichGeomEffFixedValue::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichGeomEffFixedValue::geomEfficiency ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) {

  if ( !segment->geomEfficiency().dataIsValid(id) ) {
    segment->setGeomEfficiency( id, m_fixedValue );
  }

  return segment->geomEfficiency( id );
}

double RichGeomEffFixedValue::geomEfficiencyScat ( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) {

  if ( !segment->geomEfficiencyScat().dataIsValid(id) ) {
    segment->setGeomEfficiencyScat( id, m_fixedScatValue );
  }

  return segment->geomEfficiencyScat( id );
}
