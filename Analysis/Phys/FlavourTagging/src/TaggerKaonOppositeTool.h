// $Id: TaggerKaonOppositeTool.h,v 1.11 2010-06-17 17:46:03 mgrabalo Exp $
#ifndef USER_TAGGERKAONOPPOSITETOOL_H
#define USER_TAGGERKAONOPPOSITETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// from local
#include "INNetTool.h"
#include "Kernel/IParticleDescendants.h"
#include "ITaggingUtils.h"

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
  StatusCode initialize();    ///<  initialization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

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
  double m_PID_k_cut;
  double m_PIDkp_cut;
  double m_ipPU_cut_kaon;
  double m_distPhi_cut_kaon;
  double m_ProbMin_kaon;
  double m_P0_Cal_kaon ;
  double m_P1_Cal_kaon ;
  double m_Eta_Cal_kaon ;


};

//===============================================================//
#endif // USER_TAGGERKAONOPPOSITETOOL_H
