#ifndef TaggerVertexChargeTool_H
#define TaggerVertexChargeTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "SVertexOneSeedTool.h"
#include "NNetTool_MLP.h"

using namespace std;

/** @class TaggerVertexChargeTool
 *
 *  Tagger Tool for the Vertex Charge
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */

class TaggerVertexChargeTool  : public GenericTool {

 public:

  TaggerVertexChargeTool( );
  ~TaggerVertexChargeTool( ){ delete m_svtool; delete tVch; } ///< Destructor

  Tagger* tag(Event& );

 private:

  double  m_PowerK,  m_MinimumVCharge, m_ProbMin_vtx,m_P0 , m_AverageOmega,
    m_P1, m_Gt075 ,  m_wSameSign2,  m_wSameSignMoreThan2 ;
  double m_P0_Cal_vtx ;
  double m_P1_Cal_vtx ;
  double m_Eta_Cal_vtx ;

  TString m_CombinationTechnique, m_SecondaryVertexToolName;

  bool  m_UseObsoleteSV;

  SVertexOneSeedTool* m_svtool;
  NNetTool_MLP nnet;
  Tagger* tVch;

};
#endif 
