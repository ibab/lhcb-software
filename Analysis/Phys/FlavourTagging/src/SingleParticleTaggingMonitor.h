// $Id: SingleParticleTaggingMonitor.h,v 1.1 2002-11-20 08:24:46 odie Exp $
#ifndef SINGLEPARTICLETAGGINGMONITOR_H 
#define SINGLEPARTICLETAGGINGMONITOR_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from DaVinci
#include "DaVinciTools/DVAlgorithm.h"
#include "DaVinciAssociators/Particle2MCWeightedAsct.h"

class MCParticle;
class IDebugTool;
class ParticleProperty;
class ITrReconstructible;

/** @class SingleParticleTaggingMonitor SingleParticleTaggingMonitor.h
 *  
 *
 *  @author Olivier Dormond
 *  @date   2002-07-15
 */
class SingleParticleTaggingMonitor : public DVAlgorithm {
public:
  /// Standard constructor
  SingleParticleTaggingMonitor( const std::string& name,
                                ISvcLocator* pSvcLocator );

  virtual ~SingleParticleTaggingMonitor( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  /// Vector of locations of the tags to monitor
  std::vector<std::string> m_tags_locations; 

  /// Statistics accumulator
  unsigned int m_n_reco_B;
  unsigned int m_n_mc_B;
  unsigned int m_n_no_comp;
  unsigned int m_n_too_much_comps;
  unsigned int m_n_no_lepton;
  unsigned int m_n_too_much_leptons;
  unsigned int m_n_no_c_to_k;
  unsigned int m_n_too_much_c_to_ks;
  unsigned int m_n_velo_reco;
  unsigned int m_n_tt1_reco;
  unsigned int m_n_seed_reco;
  unsigned int m_n_velo_and_seed_reco;
  unsigned int m_n_velo_unreco;
  unsigned int m_n_tt1_unreco;
  unsigned int m_n_seed_unreco;
  unsigned int m_n_velo_and_seed_unreco;
  unsigned int m_n_not_reconstructed;
  unsigned int m_n_opposite_charge;
  unsigned int m_n_wrong_charge;
  unsigned int m_n_taggable;
  unsigned int m_n_comp_oscillated;
  unsigned int m_n_wrong_tagger;
  
  bool m_beAFilter;
  std::string m_partName;    ///< Name of the particle used for tagging
  ParticleProperty *m_pp;    ///< Property of the above particle.
  std::string m_nameMCAsct;  ///< Name of tool for Part to MCPart Association
  Particle2MCWeightedAsct::IAsct* m_pAsctLinks; ///< Pointer to asso using links
  IDebugTool *m_debug;
  ITrReconstructible *m_trRecoQ;
};
#endif // SINGLEPARTICLETAGGINGMONITOR_H
