#ifndef RELINFOCONEVARIABLES_H
#define RELINFOCONEVARIABLES_H 1

// Include files
#include "Kernel/IRelatedInfoTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "Event/RelatedInfoMap.h"

/** @class ConeVariables ConeVariables.h
 *
 * \brief Calculate track isolation.
 *    Open up a cone around head, exclude all tracks
 *    that are in the decay descriptor
 *    (i.e. that belong to the decay you are looking for),
 *    build the variables with the remaining tracks.
 *
 * - m_mult : Number of tracks inside cone.
 * - m_cp  : Summed p inside cone
 * - m_cpt : Summed pt inside cone
 * - m_cpx : Summed px inside cone
 * - m_cpy : Summed py inside cone
 * - m_cpz : Summed pz inside cone
 *
 * If Verbose, or other flags are set:
 *
 * Asymmetry variables
 * - m_pasy  : (m_P  - m_cp) /(m_P  + m_cp)
 * - m_ptasy : (m_PT - m_cpt)/(m_PT + m_cpt)
 * - m_pxasy : (m_Px - m_cpx)/(m_Px + m_cpx)
 * - m_pyasy : (m_Py - m_cpy)/(m_Py + m_cpy)
 * - m_pzasy : (m_Pz - m_cpz)/(m_Pz + m_cpz)
 * Delta angle variables
 * - m_DeltaEta : Difference in eta between summed tracks and head
 * - m_DeltaPhi : Difference in phi between summed tracks and head
 *
 *
 * Options:
 *
 * - ConeAngle: Set the maximum deltaR of the cone (default = 1.0), in radians
 * - TrackType: Set the type of tracks which are considered inside the cone (default = 3)
 *
 *  Converted from TupleTool by A. Poluektov 19/02/2012
 *
 *  @author Michel De Cian
 *  @date   2009-08-04
 *
 */

class RelInfoConeVariables : public GaudiTool, virtual public IRelatedInfoTool
{

public:

  /// Standard constructor
  RelInfoConeVariables( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  virtual ~RelInfoConeVariables( ); ///< Destructor

  virtual StatusCode initialize(void);

public:

  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*
                                         , const LHCb::Particle*);

  virtual LHCb::RelatedInfoMap* getInfo(void);

private:

  /// Save all particles in your decay descriptor in a vector
  void saveDecayParticles( const LHCb::Particle *top);

  /// Calculate properties of your remaining tracks inside the cone
  std::pair< std::vector<double>, int> ConeP(const LHCb::Particle *part,
                                             const std::vector<const LHCb::Track*> *tracks,
                                             const double rcut);

  /// Check if your track belongs to your decay or not
  bool isTrackInDecay(const LHCb::Track* track);

private:

  double m_coneAngle;
  int m_trackType;
  std::string m_tracksLocation;
  std::vector<std::string> m_inputs;
  std::vector<std::string> m_variables;
  std::vector<short int> m_keys;

  int m_mult;
  double m_px;
  double m_py;
  double m_pz;
  double m_pt;
  double m_p;
  double m_ptasy;
  double m_pxasy;
  double m_pyasy;
  double m_pzasy;
  double m_pasy;
  double m_deltaEta;
  double m_deltaPhi;

  std::vector<const LHCb::Particle*> m_decayParticles;

  LHCb::RelatedInfoMap m_map;

  std::vector<const LHCb::Track*> m_tracksStorage;

};

#endif // CONEVARIABLES_H
