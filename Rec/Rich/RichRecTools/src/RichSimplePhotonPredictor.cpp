
//-----------------------------------------------------------------------------
/** @file RichSimplePhotonPredictor.cpp
 *
 *  Implementation file for tool : RichSimplePhotonPredictor
 *
 *  CVS Log :-
 *  $Id: RichSimplePhotonPredictor.cpp,v 1.3 2005-10-13 16:01:56 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichSimplePhotonPredictor.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSimplePhotonPredictor>          s_factory ;
const        IToolFactory& RichSimplePhotonPredictorFactory = s_factory ;

// Standard constructor
RichSimplePhotonPredictor::
RichSimplePhotonPredictor( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : RichRecToolBase ( type, name, parent ),
    m_geomTool      ( 0 ),
    m_Nselected     ( Rich::NRadiatorTypes, 0 ),
    m_Nreject       ( Rich::NRadiatorTypes, 0 )
{

  declareInterface<IRichPhotonPredictor>(this);

  m_minROI.push_back( 130 ); // aerogel
  m_minROI.push_back( 0 );   // c4f10
  m_minROI.push_back( 0 );   // cf4
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 350 );   // aerogel
  m_maxROI.push_back(  90 );   // c4f10
  m_maxROI.push_back( 200 );   // cf4
  declareProperty( "MaxTrackROI", m_maxROI );

}

StatusCode RichSimplePhotonPredictor::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichRecGeometry", m_geomTool );

  // Initialise some variables
  for ( int rad = 0; rad < Rich::NRadiatorTypes; ++rad )
  {
    m_minROI2.push_back( m_minROI[rad]*m_minROI[rad] );
    m_maxROI2.push_back( m_maxROI[rad]*m_maxROI[rad] );

    // printout for this rad
    std::string trad = Rich::text((Rich::RadiatorType)rad);
    trad.resize(8,' ');
    info() << trad << " : Sep. range     " << format("%5.1f",m_minROI[rad]) << " -> "
           << format("%5.1f",m_maxROI[rad]) << " mm" << endreq;

  }

  return sc;
}

StatusCode RichSimplePhotonPredictor::finalize()
{

  if ( m_Nselected[Rich::Aerogel] > 0 ||
       m_Nselected[Rich::C4F10]   > 0 ||
       m_Nselected[Rich::CF4]     > 0 )
  {

    // statistical tool
    RichPoissonEffFunctor occ("%10.2f +-%7.2f");

    // printout stats
    info() << "=======================================================" << endreq
           << "  Pixel/Segment combination selection summary : " << endreq
           << "    Aerogel : Selected "
           << occ(m_Nselected[Rich::Aerogel],m_Nselected[Rich::Aerogel]+m_Nreject[Rich::Aerogel])
           << " % of possible candidates" << endreq
           << "    C4F10   : Selected "
           << occ(m_Nselected[Rich::C4F10],m_Nselected[Rich::C4F10]+m_Nreject[Rich::C4F10])
           << " % of possible candidates" << endreq
           << "    CF4     : Selected "
           << occ(m_Nselected[Rich::CF4],m_Nselected[Rich::CF4]+m_Nreject[Rich::CF4])
           << " % of possible candidates" << endreq
           << "=======================================================" << endreq;
  }

  // Execute base class method
  return RichRecToolBase::finalize();
}

// fast decision on whether a photon is possible
bool RichSimplePhotonPredictor::photonPossible( RichRecSegment * segment,
                                                RichRecPixel * pixel ) const
{

  // Default to not selected
  bool OK = false;

  // which radiator
  const Rich::RadiatorType rad = segment->trackSegment().radiator();

  // Are they in the same Rich detector ?
  if ( segment->trackSegment().rich() == pixel->detector() )
  {

    // Hit seperation criteria : based on global coordinates
    const double sep2 = m_geomTool->trackPixelHitSep2(segment, pixel);
    if ( sep2 < m_maxROI2[rad] && sep2 > m_minROI2[rad] )
    {
      OK = true;
    }

  }

  if ( OK )
  {
    ++m_Nselected[rad];
  }
  else
  {
    ++m_Nreject[rad];
  }

  return OK;

}

