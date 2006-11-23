
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithForcedIneffic.cpp
 *
 *  Implementation file for tool : RichPixelCreatorWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorWithForcedIneffic.cpp,v 1.1 2006-11-23 18:08:29 jonrob Exp $
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
  declareProperty( "RejectFraction", m_rejFrac = 0.1 );
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
  if ( m_rejFrac>0 )
    info() << "Will reject randomly " << 100*m_rejFrac << "% of pixels" << endreq;

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
  return ( m_rejFrac>0 && m_rndm()<m_rejFrac ? 
           NULL : RichPixelCreatorFromRawBuffer::buildPixel(id) );
}
