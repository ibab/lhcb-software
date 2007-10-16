// $Id: BTaggingAnalysis.h,v 1.12 2007-10-16 10:35:43 musy Exp $
#ifndef USER_BTAGGINGANALYSIS_H 
#define USER_BTAGGINGANALYSIS_H 1

// from Gaudi
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/FlavourTag.h"
#include "Event/SelResult.h"
#include "Event/GenHeader.h"
#include "Event/RecHeader.h"
#include "Event/L0DUReport.h"
#include "Event/HltSummary.h"

#include "GaudiKernel/INTupleSvc.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h"
#include "Kernel/IDebugTool.h"
#include "Kernel/Particle2MCLinker.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/IHltSummaryTool.h"
#include "Kernel/ITriggerTisTos.h"

#include "MCInterfaces/IForcedBDecayTool.h"

#include "CaloUtils/ICaloElectron.h"

// from local
#include "ITaggingUtilsChecker.h"

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

  std::string m_SVtype, m_veloChargeName, m_TagLocation;
  IDebugTool*           m_debug;
  IGeomDispCalculator*  m_Geom;
  Particle2MCLinker*    m_linkLinks; 
  ICaloElectron*        m_electron;
  IForcedBDecayTool*    m_forcedBtool;
  IBackgroundCategory*  m_bkgCategory;
  ILifetimeFitter*      m_pLifetimeFitter;
  ITaggingUtilsChecker* m_util;
  ISecondaryVertexTool* m_vtxtool;
  IParticleDescendants* m_descend;
  IHltSummaryTool*      m_hltSummaryTool;
  ITriggerTisTos*       m_TriggerTisTosTool;

  double m_IPPU_cut, m_distphi_cut, m_thetaMin;
  std::vector<std::string> m_setInputData;

  //properties ----------------
  bool m_requireTrigger, m_UseMCTrueFlavour;

  //NTuple
  NTuple::Item<long>      m_Run;
  NTuple::Item<long>      m_Event;
  NTuple::Item<long>      m_TrueTag;
  NTuple::Item<long>      m_Tag;
  NTuple::Item<float>     m_omega;
  NTuple::Item<long>      m_TagCat;
  NTuple::Item<long>      m_type;
  NTuple::Item<long>      m_trigger;
  NTuple::Item<long>      m_L0TisTos;
  NTuple::Item<long>      m_HltTisTos;
  NTuple::Item<long>      m_bkgCat;
  NTuple::Item<long>      m_BSosc;
  NTuple::Item<float>     m_tau;
  NTuple::Item<float>     m_tauErr;
  NTuple::Item<float>     m_ctChi2;

  //signal
  NTuple::Item<long>      m_M;
  NTuple::Array<long>      m_sID;
  NTuple::Array<long>      m_sMothID;
  NTuple::Array<float>     m_sP;
  NTuple::Array<float>     m_sPt;
  NTuple::Array<float>     m_sPhi;
  NTuple::Array<float>     m_sMass;
  NTuple::Array<long>      m_sMCID;
  NTuple::Array<long>      m_sMCMothID;
  NTuple::Array<float>     m_sMCP;
  NTuple::Array<float>     m_sMCPt;
  NTuple::Array<float>     m_sMCPhi;
  NTuple::Array<float>     m_sVx;
  NTuple::Array<float>     m_sVy;
  NTuple::Array<float>     m_sVz;

  NTuple::Item<long>       m_BOID;
  NTuple::Item<long>       m_BOosc;
  NTuple::Item<float>      m_BOP;
  NTuple::Item<float>      m_BOthe;
  NTuple::Item<float>      m_BOphi;
  NTuple::Item<float>      m_BOx;
  NTuple::Item<float>      m_BOy;
  NTuple::Item<float>      m_BOz;
  NTuple::Item<float>      m_BSx;
  NTuple::Item<float>      m_BSy;
  NTuple::Item<float>      m_BSz;

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
  NTuple::Array<float>     m_distphi;
  NTuple::Array<float>     m_EOverP;
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
  NTuple::Array<long>      m_xFlag;
  NTuple::Array<float>     m_IPT;
 
  NTuple::Item<float>      m_RVx;
  NTuple::Item<float>      m_RVy;
  NTuple::Item<float>      m_RVz;

  NTuple::Item<float>      m_SVx;
  NTuple::Item<float>      m_SVy;
  NTuple::Item<float>      m_SVz;
  NTuple::Item<float>      m_Schi;
  NTuple::Item<long>       m_SVch;

  NTuple::Item<long>       m_T;
  NTuple::Array<long>      m_TaggerDecision;
  NTuple::Array<long>      m_TaggerType;
  NTuple::Array<float>     m_TaggerOmega;
  NTuple::Array<long>      m_TaggerUsedInSV;
  NTuple::Array<long>      m_TaggerRef;

};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
