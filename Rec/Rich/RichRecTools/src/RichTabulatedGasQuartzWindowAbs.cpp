
/** @file RichTabulatedGasQuartzWindowAbs.cpp
 *
 *  Implementation file for tool : RichTabulatedGasQuartzWindowAbs
 *
 *  CVS Log :-
 *  $Id: RichTabulatedGasQuartzWindowAbs.cpp,v 1.3 2004-07-27 20:15:32 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */

// local
#include "RichTabulatedGasQuartzWindowAbs.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichTabulatedGasQuartzWindowAbs>          s_factory ;
const        IToolFactory& RichTabulatedGasQuartzWindowAbsFactory = s_factory ;

// Standard constructor
RichTabulatedGasQuartzWindowAbs::RichTabulatedGasQuartzWindowAbs ( const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent )
  : RichRecToolBase( type, name, parent ),
    m_gasQWin( 0 ) 
{

  declareInterface<IRichGasQuartzWindowAbs>(this);

  declareProperty( "GasQuartzWindowLocation",
                   m_gasQWinLoc = 
                   "/dd/Materials/RichMaterialTabProperties/GasWindowQuartzAbsLengthPT" );

}

StatusCode RichTabulatedGasQuartzWindowAbs::initialize() {

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Absorption length from XML
  SmartDataPtr<TabulatedProperty> tab( detSvc(), m_gasQWinLoc );
  if ( !tab ) {
    return Error( "Failed to load gas quartz window absorption length from "+m_gasQWinLoc );
  }
  m_gasQWin = new Rich1DTabProperty( tab );

  // Get Rich Detector elements
  DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocation::Rich1 );
  DeRich2 * Rich2DE = getDet<DeRich2>( DeRichLocation::Rich2 );

  // Quartz window thicknesses. Until these parameters exist in the XML add backup default values
  try { m_qWinZSize[Rich::Rich1] = Rich1DE->userParameterAsDouble("Rh1QuartzWindowZSize"); }
  catch ( const GaudiException & excp ) {
    Warning("Parameter Rh1QuartzWindowZSize unavailable -> Reverting to default",
            StatusCode::SUCCESS);
    m_qWinZSize[Rich::Rich1] = 6 * mm;
  }
  try { m_qWinZSize[Rich::Rich2] = Rich2DE->userParameterAsDouble("Rh2QuartzWindowZSize"); }
  catch ( const GaudiException & excp ) {
    Warning("Parameter Rh2QuartzWindowZSize unavailable -> Reverting to default",
            StatusCode::SUCCESS);
    m_qWinZSize[Rich::Rich2] = 5 * mm;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichTabulatedGasQuartzWindowAbs::finalize()
{
  // clean up
  if ( m_gasQWin ) { delete m_gasQWin; m_gasQWin = 0; }

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
  return exp( -m_qWinZSize[segment->trackSegment().rich()] / (*m_gasQWin)[energy*eV] );
}
