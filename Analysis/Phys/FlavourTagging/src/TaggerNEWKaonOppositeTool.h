// $Id: TaggerNEWKaonOppositeTool.h,v 1.0 2013-01-23 $
#ifndef USER_TAGGERNEWKAONOPPOSITETOOL_H 
#define USER_TAGGERNEWKAONOPPOSITETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"
// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"

// from local
//#include "ITMVANNetTool.h"
#include "Kernel/IParticleDescendants.h"
#include "ITaggingUtils.h"

#include <tr1/array>

#include "NEWKaonOpposite_NN1.h"
#include "NEWKaonOpposite_NN2.h"

/** @class TaggerNEWKaonOppositeTool TaggerNEWKaonOppositeTool.h 
 *
 *  Tool to tag the B flavour with the new KaonOpposite Tagger
 *
 *  @author Katharina Kreplin
 *  @date   23/01/2013
 */

class TaggerNEWKaonOppositeTool : public GaudiTool,
                               virtual public ITagger {

public:
  /// Standard constructor
  TaggerNEWKaonOppositeTool( const std::string& type,
			  const std::string& name,
			  const IInterface* parent );
  virtual ~TaggerNEWKaonOppositeTool( ); ///< Destructor
  StatusCode initialize();    ///<  initialization
  StatusCode finalize  ();    ///<  finalization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&,
                            LHCb::Particle::ConstVector&);
  //-------------------------------------------------------------

private:

  ITaggingUtils* m_util;
  IParticleDescendants* m_descend;
  std::string m_CombinationTechnique, m_NeuralNetName;


  MCReaderCompileWrapper*   mymc_reader;
  DataReaderCompileWrapper* mydata_reader;


  double m_PID_k_cut;
  double m_PIDkp_cut;
  double m_ghost_cut;
  double m_distPhi_cut_kaon;

  double m_AverageOmega;
  double m_ProbMin_kaon;
  double m_ProbMax_kaon;
  double m_P0_Cal_kaon ;
  double m_P1_Cal_kaon ;

  double m_ipPU_cut_kaon;
  double m_NN1_cut_kaon;

  double m_nn_1;
  double m_nn_2;


};

//===============================================================//
#endif // USER_TAGGERNEWKAONOPPOSITETOOL_H
