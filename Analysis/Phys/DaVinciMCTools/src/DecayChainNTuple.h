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
class IPVLocator;

#ifdef MCCheck
class IMCDecayFinder;
#include "DaVinciAssociators/Particle2MCLinksAsct.h"
#include "Relations/IAssociatorWeighted.h"
#include "Event/CaloCluster.h"
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
 *  The primaries are retrieved with the PVLocator tool, default is offline
 *  -> must set PVLocation property of PVLocator tool to change it
 *  
 *  Possibility to use the TrgDispCalculator tool for online particles
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
  IPVLocator* m_PVLocator;

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

#ifdef MCCheck
  IMCDecayFinder* m_pMCDKFinder;
  // Ref. to link associator
  Particle2MCLinksAsct::IAsct* m_pAsctLinks;
  int m_gammaID; // gamma particle ID
  typedef IAssociatorWeighted<CaloCluster, MCParticle, float> IAsctCl2MCP;
  typedef IAsctCl2MCP::DirectType DirectType;
  typedef IAsctCl2MCP::InverseType InverseType;
  const IAsctCl2MCP* m_pAsctCl2MCP;
#endif

  //=============================================================================
  // Reconstructed decay
  //=============================================================================

  // Book the ntuple
  StatusCode BookNTuple(std::vector<Particle*>&);
  // Write the ntuple
  StatusCode WriteNTuple(std::vector<Particle*>&);

  // Get all the primaries
  StatusCode getPV(VertexVector&);

#ifdef MCCheck
  bool isSignal(MCParticle*, std::vector<MCParticle*>& );
  bool updateMCMomentum(MCParticle*);
  
  // Write the true part of the decay
  StatusCode WriteMCNTuple(std::vector<MCParticle*>&);
#endif

  // Trigger
  const std::string m_pathTrg;
  const std::string m_CaloClustersPath;
  const std::string m_TrgCaloClustersPath;

  // NTuple global variables
  NTuple::Item<long> m_eventNumber,m_runNumber;
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
  // The tags
  NTuple::Item<long> m_nTags;
  NTuple::Array<long> m_TagDecision;
  NTuple::Array<long> m_TagCat;
  // All taggers information
  NTuple::Item<long> m_nTaggers;
  // Matrices indexed by nTags and nTaggers
  NTuple::Matrix<long> m_IDTagger;
  NTuple::Matrix<float> m_pTagger;
  NTuple::Matrix<float> m_ptTagger;
  NTuple::Matrix<float> m_sIPSTagger;

#ifdef MCCheck
  NTuple::Item<long> m_nMCPV;
  // All MC primary vertices coordinates
  NTuple::Array<float> m_MCPVx;
  NTuple::Array<float> m_MCPVy;
  NTuple::Array<float> m_MCPVz;
#endif

  //-----------------------------------------------------------------------------
  // HandleNTuple
  //-----------------------------------------------------------------------------

  class HandleNTuple{
  public:

    HandleNTuple(NTuplePtr& nt, unsigned int& number, 
                 // ILifetimeFitter *lifetimefitter,
                 IGeomDispCalculator* iptool);

#ifndef MCCheck
    void FillNTuple(Particle& part, VertexVector& pvs, RichPIDs* globalPIDs);
#endif

#ifdef MCCheck
    void FillNTuple(Particle& part, VertexVector& pvs, 
                    bool& isSig, MCParticle* mclink, RichPIDs* globalPIDs);

    void FillMCNTuple(MCParticle& mcpart, HepPoint3D& MCPVPosition, bool& isReco);
#endif

    void clean(){m_n=0;}

#ifdef MCCheck
    void mcclean(){m_mcn=0;}
#endif
    
  private:
    // Tools
    IGeomDispCalculator* m_iptool;
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
    // Look if a final state is reconstructed and is signal (only meaningful for final tracks)
    NTuple::Array<float> m_Sig;

    // Extrapolated state vector of the associated MCParticle track (x,y,tx,ty,Q/P), tx = dx/dz, ty = dy/dz
    NTuple::Array<float> m_MCstateX;
    NTuple::Array<float> m_MCstateY;
    NTuple::Array<float> m_MCstateTX;
    NTuple::Array<float> m_MCstateTY;
    NTuple::Array<float> m_MCstateQoverP;
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
