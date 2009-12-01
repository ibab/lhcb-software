// $Id: BTaggingAnalysis.h,v 1.19 2009-12-01 15:45:01 musy Exp $
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
#
#include "GaudiKernel/INTupleSvc.h"
#include "Kernel/ISecondaryVertexTool.h"
#include "Kernel/IBTaggingTool.h"
#include "MCInterfaces/IPrintMCDecayTreeTool.h"

#include "Kernel/IParticle2MCAssociator.h"
#include "Kernel/IBackgroundCategory.h"

#include "Kernel/IParticleDescendants.h"
#include "Kernel/ILifetimeFitter.h"
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
  IPrintMCDecayTreeTool*     m_debug;
  IGeomDispCalculator*       m_Geom;
  ICaloElectron*             m_electron;
  IForcedBDecayTool*         m_forcedBtool;
  IBackgroundCategory*       m_bkgCategory;
  ILifetimeFitter*           m_pLifetimeFitter;
  ITaggingUtilsChecker*      m_util;
  ISecondaryVertexTool*      m_vtxtool;
  IParticleDescendants*      m_descend;
  ITriggerTisTos*            m_TriggerTisTosTool;
  IParticle2MCAssociator*    m_assoc; 
 
  double m_IPPU_cut, m_distphi_cut, m_thetaMin;
  std::vector<std::string> m_setInputData;

  //properties ----------------
  bool m_requireTrigger, m_UseMCTrueFlavour;

};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
