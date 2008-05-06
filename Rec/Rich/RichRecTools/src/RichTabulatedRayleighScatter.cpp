
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.cpp
 *
 *  Implementation file for tool : Rich::Rec::TabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.cpp,v 1.13 2008-05-06 15:33:38 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedRayleighScatter.h"

// Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/SystemOfUnits.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( TabulatedRayleighScatter );

// Standard constructor
TabulatedRayleighScatter::TabulatedRayleighScatter ( const std::string& type,
                                                     const std::string& name,
                                                     const IInterface* parent )
  : Rich::Rec::ToolBase( type, name, parent )
{
  // interface
  declareInterface<IRayleighScatter>(this);
}

StatusCode TabulatedRayleighScatter::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get aerogel radiator
  // for aero, get the "0" file, as the multisolid does not have any properties
  // this whole scheme needs reworking to cope properly with different aerogel tiles.
  m_aero = getDet<DeRichAerogelRadiator>( DeRichLocations::Aerogel+"T0:0" );

  return sc;
}

double
TabulatedRayleighScatter::photonScatteredProb( const LHCb::RichRecSegment * segment,
                                               const double energy ) const
{
  // check this is aerogel
  if ( Rich::Aerogel != segment->trackSegment().radiator() ) return 0;

  // check energy is valid
  if ( energy <= 0 ) return 0;

  // check path length is valid
  const double path = segment->trackSegment().pathLength();
  if ( path <= 0 ) return 0;

  // compute and return prob
  const double scatLeng = (*(m_aero->rayleigh()))[energy*Gaudi::Units::eV];
  return ( 1 - (scatLeng/path)*(1.0-exp(-path/scatLeng)) );
}
