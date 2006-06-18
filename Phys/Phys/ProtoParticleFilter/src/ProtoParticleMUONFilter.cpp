
//-----------------------------------------------------------------------------
/** @file ProtoParticleMUONFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleMUONFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleMUONFilter.cpp,v 1.1.1.1 2006-06-18 14:23:45 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleMUONFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleMUONFilter
//
// 2006-05-03 : Chris Jones  Christoper.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleMUONFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleMUONFilter::ProtoParticleMUONFilter( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent )
  : ChargedProtoParticleDLLFilter ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleMUONFilter::~ProtoParticleMUONFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ProtoParticleMUONFilter::initialize()
{
  const StatusCode sc = ChargedProtoParticleDLLFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // intialisation tasks

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ProtoParticleMUONFilter::finalize()
{
  return ChargedProtoParticleDLLFilter::finalize();
}

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleMUONFilter::createCut( const std::string & tag,
                                    const std::string & delim,
                                    const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ChargedProtoParticleDLLFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;
  // Otherwise, cut data is MUON specific, so treat here

  // Pointer to a cut object
  ProtoParticleSelection::SingleVariableCut * cut(NULL);
  if ( "MUONNSHAREDHITS" == tag )
  {

    // Create a new Cut object, of type SingleVariableCut
    cut = new ProtoParticleSelection::SingleVariableCut();

    // set cut properties
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( boost::lexical_cast<double>(value)            );
    // cut description
    cut->setDescription ( "MuonNSharedHits : "+tag+" "+delim+" "+value  );
    // the variable to cut upon
    cut->setVariable    ( ProtoParticle::MuonNShared                    );

  }

  return cut;
}

//=============================================================================
// Create a DetectorRequirements object
//=============================================================================
const ProtoParticleSelection::DetectorRequirements *
ProtoParticleMUONFilter::createDetReq( const std::string & tag,
                                       const std::string & value ) const
{
  // attempt to create a requirements object using base class method first
  const ProtoParticleSelection::DetectorRequirements * detreq =
    ChargedProtoParticleDLLFilter::createDetReq(tag,value);

  // If a non-null pointer is return, base class was able to decode the data, so return
  if ( detreq ) return detreq;
  // Otherwise, requirements data is MUON specific, so treat here

  // Currently there are no MUON specific detector requirements, so just return NULL

  return NULL;
}

//=============================================================================
