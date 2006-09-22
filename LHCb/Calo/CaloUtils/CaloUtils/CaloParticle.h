#ifndef RecEvent_CaloParticle_H
#define RecEvent_CaloParticle_H 1

// Include files
#include "CaloUtils/CaloMomentum.h"
//PhysEvent
#include "Event/Vertex.h"
#include "Event/Particle.h"



namespace LHCb 
{

  /** @class CaloMomentum CaloMomentum.h
   *
   * 
   */
  
  class CaloParticle : public CaloMomentum
  {
  public:
    

    /// Constructor
    CaloParticle(LHCb::Particle* part) : CaloMomentum(),
                                         m_parts(),
                                         m_vert(NULL),
                                         m_isCalo(true),
                                         m_caloEndTree(){
      this->addCaloPosition( part );
    }


    CaloParticle(LHCb::Particle* part, 
                 LHCb::CaloMomentum::Point point);
    CaloParticle(LHCb::Particle* part, 
                 LHCb::CaloMomentum::Point point,
                 LHCb::CaloMomentum::PointCovariance cov);
    
    
    /// Default Destructor
    virtual ~CaloParticle() {}
    
    // Setters
    void addCaloPosition(LHCb::Particle*  part );
    void addToVertex(LHCb::Vertex* vertex);

    // Getters
    std::vector<LHCb::Particle*>& particles(){return m_parts;}
    LHCb::Particle* particle(){return *(m_parts.begin()) ;} // return the first particle
    LHCb::Vertex* originVertex(){return m_vert;};
    LHCb::Particle::ConstVector& caloEndTree(){return m_caloEndTree;}
    bool isCalo(){ return m_isCalo; }
    
    // Update
    void updateParticle();
    void updateTree();
    void CaloParticleTree(const LHCb::Particle*  part );
    //
    
    
  protected:
    
  private:
    std::vector<LHCb::Particle*> m_parts;
    LHCb::Vertex*   m_vert;
    bool m_isCalo;
    LHCb::Particle::ConstVector m_caloEndTree;
  }; // class CaloParticle
} // namespace LHCb;
// -----------------------------------------------------------------------------
// end of class
// -----------------------------------------------------------------------------
inline  void LHCb::CaloParticle::addToVertex(LHCb::Vertex* vertex)
{
  m_vert= vertex;
  for(std::vector<LHCb::Particle*>::iterator ipart = m_parts.begin();ipart!=m_parts.end();++ipart){
    vertex->addToOutgoingParticles( *ipart ); 
  }  
  this->referencePoint() = vertex->position(); 
  this->pointCovMatrix() = vertex->covMatrix();
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
} ;
#endif ///RecEvent_CaloParticle_H


