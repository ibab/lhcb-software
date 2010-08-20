// $Id: ProtoParticleDeepCloner.cpp,v 1.1 2010-08-20 13:56:03 jpalac Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

// from LHCb
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/MuonPID.h"
#include "Event/RichPID.h"

// from MicroDST
#include <MicroDST/ICloneTrack.h>
#include <MicroDST/ICloneCaloHypo.h>
#include <MicroDST/ICloneMuonPID.h>

// local
#include "ProtoParticleDeepCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleDeepCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleDeepCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleDeepCloner::ProtoParticleDeepCloner( const std::string& type,
						  const std::string& name,
						  const IInterface* parent )
  : 
  base_class ( type, name , parent ),
  m_trackCloner(0),
  m_caloHypoCloner(0),
  m_muonPIDCloner(0),
  m_trackClonerName("TrackCloner"),
  m_caloHypoClonerName("CaloHypoCloner"),
  m_muonPIDClonerName("MuonPIDCloner")
{

  declareProperty("ICloneTrack", m_trackClonerName);
  declareProperty("ICloneCaloHypo", m_caloHypoClonerName);
  declareProperty("ICloneMuonPID", m_muonPIDClonerName);

}
//=============================================================================
StatusCode ProtoParticleDeepCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  m_trackCloner = tool<ICloneTrack>(m_trackClonerName, this->parent() );
  m_caloHypoCloner = tool<ICloneCaloHypo>(m_caloHypoClonerName, this->parent() );
  m_muonPIDCloner = tool<ICloneMuonPID>(m_muonPIDClonerName, this->parent() );

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::ProtoParticle* ProtoParticleDeepCloner::operator() (const LHCb::ProtoParticle* protoParticle)
{
  return this->clone(protoParticle);
}
//=============================================================================
LHCb::ProtoParticle* ProtoParticleDeepCloner::clone(const LHCb::ProtoParticle* protoParticle)
{

  if (0==protoParticle) return 0;

  LHCb::ProtoParticle* protoParticleClone = 
    cloneKeyedContainerItem<BasicProtoParticleCloner>(protoParticle);

  if (0==protoParticleClone) return 0;

  if ( m_trackCloner!=0 )
    protoParticleClone->setTrack( (*m_trackCloner)(protoParticle->track() ) );

  if (m_muonPIDCloner!=0) {
    protoParticleClone->setMuonPID( (*m_muonPIDCloner)(protoParticle->muonPID()) );
  }
  
  if (m_caloHypoCloner!=0) {
    SmartRefVector<LHCb::CaloHypo> caloHypos = protoParticle->calo();
    if (!caloHypos.empty()) {
      SmartRefVector<LHCb::CaloHypo> clonedHypos;
      SmartRefVector<LHCb::CaloHypo>::const_iterator iCalo = caloHypos.begin();
      SmartRefVector<LHCb::CaloHypo>::const_iterator caloEnd = caloHypos.end();
      for ( ; iCalo != caloEnd; ++iCalo) {
        clonedHypos.push_back((*m_caloHypoCloner)(*iCalo));
      }
      protoParticleClone->setCalo(clonedHypos);
    }
  }

  LHCb::RichPID* clonedRichPID =  
    cloneKeyedContainerItem<RichPIDCloner>(protoParticle->richPID());
  if (clonedRichPID) {
    clonedRichPID->setTrack(protoParticleClone->track());
    protoParticleClone->setRichPID(clonedRichPID);  
  }


  return protoParticleClone;
  
}
//=============================================================================
// Destructor
//=============================================================================
ProtoParticleDeepCloner::~ProtoParticleDeepCloner() {} 

//=============================================================================
