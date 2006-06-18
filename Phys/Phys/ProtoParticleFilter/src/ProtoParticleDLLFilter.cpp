
//-----------------------------------------------------------------------------
/** @file ProtoParticleDLLFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleDLLFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleDLLFilter.cpp,v 1.1.1.1 2006-06-18 14:23:44 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleDLLFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleDLLFilter );

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
// Initialise
//=============================================================================
StatusCode ProtoParticleDLLFilter::initialize()
{
  const StatusCode sc = ProtoParticleFilterBase::initialize();
  if ( sc.isFailure() ) return sc;

  // intialisation tasks

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ProtoParticleDLLFilter::finalize()
{
  return ProtoParticleFilterBase::finalize();
}

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
            << tag << " " << delim << " " << value << endreq;
  }

  // Create a new Cut object, of type DLLCut
  ProtoParticleSelection::DLLCut * dllcut = new ProtoParticleSelection::DLLCut();

  // set cut properties
  // Cut delimiter type
  dllcut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
  // cut value
  dllcut->setCutValue    ( boost::lexical_cast<double>(value)            );
  // cut description
  dllcut->setDescription ( "Combined DLL : "+tag+" "+delim+" "+value     );

  // no obvious way to avoid a big bunch of ifs for the dlls ???
  if      ( "E-PI" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "E-MU" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "E-K" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "E-P" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLe, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "MU-PI" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "MU-E" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "MU-K" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "MU-P" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLmu, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "K-PI" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "K-MU" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "K-E" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "K-P" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLk, LHCb::ProtoParticle::CombDLLp );
  }
  else if ( "P-PI" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLpi );
  }
  else if ( "P-MU" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "P-K" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLp, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "PI-E" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLe );
  }
  else if ( "PI-MU" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLmu );
  }
  else if ( "PI-K" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLk );
  }
  else if ( "PI-P" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::CombDLLpi, LHCb::ProtoParticle::CombDLLp );
  }
  else
  {
    debug() << "Unknown DLL tag " << tag << endreq;
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
// Create a DetectorRequirements object
//=============================================================================
const ProtoParticleSelection::DetectorRequirements *
ProtoParticleDLLFilter::createDetReq( const std::string & tag,
                                      const std::string & value ) const
{

  bool requiresDetOption ( false );
  int nOnlyDets ( 0 );

  ProtoParticleSelection::DetectorRequirements * detreq(NULL);

  if ( "DET" == tag )
  {
    Warning( "'Det=XXX' is depreciated. Use 'RequiresDet=XXX' or 'OnlyUseDet=XXX' instead" );
  }
  else if ( "REQUIRESDET" == tag )
  {
    if ( nOnlyDets > 0 )
    {
      Warning( "Option 'OnlyUseDet=XXX' cannot be used with 'RequiresDet=XXX' in the same selection" );
    }
    else
    {
      requiresDetOption = true;
      detreq =
        new ProtoParticleSelection::
        DetectorRequirements( ProtoParticleSelection::DetectorRequirements::detector(value),
                              ProtoParticleSelection::DetectorRequirements::MustHave,
                              tag+"="+value
                              );
    }
  }
  else if ( "ONLYUSEDET"  == tag )
  {
    if ( requiresDetOption )
    {
      Warning( "Option 'OnlyUseDet=XXX' cannot be used with 'RequiresDet=XXX' in the same selection" );
    }
    else if ( nOnlyDets > 0 )
    {
      Warning( "Can only specify one 'OnlyUseDet=XXX' option per selection" );
    }
    else
    {
      detreq = new ProtoParticleSelection::DetectorRequirements(
                                                                ProtoParticleSelection::DetectorRequirements::detector(value),
                                                                ProtoParticleSelection::DetectorRequirements::OnlyHave,
                                                                tag+"="+value
                                                                );
      ++nOnlyDets;
    }
  }

  return detreq;
}

//=============================================================================
