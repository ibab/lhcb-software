// $Id: TaggerProtonSameTool.h,v 1.9 2013-11-29 svecchi Exp $
#ifndef USER_TAGGERPROTONSAMETOOL_H
#define USER_TAGGERPROTONSAMETOOL_H 1

#include <cmath>
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
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
//#include "TMVAClassification/BDT_SSproton/BDT_SSproton_Reco14.class.C"
#include "ProtonSSWrapper.h"

/** @class TaggerProtonSameTool TaggerProtonSameTool.h
 *
 *  Tool to tag the B flavour with a ProtonSame Tagger
 *
 *  @author Stefania Vecchi
 * (tool developed by Antonio Falabella, Marta Calvi and
 *  Vava Gligorov, Davide Fazzini )
 *  @date   29/11/2013
 */

class TaggerProtonSameTool : public GaudiTool,
                             virtual public ITagger {

public:
  /// Standard constructor
  TaggerProtonSameTool( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );
  virtual ~TaggerProtonSameTool( ); ///< Destructor

  virtual StatusCode initialize();    ///<  initialization
  virtual StatusCode finalize();      ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*,
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  ProtonSSWrapper* m_myBDT_reader;

  //  INNetTool* m_nnet;
  const IVertexFit *m_fitter;
  //IParticleDescendants* m_descend;
  ITaggingUtils* m_util;
  std::string m_CombinationTechnique, m_NeuralNetName;

  //properties
  double m_Pt_cut_protonS;
  double m_Bp_Pt_cut_protonS;
  double m_Bp_vtxChi2_cut_protonS;
  //double m_P_cut_protonS;
  double m_IPs_cut_protonS;
  double m_eta_cut_protonS;
  double m_phi_cut_protonS;
  //double m_dR_cut_protonS;
  double m_dQcut_protonS;
  double m_BDT_cut_protonS;
  double m_AverageOmega;
  double m_ProtonProbMin, m_ghostprob_cut;

  double m_PIDp_cut_protonS;
  //double m_ipPU_cut_pS, m_distPhi_cut_pS;
  double m_P0_pol_protonS ;
  double m_P1_pol_protonS ;
  double m_P2_pol_protonS ;
  double m_P3_pol_protonS ;

  int    m_isMonteCarlo;

};
//===============================================================//
#endif // USER_TAGGERPROTONSAMETOOL_H
