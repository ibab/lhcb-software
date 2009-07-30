// $Id: ProtoParticleCloner.cpp,v 1.6 2009-07-30 10:03:01 jpalac Exp $
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

// local
#include "ProtoParticleCloner.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ProtoParticleCloner
//
// 2008-04-01 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ProtoParticleCloner );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ProtoParticleCloner::ProtoParticleCloner( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
  : 
  base_class ( type, name , parent ),
  m_trackCloner(0),
  m_trackClonerName("TrackCloner")
{

  declareProperty("ICloneTrack", m_trackClonerName);

}
//=============================================================================
StatusCode ProtoParticleCloner::initialize() 
{

  debug() << "==> Initialize" << endmsg;

  StatusCode sc = base_class::initialize();
  
  if (! sc.isSuccess() ) return sc;
  
  m_trackCloner = tool<ICloneTrack>(m_trackClonerName, this->parent() );

  return StatusCode::SUCCESS;
}
//=============================================================================
LHCb::ProtoParticle* ProtoParticleCloner::operator() (const LHCb::ProtoParticle* protoParticle)
{
  return this->clone(protoParticle);
}
//=============================================================================
LHCb::ProtoParticle* ProtoParticleCloner::clone(const LHCb::ProtoParticle* protoParticle)
{

  if (0==protoParticle) return 0;

  LHCb::ProtoParticle* protoParticleClone = 
    cloneKeyedContainerItem<LHCb::ProtoParticle, BasicProtoParticleCloner>(protoParticle);

  if (0==protoParticleClone) return 0;

  if ( m_trackCloner!=0 )
    protoParticleClone->setTrack( (*m_trackCloner)(protoParticle->track() ) );

  LHCb::RichPID* clonedRichPID =  
    cloneKeyedContainerItem<LHCb::RichPID, RichPIDCloner>(protoParticle->richPID());

  LHCb::MuonPID* clonedMuonPID =  
    cloneKeyedContainerItem<LHCb::MuonPID, MuonPIDCloner>(protoParticle->muonPID());

  if (clonedRichPID) clonedRichPID->setTrack(protoParticleClone->track());
  
  if (clonedMuonPID) {
    clonedMuonPID->setMuonTrack(protoParticleClone->track());
    clonedMuonPID->setIDTrack(SmartRef<LHCb::Track>(protoParticle->muonPID()->idTrack() ) );
  }
  
  protoParticleClone->setRichPID(clonedRichPID);  
  protoParticleClone->setMuonPID(clonedMuonPID);

  return protoParticleClone;
  
}
//=============================================================================
// Destructor
//=============================================================================
ProtoParticleCloner::~ProtoParticleCloner() {} 

//=============================================================================
