#ifndef DECAYCHAINNTUPLE_H
#define DECAYCHAINNTUPLE_H 1

// Include files
// from Gaudi
#include "GaudiKernel/Algorithm.h"
#include "Kernel/DVAlgorithm.h"

#include "Event/RichPID.h"
#include "TrgTools/TrgDataProvider.h"

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
#endif

/** @class DecayChainNTuple DecayChainNTuple.h
 * 
 *  Creates a NTuple for any generic decay chain (checks association if compiled with MCCheck flag)
 * 
 *  Fills the corresponding true decay variables if compiled with MCCheck flag: 
 *  -> the number of flagged particles for Decay and MCDecay must be the same!!!
 * 
 *  The children must be flagged with ^
 *  
 *  The decay chain must be written without cc for the labeling to work (otherwise the daughters get mixed)
 *
 *  Works for different decays, as long as there are the same number of decay produts:
 *
 *  Example 0: {B0 -> ^pi+ ^pi-, B~0 -> ^pi+ ^pi-, B_s0 -> ^pi+ ^K-, B_s0 -> ^pi- ^K+, B_s~0 -> ^pi- ^K+, B_s~0 -> ^pi+ ^K-}
 *  
 *  Example 1 (for a reconstructed decay):
 *  in {B0 -> ^K+ ^pi-, B~0 -> ^K- ^pi+ } the first daughter will be the pi:
 *     - ParticleName (mother) B0
 *     - Label number = 1 , ParticleName pi-
 *     - Label number = 2 , ParticleName K+
 *  and 
 *     - ParticleName (mother) B~0
 *     - Label number = 1 , ParticleName pi+
 *     - Label number = 2 , ParticleName K-
 *
 *  Example 2 (for a true decay):
 *  in {B0 -> ^pi+ ^pi-, B~0 -> ^pi+ ^pi- } the first daughter will be the pi-:
 *     - ParticleName (mother) B0
 *     - Label number = 1 , ParticleName pi-
 *     - Label number = 2 , ParticleName pi+
 *  and 
 *     - ParticleName (mother) B~0
 *     - Label number = 1 , ParticleName pi-
 *     - Label number = 2 , ParticleName pi+
 * 
 *  Example 3 (for a reconstructed decay):
 *  -> For B0 -> (^D~0 -> ^K+ ^pi-) (^K*(892)0 -> ^K+ ^pi-) we'll have:
 *    - ParticleName (mother) B0
 *    - Label number = 1 , ParticleName K*(892)0
 *    - Label number = 2 , ParticleName pi-
 *    - Label number = 3 , ParticleName K+
 *    - Label number = 4 , ParticleName D~0
 *    - Label number = 5 , ParticleName pi-
 *    - Label number = 6 , ParticleName K+
 *  
 *  Output file must be read with ROOT
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
 *  TODO: 
 *  - adapt to gammas ...
 *  - for gammas: re-valuate 4-vector at at secondary vertex for online and offline
 *  - for gammas: get MC association
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
  bool m_useRichOnlinePID;
  std::string m_richOnlinePIDLocation;
  std::string m_geomToolName ; // Name of Geometrical Tool

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
  // Ref. to Event Data Service
  IDataProviderSvc* m_EDS;
  // Reference to LifetimeFitter
  // ILifetimeFitter *m_pLifetimeFitter;

#ifdef MCCheck
  IMCDecayFinder* m_pMCDKFinder;
  // Ref. to link associator
  Particle2MCLinksAsct::IAsct* m_pAsctLinks;
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

  // Write the true part of the decay
  StatusCode WriteMCNTuple(std::vector<MCParticle*>&);
#endif

  // Trigger
  std::string m_pathTrg;
  TrgDataProvider* m_dataProvider;

  // NTuple global variables
  NTuple::Item<long> m_eventNumber,m_runNumber;
  NTuple::Item<long> m_nRecoPV;
  NTuple::Item<long> m_L0Decision;
  NTuple::Item<long> m_L1Decision;
  NTuple::Item<long> m_L1Gen;
  NTuple::Item<long> m_L1SiMu;
  NTuple::Item<long> m_L1DiMu;
  NTuple::Item<long> m_L1JPsi;
  NTuple::Item<long> m_L1Elec;
  NTuple::Item<long> m_L1Phot;
  NTuple::Item<long> m_HLTDecision;

#ifdef MCCheck
  NTuple::Item<long> m_nMCPV;
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
    // NTuple::Array<float> m_pxvar, m_pyvar, m_pzvar;
    
    // Decay vtx if any; otherwise set to -100000
    NTuple::Array<float> m_vchitwo;
    NTuple::Array<float> m_vnDoF;
    NTuple::Array<float> m_vx;
    NTuple::Array<float> m_vy;
    NTuple::Array<float> m_vz;
    // NTuple::Array<float> m_vxvar;
    // NTuple::Array<float> m_vyvar;
    NTuple::Array<float> m_vzvar;

    // Primary vertices

    // PV w.r.t. the particle has the smallest IPS    
    NTuple::Array<float> m_chitwosIPSPV;
    NTuple::Array<float> m_sIPSPVx;
    NTuple::Array<float> m_sIPSPVy;
    NTuple::Array<float> m_sIPSPVz;
    // NTuple::Array<float> m_sIPSPVxvar;
    // NTuple::Array<float> m_sIPSPVyvar;
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
    // NTuple::Array<float> m_mctau;

#endif

  };

  std::map<int, HandleNTuple*> m_HandleNTupleMap;

};
#endif // DECAYCHAINNTUPLE_H
