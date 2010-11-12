#ifndef TaggerKaonSameTool_H
#define TaggerKaonSameTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "NNetTool_MLP.h"

#include "TH1F.h"

using namespace std;

/** @class TaggerKaonSameTool
 *
 *  Tool to retrieve a tagger by using kaons 
 *   from fragmentation of Signal B meson in the event
 *
 *  @author Marco Musy
 *  @date   2010-09-07
 */
class TaggerKaonSameTool : public GenericTool {

 public:

  TaggerKaonSameTool( );
  ~TaggerKaonSameTool( ){ delete tkaonS; } ///< Destructor

  Tagger* tag(Event& );

 private:

  double m_Pt_cut_kaonS, m_P_cut_kaonS, m_IP_cut_kaonS, 
    m_phicut_kaonS, m_etacut_kaonS, m_dQcut_kaonS, m_lcs_cut, m_ProbMin_kaonS;
  double m_KaonSPID_kS_cut, m_KaonSPID_kpS_cut;
  double m_ipPU_cut_kS, m_distPhi_cut_kS;
  NNetTool_MLP nnet;
  Tagger* tkaonS;

  TH1F* hcut_kS_pidk;
  TH1F* hcut_kS_pidp;
  TH1F* hcut_kS_pt ; 
  TH1F* hcut_kS_p ;  
  TH1F* hcut_kS_lcs; 
  TH1F* hcut_kS_IPs; 
  TH1F* hcut_kS_dphi;
  TH1F* hcut_kS_deta;
  TH1F* hcut_kS_dq ; 
  TH1F* hcut_kS_ippu ;
  TH1F* hcut_kS_distphi ;
  TH1F* hcut_kS_N   ;
 
};

#endif 
