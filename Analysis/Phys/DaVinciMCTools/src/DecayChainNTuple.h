#ifndef DECAYCHAINNTUPLE_H
#define DECAYCHAINNTUPLE_H 1

// Always define MCCheck
#define MCCheck 1

// Include files
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "Kernel/DVAlgorithm.h"

#include "Event/RichPID.h"

class ParticleProperty;
class IDecayFinder;
class INTuple;
class IParticlePropertySvc;
class IGeomDispCalculator;
// class ILifetimeFitter;
class IOnOffline;

#ifdef MCCheck
class IMCDecayFinder;
#include "DaVinciAssociators/Particle2MCLinker.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeighted.h"
#include "Event/CaloCluster.h"
#include "Kernel/IVisPrimVertTool.h"
#endif

/** @class DecayChainNTuple DecayChainNTuple.h
 * 
 *  Creates a NTuple for any generic decay chain (and checks association) for particles made from offline or online objects
 * 
 *  Fills the corresponding true decay variables if required
 *  -> the number of flagged particles for Decay and MCDecay must be the same!!!
 * 
 *  Remarks :
 *
 *  - the (MC) decay to look at can be set in the (MC) Decay property and follows the syntax of the (MC)DecayFinder
 *  - the head of the decay is always booked whereas for the children a flag ^ is needed
 *  - the decay chain must be written without cc for the labeling to work (otherwise the daughters get mixed)
 *  - works for different decays, as long as there are the same number of decay produts:
 *  - output file must be read with Root
 *  - the tree variables have labels '_lab0' for the mother and '_lab1' ... for the daughters (e.g. mass_lab0)
 *  - the labels are set starting from the rightmost subdecay : 
 *    the subhead first and then the daughters with the rightmost daughter first
 *
 *  Example 1 : 
 *  in {B0 -> ^K+ ^pi-, B~0 -> ^K- ^pi+ } the first daughter will be the pi:
 *     - ParticleName (mother) B0 or B~0
 *     - Label number = 1 , ParticleName pi- or pi+
 *     - Label number = 2 , ParticleName K+ or K-
 *
 *  Example 2 : 
 *  -> For B0 -> (^D~0 -> ^K+ ^pi-) (^K*(892)0 -> ^K+ ^pi-) we'll have:
 *    - ParticleName (mother) B0
 *    - Label number = 1 , ParticleName K*(892)0
 *    - Label number = 2 , ParticleName pi-
 *    - Label number = 3 , ParticleName K+
 *    - Label number = 4 , ParticleName D~0
 *    - Label number = 5 , ParticleName pi-
 *    - Label number = 6 , ParticleName K+
 *
 *  Warning: some selections save both the B and the B~, whereas 
 *           other selections always save the B
 *          -> the decay string should be written with care
 *
 *  The primaries are retrieved with the OnOfflineTool tool, default is offline
 *  -> must set PVLocation property of OnOfflineTool tool to change it
 *  
 *  Possibility to use the TrgDispCalculator tool for online particles, automatically set through the OnOfflineTool tool
 *  -> uses the approximated HLT covariance matrix for computation of IP, vertex separation, etc ...
 * 
 *  Gammas are not re-evaluated at the vertex : 
 *  -> vertex fitting moves gammas to a reference vertex but by changing the TES particles
 *
 *  Warning : in the MCParticle format only the px, py and pz are correct
 *  -> to retrieve the true generated mass for composites, need to go HepMC
 *  -> the true masses stored are those of HepMC when running on signal
 * 
 *  Warning : if you require trigger information, tagging, online calo, etc ..., make sure you run the necessary code
 * 
 *  Warning : for the composite association can be associated for 'wrong' combinations like
 *            B -> A(K1 K2) C(K3 K4) or B -> A(K1 K3) C(K2 K4)
 *            --> check subdecays in ntuple
 *
 *  Warning : when using no PID, use option 'IgnorePID' of CompositeParticle2MCLinks for the association
 *  
 *  FIXME: 
 *  - association of online gammas requires offline CaloClusters for now
 *
 *  @author Luis Fernandez
 *  @date   2004-08-01
 */
class DecayChainNTuple : public DVAlgorithm {
public: 
  /// Standard constructor
  DecayChainNTuple( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~DecayChainNTuple( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  // Properties
  std::string m_Decay; // Decay for which the ntuple will be created 
  std::string m_ntupleName; // Name of the TDirectory
  bool m_requireTrigger;
  bool m_requireTagging;
  bool m_useRichOnlinePID;
  bool m_useOnlineCalo; // Protection for MC -> Part online association
  std::string m_richOnlinePIDLocation;
  std::string m_geomToolName; // Name of Geometrical Tool

  // Flag to book the NTuple only once
  bool m_bookedNTuple;

  std::string m_PVContainer;
  IOnOffline* m_OnOfflineTool;

  long m_event, m_run;

#ifdef MCCheck
  std::string m_MCDecay; // MC Decay 
  bool m_FillMCDecay;
#endif

  //-----------------------------------------------------------------------------
  // Tools
  //-----------------------------------------------------------------------------
  
  // Reference to ParticlePropertySvc 
  IParticlePropertySvc* m_ppSvc; 
  // Reference to DecayFinder
  IDecayFinder* m_pDKFinder;
  // Reference to GeomDispCalculator
  IGeomDispCalculator* m_IPTool;
  // Reference to LifetimeFitter
  // ILifetimeFitter *m_pLifetimeFitter;
  // Check if the MCPV is visible
  IVisPrimVertTool* m_VISPrimVertTool;


#ifdef MCCheck
  IBackgroundCategory *m_bkgCategory;

  IMCDecayFinder* m_pMCDKFinder;
  // Composite associator
  Particle2MCLinker* m_pCompositeAsct;
  // Ref. to link associator
  //Particle2MCLinks* m_pAsctLinks;
  // std::string m_inputComposite;
  std::vector<std::string>  m_inputComposite;
  
  int m_gammaID; // gamma particle ID
  typedef IRelationWeighted<LHCb::CaloCluster, LHCb::MCParticle, float> IAsctCl2MCP;
  typedef IAsctCl2MCP::DirectType DirectType;
  typedef IAsctCl2MCP::InverseType InverseType;
  const IAsctCl2MCP* m_pAsctCl2MCP;

#endif

  //=============================================================================
  // Reconstructed decay
  //=============================================================================

  // Book the ntuple
  StatusCode BookNTuple(LHCb::Particle::Vector);
  // Write the ntuple
  StatusCode WriteNTuple(LHCb::Particle::Vector);

  // Get all the primaries
  StatusCode getPV(LHCb::RecVertex::Vector&);

#ifdef MCCheck
  bool isSignal(const LHCb::MCParticle*, LHCb::MCParticle::ConstVector);//const std::vector<LHCb::MCParticle*>& );
  bool updateMCMomentum(LHCb::MCParticle*);
  
  // Write the true part of the decay
  StatusCode WriteMCNTuple(LHCb::MCParticle::Vector);
#endif

  // Trigger
  std::string m_pathTrg;
  std::string m_L1ScoreLocation;
  std::string m_HltScoreLocation;
  std::string m_CaloClustersPath;
  std::string m_TrgCaloClustersPath;

  // Tagging
  std::string m_TagLocation;

  // NTuple global variables
  NTuple::Item<long> m_eventNumber,m_runNumber;
  NTuple::Item<long> m_nCand;
  NTuple::Item<long> m_nRecoPV;
  // All reconstructed primary vertices coordinates
  NTuple::Array<float> m_RecoPVx;
  NTuple::Array<float> m_RecoPVy;
  NTuple::Array<float> m_RecoPVz;
  NTuple::Item<long> m_L0Decision;
  NTuple::Item<long> m_L1Decision;
  NTuple::Item<long> m_L1Gen;
  NTuple::Item<long> m_L1SiMu;
  NTuple::Item<long> m_L1DiMu;
  NTuple::Item<long> m_L1JPsi;
  NTuple::Item<long> m_L1Elec;
  NTuple::Item<long> m_L1Phot;
  NTuple::Item<long> m_HLTDecision;
  NTuple::Item<long> m_HLTGen;
  NTuple::Item<long> m_HLTIncB;
  NTuple::Item<long> m_HLTDiMu;
  NTuple::Item<long> m_HLTDstar;
  NTuple::Item<long> m_HLTExB;
  NTuple::Item<long> m_HLTInc;

  // The tags
  NTuple::Item<long> m_nTags;
  NTuple::Array<long> m_TagDecision;
  NTuple::Array<long> m_TagCat;
  NTuple::Array<double> m_TagW;

  /*
  // Beg of more detailed tagging information
  NTuple::Array<long> m_nTaggers; // total number of all taggers for a given tag
  NTuple::Array<long> m_nTaggerParts; // total number of all tagger particles for a given tag
  // All taggers information
  // Matrices indexed by nTags and number of taggers with a maximum of 20 taggers for a given tag
  // Ex: TaggerType[0][1] = first tag, 2nd tagger type
  NTuple::Matrix<long> m_TaggerType; 
  NTuple::Matrix<float> m_TaggerDecision;
  NTuple::Matrix<float> m_TaggerW;
  // the Particles used to build the Taggers
  // Matrices indexed by nTags and number of tagger particles with a maximum of 20 tagger particles for a given tag
  // Ex: IDTaggerPart[2][3] = 3rd tag, 4th tagger particle ID
  NTuple::Matrix<long> m_IDTaggerPart;
  NTuple::Matrix<float> m_pTaggerPart;
  NTuple::Matrix<float> m_ptTaggerPart;
  NTuple::Matrix<float> m_sIPSTaggerPart;
  // End of more detailed tagging information
  */

#ifdef MCCheck
  NTuple::Item<long> m_nMCPV;
  // Is the MCPV visible?
  NTuple::Array<long> m_bkgCat;
#endif

  //-----------------------------------------------------------------------------
  // HandleNTuple
  //-----------------------------------------------------------------------------

  class HandleNTuple{
  public:

    HandleNTuple(NTuplePtr& nt, unsigned int& number, 
                 // ILifetimeFitter *lifetimefitter,
                 IGeomDispCalculator* iptool,IVisPrimVertTool* visPrimVertTool);

#ifndef MCCheck
    void FillNTuple(const LHCb::Particle& part, LHCb::RecVertex::Vector& pvs, LHCb::RichPIDs* globalPIDs);
#endif

#ifdef MCCheck
    void FillNTuple(const LHCb::Particle& part, LHCb::RecVertex::Vector& pvs, bool& isSig, 
                    const LHCb::MCParticle* mclink, LHCb::RichPIDs* globalPIDs);
    //void FillNTuple(int ciao);
    // 
    void FillMCNTuple(LHCb::MCParticle& mcpart, const Gaudi::XYZPoint& MCPVPosition, bool& isReco);
#endif

    void clean(){m_n=0;}

#ifdef MCCheck
    void mcclean(){m_mcn=0;}
#endif
    
  private:
    // Tools
    IGeomDispCalculator* m_iptool;
    IVisPrimVertTool* m_visPrimVertTool;
    // ILifetimeFitter *m_lifetimefitter;

    // NTuple variables

    // m_n is the number of decays found per event; serves as index for arrays
    NTuple::Item<long>  m_n;
    NTuple::Array<long> m_ID;

#ifdef MCCheck
    NTuple::Array<long> m_trueID;
#endif

    // Delta log-likelihood values (DLL)
    NTuple::Array<float> m_dllepi; // dll(e-pi)
    NTuple::Array<float> m_dllmupi; // dll(mu-pi)
    NTuple::Array<float> m_dllkpi; // dll(K-pi)
    NTuple::Array<float> m_dllppi; // dll(p-pi)

    // Global Rich PIDs
    NTuple::Array<float> m_globdllpi; // dll pion
    NTuple::Array<float> m_globdllk; // dll kaon

    // Track information
    NTuple::Array<float> m_trchitwo;
    NTuple::Array<float> m_trDoF;
    NTuple::Array<float> m_trtype;

    // State vector of the track (x,y,tx,ty,Q/P), tx = dx/dz, ty = dy/dz
    NTuple::Array<float> m_stateX;
    NTuple::Array<float> m_stateXErr;
    NTuple::Array<float> m_stateY;
    NTuple::Array<float> m_stateYErr;
    NTuple::Array<float> m_stateTX;
    NTuple::Array<float> m_stateTXErr;
    NTuple::Array<float> m_stateTY;
    NTuple::Array<float> m_stateTYErr;
    NTuple::Array<float> m_stateQoverP;
    NTuple::Array<float> m_stateQoverPErr;

    // Momentum, mass, ...
    NTuple::Array<float> m_p;
    NTuple::Array<float> m_pt;
    NTuple::Array<float> m_mass;

    // 3-momentum
    NTuple::Array<float> m_px, m_py, m_pz;
    NTuple::Array<float> m_pxvar, m_pyvar, m_pzvar;

    // Point at which the momentum is given in LHCb reference frame
    NTuple::Array<float> m_onTrx, m_onTry, m_onTrz;
    NTuple::Array<float> m_onTrxvar, m_onTryvar, m_onTrzvar;

    // Decay vtx if any; otherwise set to -100000
    NTuple::Array<float> m_vchitwo;
    NTuple::Array<float> m_vnDoF;
    NTuple::Array<float> m_vx;
    NTuple::Array<float> m_vy;
    NTuple::Array<float> m_vz;
    NTuple::Array<float> m_vxvar;
    NTuple::Array<float> m_vyvar;
    NTuple::Array<float> m_vzvar;

    // Primary vertices

    // PV w.r.t. the particle has the smallest IPS    
    NTuple::Array<float> m_chitwosIPSPV;
    NTuple::Array<float> m_sIPSPVx;
    NTuple::Array<float> m_sIPSPVy;
    NTuple::Array<float> m_sIPSPVz;
    NTuple::Array<float> m_sIPSPVxvar;
    NTuple::Array<float> m_sIPSPVyvar;
    NTuple::Array<float> m_sIPSPVzvar;
    // Minimum (smallest) IP, IPe and IPS
    NTuple::Array<float> m_sIP;
    // NTuple::Array<float> m_sIPe;
    NTuple::Array<float> m_sIPS;

    // PV w.r.t. the particle has the largest IPS    
    NTuple::Array<float> m_chitwolIPSPV;
    NTuple::Array<float> m_lIPSPVx;
    NTuple::Array<float> m_lIPSPVy;
    NTuple::Array<float> m_lIPSPVz;
    // NTuple::Array<float> m_lIPSPVxvar;
    // NTuple::Array<float> m_lIPSPVyvar;
    // NTuple::Array<float> m_lIPSPVzvar;
    // Maximum (largest) IP, IPe and IPS
    NTuple::Array<float> m_lIP;
    // NTuple::Array<float> m_lIPe;
    NTuple::Array<float> m_lIPS;

    // Unsigned flight distance/significance (sec-prim) if composite
    // w.r.t. the PV for which the particle has the smallest IPS
    // Note that in case of several PVs, the B could fly more than
    // one of its daughters since the PV for which IPS is minimal
    // is not necessarily the same ...
    NTuple::Array<float> m_FD; // flight distance
    NTuple::Array<float> m_FS; // flight significance

    // Cosine of the angle between the particle momentum (p)
    // and the flight distance (F) w.r.t the PV for which the 
    // IPS of the composite particle is the smallest
    NTuple::Array<float> m_cospF;

    // Lifetime
    // NTuple::Array<float> m_taufit;
    // NTuple::Array<float> m_taufitErr;
    // NTuple::Array<float> m_ctfitChi2;

#ifdef MCCheck
    // Look if a the particle is associated to signal
    NTuple::Array<float> m_Sig;

    // Extrapolated state vector of the associated MCParticle track (x,y,tx,ty,Q/P), tx = dx/dz, ty = dy/dz
    NTuple::Array<float> m_MCstateX;
    NTuple::Array<float> m_MCstateY;
    NTuple::Array<float> m_MCstateTX;
    NTuple::Array<float> m_MCstateTY;
    NTuple::Array<float> m_MCstateQoverP;
    // All MC primary vertices coordinates
    NTuple::Array<float> m_MCPVx;
    NTuple::Array<float> m_MCPVy;
    NTuple::Array<float> m_MCPVz;
    NTuple::Array<long> m_VisMCPV;

#endif

#ifdef MCCheck
    // The true decay variables

    // m_mcn is the number of decays found per event; serves as index for arrays
    NTuple::Item<long>  m_mcn;
    NTuple::Array<long> m_mcID;
    
    // Look if the corresponding final state is reconstructed (only meaningful for final tracks)
    NTuple::Array<float> m_isReco;
    
    // Warning : in the MCParticle format only the px, py and pz are correct
    // -> to retrieve the true generated mass for composites, need to go HepMC
    // -> the masses stored are those of HepMC

    // Momentum, mass, ...
    NTuple::Array<float> m_mcp;
    NTuple::Array<float> m_mcpt;
    NTuple::Array<float> m_mcmass;
    
    // 3-momentum
    NTuple::Array<float> m_mcpx, m_mcpy, m_mcpz;
    
    // Primary vertex
    NTuple::Array<float> m_mcxPV, m_mcyPV, m_mczPV;
    
    // Origin vertex
    NTuple::Array<float> m_mcxOriVtx, m_mcyOriVtx, m_mczOriVtx;
    
    // Decay vertex
    NTuple::Array<float> m_mcxDKVtx, m_mcyDKVtx, m_mczDKVtx;
    
    // True IP, w.r.t the primary vertex
    NTuple::Array<float> m_mcIP;
    
    // True lifetime (relevant for composite particles) in picoseconds
    NTuple::Array<float> m_mctau;

#endif

  };

  std::map<int, HandleNTuple*> m_HandleNTupleMap;

};
#endif // DECAYCHAINNTUPLE_H
