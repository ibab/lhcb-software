// $Id: BTaggingAnalysis.h,v 1.2 2006-11-03 20:51:40 musy Exp $
#ifndef USER_BTAGGINGANALYSIS_H 
#define USER_BTAGGINGANALYSIS_H 1

// from Gaudi
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/FlavourTag.h"
#include "Event/SelResult.h"
#include "Event/GenHeader.h"
//#include "Event/TamperingResults.h"

#include "GaudiKernel/INTupleSvc.h"
#include "Kernel/IEvtTypeSvc.h"
#include "Kernel/IMCDecayFinder.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IGeomDispCalculator.h"

#include "DaVinciAssociators/Particle2MCLinker.h"
#include "Kernel/IDebugTool.h"

/** @class BTaggingAnalysis BTaggingAnalysis.h 
 *  
 *  @author Marco Musy
 *  @date   01/02/2005
 */

class BTaggingAnalysis : public DVAlgorithm {

 public: 
  /// Standard constructor
  BTaggingAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTaggingAnalysis( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 protected:
  StatusCode setDecayToFind( const int evtCode );

 private:
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );

  bool isinTree( const LHCb::Particle*, LHCb::Particle::ConstVector& );
  StatusCode calcIP( const LHCb::Particle*, const LHCb::Vertex*, double&, double& );
  StatusCode calcIP( const LHCb::Particle*, const LHCb::RecVertex*, double&, double& );
  StatusCode calcIP( const LHCb::Particle*, 
		     const LHCb::RecVertex::ConstVector&, double&, double& ) ;
  LHCb::Particle::ConstVector FindDaughters( const LHCb::Particle* );
  long trackType( const LHCb::Particle* );
  const LHCb::MCParticle* originof( const LHCb::MCParticle* ) ;
  const LHCb::MCParticle* originD(  const LHCb::MCParticle* ) ;
 
  std::string m_SVtype, m_TagLocation;
  ISecondaryVertexTool* m_vtxtool;
  //IVisPrimVertTool* m_visTool;
  HepMC::GenEvent* m_genobj;
  IDebugTool* m_debug;
  IGeomDispCalculator *m_Geom;

  bool            m_fromData;        ///< flag read event code from data
  int             m_evtCode;         ///< event code to test
  bool            m_setDecay;        ///< Flag is decay has been set
  IEvtTypeSvc*    m_evtTypeSvc;      ///< Pointer to service
  IMCDecayFinder* m_mcFinder;        ///< Pointer to tool
  Particle2MCLinker* m_linkLinks;    ///< Pointer to associator using links
  std::vector<std::string> m_setInputData;

  //properties ----------------
  double m_ProbMin;
  double m_VchOmega;

  bool m_RequireL0, m_RequireL1, m_RequireHLT;

  //NTuple
  NTuple::Item<long>      m_Run;
  NTuple::Item<long>      m_Event;
  NTuple::Item<long>      m_TrueTag;
  NTuple::Item<long>      m_Tag;
  NTuple::Item<long>      m_TagCat;
  NTuple::Item<long>      m_type;
  NTuple::Item<long>      m_trigger;
  NTuple::Item<long>      m_L0tamp;
  NTuple::Item<long>      m_L1tamp;
  NTuple::Item<long>      m_BSID;
  NTuple::Item<float>     m_BSthe;
  NTuple::Item<float>     m_BSphi;
  NTuple::Item<float>     m_BSmass;
  NTuple::Item<long>      m_BSosc;
  NTuple::Item<float>     m_BSP;
  NTuple::Item<float>     m_MCBSthe;
  NTuple::Item<float>     m_MCBSphi;
  NTuple::Item<float>     m_MCBSP;

  //segnale
  NTuple::Item<float>     m_flighterr;

  NTuple::Item<float>     m_BSx;
  NTuple::Item<float>     m_BSy;
  NTuple::Item<float>     m_BSz;

  NTuple::Item<long>       m_BOID;
  NTuple::Item<long>       m_BOosc;
  NTuple::Item<float>      m_BOP;
  NTuple::Item<float>      m_BOthe;
  NTuple::Item<float>      m_BOphi;
  NTuple::Item<float>      m_BOx;
  NTuple::Item<float>      m_BOy;
  NTuple::Item<float>      m_BOz;

  NTuple::Item<long>      m_K;
  NTuple::Item<float>     m_kx;
  NTuple::Item<float>     m_ky;
  NTuple::Item<float>     m_kz;
  NTuple::Item<long>      m_krec;
  NTuple::Item<long>      m_kType;

  NTuple::Item<long>      m_N;
  NTuple::Array<long>      m_ID;
  NTuple::Array<float>     m_P;
  NTuple::Array<float>     m_Pt;
  NTuple::Array<float>     m_phi;
  NTuple::Array<long>      m_ch;
  NTuple::Array<float>     m_IP;
  NTuple::Array<float>     m_IPerr;
  NTuple::Array<float>     m_IPPU;
  NTuple::Array<long>      m_trtyp;
  NTuple::Array<float>     m_lcs;
  NTuple::Array<float>     m_dQ;
  NTuple::Array<float>     m_Emeas;
  NTuple::Array<float>     m_veloch;
  NTuple::Array<float>     m_PIDe;
  NTuple::Array<float>     m_PIDm;
  NTuple::Array<float>     m_PIDk;
  NTuple::Array<float>     m_PIDp;
  NTuple::Array<long>      m_PIDfl;
  NTuple::Array<long>      m_vFlag;
  NTuple::Array<float>     m_muNSH;
 
  NTuple::Array<long>      m_MCID;
  NTuple::Array<float>     m_MCP;
  NTuple::Array<float>     m_MCPt;
  NTuple::Array<float>     m_MCphi;
  NTuple::Array<float>     m_MCz;
  NTuple::Array<long>      m_mothID;
  NTuple::Array<long>      m_ancID;
  NTuple::Array<long>      m_bFlag;
  NTuple::Array<long>      m_dFlag;
  NTuple::Array<float>     m_IPT;
 
  NTuple::Item<float>      m_RVx;
  NTuple::Item<float>      m_RVy;
  NTuple::Item<float>      m_RVz;

  NTuple::Item<long>       m_M;
  NTuple::Item<float>      m_TVx;
  NTuple::Item<float>      m_TVy;
  NTuple::Item<float>      m_TVz;
  NTuple::Item<float>      m_Tchi;
  NTuple::Item<long>       m_Vch;

};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
