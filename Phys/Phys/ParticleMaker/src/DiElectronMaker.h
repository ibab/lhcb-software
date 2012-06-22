// $Id: $
#ifndef DIELECTRONMAKER_H 
#define DIELECTRONMAKER_H 1

// Include files
#include "ChargedParticleMakerBase.h"
#include "Kernel/IProtoParticleFilter.h"
#include "Kernel/IBremAdder.h"
#include "CaloUtils/ICaloElectron.h"
#include "CaloDet/DeCalorimeter.h"
#include "TrackInterfaces/ITrackSelector.h"

/** @class DiElectronMaker DiElectronMaker.h
 *  
 *  maker algorithm to produce dielectron with correct bremsstrahlung treatment (killing overlap)
 *
 *  @author Olivier Deschamps
 *  @date   2011-02-25
 */
class DiElectronMaker : public ChargedParticleMakerBase {
public: 
  /// Standard constructor
  DiElectronMaker( const std::string& name, ISvcLocator* pSvcLocator );  
  virtual ~DiElectronMaker( ); ///< Destructor
  virtual StatusCode makeParticles (LHCb::Particle::Vector & particles ) ; 
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
  
private:
  const ITrackSelector * m_trSel;
  const ITrackSelector * trSel()const{return m_trSel;}
  double ConfLevel(LHCb::Particle* electron);
  double ePID(LHCb::Particle* electron);
  double veloCnv(LHCb::Particle* p1, LHCb::Particle* p2);
  double veloCh(LHCb::Particle* p1);
  std::pair<double,double> getY(const LHCb::ProtoParticle* proto, double zcalo);
  Gaudi::XYZPoint getPoCA(LHCb::Particle* particle, const Gaudi::XYZPoint PVpos);
  void clear(LHCb::Particle::Vector & vector);
  
  IProtoParticleFilter* m_pFilter;
  IProtoParticleFilter* m_photFilter;
  ICaloElectron* m_caloElectron;
  DeCalorimeter* m_calo;  

  std::string m_toolType;
  std::string m_toolName;
  std::vector<std::string> m_eleinputs ;
  bool m_oppSign;
  bool m_symgamma;
  const LHCb::ParticleProperty* m_ePps;
  const LHCb::ParticleProperty* m_gPps;

  // cuts
  double m_ecl;
  double m_gcl;
  double m_mmax;
  double m_mmin;
  double m_eOpMax;
  double m_eOpMin;
  double m_aFactor;
  double m_ptmin;
  double m_eptmin;
  int m_method;
  int m_addopt;
  double m_deltaY;
  double m_deltaYmax;
  double m_ecalE;
  double m_vc;
  double m_eidmax;
  double m_eidmin;
  double m_gid;
  double m_zcalo;
  bool m_addBrem;
};
#endif // DIELECTRONMAKER_H
