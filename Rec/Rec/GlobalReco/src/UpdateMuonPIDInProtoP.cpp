// $Id: UpdateMuonPIDInProtoP.cpp,v 1.1 2009-01-22 14:19:05 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "UpdateMuonPIDInProtoP.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : UpdateMuonPIDInProtoP
//
// 2007-06-28 : Tomasz Skwarnicki
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( UpdateMuonPIDInProtoP );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
UpdateMuonPIDInProtoP::UpdateMuonPIDInProtoP( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_protos( NULL )
{

  // input data
  declareProperty( "InputMuonPIDLocation",
                   m_muonPath = MuonPIDLocation::Default );

  // output data
  declareProperty( "ProtoParticleLocation",
                   m_protoPath = ProtoParticleLocation::Charged );

}

//=============================================================================
// Destructor
//=============================================================================
UpdateMuonPIDInProtoP::~UpdateMuonPIDInProtoP() { }

//=============================================================================
// Main execution
//=============================================================================
StatusCode UpdateMuonPIDInProtoP::execute() 
{
  debug() << "==> Execute" << endmsg;

  // Load the MuonPIDs
  const bool muonSc = getMuonData();
  if( !muonSc ){
    Warning( "Since no MuonPIDs, ProtoParticles will not be changed.", StatusCode::SUCCESS );
    return StatusCode::SUCCESS;
  }

  // ProtoParticle container
  if ( !exist<ProtoParticles>(m_protoPath) ){
    Warning( "No existing ProtoParticle container at " +  m_protoPath + " thus do nothing." );
    return StatusCode::SUCCESS;
  }

  m_protos = get<ProtoParticles>(m_protoPath);

  // Loop over proto particles
  for ( ProtoParticles::iterator iProto = m_protos->begin();
        iProto != m_protos->end(); ++iProto )
  {

    LHCb::ProtoParticle* pProto = *iProto;

    pProto->eraseInfo(ProtoParticle::NoPID);

    if( replaceMuon( pProto ) )continue;

    if( pProto->hasInfo(ProtoParticle::RichDLLpi) )continue;
    // if( pProto->hasInfo(ProtoParticle::MuonMuLL) )continue;
    if( pProto->hasInfo(ProtoParticle::InAccEcal) )continue;
    if( pProto->hasInfo(ProtoParticle::InAccBrem) )continue;
    if( pProto->hasInfo(ProtoParticle::InAccSpd) )continue;
    if( pProto->hasInfo(ProtoParticle::InAccPrs) )continue;
    if( pProto->hasInfo(ProtoParticle::InAccHcal) )continue;
    pProto->addInfo(ProtoParticle::NoPID,1);

  }

  return StatusCode::SUCCESS;
}

//=============================================================================

//=============================================================================
// Replace MUON info to the protoparticle
//=============================================================================
bool UpdateMuonPIDInProtoP::replaceMuon( LHCb::ProtoParticle * proto ) const
{
  int erased=0;
  erased += proto->eraseInfo( ProtoParticle::InAccMuon );
  erased += proto->eraseInfo( ProtoParticle::InAccMuon );
  erased += proto->eraseInfo( ProtoParticle::MuonMuLL );
  erased += proto->eraseInfo( ProtoParticle::MuonBkgLL );
  erased += proto->eraseInfo( ProtoParticle::MuonNShared );
  erased += proto->eraseInfo( ProtoParticle::MuonPIDStatus );

  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " Number of items erased from ProtoP " << erased << endmsg;
  }

  proto->setMuonPID(NULL);

  // Does this track have a MUON PID result ?
  TrackToMuonPID::const_iterator iM = m_muonMap.find( proto->track() );
  if ( m_muonMap.end() == iM ) return false;

  // MuonPID object is found
  const MuonPID * muonPID = (*iM).second;

  // MuonPID for this track is found, so save data
  if ( msgLevel(MSG::VERBOSE) )
  {
    verbose() << " -> Found MuonPID data : MuLL=" <<  muonPID->MuonLLMu()
              << " BkLL=" <<  muonPID->MuonLLBg()
              << " nSharedHits=" << muonPID->nShared()
              << " isMuon=" << muonPID->IsMuon()
              << endreq;
  }

  // store acceptance flag for those in acceptance (lack of flag signifies
  // track was outside acceptance)
  if ( muonPID->InAcceptance() )
  {
    proto->addInfo( ProtoParticle::InAccMuon, true );
  }

  // check IsMuon flag - Reject non-muons
  if ( !muonPID->IsMuon() ) return false;

  // reference to MuonPID object
  proto->setMuonPID( muonPID );

  // Store the PID info
  proto->addInfo( ProtoParticle::MuonMuLL,      muonPID->MuonLLMu() );
  proto->addInfo( ProtoParticle::MuonBkgLL,     muonPID->MuonLLBg() );
  proto->addInfo( ProtoParticle::MuonNShared,   muonPID->nShared()  );

  // Store History
  proto->addInfo( ProtoParticle::MuonPIDStatus, muonPID->Status()   );

  return true;
}

//=============================================================================
// Loads the Muon data
//=============================================================================
bool UpdateMuonPIDInProtoP::getMuonData()
{
  // empty the map
  m_muonMap.clear();

  // Do we have any MuonPID results
  if ( !exist<MuonPIDs>(m_muonPath) )
  {
    Warning( "No MuonPIDs at '"+m_muonPath+"'", StatusCode::SUCCESS );
    return false;
  }

  // yes, so load them
  const MuonPIDs * muonpids = get<MuonPIDs>( m_muonPath );
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << "Successfully loaded " << muonpids->size()
            << " MuonPIDs from " << m_muonPath << endreq;
  }

  // refresh the reverse mapping
  for ( MuonPIDs::const_iterator iM = muonpids->begin();
        iM != muonpids->end(); ++iM )
  {
    m_muonMap[ (*iM)->idTrack() ] = *iM;
  }

  return true;
}
