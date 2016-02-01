
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDAlg.cpp
 *
 *  Implementation file for ANN Combined PID algorithm ChargedProtoANNPIDAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/09/2010
 */
//-------------------------------------------------------------------------------

// local
#include "ChargedProtoANNPIDAlg.h"

//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDAlg::ChargedProtoANNPIDAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : ChargedProtoANNPIDAlgBase ( name , pSvcLocator ),
    m_tkType                  ( LHCb::Track::TypeUnknown )
{
  // JOs
  declareProperty( "TrackType", m_trackType );
  declareProperty( "PIDType",   m_pidType   );
  declareProperty( "NetworkVersion",      m_netVersion = "MC12TuneV3"  );
  declareProperty( "SuppressANNPrintout", m_suppressANNPrintout = true );
  // turn off histos and ntuples
  setProperty( "HistoProduce",   false );
  setProperty( "NTupleProduce",  false );
  setProperty( "EvtColsProduce", false );
  //setProperty( "OutputLevel", 1 );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDAlg::~ChargedProtoANNPIDAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::initialize()
{
  StatusCode sc = ChargedProtoANNPIDAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

  // Create a new network configation
  m_netConfig.reset( new NetConfig( m_trackType, m_pidType, m_netVersion, 
                                    m_suppressANNPrintout, this ) );
  if ( !m_netConfig->isOK() )
  { return Error( "Failed to configure the network" ); }

  // Determine the track type to fill
  if      ( "Long"       == m_trackType ) { m_tkType = LHCb::Track::Long; }
  else if ( "Downstream" == m_trackType ) { m_tkType = LHCb::Track::Downstream; }
  else if ( "Upstream"   == m_trackType ) { m_tkType = LHCb::Track::Upstream; }
  else { return Error( "Unsupported track type '" + m_trackType + "'" ); }

  // Proto variable to fill
  if      ( "Electron" == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNe; }
  else if ( "Muon"     == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNmu; }
  else if ( "Pion"     == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNpi; }
  else if ( "Kaon"     == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNk; }
  else if ( "Proton"   == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNp; }
  else if ( "Ghost"    == m_pidType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNghost; }
  else    { return Error( "Unknown particle type " + m_pidType ); }

  // return
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::execute()
{

  // Load the charged ProtoParticles
  auto * protos = getIfExists<LHCb::ProtoParticles>( m_protoPath );
  if ( !protos ) return StatusCode::SUCCESS;

  // Loop over ProtoParticles
  for ( auto * proto : *protos )
  {
    // Select Tracks
    if ( !proto->track() )
    { return Error( "Charged ProtoParticle has NULL Track pointer" ); }
    if ( !proto->track()->checkType(m_tkType) ) continue;

    // Clear current ANN PID information
    if ( proto->hasInfo(m_protoInfo) )
    {
      //       std::ostringstream mess;
      //       mess << "ProtoParticle already has '" << m_protoInfo
      //            << "' information -> Replacing.";
      //       Warning( mess.str(), StatusCode::SUCCESS, 1 ).ignore();
      proto->eraseInfo(m_protoInfo);
    }

    // ANN Track Selection.
    if ( !m_netConfig->passCuts(proto) ) continue;

    // get the ANN output for this proto
    const auto nnOut = m_netConfig->netHelper()->getOutput( proto );

    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "ProtoParticle " << *proto << endmsg;
      verbose() << " -> Inputs :";
      for ( const auto& in : m_netConfig->netHelper()->inputs() )
      {
        verbose() << " " << in->name() << "=" << in->value(proto);
      }
      verbose() << endmsg;
      verbose() << " -> ANN value = " << nnOut << endmsg;
    }

    // add to protoparticle
    proto->addInfo( m_protoInfo, nnOut );

  } // loop over protos

  return StatusCode::SUCCESS;
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDAlg )

//=============================================================================
