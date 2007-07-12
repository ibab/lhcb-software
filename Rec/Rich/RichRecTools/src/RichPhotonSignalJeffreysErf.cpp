
//-----------------------------------------------------------------------------
/** @file RichPhotonSignalJeffreysErf.cpp
 *
 *  Implementation file for tool : Rich::Rec::PhotonSignalJeffreysErf
 *
 *  CVS Log :-
 *  $Id: RichPhotonSignalJeffreysErf.cpp,v 1.2 2007-07-12 14:51:58 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichPhotonSignalJeffreysErf.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( PhotonSignalJeffreysErf );

// Standard constructor
PhotonSignalJeffreysErf::PhotonSignalJeffreysErf( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : PhotonSignalGaussProb( type, name, parent ) 
{
  declareProperty( "MaxErrorScale", m_errScale = 5 );
}

double PhotonSignalJeffreysErf::signalProbFunc( const double thetaDiff,
                                                const double thetaExpRes ) const
{
  // See http://scripts.iucr.org/cgi-bin/paper?he0278 for details
  const double a = gsl_sf_erf( thetaDiff/(             thetaExpRes*sqrt(2.0)) );
  const double b = gsl_sf_erf( thetaDiff/(m_errScale * thetaExpRes*sqrt(2.0)) );
  return 0.5*(a-b)/(thetaDiff*log(m_errScale));
}




