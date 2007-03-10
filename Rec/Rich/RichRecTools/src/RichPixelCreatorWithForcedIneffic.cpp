
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithForcedIneffic.cpp
 *
 *  Implementation file for tool : Rich::Rec::PixelCreatorWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorWithForcedIneffic.cpp,v 1.4 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2006
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorWithForcedIneffic.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//---------------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PixelCreatorWithForcedIneffic );

// Standard constructor
PixelCreatorWithForcedIneffic::
PixelCreatorWithForcedIneffic( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : PixelCreatorFromRawBuffer ( type, name, parent )
{
  declareProperty( "RejectFractionRICH1Top",   m_rejFrac[Rich::Rich1][Rich::top]    = 0.1 );
  declareProperty( "RejectFractionRICH1Bot",   m_rejFrac[Rich::Rich1][Rich::bottom] = 0.1 );
  declareProperty( "RejectFractionRICH2Left",  m_rejFrac[Rich::Rich2][Rich::left]   = 0.1 );
  declareProperty( "RejectFractionRICH2Right", m_rejFrac[Rich::Rich2][Rich::right]  = 0.1 );
}

StatusCode PixelCreatorWithForcedIneffic::initialize()
{
  // base class initilize
  const StatusCode sc = PixelCreatorFromRawBuffer::initialize();
  if ( sc.isFailure() ) { return sc; }

  // random number generator
  IRndmGenSvc * randSvc = svc<IRndmGenSvc>( "RndmGenSvc", true );
  if ( !m_rndm.initialize( randSvc, Rndm::Flat(0.,1.) ) )
  {
    return Error( "Unable to create Random generator" );
  }

  info() << "Will reject randomly " << 100*m_rejFrac[Rich::Rich1][Rich::top]    << "% of RICH1 top-panel pixels" << endreq;
  info() << "Will reject randomly " << 100*m_rejFrac[Rich::Rich1][Rich::bottom] << "% of RICH1 bottom-panel pixels" << endreq;
  info() << "Will reject randomly " << 100*m_rejFrac[Rich::Rich2][Rich::left]   << "% of RICH2 left-panel pixels" << endreq;
  info() << "Will reject randomly " << 100*m_rejFrac[Rich::Rich2][Rich::right]  << "% of RICH2 right-panel pixels" << endreq;

  return sc;
}

StatusCode PixelCreatorWithForcedIneffic::finalize()
{
  // finalize random number generator
  m_rndm.finalize();
  // Execute base class method
  return PixelCreatorFromRawBuffer::finalize();
}

LHCb::RichRecPixel *
PixelCreatorWithForcedIneffic::buildPixel( const Rich::HPDPixelCluster& cluster ) const
{
  const double rF = m_rejFrac[cluster.rich()][cluster.panel().panel()];
  return ( 0 < rF && m_rndm() < rF ?
           NULL : PixelCreatorFromRawBuffer::buildPixel(cluster) );
}
