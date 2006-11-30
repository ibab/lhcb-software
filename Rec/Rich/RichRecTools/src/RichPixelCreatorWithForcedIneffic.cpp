
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithForcedIneffic.cpp
 *
 *  Implementation file for tool : RichPixelCreatorWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorWithForcedIneffic.cpp,v 1.2 2006-11-30 15:38:31 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2006
 */
//---------------------------------------------------------------------------------

// local
#include "RichPixelCreatorWithForcedIneffic.h"

//---------------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichPixelCreatorWithForcedIneffic );

// Standard constructor
RichPixelCreatorWithForcedIneffic::
RichPixelCreatorWithForcedIneffic( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent )
  : RichPixelCreatorFromRawBuffer ( type, name, parent )
{
  declareProperty( "RejectFractionRICH1Top",   m_rejFrac[Rich::Rich1][Rich::top]    = 0.1 );
  declareProperty( "RejectFractionRICH1Bot",   m_rejFrac[Rich::Rich1][Rich::bottom] = 0.1 );
  declareProperty( "RejectFractionRICH2Left",  m_rejFrac[Rich::Rich2][Rich::left]   = 0.1 );
  declareProperty( "RejectFractionRICH2Right", m_rejFrac[Rich::Rich2][Rich::right]  = 0.1 );
}

StatusCode RichPixelCreatorWithForcedIneffic::initialize()
{
  // base class initilize
  const StatusCode sc = RichPixelCreatorFromRawBuffer::initialize();
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

StatusCode RichPixelCreatorWithForcedIneffic::finalize()
{
  // finalize random number generator
  m_rndm.finalize();
  // Execute base class method
  return RichPixelCreatorFromRawBuffer::finalize();
}

LHCb::RichRecPixel *
RichPixelCreatorWithForcedIneffic::buildPixel( const LHCb::RichSmartID id ) const
{
  const double rF = m_rejFrac[id.rich()][id.panel()];
  return ( 0 < rF && m_rndm() < rF ?
           NULL : RichPixelCreatorFromRawBuffer::buildPixel(id) );
}
