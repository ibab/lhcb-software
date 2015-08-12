
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleDLLFilter.cpp
 *
 * Implementation file for algorithm ChargedProtoParticleDLLFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "ChargedProtoParticleDLLFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ChargedProtoParticleDLLFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoParticleDLLFilter::
ChargedProtoParticleDLLFilter( const std::string& type,
                               const std::string& name,
                               const IInterface* parent )
  : ProtoParticleANNPIDFilter ( type, name , parent ),
    m_trSel                   ( NULL )
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
  const StatusCode sc = ProtoParticleANNPIDFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // get an instance of the track selector
  m_trSel = tool<ITrackSelector>( "TrackSelector", "TrackSelector", this );

  return sc;
}

bool
ChargedProtoParticleDLLFilter::isSatisfied( const ProtoParticle* const & proto ) const
{
  // First apply track selection
  const Track * track = proto->track();
  if ( !track )
  {
    Warning( "ProtoParticle has null Track reference -> Cannot used charged track selector" );
    return false;
  }
  if ( !m_trSel->accept(*track) )
  {
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> ProtoParticle fails Track selection" << endreq;
    return false;
  }
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << " -> ProtoParticle passes Track selection" << endreq;
  // if that is OK, apply cuts
  return ProtoParticleANNPIDFilter::isSatisfied( proto );
}

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ChargedProtoParticleDLLFilter::createCut( const std::string & tag,
                                          const std::string & delim,
                                          const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ProtoParticleANNPIDFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;

  // Otherwise try and decode here

  // Try to get a double from the value
  double cut_value = 0;
  if ( ! stringToDouble ( value, cut_value ) ) return NULL;

  // Create a new Cut object
  ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();

  // set cut properties
  // Cut delimiter type
  cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim)  );
  // cut value
  cut->setCutValue    ( cut_value                                      );
  // cut description
  cut->setDescription ( "Charged Track Cut : "+tag+" "+delim+" "+value );

  // Try and decode the tag
  if      ( "VELOCHARGE" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::VeloCharge );
  }
  else
  {
    debug() << "Unknown tag " << tag << endreq;
    delete cut;
    cut = NULL;
  }

  return cut;
}

//=============================================================================
