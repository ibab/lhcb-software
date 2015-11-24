#ifndef RELINFOCONEISOLATION_H 
#define RELINFOCONEISOLATION_H 1

// Include files
#include "Kernel/IRelatedInfoTool.h"
#include "GaudiAlg/GaudiTool.h"
#include "Event/RelatedInfoMap.h"

/** @class RelInfoConeIsolation RelInfoConeIsolation.h
 *
 * \brief Fill track isolation for DecayTreeTuple.
 *    Open an eta-phi cone around the head, exclude all tracks that are in the decay descriptor (i.e. that belong to the decay you are looking for), and build variables with the remaining tracks
 *
 * Options:
 *
 * - MinConeSize: set the minimum cone size (default = 0.4)
 * - MaxConeSize: set the maximum cone size (default = 0.6)
 * - SizeStep: set the cone size step between two iterations (default = 0.1)
 * - FillCharged: flag to fill the charged cone (default = true)
 * - ExtraParticlesLocation: set the type of particles that are considered in the charged cone (default = "Phys/StdAllNoPIDsMuons/Particles")
 * - TrackType: set the type of tracks that are considered in the charged cone (default = 3)
 * - FillNeutral: flag to fill the neutral cone (default = true)
 * - ExtraPhotonsLocation: set the type of particles that are considered in the neutral cone (default = "Phys/StdLooseAllPhotons/Particles")
 * - FillAsymmetry: flag to fill the asymmetry variables (default = false)
 * - FillDeltas: flag to fill the delta variables (default = false)
 * - FillIsolation: flag to fill the isolation variables (default = true)
 * - FillMaxPt: flag to fill the momentum of the max-pT object in the cone (default = true)
 * - MaxPtParticleLocation: set the type of max-pT particles that are considered in the charged cone (default = "Phys/StdAllLooseMuons/Particles")
 * - FillComponents: flag to fill the components of all variables (default = false)
 *
 * If Verbose, all options are set to true
 *
 * Cone prefix:
 *
 * - head_cc: charged cone
 * - head_nc: neutral cone
 *
 * Variables (XX = cc or nc):
 *
 * - head_XX_mult : number of objects inside the cone
 * - head_XX_sPT : scalar-summed pT of the objects inside the cone
 * - head_XX_vPT : vector-summed pT of the objects inside the cone
 * - head_XX_P : x, y and z components of the cone momentum
 *
 * - head_XX_asy_P : momentum asymmetry between the head and the cone defined as (head_P - head_XX_P) / (head_P + head_XX_P)
 * - head_XX_asy_P : x, y, z and transverse components of the momentum asymmetry
 *
 * - head_XX_deltaEta : difference in eta between the head and the cone
 * - head_XX_deltaPhi : difference in phi between the head and the cone
 *
 * - head_XX_IT : transverse isolation of the head in the cone, defined as head_PT / (head_P + head_XX_P)_T
 * - head_IT : transverse isolation of the head in the charged and neutral cones, defined as head_PT / (head_P + head_cc_P + head_nc_P)_T
 *
 * - head_cc_maxPt_Q : charge of the max-pT object in the charged cone
 * - head_XX_maxPt_P : x, y, z (and e) components of the max-pT object momentum in the cone
 *
 * \sa DecayTreeTuple, TupleToolTrackIsolation
 *
 *  @author Simone Bifani, Michel De Cian
 *  @date   2015-02-19
 *
 */


class RelInfoConeIsolation : public GaudiTool, virtual public IRelatedInfoTool {

public: 

  /// Standard constructor
  RelInfoConeIsolation( const std::string &type, 
			  const std::string &name,
			  const IInterface *parent );

  virtual ~RelInfoConeIsolation(); ///< Destructor

  virtual StatusCode initialize(); ///< Algorithm initialization  

public:

  /// Loop over differnt conesizes and fill the variables into the tuple
  virtual StatusCode calculateRelatedInfo( const LHCb::Particle*
                                         , const LHCb::Particle*);

  virtual LHCb::RelatedInfoMap* getInfo(void);

private:

  std::vector<std::string> m_inputs;
  std::vector<std::string> m_variables;
  std::vector<short int> m_keys;
  LHCb::RelatedInfoMap m_map;

  double m_coneSize;

  std::string  m_extraParticlesLocation;
  std::string  m_maxPtParticleLocation;
  std::string  m_extraPhotonsLocation;

  int m_trackType;

  bool m_fillCharged;
  bool m_fillNeutral;
  bool m_fillMaxPt; 

  std::vector<const LHCb::Particle*> m_decayParticles;

  /// Save all the particles in the decay descriptor in a vector
  void saveDecayParticles( const LHCb::Particle *top );

  /// Calculate properties of the charged cone
  StatusCode ChargedCone( const LHCb::Particle  *seed,
			  const LHCb::Particles *parts,
			  const LHCb::Particles *maxPts,
			  const double rcut,
			  int &multiplicity,
			  double &scalarPt,
			  std::vector<double> &momentum,
			  std::vector<double> &maxPt );

  /// Calculate properties of the neutral cone
  StatusCode NeutralCone( const LHCb::Particle  *seed,
			  const LHCb::Particles *photons,
			  const double rcut,
			  int &multiplicity,
			  double &scalarPt,
			  std::vector<double> &momentum,
			  std::vector<double> &maxPt );

  /// Check if a track belongs to the decay
  bool isTrackInDecay( const LHCb::Track *track );

};

#endif // RelInfoConeISOLATION_H
