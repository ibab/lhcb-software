
#ifndef OFFLINEVERTEXFITTER_H
#define OFFLINEVERTEXFITTER_H 1

// STD
#include <sstream>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgorithm.h"
#include "GaudiKernel/PhysicalConstants.h"

// Event model
#include "Event/Vertex.h"
#include "Event/Particle.h"

#include "Kernel/IVertexFit.h"
#include "Kernel/IParticleTransporter.h"
#include "Kernel/IParticleStuffer.h"

// from LHCb
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

using namespace LHCb;

/** @class OfflineVertexFitter OfflineVertexFitter.h
 *
 *  reimplementation of the KFFitTool to comply with
 *  the new interface IVertexFit.h
 *
 *  @author Yuehong Xie
 *  @date   2006-03-15
 */
class OfflineVertexFitter : public GaudiTool,
                            virtual public IVertexFit
{

private:

  typedef ROOT::Math::SMatrix<double, 10, 10,
                              ROOT::Math::MatRepSym<double,10> > SymMatrix10x10;

  typedef ROOT::Math::SMatrix<double, 12, 12,
                              ROOT::Math::MatRepSym<double,12> > SymMatrix12x12;

  typedef ROOT::Math::SMatrix<double, 13, 13,
                              ROOT::Math::MatRepSym<double,13> > SymMatrix13x13;

  typedef ROOT::Math::SMatrix<double, 14, 14,
                              ROOT::Math::MatRepSym<double,14> > SymMatrix14x14;

public:

  /// Standard constructor
  OfflineVertexFitter( const std::string& type,
                       const std::string& name,
                       const IInterface* parent);

  StatusCode initialize();

  /// Method to fit a vertex
  StatusCode fit( LHCb::Vertex& ,
                  const LHCb::Particle::ConstVector& ) const ;

  /// Method to fit a vertex returning a Particle (that should already know its PID)
  StatusCode fit
  ( const LHCb::Particle::ConstVector&,
    LHCb::Vertex&   ,
    LHCb::Particle& ) const ;

  virtual ~OfflineVertexFitter( ); ///< Destructor


  StatusCode reFit( LHCb::Particle& particle ) const
  {
    LHCb::Vertex* vertex = particle.endVertex() ;
    return fit( particle.daughtersVector(),
                *vertex  , particle ) ;
  }

  StatusCode combine
  ( const LHCb::Particle::ConstVector& daughter,
    LHCb::Particle&                    mother  ,
    LHCb::Vertex&                      vertex  ) const
  {
    return fit ( daughter , vertex , mother ) ;
  }


  StatusCode add(const LHCb::Particle*,
                 LHCb::Vertex& ) const {
    Error("add is not implemented for OffLineVertexFitter");
    return StatusCode::FAILURE;
  }

  StatusCode remove(const LHCb::Particle*,
                    LHCb::Vertex& ) const {
    Error("remove is not implemented for OffLineVertexFitter");
    return StatusCode::FAILURE;
  }

private:

  bool isResonance(const LHCb::Particle*) const;
  bool isMergedPi0(const LHCb::Particle*) const;
  bool isVertexed(const LHCb::Particle*) const;
  int countTraj(const LHCb::Particle*) const;
  bool isPhotonPair(const LHCb::Particle*) const;
  bool isPurePhotonDecay(const LHCb::Particle*) const;

  StatusCode classify(const LHCb::Particle*,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector &,
                      LHCb::Particle::ConstVector & ) const;

  StatusCode seeding(LHCb::Particle&,
                     LHCb::Particle::ConstVector&,
                     LHCb::Particle::ConstVector& ) const;

  StatusCode addVertexed(LHCb::Particle&,
                         const LHCb::Particle *) const;

  StatusCode addFlying(LHCb::Particle&,
                       const LHCb::Particle *) const;

  StatusCode addPhoton(LHCb::Particle&,
                       const LHCb::Particle *) const;

  StatusCode addMergedPi0(LHCb::Particle&,
                          const LHCb::Particle *) const;

  StatusCode addPhotonPair(LHCb::Particle&,
                           const LHCb::Particle *) const;

  StatusCode updateParticle(LHCb::Particle& part,
                            const Gaudi::Vector7& V7,
                            const Gaudi::SymMatrix7x7& C7,
                            const double chi2,
                            const int NDoF) const;

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

  bool requireMassConstraint(const LHCb::Particle*,
                             double& nominalMass) const;

  StatusCode getPhotonParameter(const LHCb::Particle& part,
                                double& zg,
                                Gaudi::Vector3& para ,
                                Gaudi::SymMatrix3x3& cov ) const;

  StatusCode getMergedPi0Parameter(const LHCb::Particle& part,
                                   double& zg1,
                                   Gaudi::Vector3& para1 ,
                                   Gaudi::SymMatrix3x3& cov1,
                                   double& zg2,
                                   Gaudi::Vector3& para2 ,
                                   Gaudi::SymMatrix3x3& cov2 ) const;

  void addToDaughters( LHCb::Particle&, const LHCb::Particle::ConstVector& ) const ; // add to particle daughters
  void addToOutgoingParticles( LHCb::Vertex&, const LHCb::Particle::ConstVector& ) const ; // add to vertex particles
  void addToOutgoingParticles( LHCb::Vertex&, const LHCb::Particle* ) const ; // add to vertex particles

  private:

    // ============================================================================
    // get the correct algorithm context 
    // ============================================================================
    std::string getMyAlg() const 
    {
      std::string myAlg = "" ;
      if ( m_printMyAlg )
      {
        const IAlgContextSvc * asvc =  contextSvc();
        const IAlgorithm *  current = ( asvc ? asvc->currentAlg() : NULL );
        if ( current ) { myAlg = " [" + current->name() + "]" ; }
      }
      return myAlg ;
    }
    // ========================================================================
    inline StatusCode _Warning
    ( const std::string& msg, 
      const StatusCode&  code = StatusCode::FAILURE,
      const size_t mx = 10 ) const 
    {
      return Warning ( msg + getMyAlg(), code, mx );
    }
    // ========================================================================
    inline StatusCode _Error 
    ( const std::string& msg, 
      const StatusCode&  code = StatusCode::FAILURE,
      const size_t mx = 10 ) const 
    {
      return Error ( msg + getMyAlg(), code, mx );
    }
    // ========================================================================

private:

  int m_photonID; ///< Photon particle ID
  int m_pi0ID; ///< Pi0 particle ID

  IParticleTransporter* m_transporter;

  LHCb::IParticlePropertySvc* m_ppSvc;

  bool m_useResonanceVertex;
  bool m_includeDauVertexChi2;
  bool m_applyDauMassConstraint;
  bool m_applyMomMassConstraint;
  double m_widthThreshold;
  int m_maxIter;
  double m_maxDeltaChi2;
  double m_maxDeltaZ;

  std::string m_transporterName;

  /// Option to include alg name in printout
  bool m_printMyAlg  ; 

};

//==================================================================
//  method to determine if a particle is a merged pi0
//==================================================================
inline bool OfflineVertexFitter::isMergedPi0(const LHCb::Particle* part) const
{
  return ( part->isBasicParticle() && part->particleID().pid() == m_pi0ID );
}

//==================================================================
//  method to determine if a particle has a reconstructed vertex
//==================================================================
inline bool OfflineVertexFitter::isVertexed(const LHCb::Particle* part) const
{
  const int nflying = countTraj(part);
  return ( nflying >= 2 );
}

//=============================================================================
//add To particle Daughters
//=============================================================================
inline void
OfflineVertexFitter::addToDaughters( LHCb::Particle& P,
                                     const LHCb::Particle::ConstVector& Parts) const
{
  for ( Particle::ConstVector::const_iterator iterP = Parts.begin();
        iterP != Parts.end(); ++iterP )
  {
    P.addToDaughters(*iterP);
    if ( msgLevel(MSG::VERBOSE) )
    {
      verbose() << "Added a  " << (*iterP)->particleID().pid() << " to particle. Size now "
                << P.daughters().size() << endmsg ;
    }
  }
  return ;
}

//=============================================================================
//add To vertex outgoing particles
//=============================================================================
inline void
OfflineVertexFitter::addToOutgoingParticles( LHCb::Vertex& V,
                                             const LHCb::Particle::ConstVector& Parts )
  const
{
  for ( Particle::ConstVector::const_iterator iterP = Parts.begin();
        iterP != Parts.end(); ++iterP )
  {
    addToOutgoingParticles(V,*iterP);
  }
}
//=============================================================================
//add To vertex outgoing particles
//=============================================================================
inline void
OfflineVertexFitter::addToOutgoingParticles( LHCb::Vertex& V,
                                             const LHCb::Particle* P ) const
{
  V.addToOutgoingParticles(P);
  if ( msgLevel(MSG::VERBOSE ))
  {
    verbose() << "Added a  " << P->particleID().pid() << " to vertex. Size now "
              << V.outgoingParticles().size() << endmsg ;
  }
}


#endif // OFFLINEVERTEXFITTER_H
