
//-----------------------------------------------------------------------------
/** @file ProtoParticleMUONFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleMUONFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleMUONFilter.cpp,v 1.2 2006-11-20 15:59:49 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleMUONFilter.h"

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
  : ProtoParticleCALOFilter ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleMUONFilter::~ProtoParticleMUONFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ProtoParticleMUONFilter::initialize()
{
  const StatusCode sc = ProtoParticleCALOFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // intialisation tasks

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ProtoParticleMUONFilter::finalize()
{
  return ProtoParticleCALOFilter::finalize();
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
    ProtoParticleCALOFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;
  // Otherwise, cut data is MUON specific, so treat here

  // Pointer to a cut object
  if ( "MUONNSHAREDHITS" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( boost::lexical_cast<double>(value)            );
    // cut description
    cut->setDescription ( "Muon NSharedHits : "+tag+" "+delim+" "+value );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonNShared              );
    basecut = cut;
  }
  else if ( "MUONMULL" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( boost::lexical_cast<double>(value)            );
    // cut description
    cut->setDescription ( "Muon MuLL : "+tag+" "+delim+" "+value        );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonMuLL                 );
    basecut = cut;
  }
  else if ( "MUONBKGLL" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( boost::lexical_cast<double>(value)            );
    // cut description
    cut->setDescription ( "Muon BkgLL : "+tag+" "+delim+" "+value       );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonBkgLL                );
    basecut = cut;
  }
  else if ( "MUONDLL(MU-BKG)" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::DLLCut * cut = new ProtoParticleSelection::DLLCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( boost::lexical_cast<double>(value)            );
    // cut description
    cut->setDescription ( "Muon MuLL-BkgLL : "+tag+" "+delim+" "+value  );
    // the variables to cut upon
    cut->setDLLs( LHCb::ProtoParticle::MuonMuLL, LHCb::ProtoParticle::MuonBkgLL );
    basecut = cut;
  }

  return basecut;
}

//=============================================================================
