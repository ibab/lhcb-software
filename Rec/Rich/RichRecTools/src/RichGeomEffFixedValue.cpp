
/** @file RichGeomEffFixedValue.cpp
 *
 *  Implementation file for tool : RichGeomEffFixedValue
 *
 *  CVS Log :-
 *  $Id: RichGeomEffFixedValue.cpp,v 1.7 2004-07-27 20:15:30 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichGeomEffFixedValue.h"

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

  m_fixedValue.push_back( 0.73 ); // aerogel
  m_fixedValue.push_back( 0.73 ); // c4f10
  m_fixedValue.push_back( 0.73 ); // cf4
  declareProperty( "FixedSignalEfficiency", m_fixedValue );
  declareProperty( "FixedScatterEfficiency", m_fixedScatValue = 0.73 );

}

StatusCode RichGeomEffFixedValue::initialize() {

  // Sets up various tools and services
  StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  acquireTool( "RichCherenkovAngle", m_ckAngle );

  return StatusCode::SUCCESS;
}

StatusCode RichGeomEffFixedValue::finalize() 
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichGeomEffFixedValue::geomEfficiency ( RichRecSegment * segment,
                                               const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiency().dataIsValid(id) ) {

    // compute efficiency
    const double eff = ( m_ckAngle->avgCherenkovTheta( segment, id ) > 0 ?
                         m_fixedValue[segment->trackSegment().radiator()] : 0 );

    // Set the geom eff
    segment->setGeomEfficiency( id, eff );

    // flag where hits are.. Need to do this better
    segment->setPhotonsInXPlus(1);
    segment->setPhotonsInXMinus(1);
    segment->setPhotonsInYPlus(1);
    segment->setPhotonsInYMinus(1);

  }

  return segment->geomEfficiency( id );
}

double RichGeomEffFixedValue::geomEfficiencyScat ( RichRecSegment * segment,
                                                   const Rich::ParticleIDType id ) const {

  if ( !segment->geomEfficiencyScat().dataIsValid(id) ) {

    double eff = 0;
    if ( segment->trackSegment().radiator() == Rich::Aerogel ) {
      eff = ( m_ckAngle->avgCherenkovTheta(segment,id) > 0 ? m_fixedScatValue : 0 );
    }

    // Assign this efficiency to all hypotheses
    segment->setGeomEfficiencyScat( Rich::Electron, eff );
    segment->setGeomEfficiencyScat( Rich::Muon, eff );
    segment->setGeomEfficiencyScat( Rich::Pion, eff );
    segment->setGeomEfficiencyScat( Rich::Kaon, eff );
    segment->setGeomEfficiencyScat( Rich::Proton, eff );

  }

  return segment->geomEfficiencyScat( id );
}
