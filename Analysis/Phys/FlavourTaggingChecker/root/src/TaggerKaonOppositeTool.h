#ifndef TaggerKaonOppositeTool_H
#define TaggerKaonOppositeTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "NNetTool_MLP.h"

#include "TH1F.h"


using namespace std;

/** @class TaggerKaonOppositeTool
 *
 *  Tool to retrieve a tagger by using kaons 
 *   from the opposite B meson in the event
 *
 *  @author Marco Musy
 *  @date   2010-09-07
 */
class TaggerKaonOppositeTool : public GenericTool {

 public:

  TaggerKaonOppositeTool( ) ;
  ~TaggerKaonOppositeTool( ){ delete tkaon;} ///< Destructor

  Tagger* tag(Event& );

 private:
  double m_Pt_cut_kaon;
  double m_P_cut_kaon ; 
  double m_IPs_cut_kaon;
  double m_IP_cut_kaon;
  double m_lcs_kaon;
  double m_ghost_cut_kaon;
  double m_ipPU_cut_kaon;
  double m_distPhi_cut_kaon;
  double m_ProbMin_kaon;

  NNetTool_MLP nnet;

  Tagger* tkaon;

  TH1F*   hcut_ko_pidk;
  TH1F*   hcut_ko_pidp;
  TH1F*   hcut_ko_pt  ;
  TH1F*   hcut_ko_p   ;
  TH1F*   hcut_ko_lcs ;
  TH1F*   hcut_ko_gho ;
  TH1F*   hcut_ko_IPs ;
  TH1F*   hcut_ko_IP  ;
  TH1F*   hcut_ko_ippu;
  TH1F*   hcut_ko_distphi;
  TH1F*   hcut_ko_N   ;

};  

#endif 
