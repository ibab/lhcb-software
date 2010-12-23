#ifndef TaggerElectronTool_H
#define TaggerElectronTool_H 1

#include <iostream>
#include <vector>
#include <TLorentzVector.h>
#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"
#include "NNetTool_MLP.h"

#include "TH1F.h"

using namespace std;

/** @class TaggerElectronTool
 *
 *  Tool to retrieve a tagger by using electrons 
 *   from the opposite B meson in the event
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class TaggerElectronTool : public GenericTool {

public:

  TaggerElectronTool( );
  ~TaggerElectronTool( ){delete tele;} ///< Destructor

  Tagger* tag(Event& );

private:

  double m_Pt_cut_ele ;
  double m_P_cut_ele;
  double m_lcs_cut_ele;
  double m_ghost_cut_ele;
  double m_VeloChMin  ;
  double m_VeloChMax  ;
  double m_EoverP ;
  double m_IPs_cut_ele;
  double m_ipPU_cut_ele;
  double m_distPhi_cut_ele;
  double m_ProbMin_ele ;
  double m_P0_Cal_ele ;
  double m_P1_Cal_ele ;
  double m_Eta_Cal_ele ;
  
  NNetTool_MLP nnet;
  Tagger* tele;

  TH1F* hcut_ele_pidk;
  TH1F* hcut_ele_pt  ;
  TH1F* hcut_ele_p ;  
  TH1F* hcut_ele_lcs ;
  TH1F* hcut_ele_gho ;
  TH1F* hcut_ele_EoP ;
  TH1F* hcut_ele_velo;
  TH1F* hcut_ele_ippu;
  TH1F* hcut_ele_distphi;
  TH1F* hcut_ele_N;
  TH1F* hcut_ele_ips ;

};

#endif 
