// $Id: BTaggingAnalysis.h,v 1.3 2007-03-01 21:15:44 musy Exp $
#ifndef USER_BTAGGINGANALYSIS_H 
#define USER_BTAGGINGANALYSIS_H 1

// from Gaudi
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/FlavourTag.h"
#include "Event/SelResult.h"
#include "Event/GenHeader.h"
#include "Event/RecHeader.h"
//#include "Event/L0DUReport.h"

#include "GaudiKernel/INTupleSvc.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IGeomDispCalculator.h"
#include "Kernel/IDebugTool.h"
#include "Kernel/ICaloElectron.h"

#include "DaVinciAssociators/Particle2MCLinker.h"

//#include "IForcedBDecayTool.h"

/** @class BTaggingAnalysis BTaggingAnalysis.h 
 *  
 *  @author Marco Musy
 *  @date   28/02/2007
 */

class BTaggingAnalysis : public DVAlgorithm {

 public: 
  /// Standard constructor
  BTaggingAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTaggingAnalysis( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

 private:
  bool isinTree(const LHCb::Particle*, LHCb::Particle::ConstVector&, double&);
  StatusCode calcIP( const LHCb::Particle*, const LHCb::Vertex*, 
		     double&, double& );
  StatusCode calcIP( const LHCb::Particle*, const LHCb::RecVertex*, 
		     double&, double& );
  StatusCode calcIP( const LHCb::Particle*, 
		     const LHCb::RecVertex::ConstVector&, double&, double& ) ;
  LHCb::Particle::ConstVector FindDaughters( const LHCb::Particle* );
  const LHCb::MCParticle* originof( const LHCb::MCParticle* ) ;
  const LHCb::MCParticle* originD(  const LHCb::MCParticle* ) ;

  const LHCb::MCParticle* forcedB(void);

  LHCb::MCParticle::ConstVector prodsBstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstarstar( const LHCb::MCParticle*  );
  LHCb::MCParticle::ConstVector prodsBstring( const LHCb::MCParticle*  );
  HepMC::GenParticle* HEPassociated(const LHCb::MCParticle* );
  LHCb::MCParticle* associatedofHEP(HepMC::GenParticle* );
  int comes_from_excitedB(const LHCb::MCParticle* ,
			  const LHCb::MCParticle* );

  std::string m_SVtype, m_veloChargeName, m_TagLocation;
  ISecondaryVertexTool* m_vtxtool;
  IDebugTool* m_debug;
  IGeomDispCalculator *m_Geom;
  Particle2MCLinker* m_linkLinks;    ///< Pointer to associator using links
  ICaloElectron*  m_electron;
  //IForcedBDecayTool* m_forcedBtool;

  double m_IPPU_cut, m_distphi_cut, m_thetaMin;
  std::vector<std::string> m_setInputData;

  //properties ----------------
  bool m_RequireL0, m_RequireL1, m_RequireHLT;

  //NTuple
  NTuple::Item<long>      m_Run;
  NTuple::Item<long>      m_Event;
  NTuple::Item<long>      m_TrueTag;
  NTuple::Item<long>      m_Tag;
  NTuple::Item<long>      m_TagCat;
  NTuple::Item<long>      m_Taggers;
  NTuple::Item<long>      m_type;
  NTuple::Item<long>      m_trigger;
  NTuple::Item<long>      m_Tamper;
  NTuple::Item<long>      m_BSID;
  NTuple::Item<float>     m_BSthe;
  NTuple::Item<float>     m_BSphi;
  NTuple::Item<float>     m_BSmass;
  NTuple::Item<long>      m_BSosc;
  NTuple::Item<float>     m_BSP;
  NTuple::Item<float>     m_MCBSthe;
  NTuple::Item<float>     m_MCBSphi;
  NTuple::Item<float>     m_MCBSP;

  //signal
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
  NTuple::Array<float>     m_elChi;
  NTuple::Array<float>     m_distphi;
  NTuple::Array<float>     m_Emeas;
  NTuple::Array<float>     m_EOverP;
  NTuple::Array<float>     m_showerZ;
  NTuple::Array<float>     m_veloch;
  NTuple::Array<float>     m_PIDe;
  NTuple::Array<float>     m_PIDm;
  NTuple::Array<float>     m_PIDk;
  NTuple::Array<float>     m_PIDp;
  NTuple::Array<long>      m_PIDfl;
  NTuple::Array<long>      m_RichPID;
  NTuple::Array<long>      m_vFlag;

  NTuple::Array<long>      m_MCID;
  NTuple::Array<float>     m_MCP;
  NTuple::Array<float>     m_MCPt;
  NTuple::Array<float>     m_MCphi;
  NTuple::Array<float>     m_MCz;
  NTuple::Array<long>      m_mothID;
  NTuple::Array<long>      m_ancID;
  NTuple::Array<long>      m_bFlag;
  NTuple::Array<long>      m_dFlag;
  NTuple::Array<long>      m_xFlag;
  NTuple::Array<float>     m_IPT;
 
  NTuple::Item<float>      m_RVx;
  NTuple::Item<float>      m_RVy;
  NTuple::Item<float>      m_RVz;

  NTuple::Item<float>      m_TVx;
  NTuple::Item<float>      m_TVy;
  NTuple::Item<float>      m_TVz;
  NTuple::Item<float>      m_Tchi;
  NTuple::Item<long>       m_Vch;

};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
