
//-----------------------------------------------------------------------------
/** @file ProtoParticleRICHFilter.cpp
 *
 * Implementation file for algorithm ProtoParticleRICHFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleRICHFilter.cpp,v 1.1 2006-11-20 15:59:49 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// local
#include "ProtoParticleRICHFilter.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleRICHFilter );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleRICHFilter::
ProtoParticleRICHFilter( const std::string& type,
                         const std::string& name,
                         const IInterface* parent )
  : ChargedProtoParticleDLLFilter ( type, name , parent )
{ }

//=============================================================================
// Destructor
//=============================================================================
ProtoParticleRICHFilter::~ProtoParticleRICHFilter() { }

//=============================================================================
// Initialise
//=============================================================================
StatusCode ProtoParticleRICHFilter::initialize()
{
  const StatusCode sc = ChargedProtoParticleDLLFilter::initialize();
  if ( sc.isFailure() ) return sc;

  // intialisation tasks

  return sc;
}

//=============================================================================
// Finalise
//=============================================================================
StatusCode ProtoParticleRICHFilter::finalize()
{
  return ChargedProtoParticleDLLFilter::finalize();
}

//=============================================================================
// Create a cut object from decoded cut options
//=============================================================================
const ProtoParticleSelection::Cut *
ProtoParticleRICHFilter::createCut( const std::string & tag,
                                    const std::string & delim,
                                    const std::string & value ) const
{
  // Attempt to create a cut object using base class method first
  const ProtoParticleSelection::Cut * basecut =
    ChargedProtoParticleDLLFilter::createCut(tag,delim,value);

  // If a non-null pointer is returned, base class was able to decode the data, so return
  if ( basecut ) return basecut;
  // Otherwise, cut data is RICH specific, so treat here

  // Create a new Cut object, of type DLLCut
  ProtoParticleSelection::DLLCut * dllcut = new ProtoParticleSelection::DLLCut();

  // set cut properties
  // Cut delimiter type
  dllcut->setDelim       ( ProtoParticleSelection::Cut::delimiter(delim) );
  // cut value
  dllcut->setCutValue    ( boost::lexical_cast<double>(value)            );
  // cut description
  dllcut->setDescription ( "Rich DLL : "+tag+" "+delim+" "+value     );

  // no obvious way to avoid a big bunch of ifs for the dlls ???
  if      ( "RICHDLL(E-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLe, LHCb::ProtoParticle::RichDLLpi );
  }
  else if ( "RICHDLL(E-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLe, LHCb::ProtoParticle::RichDLLmu );
  }
  else if ( "RICHDLL(E-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLe, LHCb::ProtoParticle::RichDLLk );
  }
  else if ( "RICHDLL(E-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLe, LHCb::ProtoParticle::RichDLLp );
  }
  else if ( "RICHDLL(MU-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLmu, LHCb::ProtoParticle::RichDLLpi );
  }
  else if ( "RICHDLL(MU-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLmu, LHCb::ProtoParticle::RichDLLe );
  }
  else if ( "RICHDLL(MU-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLmu, LHCb::ProtoParticle::RichDLLk );
  }
  else if ( "RICHDLL(MU-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLmu, LHCb::ProtoParticle::RichDLLp );
  }
  else if ( "RICHDLL(K-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLk, LHCb::ProtoParticle::RichDLLpi );
  }
  else if ( "RICHDLL(K-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLk, LHCb::ProtoParticle::RichDLLmu );
  }
  else if ( "RICHDLL(K-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLk, LHCb::ProtoParticle::RichDLLe );
  }
  else if ( "RICHDLL(K-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLk, LHCb::ProtoParticle::RichDLLp );
  }
  else if ( "RICHDLL(P-PI)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLp, LHCb::ProtoParticle::RichDLLpi );
  }
  else if ( "RICHDLL(P-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLp, LHCb::ProtoParticle::RichDLLmu );
  }
  else if ( "RICHDLL(P-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLp, LHCb::ProtoParticle::RichDLLk );
  }
  else if ( "RICHDLL(PI-E)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLpi, LHCb::ProtoParticle::RichDLLe );
  }
  else if ( "RICHDLL(PI-MU)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLpi, LHCb::ProtoParticle::RichDLLmu );
  }
  else if ( "RICHDLL(PI-K)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLpi, LHCb::ProtoParticle::RichDLLk );
  }
  else if ( "RICHDLL(PI-P)" == tag )
  {
    dllcut->setDLLs( LHCb::ProtoParticle::RichDLLpi, LHCb::ProtoParticle::RichDLLp );
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
