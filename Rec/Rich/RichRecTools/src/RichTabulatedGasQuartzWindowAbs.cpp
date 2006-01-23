
//-----------------------------------------------------------------------------
/** @file RichTabulatedGasQuartzWindowAbs.cpp
 *
 *  Implementation file for tool : RichTabulatedGasQuartzWindowAbs
 *
 *  CVS Log :-
 *  $Id: RichTabulatedGasQuartzWindowAbs.cpp,v 1.7 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichTabulatedGasQuartzWindowAbs.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedGasQuartzWindowAbs>          s_factory ;
const        IToolFactory& RichTabulatedGasQuartzWindowAbsFactory = s_factory ;

// Standard constructor
RichTabulatedGasQuartzWindowAbs::RichTabulatedGasQuartzWindowAbs ( const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent )
  : RichRecToolBase( type, name, parent )
{

  declareInterface<IRichGasQuartzWindowAbs>(this);

}

StatusCode RichTabulatedGasQuartzWindowAbs::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get Rich Detector elements
  const DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocation::Rich1 );
  const DeRich2 * Rich2DE = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Absorption length
  m_gasQWin[Rich::Rich1] = new Rich1DTabProperty( Rich1DE->gasWinAbsLength() );
  m_gasQWin[Rich::Rich2] = new Rich1DTabProperty( Rich2DE->gasWinAbsLength() );

  // Quartz window thicknesses.
  m_qWinZSize[Rich::Rich1] = Rich1DE->param<double>("Rich1GasQuartzWindowThickness");
  m_qWinZSize[Rich::Rich2] = Rich2DE->param<double>("Rich2GasQuartzWindowThickness");

  return sc;
}

StatusCode RichTabulatedGasQuartzWindowAbs::finalize()
{
  // clean up
  if ( m_gasQWin[Rich::Rich1] ) { delete m_gasQWin[Rich::Rich1]; m_gasQWin[Rich::Rich1] = 0; }
  if ( m_gasQWin[Rich::Rich2] ) { delete m_gasQWin[Rich::Rich2]; m_gasQWin[Rich::Rich2] = 0; }

  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedGasQuartzWindowAbs::photonTransProb( const RichRecSegment * segment,
                                                  const double energy ) const
{
  // check energy is valid
  if ( energy <= 0 ) return 0;

  // compute and return transmission probability
  return exp( -m_qWinZSize[segment->trackSegment().rich()] /
              (*m_gasQWin[segment->trackSegment().rich()])[energy*eV] );
}
