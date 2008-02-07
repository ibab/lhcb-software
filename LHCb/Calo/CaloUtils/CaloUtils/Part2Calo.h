// $Id: Part2Calo.h,v 1.3 2008-02-07 12:25:54 cattanem Exp $
#ifndef PART2CALO_H 
#define PART2CALO_H 1

// Include files
#include "CaloUtils/Track2Calo.h"
#include "Event/ProtoParticle.h"
// from Gaudi
#include "CaloInterfaces/IPart2Calo.h"            



/** @class Part2Calo Part2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class Part2Calo : public Track2Calo, virtual public IPart2Calo {
public: 
  /// Standard constructor
  Part2Calo( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~Part2Calo( ); ///< Destructor
  virtual StatusCode initialize();
  
  bool match(const  LHCb::ProtoParticle* proto,
                     std::string det = DeCalorimeterLocation::Ecal,
                     CaloPlane::Plane plane = CaloPlane::ShowerMax, 
                     double delta = 0.
                     );
  bool match(const  LHCb::Particle* part,
                     std::string det = DeCalorimeterLocation::Ecal,
                     CaloPlane::Plane plane = CaloPlane::ShowerMax, 
                     double delta = 0.
                     );
  inline bool inAcceptance();


protected:
  bool setting (const  LHCb::Particle* part);
  bool setting (const  LHCb::ProtoParticle* proto);
  const LHCb::Particle*      m_particle;
  const LHCb::ProtoParticle* m_proto;
private:
};
#endif // PART2CALO_H

inline bool Part2Calo::inAcceptance(){
  if(m_det == DeCalorimeterLocation::Spd)  return m_proto->info(LHCb::ProtoParticle::InAccSpd,  0.);
  if(m_det == DeCalorimeterLocation::Prs)  return m_proto->info(LHCb::ProtoParticle::InAccPrs,  0.);
  if(m_det == DeCalorimeterLocation::Ecal) return m_proto->info(LHCb::ProtoParticle::InAccEcal, 0.);
  if(m_det == DeCalorimeterLocation::Hcal) return m_proto->info(LHCb::ProtoParticle::InAccHcal, 0.);
  return false;    
}
