// $Id: TaggerKaonOppositeTool.h,v 1.11 2010-06-17 17:46:03 mgrabalo Exp $
#ifndef USER_TAGGERKAONOPPOSITETOOL_H
#define USER_TAGGERKAONOPPOSITETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
// from ROOT
#include <TROOT.h>

// from local
#include "KaonOSWrapper.h"
#include "MCKaonOSWrapper.h"
#include "INNetTool.h"
#include "Kernel/IParticleDescendants.h"
#include "FlavourTagging/ITaggingUtils.h"
#include <list>
#include <utility>
#include <string>

#include "MultiplePersonalityCall.h"
/** @class TaggerKaonOppositeTool TaggerKaonOppositeTool.h
 *
 *  Tool to tag the B flavour with a KaonOpposite Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerKaonOppositeTool : public GaudiTool,
                               virtual public ITagger {

public:
  /// Standard constructor
  TaggerKaonOppositeTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );
  virtual ~TaggerKaonOppositeTool( ); ///< Destructor
  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();
  
  //-------------------------------------------------------------
  MultiplePersonalityCall<boost::function<
  LHCb::Tagger(
               const LHCb::Particle*, 
               const LHCb::RecVertex*,
               const int, 
               LHCb::Particle::ConstVector&) > > 
  m_tag;

  virtual LHCb::Tagger  tag( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             const int,
                             LHCb::Particle::ConstVector&);

  virtual LHCb::Tagger  tagReco12( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             const int,
                             LHCb::Particle::ConstVector&);
  virtual LHCb::Tagger  tagReco14( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             const int,
                             LHCb::Particle::ConstVector&);
  /*
  virtual LHCb::Tagger tag( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&, 
                            LHCb::Particle::ConstVector&);
  virtual LHCb::Tagger tagReco12( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&, 
                            LHCb::Particle::ConstVector&);

  virtual LHCb::Tagger tagReco14( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&, 
                            LHCb::Particle::ConstVector&);
  */
  //-------------------------------------------------------------

private:
  MCKaonOSWrapper*   m_myMCreader;
  KaonOSWrapper*     m_myDATAreader;

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend;
  INNetTool* m_nnet;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_kaon;
  double m_P_cut_kaon ;
  double m_IP_cut_kaon ;
  double m_IPs_cut_kaon ;
  double m_lcs_kaon;
  double m_AverageOmega;
  double m_ghost_cut;
  double m_PIDk_cut;
  double m_PIDkp_cut;
  double m_ipPU_cut_kaon;
  double m_distPhi_cut_kaon;

  double m_PIDNNm_cut_kaon;
  double m_PIDNNe_cut_kaon;
  double m_PIDNNk_cut_kaon;
  double m_PIDNNpi_cut_kaon;
  double m_PIDNNp_cut_kaon; 
  double m_PIDNNkp_cut_kaon;
  double m_PIDNNkpi_cut_kaon;
  double m_ghostProb_kaon;
  double m_ProbMin_kaon;
  double m_P0_Cal_kaon ;
  double m_P1_Cal_kaon ;
  double m_Eta_Cal_kaon ;
  std::string m_personality;
  int    m_isMonteCarlo;
  double m_P0k, m_P1k, m_P2k, m_P3k;
  
};

//===============================================================//
#endif // USER_TAGGERKAONOPPOSITETOOL_H
