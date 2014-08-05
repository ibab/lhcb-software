// $Id: $
#ifndef PARTICLEVETO_H 
#define PARTICLEVETO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleVeto.h"            // Interface
#include "Event/Particle.h"
#include "CaloUtils/CaloParticle.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 
#include "GaudiKernel/Incident.h" 
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

/** @class ParticleVeto ParticleVeto.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2012-04-25
 */
class ParticleVeto : public GaudiTool, virtual public IParticleVeto    , virtual public IIncidentListener {
public: 
  /// Standard constructor
  ParticleVeto( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);

  virtual ~ParticleVeto( ); ///< Destructor
  virtual StatusCode initialize();
  virtual StatusCode finalize();
  
  bool foundOverlap(const LHCb::Particle* particle, std::vector<std::string> containers,int mode=0);
  bool foundOverlap(const LHCb::Particle* particle, std::string container,int mode=0);
  bool foundOverlap(const LHCb::Particle* p1, const LHCb::Particle* p2, int mode=0);

  /// Inform that a new incident has occurred
  virtual void handle(const Incident& /* inc */ ) { 
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "IIncident Svc reset" << endmsg;
    m_cont="";
    //counter("BeginEvent reset")+=1;
  } 



protected:

private:
  bool foundOverlap(std::vector<const LHCb::Particle*> vp1, std::vector<const LHCb::Particle*> vp2, int mode=0);
  inline bool isPureNeutralCalo(const LHCb::Particle* P)const{
    LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
    return caloP.isPureNeutralCalo();
  };
  std::vector<const LHCb::Particle*> getTree(const LHCb::Particle* P);  
  std::string m_cont;
  LHCb::Particle::Range m_parts;
  LHCb::IParticlePropertySvc* m_ppsvc;

};
#endif // PARTICLEVETO_H
