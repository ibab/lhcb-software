// $Id: BTagging.h,v 1.9 2005-02-02 07:10:27 pkoppenb Exp $
#ifndef USER_BTAGGING_H 
#define USER_BTAGGING_H 1

// from STL
#include <fstream>
#include <string>
#include <math.h>
// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "Kernel/DVAlgorithm.h"
// from Event 
#include "Event/EventHeader.h"
#include "Event/FlavourTag.h"
#include "Event/SelResult.h"
#include "Event/TrgDecision.h"
#include "Event/TamperingResults.h"
// from RecoTools
#include "RecoTools/ITrVeloCharge.h"
#include "FlavourTagging/ISecondaryVertexTool.h"
#include "FlavourTagging/INNetTool.h"

#define MAXSIZETAGS 100

/** @class BTagging BTagging.h 
 *  
 *  @author Marco Musy
 *  @date   01/02/2005
 */

class BTagging : public DVAlgorithm {

  bool isinTree( Particle*, ParticleVector& );
  void printInfo( Particle* );
  void PileUpIP( Particle*, double&, double& ) ;
  std::vector<Particle*> toStdVector( SmartRefVector<Particle>& );
  ParticleVector FindDaughters( Particle* );
  double min(double, double);
  double max(double, double);
  double pol2(double, double, double );
  double pol3(double, double, double, double );
  double pol4(double, double, double, double, double );

public: 
  /// Standard constructor
  BTagging( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~BTagging( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  VertexVector PileUpVtx;

  std::string m_veloChargeName, m_TagLocation;
  std::string m_SVtype, m_CombinationTechnique;
  ITrVeloCharge* m_veloCharge;
  ISecondaryVertexTool* m_vtxtool;
  INNetTool* m_nnet;

  //properties ----------------
  double m_AXPt_cut_muon;
  double m_AXP_cut_muon;

  double m_AXPt_cut_ele;
  double m_AXP_cut_ele;
  long   m_VeloChMin;
  long   m_VeloChMax;
  double m_EoverP;

  double m_AXPt_cut_kaon;
  double m_AXP_cut_kaon ;
  double m_IP_cut_kaon ;
  double m_IPPU_cut_kaon;

  double m_AXPt_cut_kaonS;
  double m_AXP_cut_kaonS;
  double m_IP_cut_kaonS;
  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dQcut_kaonS;

  double m_AXPt_cut_pionS;
  double m_AXP_cut_pionS;
  double m_IP_cut_pionS;
  double m_dQcut_pionS;
  double m_dQ2cut_pionS;

  double m_ProbMin;
  double m_VchOmega;

  bool m_RequireL0, m_RequireL1, m_RequireHLT, m_RequireTamp;
};
//===========================================================================//
#endif // USER_BTAGGING_H
