// $Id: TaggerMuonTool.h,v 1.9 2010-06-17 17:46:04 mgrabalo Exp $
#ifndef USER_TAGGERMUONTOOL_H
#define USER_TAGGERMUONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
// from ROOT
#include <TROOT.h>

// from local
#include "ITaggingUtils.h"
#include "Kernel/IParticleDescendants.h"
#include "INNetTool.h"
#include <list>
#include <utility>
#include <string>

#include "MultiplePersonalityCall.h"

/** @class TaggerMuonTool TaggerMuonTool.h
 *
 *  Tool to tag the B flavour with a Muon Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerMuonTool : public GaudiTool,
                       virtual public ITagger {

public:
  /// Standard constructor
  TaggerMuonTool( const std::string& type,
                  const std::string& name,
                  const IInterface* parent );
  virtual ~TaggerMuonTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization

  //-------------------------------------------------------------
  MultiplePersonalityCall<boost::function<
  LHCb::Tagger(
               const LHCb::Particle*, 
               const LHCb::RecVertex*,
               std::vector<const LHCb::Vertex*>&, 
               LHCb::Particle::ConstVector&) > > 
  m_tag;

  virtual LHCb::Tagger  tag( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             std::vector<const LHCb::Vertex*>&, 
                             LHCb::Particle::ConstVector&);

  virtual LHCb::Tagger  tagReco12( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             std::vector<const LHCb::Vertex*>&, 
                             LHCb::Particle::ConstVector&);

  virtual LHCb::Tagger  tagReco14( const LHCb::Particle*, 
                             const LHCb::RecVertex*,
                             std::vector<const LHCb::Vertex*>&, 
                             LHCb::Particle::ConstVector&);

  //-------------------------------------------------------------

private:

  INNetTool* m_nnet;
  IParticleDescendants* m_descend;
  ITaggingUtils* m_util;

  IDataProviderSvc* m_eventSvc;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_muon;
  double m_P_cut_muon;
  double m_IPs_cut_muon;
  double m_lcs_cut_muon;
  double m_PIDm_cut;
  double m_ipPU_cut_muon;
  double m_distPhi_cut_muon;
  double m_ProbMin_muon;
double m_ghostprob_cut;
double m_PIDNNm_cut_muon;
double m_PIDNNpi_cut_muon;
double m_PIDNNe_cut_muon;
double m_PIDNNk_cut_muon;
double m_PIDNNp_cut_muon;
  double m_AverageOmega;
  double m_P0_Cal_muon ;
  double m_P1_Cal_muon ;
  double m_Eta_Cal_muon ;
  std::string m_personality;
};

//===============================================================//
#endif // USER_TAGGERMUONTOOL_H
