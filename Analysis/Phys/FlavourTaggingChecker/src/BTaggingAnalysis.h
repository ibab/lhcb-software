// $Id: BTaggingAnalysis.h,v 1.22 2010-02-11 18:39:15 musy Exp $
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
#include "Event/HltDecReports.h"

//ntuple
#include "GaudiKernel/INTupleSvc.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/IBackgroundCategory.h"

#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IPVReFitter.h"

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

  const LHCb::Particle*  chooseBHypothesis(const LHCb::Particle::ConstVector&);
  const LHCb::Particle::ConstVector 
  chooseParticles(const LHCb::Particle::ConstVector& ,
                  LHCb::Particle::ConstVector,
                  LHCb::RecVertex::ConstVector);
  const LHCb::RecVertex::ConstVector 
  choosePrimary(const LHCb::Particle* AXB,
                const LHCb::MCParticle* BS,
                const LHCb::RecVertex::Container* verts,
                const LHCb::RecVertex*& RecVert,
                LHCb::RecVertex& RefitRecVert) ;
  LHCb::FlavourTag* tagevent (const LHCb::Particle* AXBS);
  

  std::string m_SVtype, m_veloChargeName, 
    m_TagLocation, m_BHypoCriterium, m_ChoosePV ;
  IPrintMCDecayTreeTool*     m_debug;
  ICaloElectron*             m_electron;
  IForcedBDecayTool*         m_forcedBtool;
  IBackgroundCategory*       m_bkgCategory;
  ILifetimeFitter*           m_pLifetimeFitter;
  ITaggingUtilsChecker*      m_util;
  ISecondaryVertexTool*      m_vtxtool;
  IParticleDescendants*      m_descend;
  IPVReFitter*               m_pvReFitter;
  ITriggerTisTos*            m_TriggerTisTosTool;
  IParticle2MCAssociator*    m_assoc; 
 
  double m_IPPU_cut, m_distphi_cut, m_thetaMin;

  //properties ----------------
  bool m_requireTrigger, m_UseMCTrueFlavour, m_requireTisTos;

};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
