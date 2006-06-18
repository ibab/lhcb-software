
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleDLLFilter.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleDLLFilter
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleDLLFilter.cpp,v 1.2 2006-06-18 15:54:36 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ChargedProtoParticleDLLFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ChargedProtoParticleDLLFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleDLLFilter::
ChargedProtoParticleDLLFilter( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : ProtoParticleDLLFilter ( type, name , parent ),
    m_trSel                ( NULL )
{ }

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoParticleDLLFilter::~ChargedProtoParticleDLLFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ChargedProtoParticleDLLFilter::initialize()
{
  const StatusCode sc = ProtoParticleDLLFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ChargedProtoParticleDLLFilter::finalize()
{
  return ProtoParticleDLLFilter::finalize();
}

bool
ChargedProtoParticleDLLFilter::isSatisfied( const ProtoParticle* const & proto ) const
{
  // First apply track selection
  const Track * track = proto->track();
  if ( !track )
  {
    Warning( "ProtoParticle has null Track reference -> Cannot used Charged track selector" );
    return false;
  }
  if ( !m_trSel->accept(*track) )
  {
    verbose() << " -> ProtoParticle fails Track selection" << endreq;
    return false;
  }
  verbose() << " -> ProtoParticle passes Track selection" << endreq;
  // if that is OK, apply cuts
  return ProtoParticleDLLFilter::isSatisfied( proto );
}

//=============================================================================
