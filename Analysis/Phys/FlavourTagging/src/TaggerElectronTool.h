// $Id: TaggerElectronTool.h,v 1.13 2010-06-17 17:46:03 mgrabalo Exp $
#ifndef USER_TAGGERELECTRONTOOL_H
#define USER_TAGGERELECTRONTOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Kernel/ITagger.h"
#include "CaloUtils/ICaloElectron.h"

// from local
#include "ITaggingUtils.h"
#include "INNetTool.h"
#include "Kernel/IParticleDescendants.h"

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
  StatusCode initialize();    ///<  initialization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:
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
  double m_EoverP, m_lcs_cut_ele;
  double m_AverageOmega;
  double m_ghost_cut_ele;
  double m_PIDe_cut;
  double m_ipPU_cut_ele;
  double m_distPhi_cut_ele;
  double m_ProbMin_ele;
  double m_P0_Cal_ele ;
  double m_P1_Cal_ele ;
  double m_Eta_Cal_ele ;

};

//===============================================================//
#endif // USER_TAGGERELECTRONTOOL_H
