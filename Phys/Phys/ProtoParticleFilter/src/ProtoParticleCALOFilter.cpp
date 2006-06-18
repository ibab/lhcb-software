
//-----------------------------------------------------------------------------
/** @file ProtoParticleCALOFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleCALOFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleCALOFilter.cpp,v 1.1.1.1 2006-06-18 14:23:45 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleCALOFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleCALOFilter
//
// 2006-05-03 : Chris Jones  Christoper.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleCALOFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleCALOFilter::ProtoParticleCALOFilter( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : ChargedProtoParticleDLLFilter ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleCALOFilter::~ProtoParticleCALOFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ProtoParticleCALOFilter::initialize()
{
  const StatusCode sc = ChargedProtoParticleDLLFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // intialisation tasks

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ProtoParticleCALOFilter::finalize()
{
  return ChargedProtoParticleDLLFilter::finalize();
}

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleCALOFilter::createCut( const std::string & tag,
                                    const std::string & delim,
                                    const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ChargedProtoParticleDLLFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;

  // Otherwise, cut data is CALO specific, so treat here

  // Currently, no CALO specific stuff, so just return a NULL cut

  return NULL;
}

//=============================================================================
// Create a DetectorRequirements object
//=============================================================================
const ProtoParticleSelection::DetectorRequirements *
ProtoParticleCALOFilter::createDetReq( const std::string & tag,
                                       const std::string & value ) const
{
  // attempt to create a requirements object using base class method first
  const ProtoParticleSelection::DetectorRequirements * detreq =
    ChargedProtoParticleDLLFilter::createDetReq(tag,value);

  // If a non-null pointer is return, base class was able to decode the data, so return
  if ( detreq ) return detreq;
  // Otherwise, requirements data is CALO specific, so treat here

  // Currently there are no CALO specific detector requirements, so just return NULL

  return NULL;
}

//=============================================================================
