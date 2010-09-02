// $Id: Track2Calo.h,v 1.1.1.1 2008-05-08 09:09:02 cattanem Exp $
#ifndef TRACK2CALO_H 
#define TRACK2CALO_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
//from LHCb
#include "CaloInterfaces/ITrack2Calo.h"            

// Forward declarations
class ITrackExtrapolator;
namespace LHCb 
{
  class Track;
}

/** @class Track2Calo Track2Calo.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2006-11-30
 */
class Track2Calo : public GaudiTool, virtual public ITrack2Calo {
public: 
  /// Standard constructor
  Track2Calo( const std::string& type, 
               const std::string& name,
               const IInterface* parent);
  
  virtual ~Track2Calo( ); ///< Destructor
  virtual StatusCode initialize();
  
  bool match(const  LHCb::Track* track,
                     std::string det = DeCalorimeterLocation::Ecal,
                     CaloPlane::Plane plane = CaloPlane::ShowerMax, 
                     double delta = 0.,
                     LHCb::ParticleID pid = LHCb::ParticleID(211)
                     );

  LHCb::State caloState(){return m_state;};
  LHCb::CaloCellID caloCellID(){return m_cell;};
  bool isValid(){return m_valid;};

  // Closest State
  LHCb::State closestState(LHCb::CaloHypo*    hypo   ,LHCb::ParticleID pid = LHCb::ParticleID(211));
  LHCb::State closestState(LHCb::CaloCluster* cluster,LHCb::ParticleID pid = LHCb::ParticleID(211));
  LHCb::State closestState(LHCb::CaloPosition calopos,LHCb::ParticleID pid = LHCb::ParticleID(211));
  LHCb::State closestState(LHCb::CaloCellID   cellID ,LHCb::ParticleID pid = LHCb::ParticleID(211));
  const LHCb::Track* track(){return m_track;};  
  
    

protected:
  LHCb::State closestState(double x, double y,LHCb::ParticleID pid = LHCb::ParticleID(211));
  LHCb::State caloState(CaloPlane::Plane plane= CaloPlane::ShowerMax ,
                        double delta =0 , 
                        LHCb::ParticleID pid= LHCb::ParticleID(211) );
  bool setting (const  LHCb::Track* track);
  bool                    m_status;
  const LHCb::Track*      m_track;
  LHCb::State             m_state;
  DeCalorimeter*          m_calo;
  std::string             m_det;
private:
  ITrackExtrapolator*  m_extrapolator;
  std::string m_extrapolatorType;
  double m_tolerance;
  LHCb::CaloCellID        m_cell;
  bool                    m_valid;
};
#endif // TRACK2CALO_H
