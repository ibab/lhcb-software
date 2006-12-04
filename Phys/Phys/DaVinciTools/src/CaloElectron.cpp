// $Id: CaloElectron.cpp,v 1.1 2006-12-04 14:13:36 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "CaloUtils/CaloMomentum.h"
#include "CaloElectron.h"

//-----------------------------------------------------------------------------
// Implementation file for class : CaloElectron
//
// 2006-11-30 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloElectron );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloElectron::CaloElectron( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<ICaloElectron>(this);

}
//=============================================================================
// Destructor
//=============================================================================
CaloElectron::~CaloElectron() {} 

//=============================================================================

LHCb::CaloHypo* CaloElectron::hypo(const  LHCb::Particle* particle){
  return calo(particle,"Electron");
}

LHCb::CaloMomentum CaloElectron::bremstrahlung(const  LHCb::Particle* particle){
  LHCb::CaloHypo* brem = calo(particle,"BremStrahlung");
  if( NULL == brem ) return LHCb::CaloMomentum();
  LHCb::CaloMomentum bremPhoton( brem ,particle->referencePoint(), particle->posCovMatrix() );
  return bremPhoton;
}

double CaloElectron::e(const  LHCb::Particle* particle){
  LHCb::CaloHypo* elec = calo(particle,"Electron");
  if( NULL == elec ) return 0.;
  return elec->e() ;
}

double CaloElectron::eOverP(const  LHCb::Particle* particle){
  double eCalo=e(particle);
  const LHCb::ProtoParticle* proto = particle -> proto();
  if( NULL == proto) return 0.;
  const LHCb::Track* track = proto->track();
  if( NULL == track) return 0.;
  double pTrack = track->p();
  if( 0 == pTrack) return 0.;
  return eCalo/pTrack;
}


LHCb::CaloHypo* CaloElectron::calo(const  LHCb::Particle* particle, std::string type){ 

  const LHCb::ProtoParticle* proto = particle -> proto();
  if( NULL == proto) return NULL;
  const LHCb::Track* track = proto->track();
  if( NULL == track) return NULL;
  const SmartRefVector<LHCb::CaloHypo> hypos = proto->calo();
  if(0 == hypos.size())return NULL;

  //
  LHCb::CaloHypo* caloElectron = NULL;
  LHCb::CaloHypo* caloBrem = NULL;
  for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo =  hypos.begin(); ihypo != hypos.end() ; ++ihypo){
    const LHCb::CaloHypo* hypo =  *ihypo;
    if(NULL == hypo)continue;
    if( LHCb::CaloHypo::EmCharged == hypo->hypothesis() ) caloElectron = (LHCb::CaloHypo*)  hypo;
    if( LHCb::CaloHypo::Photon ==   hypo->hypothesis() )   caloBrem     = (LHCb::CaloHypo*) hypo;
  }
  if( type == "Electron") return caloElectron;
  if( type == "BremStrahlung") return caloBrem;
  return NULL;
}

