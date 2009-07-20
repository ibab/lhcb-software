
//-----------------------------------------------------------------------------
/** @file ProtoParticleCALOFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleCALOFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleCALOFilter.cpp,v 1.3 2009-07-20 16:43:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleCALOFilter.h"

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
  : ProtoParticleRICHFilter ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleCALOFilter::~ProtoParticleCALOFilter() { }

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
    ProtoParticleRICHFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;

  // Otherwise, cut data is CALO specific, so treat here

  // True to get a double from the value
  double cut_value = 0;
  if ( ! stringToDouble ( value, cut_value ) ) return NULL;
  
  // Create a new Cut object
  ProtoParticleSelection::SingleVariableCut * dllcut = new ProtoParticleSelection::SingleVariableCut();

  // set cut properties
  // Cut delimiter type
  dllcut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
  // cut value
  dllcut->setCutValue    ( cut_value                                     );
  // cut description
  dllcut->setDescription ( "Calo Cut : "+tag+" "+delim+" "+value         );

  // Try and decode the tag
  if      ( "ECALPIDE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::EcalPIDe );
  }
  else if ( "PRSPIDE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::PrsPIDe );
  }
  else if ( "BREMPIDE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::BremPIDe );
  }
  else if ( "HCALPIDE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::HcalPIDe );
  }
  else if ( "HCALPIDMU" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::HcalPIDmu );
  }
  else if ( "ECALPIDMU" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::EcalPIDmu );
  }
  else if ( "CALOTRMATCH" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloTrMatch );
  }
  else if ( "CALOELECTRONMATCH" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloElectronMatch );
  }
  else if ( "CALOBREMMATCH" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloBremMatch );
  }
  else if ( "CALOCHARGEDSPD" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloChargedSpd );
  }
  else if ( "CALOCHARGEDPRS" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloChargedPrs );
  }
  else if ( "CALOSPDE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloSpdE );
  }
  else if ( "CALOPRSE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloPrsE );
  }
  else if ( "CALOECALE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloEcalE );
  }
  else if ( "CALOHCALE" == tag )
  {
    dllcut->setVariable( LHCb::ProtoParticle::CaloHcalE );
  }
  else
  {
    debug() << "Unknown tag " << tag << endreq;
    delete dllcut;
    dllcut = NULL;
  }

  if ( msgLevel(MSG::DEBUG) && dllcut )
  {
    debug() << "  -> Created new DLLCut : "
            << tag << " " << delim << " " << dllcut->cutValue() << endreq;
  }

  return dllcut;
}

//=============================================================================
