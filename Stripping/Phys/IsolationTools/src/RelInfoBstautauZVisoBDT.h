// $Id: TupleToolMuonVariables.h 
#ifndef RELINFOBSTAUTAUZVISOBDTV2_H 
#define RELINFOBSTAUTAUZVISOBDTV2_H 1

// Include files
#include "Kernel/IEventTupleTool.h"            // Interface
#include "GaudiKernel/ToolFactory.h" 
//#include "Kernel/IParticleTupleTool.h" 
//#include "DecayTreeTupleBase/TupleToolBase.h"
#include "Kernel/IDVAlgorithm.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Event/RecVertex.h"
#include "LoKi/ParticleCuts.h"
#include "LoKi/AParticleCuts.h"
#include "LoKi/ParticleContextCuts.h"
#include "TH1D.h"
#include "IncTopoVert/IVertexFunctionTool.h"
#include "IncTopoVert/ITopoVertexTool.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/IBackgroundCategory.h"
#include "Kernel/ILifetimeFitter.h"
#include "Kernel/ITriggerTisTos.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IDistanceCalculator.h"
#include "Kernel/IParticleIsolation.h"
#include "Kernel/IParticleTransporter.h"
#include "OTDAQ/IOTRawBankDecoder.h"
#include "Event/RelatedInfoMap.h"
#include "GaudiAlg/GaudiTool.h"
#include <TMVA/Tools.h>
#include <TMVA/Reader.h>
#include "MVADictTools/TMVATransform.h"
#include "CaloUtils/CaloParticle.h"
#include "Kernel/DaVinciAlgorithm.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IRelatedInfoTool.h"
#include "Kernel/IParticleDictTool.h"
//#include "IsolationInfo.h"



//#if not defined(__CINT__) || defined(__MAKECINT__)
//#include "TMVA/Reader.h"
//#endif




/** @class RelInfoBstautauZVisoBDT RelInfoBstautauZVisoBDT.h

 * 
 *  @author Alessadro Morda'
 *  @date   2012-09-13
 */

using namespace TMVA;
using namespace std;

class IDistanceCalculator;
class IDVAlgorithm;
class IPVReFitter;
class IVertexFunctionTool;
class ITopoVertexTool;
class IDVAlgorithm;
//class Reader;



class RelInfoBstautauZVisoBDT : public GaudiTool, virtual public IRelatedInfoTool/*, virtual public IParticleTupleTool*/ {
public: 



  /// Standard constructor
  RelInfoBstautauZVisoBDT( const std::string& type, 
              const std::string& name,
              const IInterface* parent );
  

  virtual StatusCode calculateRelatedInfo( const LHCb::Particle* ,
                                       const LHCb::Particle*	);
  virtual LHCb::RelatedInfoMap* getInfo(void);
  
  virtual std::string infoPath(void);

  virtual ~RelInfoBstautauZVisoBDT( ); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  
  // virtual StatusCode finalize();
  

  typedef std::vector<LHCb::RecVertex*> RecVertexVector;
  typedef std::vector<const LHCb::Track*> TrackVector;

private:

        /// Check if a pure CALO Particle
        inline bool isPureNeutralCalo(const LHCb::Particle* P)const
        {
            LHCb::CaloParticle caloP(  (LHCb::Particle*) P );
            return caloP.isPureNeutralCalo();
        }  // Helpers
private:

  //   std::vector<std::string> m_variables;
  LHCb::RelatedInfoMap m_map;
  std::vector<short int> m_keys; 
  std::vector<std::string> m_variables;
  //std::vector<const LHCb::Particle*> m_decayParticles;
  double m_pvdis_h;
  double m_pvdis;
  double m_svdis_h;
  double m_svdis;
  int    m_tracktype;
  double m_ips;
  double m_fc;
  double m_angle;

  double m_cosnk;
  double m_doca_iso;

  double m_cut_ips;
  bool   m_is_data;
  double m_cut_ghost;
  double m_cut_ips_VF;
  double m_cut_ips_high;
  double m_cut_imp;
  bool   m_recoverVF;
  bool   m_recover;
  bool   m_VF_from_MC;
  int    m_nvertex;
  int    m_event;
  double m_B_sPVz;
    
  float m_zv_ipsall;
  float m_zv_ipall   ;
  float m_zv_proba    ;
  float m_zv_proba_close ;
  float m_zv_chi2      ;
  float m_zv_dist      ;
  float m_zv_ntrkvtx  ;


  float  m_z_first_max;
  float  m_Vf_B;
  float  m_nb_peak;
  float  m_nb_peak_DwS;
  float  m_nb_peak_UpS;
  float  m_Vf_Muons_Max;
  float  m_Position_Muons_Max;
  float  m_Vf_Muons_Max_DwS;
  float  m_Position_Muons_Max_DwS ;
  float  m_Vf_Muons_Max_UpS;
  float  m_Position_Muons_Max_UpS;
  float  m_sum_VF_maxs;
  float  m_sum_VF_maxs_UpStr;
  float  m_zv_same;
  float  m_zv_nvtx;

  float m_par_zv_ipsall ;
  float m_par_zv_ipall ;
  float m_par_zv_proba ;
  float m_par_zv_proba_close  ;
  float m_par_zv_chi2  ;
  float m_par_zv_dist  ;
  float m_par_zv_ntrkvtx ; 

  float m_min_zv_ipsall ;
  float m_min_zv_ipall ;
  float m_min_zv_proba ;
  float m_min_zv_proba_close  ;
  float m_min_zv_chi2  ;
  float m_min_zv_dist  ;
  float m_min_zv_ntrkvtx ; 

  float m_max_zv_ipsall ;
  float m_max_zv_ipall ;
  float m_max_zv_proba ;
  float m_max_zv_proba_close  ;
  float m_max_zv_chi2  ;
  float m_max_zv_dist  ;
  float m_max_zv_ntrkvtx ; 
  int m_n_step_m;
  int m_n_step_p;
  float m_step;
  float m_cut;
  int m_cut_ntrk;
    
  
 
  std::string m_transformName ;
  
  
  TMVATransform m_tmva ;
  TMVATransform::optmap m_optmap ;
  IParticleDictTool::DICT m_varmap ; 
  IParticleDictTool::DICT m_out ; 
  



  std::string m_Weights_BDTG_ZViso;
  float m_ZViso;
  float m_ZViso_TauP;
  float m_ZViso_TauM;

  std::string m_TracksPath;
  std::string m_ParticlePath;
  std::string m_PVInputLocation;


  std::vector<const LHCb::Track*> m_tracksVF;
  std::vector<const LHCb::Track*> m_tracksZVTop;




  ITopoVertexTool* m_topotool;

  IVertexFunctionTool* m_vertextool;
  const IRelatedPVFinder* pFinder; 
  IDistanceCalculator*  m_Geom;
  IParticleTransporter* m_transporter;
  IParticleDescendants* m_descend;
  
  TMVA::Reader* m_reader_ZViso;//= new TMVA::Reader( "!Color:!Silent" );
   
  

  StatusCode Initialize_tracksVF_ZVtop();
  StatusCode IsoTopo2Body(const LHCb::Particle *part);  //Isolation of Mathieu
  double VfAlongTrack(double zB, const LHCb::Particle* Part, double delta, float* z_f_m );//
  int VfProfile(double zB,const LHCb::Particle* Part, std::vector<double> *v_Vf, std::vector<double>* v_Position);//
  StatusCode GMPiso(const LHCb::Particle *part );//
  std::vector<LHCb::RecVertex*> zvtop(const LHCb::Particle* dau1, const LHCb::Particle* dau2, const LHCb::Particle* dau3 );//
  void zvtop_iso(const LHCb::Particle* dau, const std::vector<LHCb::RecVertex*> vertices, const LHCb::VertexBase* primary);//
  Gaudi::XYZPoint close_point(Gaudi::XYZPoint o, Gaudi::XYZPoint o_mu, Gaudi::XYZVector p_mu);// 
  StatusCode ZViso( );//

  

};



#endif // TUPLETOOLMUONVARIABLES_H
