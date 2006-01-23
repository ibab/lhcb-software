
//-----------------------------------------------------------------------------
/** @file RichFunctionalRayleighScatter.cpp
 *
 *  Implementation file for tool : RichFunctionalRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichFunctionalRayleighScatter.cpp,v 1.12 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichFunctionalRayleighScatter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichFunctionalRayleighScatter>          s_factory ;
const        IToolFactory& RichFunctionalRayleighScatterFactory = s_factory ;

// Standard constructor
RichFunctionalRayleighScatter::
RichFunctionalRayleighScatter ( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_eVToMicron   ( 0 ),
    m_AeroClarity  ( 0 )
{
  // interface
  declareInterface<IRichRayleighScatter>(this);
}

StatusCode RichFunctionalRayleighScatter::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get Rich1 Detector element
  const DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocation::Rich1 );

  // Rayleigh scattering parameters
  m_eVToMicron  = ( h_Planck/(joule*s) * c_light/(m/s) / e_SI ) / nanometer ;
  m_AeroClarity = Rich1DE->param<double>( "AerogelClarity" )/cm;

  // Informational Printout
  debug() << " Using analytic implementation"   << endreq
          << " eV to mm conversion factor   = " << m_eVToMicron << endreq
          << " Aerogel clarity              = " << m_AeroClarity << endreq;

  return sc;
}

StatusCode RichFunctionalRayleighScatter::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichFunctionalRayleighScatter::photonScatteredProb( const RichRecSegment * segment,
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
  const double lambda   = m_eVToMicron/energy;
  const double scatLeng = lambda*lambda*lambda*lambda/m_AeroClarity;

  return ( 1 - (scatLeng/path)*(1.0-exp(-path/scatLeng)) );
}
