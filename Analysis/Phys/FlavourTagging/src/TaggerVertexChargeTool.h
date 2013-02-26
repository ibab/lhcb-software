// $Id: TaggerVertexChargeTool.h,v 1.7 2010-06-17 17:46:04 mgrabalo Exp $
#ifndef USER_TAGGERVERTEXCHARGETOOL_H
#define USER_TAGGERVERTEXCHARGETOOL_H 1

// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/AlgTool.h"
#include "GaudiKernel/ToolFactory.h"

#include "Kernel/IVertexFit.h"

// from Event
#include "Event/FlavourTag.h"
#include "Kernel/ITagger.h"
#include "Kernel/ISecondaryVertexTool.h"

// from local
#include "ITaggingUtils.h"
#include "INNetTool.h"


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
  StatusCode initialize();    ///<  initialization

  //-------------------------------------------------------------
  virtual LHCb::Tagger tag( const LHCb::Particle*, const LHCb::RecVertex*,
                            std::vector<const LHCb::Vertex*>&,
                            LHCb::Particle::ConstVector& );
  //-------------------------------------------------------------

private:

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

};

//===============================================================//
#endif // USER_TAGGERVERTEXCHARGETOOL_H
