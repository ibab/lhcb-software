// $Id: TaggerElectronTool.h,v 1.13 2010-06-17 17:46:03 mgrabalo Exp $
#ifndef USER_TAGGERELECTRONTOOL_H
#define USER_TAGGERELECTRONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Kernel/ITagger.h"
#include "CaloUtils/ICaloElectron.h"
//from ROOT
#include <TROOT.h>

// from local
#include "MCElectronOSWrapper.h"
#include "ElectronOSWrapper.h"
#include "FlavourTagging/ITaggingUtils.h"
#include "INNetTool.h"
#include "Kernel/IParticleDescendants.h"

#include "MultiplePersonalityCall.h"

/** @class TaggerElectronTool TaggerElectronTool.h
 *
 *  Tool to tag the B flavour with a Electron Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerElectronTool : public GaudiTool,
                           virtual public ITagger {

public:
  /// Standard constructor
  TaggerElectronTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );
  virtual ~TaggerElectronTool( ); ///< Destructor
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
  MultiplePersonalityCall<boost::function<
  LHCb::Tagger(
               const LHCb::Particle*, 
               const LHCb::RecVertex*,
               LHCb::Vertex::ConstVector&,
               LHCb::Particle::ConstVector&) > > 
  m_tagOLD;
  
  virtual LHCb::Tagger  tag( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             LHCb::Vertex::ConstVector&,
                             LHCb::Particle::ConstVector&);
  
  virtual LHCb::Tagger  tagReco12( const LHCb::Particle*, 
                                   const LHCb::RecVertex*,
                                   LHCb::Vertex::ConstVector&,
                                   LHCb::Particle::ConstVector&);
  

  virtual LHCb::Tagger  tagReco14( const LHCb::Particle*, 
                                   const LHCb::RecVertex*,
                                   LHCb::Vertex::ConstVector&,
                                   LHCb::Particle::ConstVector&);
  */
 //-------------------------------------------------------------

private:
  MCElectronOSWrapper*   m_myMCreader;
  ElectronOSWrapper*     m_myDATAreader;

  INNetTool* m_nnet;
  ITaggingUtils*  m_util;
  IParticleDescendants* m_descend;
  ICaloElectron*  m_electron;

  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_ele;
  double m_P_cut_ele;
  double m_IPs_cut_ele;
  double m_VeloChMin;
  double m_VeloChMax;
  double m_EoverP, m_EoverPmax, m_lcs_cut_ele;
  double m_AverageOmega;
  double m_ghost_cut_ele;
  double m_PIDe_cut;
  double m_ipPU_cut_ele;
  double m_distPhi_cut_ele;
  double m_ProbMin_ele;
  double m_P0_Cal_ele ;
  double m_P1_Cal_ele ;
  double m_Eta_Cal_ele ;

  double m_ghostProb_ele;
  double m_PIDNNk_cut_ele;
  double m_PIDNNp_cut_ele;
  double m_PIDNNpi_cut_ele;
  double m_PIDNNe_cut_ele;
  double m_PIDNNepi_cut_ele;
  
  std::string m_personality;
  int m_isMonteCarlo;
  double m_P0e, m_P1e, m_P2e, m_P3e;
  
};

//===============================================================//
#endif // USER_TAGGERELECTRONTOOL_H
