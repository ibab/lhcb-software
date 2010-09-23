#ifndef NNETTOOL_MLP_H 
#define NNETTOOL_MLP_H 1

#include "GenericTool.h"
#include "TString.h"

#include "NNmuon.h"
#include "NNele.h"
#include "NNkaon.h"
#include "NNkaonS.h"
#include "NNpionS.h"
#include "NNvtx.h"

/** @class NNetTool_MLP
 *
 *  Tool that calculates the mistag value for a tagger
 *
 *  @author Marco Musy
 *  @date   2010-06-29
 */
class NNetTool_MLP : public GenericTool {

public:
  /// Standard constructor
  NNetTool_MLP( );
  ~NNetTool_MLP( ){}; ///< Destructor
  
  double MLPm(std::vector<double>&);
  double MLPe(std::vector<double>&);
  double MLPk(std::vector<double>&);
  double MLPkS(std::vector<double>&);
  double MLPpS(std::vector<double>&);
  double MLPvtx(std::vector<double>&);

private:

  void normaliseOS(std::vector<double>& );
  void normaliseSS(std::vector<double>& );
  void normaliseVtx(std::vector<double>& );

  double m_P0mu, m_P1mu, m_P0e, m_P1e, m_P0k, m_P1k; 
  double m_P0ks, m_P1ks, m_P0ps, m_P1ps, m_P0vtx, m_P1vtx, m_P2vtx, m_P3vtx;  
  double m_Gt075, m_wSameSign2, m_wSameSignMoreThan2;
  double m_ProbMinVtx, m_MinimumVCharge;

};
#endif // NNETTOOL_MLP_H
