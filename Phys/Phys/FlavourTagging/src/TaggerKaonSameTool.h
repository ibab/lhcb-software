// $Id: TaggerKaonSameTool.h,v 1.11 2010-06-17 17:46:04 mgrabalo Exp $
#ifndef USER_TAGGERKAONSAMETOOL_H
#define USER_TAGGERKAONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Event/FlavourTag.h"
#include "Event/ProtoParticle.h"
#include "Kernel/ITagger.h"

// from local
#include "INNetTool.h"
#include "MCKaonSSWrapper.h"
#include "Kernel/IParticleDescendants.h"
#include "FlavourTagging/ITaggingUtils.h"

/** @class TaggerKaonSameTool TaggerKaonSameTool.h
 *
 *  Tool to tag the B flavour with a KaonSame Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerKaonSameTool : public GaudiTool,
                           virtual public ITagger {

public:

  /// Standard constructor
  TaggerKaonSameTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );
  virtual ~TaggerKaonSameTool( ); ///< Destructor
  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:
  MCKaonSSWrapper *m_myMCreader;
  

  INNetTool* m_nnet;
  IParticleDescendants* m_descend;
  ITaggingUtils* m_util;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_kaonS;
  double m_P_cut_kaonS;
  double m_IP_cut_kaonS;
  double m_phicut_kaonS;
  double m_etacut_kaonS;
  double m_dRcut_kaonS;
  double m_dQcut_kaonS;
  double m_lcs_cut;
  double m_KaonSPID_kpS_cut;
  double m_KaonSPID_kS_cut;
  double m_ipPU_cut_kS;
  double m_distPhi_cut_kS;
  double m_ProbMin_kaonS;
  double m_P0_Cal_kaonS ;
  double m_P1_Cal_kaonS ;
  double m_Eta_Cal_kaonS ;
  double m_AverageOmega;
  int    m_isMonteCarlo;
  double m_P0ks,  m_P1ks,  m_P2ks,  m_P3ks;
  
};

//===============================================================//
#endif // USER_TAGGERKAONSAMETOOL_H
