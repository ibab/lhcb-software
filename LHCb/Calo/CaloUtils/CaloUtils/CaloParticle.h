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
                                         m_part(part),
                                         m_vert(NULL),
                                         m_isCalo(true),
                                         m_caloEndTree(){
      this->addCaloPosition( part );
    }

    // Special constructors
    CaloParticle(LHCb::Particle* part, LHCb::Vertex* vert) : CaloMomentum(),
                                                             m_part(part),
                                                             m_vert(vert),
                                                             m_isCalo(true),
                                                             m_caloEndTree(){
      this->addCaloPosition( part );
      this->addToVertex( vert );
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
    LHCb::Particle* particle(){return m_part;}
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
    LHCb::Particle* m_part;
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
  vertex->addToOutgoingParticles( m_part ); 
  this->referencePoint() = vertex->position(); 
  this->pointCovMatrix() = vertex->covMatrix();
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
} ;
#endif ///RecEvent_CaloParticle_H


