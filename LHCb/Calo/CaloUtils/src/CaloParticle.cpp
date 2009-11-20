// $Id: CaloParticle.cpp,v 1.6 2009-11-20 15:46:17 odescham Exp $ 
// ============================================================================
// CVS Tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $
// ============================================================================
// Incldue files 
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
#include "Event/Vertex.h"
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloParticle.h"
// ============================================================================
/** @file 
 *  Implementation file for class LHCb::CaloParticle
 *  @author Olivier Deschamps
 */
// ============================================================================
// Default Destructor
// ============================================================================
LHCb::CaloParticle::~CaloParticle() {}
// ============================================================================
// constructor from the particle 
// ============================================================================
LHCb::CaloParticle::CaloParticle
( LHCb::Particle* part ) 
  : CaloMomentum  ( ) ,
    m_parts       ( ) ,
    m_vert        ( NULL ) ,
    m_isCalo      ( true ) ,
    m_neutral     ( true ) ,
    m_caloEndTree ( )
{
  if( NULL == part)m_isCalo = false;
  this -> addCaloPosition( part );
}
// ============================================================================
// constructor from particle & origin point 
// ============================================================================
LHCb::CaloParticle::CaloParticle 
( LHCb::Particle*                  part  , 
  const LHCb::CaloMomentum::Point& point )
  : CaloMomentum ()
  , m_parts      () 
  , m_vert       ( NULL )
  , m_isCalo     ( true )
  , m_neutral     ( true ) 
  ,  m_caloEndTree()
{
  if( NULL == part)m_isCalo = false;
  setReferencePoint  ( point ) ;
  this -> addCaloPosition ( part );
  addToFlag       ( LHCb::CaloMomentum::NewReferencePoint);
}
// ============================================================================
// constructor from particle & origin point & covarinace  
// ============================================================================
LHCb::CaloParticle::CaloParticle 
( LHCb::Particle*                            part  ,
  const LHCb::CaloMomentum::Point&           point , 
  const LHCb::CaloMomentum::PointCovariance& cov   )
  : CaloMomentum  ()
  , m_parts       ()
  , m_vert        ( NULL )
  , m_isCalo      ( true )
  , m_neutral     ( true ) 
  , m_caloEndTree ()
{
  if( NULL == part)m_isCalo = false;
  setReferencePoint ( point , cov ) ;
  this->addCaloPosition( part );
  addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  addToFlag( LHCb::CaloMomentum::NewPointCovariance);
}
// ============================================================================
// Particle
// ============================================================================
void LHCb::CaloParticle::addCaloPosition ( LHCb::Particle* part )
{
  // --- Fill CaloPosition vector from particle
  //  2 configuration : 
  //    - the particle is basic (e.g. photon, mergedPi0)  : use proto->calo
  //    - the particle is composite (e.g. pi0/eta->gg, Ks/B->pi0pi0->gggg, ...) : run along the decay tree

  
  
  // Some checks
  this->addToFlag( LHCb::CaloMomentum::FromPart);  
  if( 0 == part){
    this->addToStatus( LHCb::CaloMomentum::NullPart);
    m_neutral = false;
    return;
  }
  if( 0 != part->charge() ){
    this->addToStatus( LHCb::CaloMomentum::ChargedParticle);
    m_neutral = false;
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
      m_caloEndTree.push_back( part );
      LHCb::CaloMomentum::addCaloPosition( part->proto() );
    }
  }else if( !part->isBasicParticle() ){
    // particle is composite
    this->addToFlag( LHCb::CaloMomentum::FromPartDaughters);
    this->CaloParticleTree( part );
    if( m_isCalo ){
      m_parts.push_back(part);
      // the end-tree particles are pure calo objects
      for(LHCb::Particle::ConstVector::const_iterator icalo = m_caloEndTree.begin();
          icalo != m_caloEndTree.end(); ++icalo){
        if( (*icalo)->charge() != 0 )m_neutral = false;
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
// ============================================================================
// update the tree 
// ============================================================================
void LHCb::CaloParticle::CaloParticleTree ( const LHCb::Particle* part )
{
  
  if( !m_isCalo ){
    m_caloEndTree.clear();
    return;
  }
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
// ============================================================================
// update particle 
// ============================================================================
void LHCb::CaloParticle::updateParticle ()
{
  if( LHCb::CaloMomentum::OK == this->status() )
  {
    for ( LHCb::Particle::Vector::iterator ipart = m_parts.begin();ipart!=m_parts.end();++ipart){
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
// ============================================================================
// update tree
// ============================================================================
void LHCb::CaloParticle::updateTree()
{
  this->updateParticle();
  
  for( LHCb::Particle::Vector::iterator ipart = m_parts.begin() ;ipart!=m_parts.end();++ipart){
    LHCb::Particle* part = *ipart;
    if( !part->isBasicParticle() && LHCb::CaloMomentum::OK == this->status() ){ 
      const SmartRefVector<LHCb::Particle>& daughters = part->daughters() ;
      for ( SmartRefVector<LHCb::Particle>::const_iterator idau = daughters.begin();
            idau != daughters.end(); ++idau ){
        const LHCb::Particle* daughter = *idau;
        LHCb::CaloParticle caloDau 
          ( const_cast<LHCb::Particle*> ( daughter ) , // convert to non-const
            this -> referencePoint () ,
            this -> pointCovMatrix () ) ;
        caloDau.updateTree();
      }
      this->addToFlag( LHCb::CaloMomentum::ParticleTreeUpdated);  
    }
  }
  return;
}
// ============================================================================
// add vertex 
// ============================================================================
void LHCb::CaloParticle::addToVertex 
( LHCb::Vertex* vertex )
{
  m_vert = vertex;
  for( LHCb::Particle::Vector::iterator ipart = m_parts.begin() ;
       ipart!=m_parts.end();++ipart)
  {
    vertex->addToOutgoingParticles( *ipart ); 
  }  
  setReferencePoint ( vertex ) ;
  addToFlag ( LHCb::CaloMomentum::NewReferencePoint);
  addToFlag ( LHCb::CaloMomentum::NewPointCovariance);
} 
// ============================================================================
// get the particle 
// ============================================================================
LHCb::Particle* LHCb::CaloParticle::particle() const 
{ 
  if ( m_parts.empty() ) { return 0 ; }
  return m_parts.front() ;
} 

