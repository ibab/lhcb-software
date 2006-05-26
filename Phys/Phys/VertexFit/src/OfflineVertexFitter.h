
#ifndef OFFLINEVERTEXFITTER_H 
#define OFFLINEVERTEXFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVertexFit.h"            // Interface

using namespace LHCb;

typedef ROOT::Math::SMatrix<double, 12, 12,
                              ROOT::Math::MatRepSym<double,12> > SymMatrix12x12;

typedef ROOT::Math::SMatrix<double, 13, 13,
                              ROOT::Math::MatRepSym<double,13> > SymMatrix13x13;

typedef ROOT::Math::SMatrix<double, 14, 14,
                              ROOT::Math::MatRepSym<double,14> > SymMatrix14x14;


/** @class OfflineVertexFitter OfflineVertexFitter.h
 *  
 *  reimplementation of the KFFitTool to comply with
 *  the new interface IVertexFit.h
 *
 *  @author Yuehong Xie
 *  @date   2006-03-15
 */
class OfflineVertexFitter : public GaudiTool, virtual public IVertexFit {
public: 
  /// Standard constructor
  OfflineVertexFitter( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  StatusCode initialize();

  /// Method to fit a vertex 
  StatusCode fit( const LHCb::Particle::ConstVector&,  LHCb::Vertex& ) const ;  

  /// Method to fit a vertex returning a Particle (that should already know its PID)
  StatusCode fit( const LHCb::Particle::ConstVector&,  LHCb::Particle&, LHCb::Vertex& ) const ; 

  virtual ~OfflineVertexFitter( ); ///< Destructor


  StatusCode reFit( LHCb::Particle& particle ) const {
     LHCb::Vertex* vertex = particle.endVertex() ;
     return fit( particle.daughtersVector(),
                 particle , *vertex         ) ;
  }

  StatusCode combine( const LHCb::Particle::ConstVector& daughters , 
                      LHCb::Particle&        mother   , 
                      LHCb::Vertex&          vertex   ) const {
    Error("combine is not implemented for OffLineVertexFitter");
    return StatusCode::FAILURE;
  }


  StatusCode add(const LHCb::Particle* p,LHCb::Vertex& v) const {
    Error("add is not implemented for OffLineVertexFitter");
    return StatusCode::FAILURE;
  }

  StatusCode remove(const LHCb::Particle* p,LHCb::Vertex& v) const {
    Error("remove is not implemented for OffLineVertexFitter");
    return StatusCode::FAILURE;
  }


protected:

  bool isResonance(const LHCb::Particle*) const; 
  bool isVertexed(const LHCb::Particle*) const;
  int countTraj(const LHCb::Particle*) const;
  bool isPhotonPair(const LHCb::Particle*) const;
  bool isPurePhotonDecay(const LHCb::Particle*) const;

  StatusCode classify(const LHCb::Particle*, 
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector & ) const;

  StatusCode seeding(LHCb::Particle&,
                     LHCb::Particle::ConstVector &,
                     LHCb::Particle::ConstVector & ) const;

  StatusCode addVertexed(LHCb::Particle&, 
                         const LHCb::Particle *) const;

  StatusCode addFlying(LHCb::Particle&, 
                       const LHCb::Particle *) const;

  StatusCode addPhoton(LHCb::Particle&, 
                       const LHCb::Particle *) const;

  StatusCode addPhotonPair(LHCb::Particle&, 
                           const LHCb::Particle *) const;

  StatusCode updateParticle(LHCb::Particle& part,  
                    Gaudi::Vector7& V7,
                    Gaudi::SymMatrix7x7& C7,
                    double& chi2,
                    int& NDoF) const;

  void getParticleInfo(const LHCb::Particle& part,
                       Gaudi::Vector7& V7,
                       Gaudi::SymMatrix7x7& C7,
                       double& chi2,
                       int& NDoF) const;

  void convertE2M(const Gaudi::Vector7& V7, 
                  const Gaudi::SymMatrix7x7& C7,
                  Gaudi::Vector7& Vm7, 
                  Gaudi::SymMatrix7x7& Cm7) const;


  void convertM2E(const Gaudi::Vector7& Vm7, 
                  const Gaudi::SymMatrix7x7& Cm7,
                  Gaudi::Vector7& V7, 
                  Gaudi::SymMatrix7x7& C7) const;


  StatusCode fitTwo(const LHCb::Particle* dau1, 
                    const LHCb::Particle* dau2,
                    Gaudi::Vector7& V7,
                    Gaudi::SymMatrix7x7& C7,
                    double& chi2) const;

  double getZEstimate (const LHCb::Particle* part1,
                       const LHCb::Particle* part2) const;

  StatusCode constrainMass(Gaudi::Vector7& V7, 
                           Gaudi::SymMatrix7x7& C7, 
                           double& nominalMass) const;
  bool requireMassConstraint(const LHCb::Particle*, double& nominalMass) const;
 
private:
      
  int m_photonID; ///< Photon particle ID

  IParticleStuffer* m_stuffer ; ///< Particle stuffer

  IParticleTransporter* m_transporter;

  IParticlePropertySvc* m_ppSvc;

  bool m_useResonanceVertex;
  bool m_applyDauMassConstraint;
  double m_widthThreshold;
  int m_maxIter;
  double m_maxDeltaChi2;
  double m_maxDeltaZ;

};
#endif // OFFLINEVERTEXFITTER_H
