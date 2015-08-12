
//-----------------------------------------------------------------------------
/** @file ProtoParticleMUONFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleMUONFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "ProtoParticleMUONFilter.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleMUONFilter
//
// 2006-05-03 : Chris Jones  Christoper.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleMUONFilter )

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
  if ( "ISMUON" == tag || "ISMUONLOOSE" == tag )
  {
    if ( "=" != delim )
    {
      Exception( "Cut "+tag+" only supports '=' delimitor" );
    }
    // Create a new Cut object, of type IsMuonCut
    ProtoParticleSelection::IsMuonCut * cut 
      = new ProtoParticleSelection::IsMuonCut( "ISMUONLOOSE" == tag );
    // cut value and delimitor
    if      ( "TRUE"  == to_upper(value) )
    {
      cut->setDelim    ( ProtoParticleSelection::Cut::GT );
      cut->setCutValue ( 0                               );
    }
    else if ( "FALSE" == to_upper(value) )
    {
      cut->setDelim    ( ProtoParticleSelection::Cut::LT );
      cut->setCutValue ( 1                               );
    }
    else
    {
      delete cut;
      Exception( "Unknown value "+value+" for "+tag+" cut" );
    }
    // cut description
    cut->setDescription ( "Muon : "+tag+" = "+value );
    // set this cut object as the one to return
    basecut = cut;
  }

  // If the above worked, return
  if ( basecut ) return basecut;
  // otherwise, try again

  // True to get a double from the value
  double cut_value = 0;
  if ( ! stringToDouble ( value, cut_value ) ) return NULL;

  // try other cuts ...
  if ( "MUONNSHAREDHITS" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( cut_value                                     );
    // cut description
    cut->setDescription ( "Muon NSharedHits : "+tag+" "+delim+" "+value );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonNShared              );
    // set this cut object as the one to return
    basecut = cut;
  }
  else if ( "MUONMULL" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    (cut_value                                      );
    // cut description
    cut->setDescription ( "Muon MuLL : "+tag+" "+delim+" "+value        );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonMuLL                 );
    // set this cut object as the one to return
    basecut = cut;
  }
  else if ( "MUONBKGLL" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( cut_value                                     );
    // cut description
    cut->setDescription ( "Muon BkgLL : "+tag+" "+delim+" "+value       );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonBkgLL                );
    // set this cut object as the one to return
    basecut = cut;
  }
  else if ( "MUONDLL(MU-BKG)" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::DLLCut * cut = new ProtoParticleSelection::DLLCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( cut_value                                     );
    // cut description
    cut->setDescription ( "Muon MuLL-BkgLL : "+tag+" "+delim+" "+value  );
    // the variables to cut upon
    cut->setDLLs( LHCb::ProtoParticle::MuonMuLL, LHCb::ProtoParticle::MuonBkgLL );
    // set this cut object as the one to return
    basecut = cut;
  }
  else if ( "MUONPROBABILITY" == tag )
  {
    // Create a new Cut object, of type SingleVariableCut
    ProtoParticleSelection::SingleVariableCut * cut = new ProtoParticleSelection::SingleVariableCut();
    // Cut delimiter type
    cut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
    // cut value
    cut->setCutValue    ( std::log(cut_value)                           );
    // cut description
    cut->setDescription ( "Muon Probability : "+tag+" "+delim+" "+value );
    // the variable to cut upon
    cut->setVariable    ( LHCb::ProtoParticle::MuonMuLL                 );
    // set this cut object as the one to return
    basecut = cut;
  }

  return basecut;
}

//=============================================================================
