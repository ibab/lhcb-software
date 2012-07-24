// $Id: 
// ============================================================================
#ifndef PHOTONMAKER_H
#define PHOTONMAKER_H 1
// Include files
#include "GaudiAlg/GaudiTool.h"
#include "Event/Particle.h"
#include "Kernel/ICaloParticleMaker.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"

namespace LHCb{
  class ProtoParticle ;
  class CaloHypo      ;
}

/** @class PhotonMaker PhotonMaker.h
 *
 *  The specialized producer of photons
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-01-19
 */

class PhotonMaker : public GaudiTool,
                    virtual public ICaloParticleMaker
{
public:


  PhotonMaker( const std::string& type   ,
               const std::string& name   ,
               const IInterface*  parent ) ;
  virtual ~PhotonMaker() ;

  // Make the particles
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ;
  virtual void setPoint ( const Gaudi::XYZPoint pos ){ m_point = pos; }
  virtual void setPoint ( const Gaudi::XYZPoint pos, const Gaudi::SymMatrix3x3 cov ){
    m_point = pos;
    m_pointErr = cov;
  }
  virtual void setPoint    ( const LHCb::Vertex* vert ){
    m_point  = vert->position();
    m_pointErr = vert->covMatrix();
  }

  virtual StatusCode initialize    () ;
  virtual StatusCode finalize      () ;

protected:

  // confidence level evaluator
  double confLevel        ( const LHCb::ProtoParticle* pp ) const ;
  // setters

private:

  long m_Id;
  // Input PP container
  std::string                        m_input            ;
  // nominal production vertex
  Gaudi::XYZPoint                    m_point            ;
  Gaudi::SymMatrix3x3                m_pointErr         ;

  // techniques for CL evaluation
  bool                               m_useCaloTrMatch   ;
  bool                               m_useCaloDepositID ;
  bool                               m_useShowerShape   ;
  bool                               m_useClusterMass   ;
  bool                               m_usePhotonID      ;
  // Filters
  double                             m_clCut            ;
  double                             m_ptCut            ;
  bool                               m_converted        ;
  bool                               m_unconverted      ;
  unsigned long  m_count[3];
  double m_minPrs;
  std::string m_part;
  bool m_addHcal;
  double m_maxHcal;
  double m_minHcal;
  double m_maxPrs;
  double m_mas;

};

#endif // PHOTONMAKER_H
