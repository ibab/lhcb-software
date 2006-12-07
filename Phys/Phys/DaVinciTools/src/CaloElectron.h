// $Id: CaloElectron.h,v 1.2 2006-12-07 17:58:44 odescham Exp $
#ifndef CALOELECTRON_H 
#define CALOELECTRON_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/ICaloElectron.h"            // Interface
//from LHCb
//#include "Event/CaloHypo.h"
#include "Event/ProtoParticle.h"
//#include "Event/Particle.h"
#include "Event/Track.h"
//#include "Event/State.h"
#include "TrackInterfaces/ITrackExtrapolator.h"
//#include "CaloDet/DeCalorimeter.h"




/** @class CaloElectron CaloElectron.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class CaloElectron : public GaudiTool, virtual public ICaloElectron {
public: 
  /// Standard constructor
  CaloElectron( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~CaloElectron( ); ///< Destructor
  virtual StatusCode initialize();
  
  bool  setParticle(const  LHCb::Particle* particle);
  const LHCb::CaloHypo*    electron();
  const LHCb::CaloMomentum bremstrahlung();
  double e();
  double eOverP();
  LHCb::State caloState(CaloPlane::Plane plane = CaloPlane::ShowerMax , double deltaShower = 0. );
  LHCb::State closestState(std::string toWhat = "hypo");
  double showerZ(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo");
  double showerDepth(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo");
  

  

protected:
  bool setting (const  LHCb::Particle* particle);

private:
  std::string m_extrapolatorType;
  double m_tolerance;
  double m_zOffset;
  //
  ITrackExtrapolator*  m_extrapolator;
  //
  bool m_status;
  const LHCb::Particle*      m_particle;
  const LHCb::ProtoParticle* m_proto;
  const LHCb::Track*         m_track;
  const LHCb::CaloHypo*      m_electron;
  const LHCb::CaloHypo*      m_bremstrahlung;
  const LHCb::CaloPosition*  m_calopos;
  DeCalorimeter*       m_calo;
  std::string m_det;
  

};
#endif // CALOELECTRON_H
