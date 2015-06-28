// $Id: TaggerPionBDTSameTool.h,v 1.9 2013-11-29 svecchi Exp $
#ifndef USER_TAGGERPIONBDTSAMETOOL_H
#define USER_TAGGERPIONBDTSAMETOOL_H 1

#include <cmath>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/ProtoParticle.h"
#include "Event/FlavourTag.h"

#include "Kernel/ITagger.h"
#include "Math/Boost.h"
// from local
#include "FlavourTagging/ITaggingUtils.h"
#include "Kernel/IParticleDescendants.h"
//#include "INNetTool.h"
#include "Kernel/IVertexFit.h"
#include "PionSSWrapper.h"

/** @class TaggerPionBDTSameTool TaggerPionBDTSameTool.h
 *
 *  Tool to tag the B flavour with a PionBDTSame Tagger
 *
 *  @author Stefania Vecchi
 * (tool developed by Antonio Falabella, Marta Calvi and
 *  Vava Gligorov )
 *  @date   29/11/2013
 */

class TaggerPionBDTSameTool : public GaudiTool,
                              virtual public ITagger {

public:
  /// Standard constructor
  TaggerPionBDTSameTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );
  virtual ~TaggerPionBDTSameTool( ); ///< Destructor

  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();      ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*,
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  PionSSWrapper* m_myBDT_reader;

  //  INNetTool* m_nnet;
  const IVertexFit *m_fitter;
  //IParticleDescendants* m_descend;
  ITaggingUtils* m_util;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_pionS;
  double m_Bp_Pt_cut_pionS;
  double m_Bp_vtxChi2_cut_pionS;
  //double m_P_cut_pionS;
  double m_IPs_cut_pionS;
  double m_eta_cut_pionS;
  double m_phi_cut_pionS;
  //double m_dR_cut_pionS;
  double m_dQcut_pionS;
  //  double m_BDT_cut_pionS;
  double m_AverageOmega;
  double m_PionProbMin;
  double m_ghostprob_cut;
  double m_PIDp_cut_pionS;
  double m_PIDk_cut_pionS;

  double m_P0_pol_pionS ;
  double m_P1_pol_pionS ;
  double m_P2_pol_pionS ;
  double m_P3_pol_pionS ;

  int    m_isMonteCarlo;

};
//===============================================================//
#endif // USER_TAGGERPIONBDTSAMETOOL_H
