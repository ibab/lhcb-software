
//-----------------------------------------------------------------------------
/** @file RichSimplePhotonPredictor.cpp
 *
 *  Implementation file for tool : Rich::Rec::SimplePhotonPredictor
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichSimplePhotonPredictor.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( SimplePhotonPredictor )

// Standard constructor
SimplePhotonPredictor::
SimplePhotonPredictor( const std::string& type,
                       const std::string& name,
                       const IInterface* parent )
  : ToolBase     ( type, name, parent ),
    m_geomTool   ( 0 ),
    m_Nselected  ( Rich::NRadiatorTypes, 0 ),
    m_Nreject    ( Rich::NRadiatorTypes, 0 )
{

  declareInterface<IPhotonPredictor>(this);

  m_minROI.push_back( 130 ); // aerogel
  m_minROI.push_back( 0 );   // rich1Gas
  m_minROI.push_back( 0 );   // rich2Gas
  declareProperty( "MinTrackROI", m_minROI );

  m_maxROI.push_back( 350 );   // aerogel
  m_maxROI.push_back(  90 );   // rich1Gas
  m_maxROI.push_back( 200 );   // rich2Gas
  declareProperty( "MaxTrackROI", m_maxROI );

}

StatusCode SimplePhotonPredictor::initialize()
{

  // Sets up various tools and services
  const StatusCode sc = ToolBase::initialize();
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
    info() << trad << " : Sep. range     " 
           << boost::format("%5.1f") % m_minROI[rad] << " -> "
           << boost::format("%5.1f") % m_maxROI[rad] << " mm" << endmsg;
  }

  return sc;
}

StatusCode SimplePhotonPredictor::finalize()
{

  if ( m_Nselected[Rich::Aerogel]  > 0 ||
       m_Nselected[Rich::Rich1Gas] > 0 ||
       m_Nselected[Rich::Rich2Gas] > 0 )
  {
    // statistical tool
    const PoissonEffFunctor occ("%10.2f +-%7.2f");

    // printout stats
    info() << "======================================================================" << endmsg
           << "           Pixel/Segment combination selection summary : " << endmsg
           << "  Aerogel   : Selected "
           << occ(m_Nselected[Rich::Aerogel],m_Nselected[Rich::Aerogel]+m_Nreject[Rich::Aerogel])
           << " % of possible candidates" << endmsg
           << "  Rich1Gas  : Selected "
           << occ(m_Nselected[Rich::Rich1Gas],m_Nselected[Rich::Rich1Gas]+m_Nreject[Rich::Rich1Gas])
           << " % of possible candidates" << endmsg
           << "  Rich2Gas  : Selected "
           << occ(m_Nselected[Rich::Rich2Gas],m_Nselected[Rich::Rich2Gas]+m_Nreject[Rich::Rich2Gas])
           << " % of possible candidates" << endmsg
           << "======================================================================" << endmsg;
  }

  // Execute base class method
  return ToolBase::finalize();
}

// fast decision on whether a photon is possible
bool
SimplePhotonPredictor::photonPossible( LHCb::RichRecSegment * segment,
                                       LHCb::RichRecPixel * pixel ) const
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

