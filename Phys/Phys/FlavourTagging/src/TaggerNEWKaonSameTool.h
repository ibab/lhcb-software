// $Id: TaggerNEWKaonSameTool.h,v 1.0 2013-01-23 $
#ifndef USER_TAGGERNEWKAONSAMETOOL_H
#define USER_TAGGERNEWKAONSAMETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// from local
//#include "ITMVANNetTool.h"
#include "Kernel/IParticleDescendants.h"
#include "FlavourTagging/ITaggingUtils.h"

#include <tr1/array>

#include "NEWKaonSame_NN1.h"
#include "NEWKaonSame_NN2.h"

/** @class TaggerNEWKaonSameTool TaggerNEWKaonSameTool.h
 *
 *  Tool to tag the B flavour with the new KaonSame Tagger
 *
 *  @author Georg Krocker
 *  @date   03/04/2013
 */

class TaggerNEWKaonSameTool : public GaudiTool,
                                  virtual public ITagger {

public:
  /// Standard constructor
  TaggerNEWKaonSameTool( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );
  virtual ~TaggerNEWKaonSameTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, 
                            const LHCb::RecVertex*,
                            const int,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend;
  std::string m_CombinationTechnique, m_NeuralNetName;


  NN1ReaderCompileWrapper*   mynn1_reader;
  NN2ReaderCompileWrapper*   mynn2_reader;


  double m_PID_k_cut;
  double m_PIDkp_cut;
  double m_ghost_cut;
  double m_distPhi_cut_kaon;

  double m_AverageOmega;
  double m_ProbMin_kaon;
  double m_ProbMax_kaon;
  double m_P0_Cal_kaon ;
  double m_P1_Cal_kaon ;

  //double m_ipPU_cut_kaon;
  double m_NN1_cut_kaon;

  double m_nn_1;
  double m_nn_2;


};

//===============================================================//
#endif // USER_TAGGERNEWKAONSAMETOOL_H
