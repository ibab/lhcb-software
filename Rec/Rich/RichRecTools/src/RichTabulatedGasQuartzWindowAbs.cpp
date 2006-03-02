
//-----------------------------------------------------------------------------
/** @file RichTabulatedGasQuartzWindowAbs.cpp
 *
 *  Implementation file for tool : RichTabulatedGasQuartzWindowAbs
 *
 *  CVS Log :-
 *  $Id: RichTabulatedGasQuartzWindowAbs.cpp,v 1.8 2006-03-02 15:29:20 jonrob Exp $
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
  : RichRecToolBase ( type, name, parent ),
    m_riches        ( Rich::NRiches )
{
  // interface
  declareInterface<IRichGasQuartzWindowAbs>(this);
}

StatusCode RichTabulatedGasQuartzWindowAbs::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Rich1 and Rich2
  m_riches[Rich::Rich1] = getDet<DeRich1>( DeRichLocation::Rich1 );
  m_riches[Rich::Rich2] = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Quartz window thicknesses.
  m_qWinZSize[Rich::Rich1] = m_riches[Rich::Rich1]->param<double>("Rich1GasQuartzWindowThickness");
  m_qWinZSize[Rich::Rich2] = m_riches[Rich::Rich2]->param<double>("Rich2GasQuartzWindowThickness");

  return sc;
}

StatusCode RichTabulatedGasQuartzWindowAbs::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double
RichTabulatedGasQuartzWindowAbs::photonTransProb( const RichRecSegment * segment,
                                                  const double energy ) const
{
  // check energy is valid
  if ( energy <= 0 ) return 0;

  // RICH
  const Rich::DetectorType rich = segment->trackSegment().rich();

  // compute and return transmission probability
  return exp( -m_qWinZSize[rich] /
              (*(m_riches[rich]->gasWinAbsLength()))[energy*eV]
              );
}
