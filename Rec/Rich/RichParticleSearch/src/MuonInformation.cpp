// Include files

// local
#include "MuonInformation.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonInformation
//
// 2010-10-04 : Matt Coombes
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( MuonInformation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
  MuonInformation::MuonInformation( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
    : GaudiTool ( type, name , parent )
    , m_caloElectron(0)
{
  declareInterface<IMuonInformation>(this);
}

//=============================================================================
// Destructor
//=============================================================================
MuonInformation::~MuonInformation() {}
//=============================================================================

StatusCode MuonInformation::initialize()
{
  StatusCode sc = GaudiTool::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  //if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_caloElectron = tool<ICaloElectron>("CaloElectron", this);

  //debug() << "Finished Initialization" << endmsg;
  return sc;
}

int MuonInformation::HasMuonInformation(const LHCb::Track* track)
{
  int hasMuonPID =0;
  const LHCb::ProtoParticles* pps = get< LHCb::ProtoParticles > (LHCb::ProtoParticleLocation::Charged);

  const LHCb::Track* Prototrack;

  // Loop over PP
  for( LHCb::ProtoParticles::const_iterator ipp = pps->begin() ;
       pps->end() != ipp ; ++ipp ){

    LHCb::ProtoParticle* pp = *ipp;

    Prototrack = pp->track();
    if (Prototrack != track){
      if ( msgLevel(MSG::DEBUG) ) debug()<<"Tracks do not match"<<endmsg;
      continue;
    }
    if (Prototrack == track){
      hasMuonPID =0;
      const LHCb::MuonPID* muonPID = pp->muonPID();




      //const LHCb::RichPID* richPID = pp->richPID();
      if(muonPID == 0){
        hasMuonPID =0;
        if ( msgLevel(MSG::DEBUG) ) debug()<<" No Muon PID"<<endmsg;
        continue;
      }
      if (muonPID != 0 ){
        hasMuonPID = 1;
        if ( msgLevel(MSG::DEBUG) ) debug()<<"Muon PID "<<muonPID<<endmsg;
      }

      bool isloosemuontrack = muonPID->IsMuonLoose();

      if (isloosemuontrack){
        hasMuonPID = 2;
        //info()<<"Is muon Loose"<<endmsg;
      }

      bool ismuontrack = muonPID->IsMuon();
      if (ismuontrack){
        hasMuonPID = 3;
        //info()<<"Is muon"<<endmsg;
      }

      LHCb::CaloHypo* hypo = m_caloElectron->electron();
      if ( NULL != hypo ){
        hasMuonPID = 8;
      }
      /*
        if( !m_caloElectron->set(pp)){
        info()<<"No Calo information"<<endmsg;
        }
      */
      //LHCb::CaloHypo* hypo = m_caloElectron->electron();
      /*
        if ( NULL != hypo ){
        info()<<"Electron"<<endmsg;
        hasMuonPID = 6;//electron
        }
      */
      if (Prototrack == track){
        break;
      }
    }        // end for loop
  }// End match tracks
  return hasMuonPID;
}
