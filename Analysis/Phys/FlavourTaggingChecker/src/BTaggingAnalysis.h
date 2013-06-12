// $Id: BTaggingAnalysis.h,v 1.27 2010-06-30 16:34:33 mgrabalo Exp $
#ifndef USER_BTAGGINGANALYSIS_H 
#define USER_BTAGGINGANALYSIS_H 1

// from Gaudi
#include "Kernel/DaVinciTupleAlgorithm.h"
// from Event 
#include "Event/FlavourTag.h"
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
#include "Kernel/IVertexFit.h"

//#include "MCInterfaces/IForcedBDecayTool.h"
#include "BDecayTool.h"

#include "CaloUtils/ICaloElectron.h"

// from local
#include "ITaggingUtilsChecker.h"

/** @class BTaggingAnalysis BTaggingAnalysis.h 
 *  
 *  @author Marco Musy
 *  @date   28/02/2007
 */

using namespace LHCb;
using namespace Gaudi::Units;

class BTaggingAnalysis : public DaVinciTupleAlgorithm {

public: 
  /// Standard constructor
  BTaggingAnalysis( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTaggingAnalysis( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  const Particle*  chooseBHypothesis(const Particle::Range&);
  const Particle::ConstVector 
  chooseCandidates(const Particle::Range& ,
                   Particle::ConstVector,
                   const RecVertex::ConstVector);
  const RecVertex::ConstVector 
  choosePrimary(const Particle* AXB,
                const RecVertex::Range& verts,
                RecVertex& RecVert);
  const ProtoParticle::ConstVector tagevent (Tuple& tuple, 
                                             //const Particle* AXBS, const RecVertex* PV ); //to select ourselves PV
                                             const Particle* AXBS );
  StatusCode FillTrigger (Tuple& tuple, const Particle* AXBS) ;
  Particle::ConstVector FillSelectedB (Tuple& tuple, const Particle* AXBS);
  StatusCode FillMCInfoOfB(Tuple& tuple, const MCParticles* mcpart);
  void FillSeedInfo(Tuple& tuple, 
		    const RecVertex* RecVert,
		    const Particle::ConstVector& vtags,
		    std::vector<Vertex>& svertices,
		    const Particle *&SVpart1, 
		    const Particle *&SVpart2 ) ;

  double GetInvariantMass(double MA, Gaudi::LorentzVector PA, double MB, Gaudi::LorentzVector PB);
  
  std::string m_veloChargeName, 
    m_SecondaryVertexToolName,
    m_TagLocation, 
    m_BHypoCriterium, 
    m_ChoosePV, 
    m_taggerLocation ;
  bool m_ReFitPVs;
  
  IPrintMCDecayTreeTool*     m_debug;
  ICaloElectron*             m_electron;
  IBDecayTool*               m_forcedBtool;
  IBackgroundCategory*       m_bkgCategory;
  ILifetimeFitter*           m_pLifetimeFitter;
  ITaggingUtilsChecker*      m_util;
  IParticleDescendants*      m_descend;
  IPVReFitter*               m_pvReFitter;
  ITriggerTisTos*            m_TriggerTisTosTool;
  ITriggerTisTos*            m_L0TriggerTisTosTool;

  IParticle2MCAssociator*    m_assoc; 
  ISecondaryVertexTool*      m_svtool;
  IVertexFit *               m_fitter;
  const MCParticle*          m_BS;

  double m_IPPU_cut, m_distphi_cut, m_thetaMin, m_ghostprob_cut;

  //properties ----------------
  bool m_requireTrigger, m_requireTisTos;
  bool m_saveHlt1Lines, m_saveHlt2Lines;

  //mc
  bool m_EnableMC;
};

//===========================================================================//
#endif // USER_BTAGGINGANALYSIS_H
