// $Id: CaloElectron.h,v 1.4 2007-06-25 20:33:34 odescham Exp $
#ifndef CALOELECTRON_H 
#define CALOELECTRON_H 1

// Include files
#include "CaloUtils/Part2Calo.h"
//from LHCb
#include "CaloUtils/ICaloElectron.h"            
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
//
#include "TrackInterfaces/ITrackExtrapolator.h"
#include "CaloUtils/CaloMomentum.h"



/** @class CaloElectron CaloElectron.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class CaloElectron : public Part2Calo, virtual public ICaloElectron {
public: 
  /// Standard constructor
  CaloElectron( const std::string& type, 
              const std::string& name,
              const IInterface* parent);

  virtual ~CaloElectron( ); ///< Destructor
  virtual StatusCode initialize();
  
  bool  set(const  LHCb::Particle* particle, 
            std::string det = DeCalorimeterLocation::Ecal,
            CaloPlane::Plane plane = CaloPlane::ShowerMax,
            double delta =0 );
  bool  set(const  LHCb::ProtoParticle* proto,
            std::string det = DeCalorimeterLocation::Ecal,
            CaloPlane::Plane plane = CaloPlane::ShowerMax,
            double delta =0 );

  LHCb::CaloHypo*    electron();
  LHCb::CaloHypo*    bremstrahlung();
  LHCb::CaloMomentum bremCaloMomentum();
  double ecalE();
  double eOverP();
  LHCb::State closestState(std::string toWhat = "hypo");
  double caloTrajectoryZ(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo");
  double caloTrajectoryL(CaloPlane::Plane refPlane = CaloPlane::ShowerMax ,std::string toWhat = "hypo");



protected:
  bool caloSetting ();
private:
  LHCb::CaloHypo*            m_electron;
  LHCb::CaloHypo*            m_bremstrahlung;
  const LHCb::CaloPosition*  m_calopos;
  double m_zOffset;
};
#endif // CALOELECTRON_H
