#include "CaloUtils/CaloParticle.h"

// Special constructor


LHCb::CaloParticle::CaloParticle(LHCb::Particle* part, 
                                 LHCb::CaloMomentum::Point point): 
  CaloMomentum(), m_parts(), m_vert(NULL),m_isCalo(true),  m_caloEndTree(){
  this->referencePoint() = point;
  this->addCaloPosition( part );
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
}

LHCb::CaloParticle::CaloParticle(LHCb::Particle* part,
                                 LHCb::CaloMomentum::Point point, 
                                 LHCb::CaloMomentum::PointCovariance cov):
  CaloMomentum(), m_parts(),m_vert(NULL),  m_isCalo(true),  m_caloEndTree(){
  this->referencePoint() = point;
  this->pointCovMatrix() = cov;
  this->addCaloPosition( part );
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
}

//-------
void LHCb::CaloParticle::addCaloPosition(LHCb::Particle* part)
{
  // --- Fill CaloPosition vector from particle
  //  2 configuration : 
  //    - the particle is basic (e.g. photon, mergedPi0)  : use proto->calo
  //    - the particle is composite (e.g. pi0/eta->gg, Ks/B->pi0pi0->gggg, ...) : run along the decay tree

  
  // Some checks
  this->addToFlag( LHCb::CaloMomentum::FromPart);  
  if( 0 == part){
    this->addToStatus( LHCb::CaloMomentum::NullPart);
    return;
  }
  if( 0 != part->charge() ){
    this->addToStatus( LHCb::CaloMomentum::ChargedParticle);
    return;    
  }
  
  // Check the particle origin (protoParticle)
  if( 0 != part->proto() ){
    if( part->proto()->calo().empty() ){      
      m_isCalo = false ; 
      this->addToStatus(LHCb::CaloMomentum::PartIsNotCaloObject);
      return;
    }
    else{      
      // particle derive from proto->caloHypo (-> photon, mergedPi0 )
      m_parts.push_back(part);
      LHCb::CaloMomentum::addCaloPosition( part->proto() );
    }
  }else if( !part->isBasicParticle() ){
    // particle is composite
    this->addToFlag( LHCb::CaloMomentum::FromPartDaughters);
    //    m_caloEndTree.clear();
    for(SmartRefVector<LHCb::Particle>::const_iterator idau = part->daughters().begin();
        idau != part->daughters().end();++idau){ 
      this->CaloParticleTree( *idau );
    }
    if( m_isCalo ){
      m_parts.push_back(part);
      // the end-tree particles are pure calo objects
      for(LHCb::Particle::ConstVector::const_iterator icalo = m_caloEndTree.begin();
          icalo != m_caloEndTree.end(); ++icalo){
        LHCb::CaloMomentum::addCaloPosition( (* icalo)->proto() );
      }
    }
    else{
      //      m_caloEndTree.clear();
      this->addToStatus(LHCb::CaloMomentum::PartIsNotCaloObject);
    }
  }        
  else{
    //    m_caloEndTree.clear();
    this->addToStatus(LHCb::CaloMomentum::PartIsNotCaloObject);
  }
}

//-------
void LHCb::CaloParticle::CaloParticleTree(const LHCb::Particle* part)
{

  if( !m_isCalo )return;
  if( part->isBasicParticle() ){
    if( 0 == part->proto() )                { m_isCalo = false; }
    else if( part->proto()->calo().empty() ){ m_isCalo = false; }
    else                                    { m_caloEndTree.push_back( part ); }
  }
  else{
    for (SmartRefVector<LHCb::Particle>::const_iterator idau = part->daughters().begin();
         idau!=part->daughters().end();++idau){
      this -> CaloParticleTree( *idau);
    }
  }
  return;
}

//---
void LHCb::CaloParticle::updateParticle()
{
  if( LHCb::CaloMomentum::OK == this->status() ){
    for(std::vector<LHCb::Particle*>::iterator ipart = m_parts.begin();ipart!=m_parts.end();++ipart){
      LHCb::Particle* part = *ipart;
      part->setReferencePoint( this->referencePoint() );
      part->setPosCovMatrix( this->pointCovMatrix() );
      part->setMomentum( this->momentum() );
      part->setMomCovMatrix( this->momCovMatrix() );
      part->setPosMomCovMatrix( this->momPointCovMatrix() );
      this->addToFlag( LHCb::CaloMomentum::ParticleUpdated);  
    }    
  }
}


//---
void LHCb::CaloParticle::updateTree()
{
  this->updateParticle();

  for(std::vector<LHCb::Particle*>::iterator ipart = m_parts.begin();ipart!=m_parts.end();++ipart){
    LHCb::Particle* part = *ipart;
    if( !part->isBasicParticle() && LHCb::CaloMomentum::OK == this->status() ){ 
      for (SmartRefVector<LHCb::Particle>::const_iterator idau = part->daughters().begin();
           idau != part->daughters().end();++idau){
        const LHCb::Particle* daughter = *idau;
        LHCb::CaloParticle caloDau( (LHCb::Particle*)daughter , // convert to non-const
                                    this->referencePoint() ,
                                    this->pointCovMatrix() );
        caloDau.updateTree();
      }
      this->addToFlag( LHCb::CaloMomentum::ParticleTreeUpdated);  
    }
  }
  return;
}




