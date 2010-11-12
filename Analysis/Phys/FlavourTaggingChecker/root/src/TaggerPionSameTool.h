#ifndef TaggerPionSameTool_H
#define TaggerPionSameTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "NNetTool_MLP.h"

#include "TH1F.h"

using namespace std;

/** @class TaggerPionSameTool
 *
 *  Tool to retrieve a tagger by using pions 
 *   from fragmentation close to signal B meson
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */

class TaggerPionSameTool  : public GenericTool {
 
 public:
 
  TaggerPionSameTool( );
  ~TaggerPionSameTool( ){ delete tpionS; } ///< Destructor

  Tagger* tag(Event& );

 private:

  double  m_PionProbMin, m_Pt_cut_pionS, m_P_cut_pionS, m_IPs_cut_pionS, 
    m_lcs_cut,m_dQcut_pionS,  m_dQcut_extra_pionS, m_ghost_cut_pS;
  double m_ipPU_cut_pS, m_distPhi_cut_pS;
  NNetTool_MLP nnet;
  Tagger* tpionS;

  TH1F* hcut_pS_pt ;
  TH1F* hcut_pS_p   ;
  TH1F* hcut_pS_lcs ;
  TH1F* hcut_pS_IPs ;
  TH1F* hcut_pS_gho ;
  TH1F* hcut_pS_dq  ;
  TH1F* hcut_pS_dqe ;
  TH1F* hcut_pS_ippu ;
  TH1F* hcut_pS_distphi ;
};

#endif 
