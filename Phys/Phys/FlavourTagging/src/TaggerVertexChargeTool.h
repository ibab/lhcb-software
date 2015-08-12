// $Id: TaggerVertexChargeTool.h,v 1.7 2010-06-17 17:46:04 mgrabalo Exp $
#ifndef USER_TAGGERVERTEXCHARGETOOL_H
#define USER_TAGGERVERTEXCHARGETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"

#include "Kernel/IVertexFit.h"

// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "Kernel/ISecondaryVertexTool.h"
// from ROOT
#include <TROOT.h>

// from local
#include "MCVertexOSWrapper.h"
#include "VertexOSWrapper.h"
#include "FlavourTagging/ITaggingUtils.h"
#include "INNetTool.h"
#include <list>
#include <utility>
#include <string>

#include "MultiplePersonalityCall.h"


/** @class TaggerVertexChargeTool TaggerVertexChargeTool.h
 *
 *  Tool to tag the B flavour with a VertexCharge Tagger
 *
 *  @author Marco Musy
 *  @date   30/06/2005
 */

class TaggerVertexChargeTool : public GaudiTool,
                               virtual public ITagger {

public:
  /// Standard constructor
  TaggerVertexChargeTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );
  virtual ~TaggerVertexChargeTool( ); ///< Destructor
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
  //-------------------------------------------------------------

private:
  MCVertexOSWrapper * m_myMCreader;
  VertexOSWrapper * m_myDATAreader;
  
  INNetTool* m_nnet;
  ITaggingUtils* m_util;
  ISecondaryVertexTool* m_svtool;
  std::string m_SecondaryVertexToolName;

  std::string m_CombinationTechnique;
  std::string m_NeuralNetName;
  double m_AverageOmega, m_PowerK, m_MinimumVCharge;
  double m_ProbMin_vtx;
  double m_Ptsum_vtx, m_Ptmean_vtx, m_IPSsum_vtx, m_DocaMaxsum_vtx, m_Psum_vtx, m_Msum_vtx;
  double m_P0_Cal_vtx ;
  double m_P1_Cal_vtx ;
  double m_Eta_Cal_vtx ;
  double m_wSameSign2, m_wSameSignMoreThan2, m_P0, m_P1, m_Gt075;
  std::string m_personality;
  int    m_isMonteCarlo;
  double m_P0vtx, m_P1vtx, m_P2vtx, m_P3vtx;  
};

//===============================================================//
#endif // USER_TAGGERVERTEXCHARGETOOL_H
