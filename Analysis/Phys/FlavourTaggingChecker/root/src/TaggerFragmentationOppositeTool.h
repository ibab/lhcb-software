#ifndef TaggerFragmentationOppositeTool_H
#define TaggerFragmentationOppositeTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "NNetTool_MLP.h"

#include "TH1F.h"


using namespace std;

/** @class TaggerFragmentOppositeTool
 *
 *  Tool to retrieve a tagger by using kaons 
 *   from the opposite B meson in the event
 *
 *  @author Marco Musy
 *  @date   2010-09-07
 */
class TaggerFragmentationOppositeTool : public GenericTool {

 public:

  TaggerFragmentationOppositeTool( ) ;
  ~TaggerFragmentationOppositeTool( ){ delete tfrag;} ///< Destructor

  Tagger* tag(Event& );

 private:
  double m_Pt_cut_frag;
  double m_P_cut_frag ; 
  double m_IPs_cut_frag;
  double m_IP_cut_frag;
  double m_lcs_frag;
  double m_ghost_cut;
  double m_IP_kl ;
  double m_IP_ku;
  double m_ProbMin_frag;

  NNetTool_MLP nnet;

  Tagger* tfrag;

  TH1F*   hcut_frag_pidk;
  TH1F*   hcut_frag_pidp;
  TH1F*   hcut_frag_pt  ;
  TH1F*   hcut_frag_p   ;
  TH1F*   hcut_frag_lcs ;
  TH1F*   hcut_frag_gho ;
  TH1F*   hcut_frag_IPs ;
  TH1F*   hcut_frag_IP  ;

};  

#endif 
