// $Id: CaloParticle.h,v 1.6 2009-11-20 15:46:17 odescham Exp $ 
// ============================================================================
#ifndef RecEvent_CaloParticle_H
#define RecEvent_CaloParticle_H 1
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.6 $ 
// ============================================================================
// Include files
// ============================================================================
// CaloUtils 
// ============================================================================
#include "CaloUtils/CaloMomentum.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Forward declarations
// ============================================================================
namespace LHCb { class Vertex  ; }
// ============================================================================
namespace LHCb{
  // ==========================================================================
  /** @class CaloMomentum CaloUtils/CaloMomentum.h
   *  Helepr class to evalauet the parameters of "Calo"-particles 
   *  @author Olivier Deschamps
   */
  class CaloParticle : public LHCb::CaloMomentum{
    // ========================================================================
  public:
    // ========================================================================
    /// Constructor fom particle 
    CaloParticle( LHCb::Particle*                            part  ) ;
    /// Constructor fom particle & origin vertex     
    CaloParticle( LHCb::Particle*                            part  , 
      const LHCb::CaloMomentum::Point&           point ) ;
    /// Constructor fom particle & origin vertex & covariance      
    CaloParticle( LHCb::Particle*                            part  , 
                  const LHCb::CaloMomentum::Point&           point ,
                  const LHCb::CaloMomentum::PointCovariance& cov   ) ;
    /// Default Destructor
    virtual ~CaloParticle() ;
    // ========================================================================
  public:
    // ========================================================================
    // Setters
    // ========================================================================
    void addCaloPosition( LHCb::Particle*  part   ) ;
    void addToVertex    ( LHCb::Vertex*    vertex ) ;
    // ========================================================================
  public:
    // ========================================================================    
    // Getters
    // ========================================================================    
    const std::vector<LHCb::Particle*>& particles   () const 
    { return m_parts      ; }
    const LHCb::Vertex*                originVertex () const 
    { return m_vert       ; }
    const LHCb::Particle::ConstVector& caloEndTree  () const 
    { return m_caloEndTree; }
    // get (the firts) particle 
    LHCb::Particle* particle() const ;
    // calo?
    bool isCalo() const { return m_isCalo; }
    bool isPureNeutralCalo() const { return m_isCalo && m_neutral; }
    // ========================================================================
  public: 
    // ========================================================================
    // Update
    // ========================================================================
    void updateParticle   () ;
    void updateTree       () ;
    void CaloParticleTree ( const LHCb::Particle*  part ) ;
    // ========================================================================
  private:
    // ========================================================================
    /// the default constructor is disabled 
    CaloParticle() ;                     // the default constructor is disabled 
    // ========================================================================
  private:
    // ========================================================================
    LHCb::Particle::Vector      m_parts       ;
    LHCb::Vertex*               m_vert        ;
    bool                        m_isCalo      ;
    bool                        m_neutral     ;
    LHCb::Particle::ConstVector m_caloEndTree ;
    // ========================================================================
  }; // class CaloParticle
  // ==========================================================================
} // end of namespace LHCb
#endif ///RecEvent_CaloParticle_H
// ============================================================================


