// $Id: TaggerPionSameTool.h,v 1.9 2009-12-11 15:15:05 musy Exp $
#ifndef USER_TAGGERPIONSAMETOOL_H
#define USER_TAGGERPIONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Event/ProtoParticle.h"
#include "Event/FlavourTag.h"

#include "Kernel/ITagger.h"

// from local
#include "MCPionSSWrapper.h"
#include "FlavourTagging/ITaggingUtils.h"
#include "Kernel/IParticleDescendants.h"
#include "INNetTool.h"

/** @class TaggerPionSameTool TaggerPionSameTool.h
 *
 *  Tool to tag the B flavour with a PionSame Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerPionSameTool : public GaudiTool,
                           virtual public ITagger {

public:
  /// Standard constructor
  TaggerPionSameTool( const std::string& type,
                      const std::string& name,
                      const IInterface* parent );
  virtual ~TaggerPionSameTool( ); ///< Destructor
  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:
  MCPionSSWrapper * m_myMCreader;
  
  INNetTool* m_nnet;
  IParticleDescendants* m_descend;
  ITaggingUtils* m_util;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_pionS;
  double m_P_cut_pionS;
  double m_IPs_cut_pionS;
  double m_eta_max_cut_pionS;
  double m_eta_min_cut_pionS;
  double m_phi_cut_pionS;
  double m_dR_cut_pionS;
  double m_dQcut_pionS;
  double m_dQcut_extra_pionS;
  double m_AverageOmega;
  double m_PionProbMin, m_lcs_cut;
  double m_ghost_cut;
  double m_PionSame_PIDNoK_cut;
  double m_PionSame_PIDNoP_cut;
  double m_ipPU_cut_pS, m_distPhi_cut_pS;
  double m_P0_Cal_pionS ;
  double m_P1_Cal_pionS ;
  double m_P2_Cal_pionS ;
  double m_Eta_Cal_pionS ;
  int    m_isMonteCarlo;
  double m_P0ps, m_P1ps, m_P2ps, m_P3ps;
  
};
//===============================================================//
#endif // USER_TAGGERPIONSAMETOOL_H
