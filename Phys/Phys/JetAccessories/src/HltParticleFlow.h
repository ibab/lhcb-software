#ifndef HLTPARTICLEFLOW_H
#define HLTPARTICLEFLOW_H 1

// STL.
#include <unordered_set>
#include <unordered_map>

// Gaudi.
#include "GaudiAlg/GaudiHistoAlg.h"

// Relations.
#include "CaloUtils/Calo2Track.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Relations/IRelationWeighted2D.h"
#include "Kernel/Particle2Vertex.h"

// Event.
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Event/ProtoParticle.h"
#include "Event/Track.h"
#include "Event/CaloCluster.h"
#include "Kernel/ParticleProperty.h"

// Tools.
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/IParticle2State.h"
#include "Kernel/IRelatedPVFinder.h"

// LoKi.
#include "LoKi/PhysExtract.h"
#include "LoKi/ParticleCuts.h"

// ROOT.
#include "TF1.h"

using namespace std;
using namespace LHCb;

/**
 * Particle flow (PF) algorithm class for use in the HLT and offline.
 *
 * PF takes inputs and creates a common output of Particles, ensuring
 * that no two output Particles share associated Tracks or
 * CaloClusters. When objects with associated Tracks are added to the
 * output, the CaloClusters matched to the Tracks are marked as used
 * but not associated to the object. Consequently, an input of two
 * unique Tracks sharing a matched CaloCluster results in both Tracks
 * added to the output. An input of a Track with a matched CaloCluster
 * and a neutral Particle associated to the same CaloCluster would
 * result in only the Track being added to the output. Reversing the
 * order would result in both the neutral Particle and Track being
 * added to the output.
 *
 * The PF algorithm itself does not filter input; the passed inputs
 * must already be filtered. Six classes of inputs can be passed with
 * class names 'Particle', 'ProtoParticle', 'Track', 'CaloCluster',
 * 'RecVertex', and 'IClusTrTable2D'. Inputs are set via the 'Inputs'
 * property which must be of the form [[<class name>, <object type>,
 * <location>], ...] where all three arguments for each input are
 * strings. Inputs of the first four classes are used to build the PF
 * output. With these four classes, if the object type is 'ban', the
 * object is not added to the PF output, but all associated Tracks and
 * CaloClusters to each object are marked as used.
 *
 * For inputs of class 'Particle' the object type can either be
 * 'particle', where the Particle itself is added to the output, or
 * 'daughters', where only the final basic daughters of the Particle
 * are added to the output. For basic particles with no daughters,
 * both object types produce the same behaviour.
 *
 * For inputs of class 'ProtoParticle' the object type should be the
 * particle name to assign these ProtoParticles, where charge
 * conjugation is automatically handled. The ProtoParticle object type
 * can also be specified as 'best' and an attempt will be made to use
 * associated ProbNN variables to determine the best hypothesis. The
 * ProbNNs are specified by the 'ProBestNames', 'ProBestKeys', and
 * 'ProBestMins' properties which are lists of equal length containing
 * strings, ints, and doubles, respectively. If the object type is
 * 'best' for a neutral ProtoParticle, the particle type of photon is
 * always used.
 *
 * For inputs of class 'Track' or 'CaloCluster', the object type
 * should be the particle name to assign these objects, where again
 * charge conjugation is automatically handeled.
 *
 * Only one input of class 'RecVertex' should be supplied and should
 * be given object type 'vertex'. This input is used when associating
 * Particles to primary vertices. If such a location is not supplied,
 * no association will be performed.
 *
 * The input class 'IClusTrTable2D' is used for matching CaloClusters
 * (ECAL or HCAL) to Tracks. Consequently, the corresponding object
 * type should be either 'ecal' or 'hcal'. Only one of each object
 * type should be supplied as input. If not supplied, then matching
 * for that calorimeter type will not be performed.
 *
 * The PF algorithm can also perform neutral energy recovery if the
 * property "SprRecover" is set to true. Here, SuperClusters of Tracks
 * sharing matched CaloClusters are built, and the expected energy
 * from the Tracks is subtracted from the total CaloCluster
 * energy. Particles containing this remainder energy are then added
 * to the PF output. See LHCb-INT-2013-034 for more details on charged
 * particle responses in the calorimeters.
 *
 * @class  HltParticleFlow
 * @file   HltParticleFlow.h
 * @author Victor Coco, Cedric Potterat, and Philip Ilten
 * @date   2016-02-02
 */
class HltParticleFlow : public GaudiHistoAlg {
public:
  /// Constructor.
  HltParticleFlow(const string &name, ISvcLocator *svc);
  virtual StatusCode initialize(); ///< Initialize.
  virtual StatusCode execute   (); ///< Execute.
  virtual StatusCode finalize  (); ///< Finalize.

private:
  /**
   * Input structure class for PF. 
   *
   * Used to parse the input lists passed via #m_inLocs.
   */
  class Input {
  public:
    /// Constructor.
    Input(const unsigned int *idx = 0, const vector<string> *prp = 0,
	  const IParticlePropertySvc *svc = 0);
    /// Print the input configuration to #out.
    inline void print(MsgStream &msg, bool idx = true);
    
    // Members.
    string warn;        ///< Any warnings that may have been generated.
    string name;        ///< The class name of objects from this input.
    string type;        ///< The object type for this input.
    string loc;         ///< The TES location for this input.
    bool   valid;       ///< Flag if input is valid.
    bool   ban;         ///< Flag if objects from this input should be banned.
    bool   dtrs;        ///< Flag if daughters from Particle should be written.
    bool   ecal;        ///< Flag if ECAL/HCAL map for IClusTrTable2D.
    unsigned int index; ///< Index for this input.
    int    pid;         ///< PDG ID to assign neutral or positive Particles.
    double m;           ///< Mass to assign Particles.
  };

  /**
   * Neutral energy recovery cluster class for PF.
   *
   * This class is meant for internal use by PF. The primary member is
   * #vec which stores the running energy weighted position and energy
   * of the SuperCluster. This is the sum of all CaloClusters in the
   * SuperCluster less the expected energy deposition from all
   * Particles in the SuperCluster.
   */
  class SuperCluster {
  public:
    /// Constructor.
    SuperCluster();
   
    // Members.
    bool                used; ///< Flag if used.
    bool          ecal, hcal; ///< Flags whether contains ECAL or HCAL energy.
    Gaudi::LorentzVector vec; ///< Energy weighted position and energy vector.
    ///< Pointer to updated version of this SuperCluster.
    shared_ptr<SuperCluster> spr;
  };
  /// Definition of shared pointer for SuperCluster.
  typedef shared_ptr<SuperCluster> SuperClusterPtr;

  // Add an object to the PF output Particles.
  /// Add Objects from #in to the PF output Particles.
  template<class Object> bool add(const vector<Input> *ins,
				  vector<Input>::iterator &in,
				  const unsigned int &idx) {
    if (!ins || in == ins->end() || idx != in->index) return false;
    m_inNow = &(*in);
    if (exist< typename Gaudi::Range_< vector<const Object*> > >(in->loc)) {
      Gaudi::Range_< vector<const Object*> > objs = 
	get< typename Gaudi::Range_< vector<const Object*> > >(in->loc);
      for (typename Gaudi::Range_< vector<const Object*> >::iterator obj = 
	     objs.begin(); obj != objs.end(); ++obj) add(*obj, *in);
    } else warning() << "Could not retrieve " << in->name << "s from " 
		     << in->loc << endmsg;
    ++in;
    return true;
  }
  /// Add a Particle to the PF output Particles.
  void add(const Particle *prt, const Input &in);
  /// Add a ProtoParticle to the PF output Particles.
  void add(const ProtoParticle *pro, const Input &in);
  /// Add a Track to the PF output Particles.
  void add(const Track *trk, const Input &in);
  /// Add a CaloCluster to the PF output Particles.
  void add(const CaloCluster *cal, const Input &in);

  // Create a Particle from an object to use in the PF output Particles.
  /// Create a Particle from a Particle.
  Particle *create(const Particle *prt);
  /// Create a Particle from a ProtoParticle.
  Particle *create(const ProtoParticle *pro, const int &id,
		   const double &m);
  /// Create a Particle from a Track.
  Particle *create(const Track *trk, const int &id, const double &m);
  /// Create a Particle from a CaloCluster.
  Particle *create(const CaloCluster *cal, const int &id,
		   const double &m);
  /// Create a Particle from a SuperCluster.
  Particle *create(const SuperClusterPtr &spr);

  // Mark the Tracks and/or CaloClusters of an object as used.
  /// Mark the Tracks and CaloClusters of a ProtoParticle as used. If
  /// #prt is passed, neutral recovery is performed.
  void use(const ProtoParticle *pro, const Particle *prt);
  /// Mark a Track and its matched CaloClusters as used. If #prt is
  /// passed, neutral recovery is performed.
  void use(const Track *trk, const Particle *prt);
  /// Match CaloClusters to a Track, for a given calorimeter with
  /// relations #trksCals and matching criteria #chi2Max. If #best,
  /// only the first CaloCluster is used. Once matched, the
  /// CaloClusters are marked as used. If #spr and #vec are passed,
  /// neutral recovery is performed.
  void use(const Track *trk, Gaudi::LorentzVector &vec, SuperClusterPtr &spr,
	   bool *calo, const Calo2Track::ITrClusTable *trksCals,
	   const bool &best, const double &chi2Max);
  /// Mark a CaloCluster as used. If #spr is passed neutral recovery
  /// is performed.
  void use(const CaloCluster *cal, SuperClusterPtr spr);
  
  // Check if the Tracks and/or CaloClusters of an object are used.
  inline bool used(const ProtoParticle *pro);
  inline bool used(const Track *trk);
  inline bool used(const CaloCluster *cal);

  // Additional methods.
  /// Sum a Gaudi::LorentzVector and a CaloCluster.
  static inline void sum(Gaudi::LorentzVector &vec, const CaloCluster *cal);
  /// Relate a Particle with a Track to a RecVertex.
  inline void relate(const Particle *prt);
  /// Retrieve an Object #obj given Input #in.
  template<class Object> bool retrieve(Object &obj, const Input &in) {
    if (exist<Object>(in.loc)) {obj = get<Object>(in.loc); return true;}
    else {obj = 0; warning() << "Could not retrieve " << in.name << "s from " 
			     << in.loc << endmsg; return false;}
  }
  
  // Input/output property members.
  /// PF inputs of the form [<class name>, <object type>, <location>].
  vector< vector<string> > m_inLocs;
  /// PF output location prefix; objects will be placed in
  /// <Output>/Particles, <Output>/ProtoParticles, etc.
  string m_outLoc;

  // ProtoParticle property members.
  /// Particle names to use when assigning the 'best' PID to ProtoParticles.
  vector<string> m_proBestNames;
  /// ExtraInfo keys to check when assigning the 'best' PID to ProtoParticles.
  vector<int> m_proBestKeys;
  /// Minimum required ExtraInfo values when assigning the 'best' PID
  /// to ProtoParticles.
  vector<double> m_proBestMins;

  // Track property members.
  /// Momentum to assign VELO Tracks.
  double m_trkVlP;
  /// Momentum to assign long Tracks with large curvature error.
  double m_trkLnP;
  /// Momentum to assign upstream Tracks with large curvature error.
  double m_trkUpP;
  /// Momentum to assign downstream Tracks with large curvature error.
  double m_trkDnP;
  /// Fix momentum to long Tracks with curvature error above this (-1 is +inf).
  double m_trkLnErrMax;
  /// Fix momentum for upstream Tracks with curvature error above this
  /// (-1 is +inf).
  double m_trkUpErrMax;
  /// Fix momentum for downstream Tracks with curvature error above
  /// this (-1 is +inf).
  double m_trkDnErrMax;

  // Calorimeter property members.
  /// Flag to perform neutral recovery using expected calorimeter response.
  bool m_sprRecover;
  /// If (expected track calorimeter energy) / (calorimeter energy) is
  /// greater than this when building a SuperCluster, set the expected
  /// energy to zero.
  double m_sprRatioMax;
  /// ID to assign SuperClusters from neutral recovery.
  int m_sprPid;
  /// Mass to assign SuperClusters from neutral recovery.
  int m_sprM;
  /// Flag to only match the best ECAL CaloCluster for a Track.
  bool m_ecalBest; 
  /// Flag to only match the best HCAL CaloCluster for a Track.
  bool m_hcalBest;
  /// Maximum chi2 to match an ECAL CaloCluster with a Track (-1 is +inf).
  double m_ecalChi2Max;
  /// Maximum chi2 to match an HCAL CaloCluster with a Track (-1 is +inf).
  double m_hcalChi2Max;
  /// Function string for the ECAL energy electron response.
  string m_ecalElcEStr;
  /// Function string for the ECAL energy charged hadron response.
  string m_ecalHadEStr;
  /// Function string for the HCAL energy electron response.
  string m_hcalElcEStr;
  /// Function string for the HCAL energy charged hadron response.
  string m_hcalHadEStr;
  /// Function string for the ECAL + HCAL energy electron response.
  string m_ecalHcalElcEStr;
  /// Function string for the ECAL + HCAL energy charged hadron response.
  string m_ecalHcalHadEStr;

  // Tool members.
  IParticlePropertySvc *m_prtSvc; ///< Particle property service tool.
  IParticle2State      *m_prtSta; ///< Particle to State conversion tool.
  IRelatedPVFinder     *m_prtVrt; ///< Particle to RecVertex relation tool.

  // Additional members (not properties).
  VertexBase::ConstVector  m_vrts;     ///< Input RecVertex container.
  Particles               *m_prts;     ///< Output Particle container.
  ProtoParticles          *m_pros;     ///< Output ProtoParticle container.
  CaloHypos               *m_hyps;     ///< Output CaloHypo container.
  CaloClusters            *m_cals;     ///< Output CaloCluster container.
  Particle2Vertex::WTable *m_prtsVrts; ///< Output Particle to RecVertex table.
  vector<double> m_proBestMs;   ///< Masses for "best" PID ProtoParticle.
  vector<int>    m_proBestPids; ///< PIDs for "best" PID ProtoParticle.
  vector<Input> m_inPrts; ///< Vector of Particle inputs.
  vector<Input> m_inPros; ///< Vector of ProtoParticle inputs.
  vector<Input> m_inTrks; ///< Vector of Track inputs.
  vector<Input> m_inCals; ///< Vector of CaloCluster inputs.
  Input  m_inVrts;     ///< RecVertex input.
  Input  m_inEcalTrks; ///< ECAL to Track map input.
  Input  m_inHcalTrks; ///< HCAL to Track map input.
  Input *m_inNow;      ///< Pointer to the current input.
  const Calo2Track::IClusTrTable2D *m_ecalTrks; ///< ECAL to Track map.
  const Calo2Track::ITrClusTable   *m_trksEcal; ///< Track to ECAL map.
  const Calo2Track::IClusTrTable2D *m_hcalTrks; ///< HCAL to Track map.
  const Calo2Track::ITrClusTable   *m_trksHcal; ///< Track to HCAL map.
  TF1 m_ecalElcE;     ///< ECAL energy electron response.
  TF1 m_ecalHadE;     ///< ECAL energy charged hadron response.
  TF1 m_hcalElcE;     ///< HCAL energy electron response.
  TF1 m_hcalHadE;     ///< HCAL energy charged hadron response.
  TF1 m_ecalHcalElcE; ///< ECAL + HCAL energy electron response.
  TF1 m_ecalHcalHadE; ///< ECAL + HCAL energy charged hadron response.
  /// Set of used Tracks.
  unordered_set<const Track*> m_trks;
  /// Map of used CaloClusters and associated SuperClusters.
  unordered_map<const CaloCluster*, shared_ptr<SuperCluster> > m_calsSprs;
};

#endif // HLTPARTICLEFLOW_H
