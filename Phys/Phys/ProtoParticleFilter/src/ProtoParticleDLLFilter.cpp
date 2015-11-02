
//-----------------------------------------------------------------------------
/** @file ProtoParticleDLLFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleDLLFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// local
#include "ProtoParticleDLLFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleDLLFilter )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleDLLFilter::ProtoParticleDLLFilter( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : ProtoParticleFilterBase ( type, name , parent ) { }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleDLLFilter::~ProtoParticleDLLFilter() { }

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleDLLFilter::createCut( const std::string & tag,
                                   const std::string & delim,
                                   const std::string & value ) const
{

  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "  -> Cut data : "
            << tag << " " << delim << " " << value << endmsg;
  }

  // Try to get a double from the value
  double cut_value = 0;
  if ( ! stringToDouble ( value, cut_value ) ) return NULL;

  // Create a new Cut object, of type DLLCut
  ProtoParticleSelection::DLLCut * dllcut = new ProtoParticleSelection::DLLCut();

  // set cut properties
  // Cut delimiter type
  dllcut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
  // cut value
  dllcut->setCutValue    ( cut_value                                     );
  // cut description
  dllcut->setDescription ( "Combined DLL : "+tag+" "+delim+" "+value     );

  // Try and decode the tag
  bool OK = tryDllTypes(tag,dllcut);
  // if failed, try again for old syntax (to be removed)
  if (!OK)
  {
    OK = tryDllTypes("COMBDLL("+tag+")",dllcut);
    if (OK)
    {
      Warning( "DLL syntax '"+tag+"' is obsolete. Please use 'CombDLL("+tag+")' instead.",
               StatusCode::SUCCESS );
    }
    else
    {
      delete dllcut;
      dllcut = NULL;
    }
  }

  if ( msgLevel(MSG::DEBUG) && dllcut )
  {
    debug() << "  -> Created new DLLCut : "
            << tag << " " << delim << " " << dllcut->cutValue() << endmsg;
  }

  return dllcut;
}

bool
ProtoParticleDLLFilter::tryDllTypes( const std::string & tag,
                                     ProtoParticleSelection::DLLCut * dllcut ) const
{
  bool OK = true;
  // no obvious way to avoid a big bunch of ifs for the dlls ???
  if      ( "COMBDLL(E-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "COMBDLL(E-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "COMBDLL(E-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "COMBDLL(E-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "COMBDLL(MU-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "COMBDLL(MU-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "COMBDLL(MU-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "COMBDLL(MU-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "COMBDLL(K-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "COMBDLL(K-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "COMBDLL(K-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "COMBDLL(K-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "COMBDLL(P-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "COMBDLL(P-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "COMBDLL(P-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "COMBDLL(PI-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "COMBDLL(PI-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "COMBDLL(PI-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "COMBDLL(PI-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLp );
  }
  else
  {
    debug() << "Unknown DLL tag " << tag << endmsg;
    OK = false;
  }
  return OK;
}

//=============================================================================
// Create a DetectorRequirements object
//=============================================================================
const ProtoParticleSelection::DetectorRequirements *
ProtoParticleDLLFilter::createDetReq( const std::string & tag,
                                      const std::string & value ) const
{
  ProtoParticleSelection::DetectorRequirements * detreq(NULL);

  if ( "DET" == tag )
  {
    Warning( "'Det=XXX' is depreciated. Use 'RequiresDet=XXX' or 'OnlyUseDet=XXX' instead" );
  }
  else if ( "REQUIRESDET" == tag )
  {
    detreq =
      new ProtoParticleSelection::
      DetectorRequirements( ProtoParticleSelection::DetectorRequirements::detector(value),
                            ProtoParticleSelection::DetectorRequirements::MustHave,
                            tag+"="+value
                            );
  }
  else if ( "ONLYUSEDET" == tag )
  {
    detreq =
      new ProtoParticleSelection::
      DetectorRequirements( ProtoParticleSelection::DetectorRequirements::detector(value),
                            ProtoParticleSelection::DetectorRequirements::OnlyHave,
                            tag+"="+value
                            );
  }
  else if ( "MUSTNOTHAVEDET" == tag )
  {
    detreq =
      new ProtoParticleSelection::
      DetectorRequirements( ProtoParticleSelection::DetectorRequirements::detector(value),
                            ProtoParticleSelection::DetectorRequirements::MustNotHave,
                            tag+"="+value
                            );
  }

  return detreq;
}

//=============================================================================
