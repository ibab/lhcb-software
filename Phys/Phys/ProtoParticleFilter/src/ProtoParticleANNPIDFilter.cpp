
//-----------------------------------------------------------------------------
/** @file ProtoParticleANNPIDFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleANNPIDFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2011-01-12
 */
//-----------------------------------------------------------------------------

// local
#include "ProtoParticleANNPIDFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleANNPIDFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleANNPIDFilter::
ProtoParticleANNPIDFilter( const std::string& type,
                           const std::string& name,
                           const IInterface* parent )
  : ProtoParticleDLLFilter ( type, name , parent )
{ }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleANNPIDFilter::~ProtoParticleANNPIDFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ProtoParticleANNPIDFilter::initialize()
{
  const StatusCode sc = ProtoParticleDLLFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // setup any options ....

  return sc;
}

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleANNPIDFilter::createCut( const std::string & tag,
                                      const std::string & delim,
                                      const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ProtoParticleDLLFilter::createCut(tag,delim,value);

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
  cut->setDescription ( "ANN PID Cut : "+tag+" "+delim+" "+value       );

  // Try and decode the tag
  if      ( "PROBNNE" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNe );
  }
  else if ( "PROBNNMU" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNmu );
  }
  else if ( "PROBNNPI" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNpi );
  }
  else if ( "PROBNNK" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNk );
  }
  else if ( "PROBNNP" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNp );
  }
  else if ( "PROBNNGHOST" == tag )
  {
    cut->setVariable( LHCb::ProtoParticle::ProbNNghost );
  }
  else
  {
    debug() << "Unknown tag " << tag << endmsg;
    delete cut;
    cut = NULL;
  }

  return cut;
}

//=============================================================================
