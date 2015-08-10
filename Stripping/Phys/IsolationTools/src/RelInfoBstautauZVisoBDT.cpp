// Include files
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/AlgFactory.h"
#include "Event/Particle.h"
// kernel
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
//#include "Kernel/IOnOffline.h"
#include <Kernel/IDVAlgorithm.h>
#include <Kernel/GetIDVAlgorithm.h>
//#include "Kernel/DVAlgorithm.h"
#include "Kernel/IParticleCombiner.h"
#include "Kernel/RelatedInfoNamed.h"

// MC stuff
//#include "Event/GenHeader.h"
//#include "Event/MCHeader.h"

#include "RelInfoBstautauZVisoBDT.h"
#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
//#include "TMVA/Reader.h"
#include "Math/Boost.h"
#include "Event/VertexBase.h"
#include "Event/RecVertex.h"
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"
#include "GaudiKernel/IRegistry.h" //
#include "Event/TrackTypes.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/RelationWeighted.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/RelationWeighted2D.h"

#include <TMVA/Tools.h>
#include <TMVA/Reader.h>


//#if not defined(__CINT__) || defined(__MAKECINT__)

//#endif




//-----------------------------------------------------------------------------
// Implementation file for class : TupleZVtop
//
// 2014-05-08 : Alessandro Morda'
//-----------------------------------------------------------------------------

using namespace TMVA;
using namespace Gaudi;
using namespace LHCb;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted ;
template <class FROM,class TO,class WEIGHT> class IRelationWeighted2D ;



// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoBstautauZVisoBDT )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoBstautauZVisoBDT::RelInfoBstautauZVisoBDT( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent)
: GaudiTool ( type, name , parent )


   , m_Geom( NULL )
  /* , m_par_zv_ipsall(1000)
   , m_par_zv_ipall(1000)
   , m_par_zv_proba(1000)
   , m_par_zv_proba_close(1000)
   , m_par_zv_chi2 (1000)
   //, m_par_zv_dist(1000)
   // , m_zv_ipall(0)
   , m_zv_proba(0)
   , m_zv_proba_close(0)
   , m_zv_chi2(0)
   , m_zv_dist (0)
   , m_zv_ntrkvtx(0)
   , m_min_zv_ipsall(1000)
   , m_min_zv_ipall(1000)
   , m_min_zv_proba(1000)
   , m_min_zv_proba_close(1000)
   , m_min_zv_chi2 (1000)
   , m_min_zv_dist(1000)
   , m_min_zv_ntrkvtx(1000)
   , m_max_zv_ipsall(0)
   , m_max_zv_ipall(0)
   , m_max_zv_proba(0)
   , m_max_zv_proba_close(0)
   , m_max_zv_chi2 (0)
   , m_max_zv_dist(0)
   , m_max_zv_ntrkvtx(0)  */
{





  declareInterface<IRelatedInfoTool>(this);
  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");

  declareProperty("ParticlePath",   m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");
  declareProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);
  declareProperty( "Variables", m_variables,
                   "List of variables to store (store all if empty)");

  declareProperty("angle"     , m_angle  = 0.27     ); //
  declareProperty("fc"        , m_fc  = 0.60     ); //
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); //
  declareProperty("ips"       , m_ips  = 3.0     ); //
  declareProperty("svdis"     , m_svdis  = -0.15     ); //
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); //
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); //
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); //


  declareProperty("cut_ips_high"     , m_cut_ips_high  = 100.     ); //
  declareProperty("VF_from_MC"       , m_VF_from_MC  = false     ); //
  declareProperty("recover"          , m_recover  = true     ); //
  declareProperty("recoverVF"        , m_recoverVF  = false     ); //
  declareProperty("cut_ghost"        , m_cut_ghost  = 0.35     ); //
  declareProperty("cut_imp"          , m_cut_imp  = 0.2     ); //
  declareProperty("is_data"          , m_is_data = false);
  declareProperty("Cut_IPS"          , m_cut_ips = 30.0);
  declareProperty("Cut_IPS_VF"       , m_cut_ips_VF = 20.0);
  declareProperty("Cut_maxVF"        , m_cut = 0.5);


  declareProperty("Weights_BDTG_ZViso",m_Weights_BDTG_ZViso="IsolationTools_Bstautau_ZVisoBDTG_v1r4.xml");
  declareProperty("MVATransform",m_transformName="ZViso");

  declareProperty("Profile_step_size",m_step = 0.05 ); //10microns
  declareProperty("Profile_step_number_p",m_n_step_p = 1000 );
  declareProperty("Profile_step_number_m",m_n_step_m = 600 );

  declareProperty("cut_ntrk"     , m_cut_ntrk  = 150     ); //


  m_keys.clear();

  m_keys.push_back(RelatedInfoNamed:: ZVISOTAUP);
  m_keys.push_back(RelatedInfoNamed:: ZVISOTAUM);


}
//=============================================================================
// Destructor
//=============================================================================
RelInfoBstautauZVisoBDT::~RelInfoBstautauZVisoBDT() {}

//=============================================================================
// Initialization
//=============================================================================
StatusCode RelInfoBstautauZVisoBDT::initialize() {
  //  cout <<"*CCC***"<<endl;
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  m_Geom = tool<IDistanceCalculator>("LoKi::DistanceCalculator");
  if ( ! m_Geom ) {
    fatal() << "DistanceCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  m_vertextool = tool<IVertexFunctionTool>("VertexFunctionTool", this );
  if(! m_vertextool) {
    fatal() << "Unable to retrieve ZVtop" << endreq;
    return StatusCode::FAILURE;
  }

  m_topotool = tool<ITopoVertexTool>("TopoVertexTool", this );
  if(! m_topotool) {
    fatal() << "Unable to retrieve ZVtop" << endreq;
    return StatusCode::FAILURE;
  }

  m_Geom = tool<IDistanceCalculator>("LoKi::DistanceCalculator", this);
  if ( ! m_Geom ) {
    fatal() << "DistanceCalculator could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  m_transporter = tool<IParticleTransporter>("ParticleTransporter:PUBLIC", this);
  if ( ! m_transporter ) {
    fatal() << "ParticleTransporter could not be found" << endreq;
    return StatusCode::FAILURE;
  }

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
  //pFinder = tool<IRelatedPVFinder>("GenericParticle2PVRelator__p2PVWithIPChi2_OfflineDistanceCalculatorName_",this);

  if (!m_Weights_BDTG_ZViso.size())
    return Error("WeightsFile _BDTS MUST be defined", StatusCode::FAILURE);

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;


  m_optmap["Name"] = m_transformName ;
  m_optmap["KeepVars"] = "0" ;
  m_optmap["XMLFile"] = System::getEnv("TMVAWEIGHTSROOT") + "/data/" + m_Weights_BDTG_ZViso ;
  m_tmva.Init( m_optmap, debug().stream(), msgLevel(MSG::DEBUG) ) ;


  return StatusCode::SUCCESS;


}




//StatusCode RelInfoBstautauZVisoBDT::finalize(){
//  return GaudiTool::finalize();
//}


//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode RelInfoBstautauZVisoBDT::calculateRelatedInfo( const LHCb::Particle* top,
                                                          const LHCb::Particle *top_bis){//, const std::string &head, Tuples::Tuple &tuple ) {

  m_ZViso=-10;
  m_ZViso_TauP=-10;
  m_ZViso_TauM=-10;



  //  const std::string prefix=fullName(head);
  if ( msgLevel(MSG::DEBUG) ) debug() << "RelInfoBstautauZVisoBDT ==> Fill" << endmsg;
  //  if ( msgLevel(MSG::DEBUG) ) cout <<"A*** "<<part <<" "<<top<<endl;
  //  if ( msgLevel(MSG::DEBUG) ) cout <<"AA*** "<<part->particleID().pid() <<" "<<top->particleID().pid()<<endl;
  if ( top->isBasicParticle() || isPureNeutralCalo(top) || top!=top_bis)
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Running ZVTOP isolation on basic, neutral or calorimetric particles, skipping" << endmsg;
    return StatusCode::SUCCESS ;
  }

  LHCb::Particle::ConstVector Daughters = m_descend->descendants(top,1);
  if ( msgLevel(MSG::DEBUG) ) debug()<<"Number of ID "<<top->particleID().pid()<<" : "<<Daughters.size()<<endmsg;
  LHCb::Particle::ConstVector::const_iterator i_daug;
  //Int_t flag(0);
  for ( i_daug = Daughters.begin(); i_daug != Daughters.end(); i_daug++){
    const LHCb::Particle* part = *i_daug;
    if(!part) {
      return Warning( "Found an invalid particle" ,StatusCode::FAILURE,50);
    }else if (part && part->isBasicParticle())
    {
      if ( msgLevel(MSG::DEBUG) ) debug()<<"part->particleID().pid() : "<<part->particleID().pid()<<" , "<<"top->particleID().pid() : "<<top->particleID().pid()<<endreq;
      if ( msgLevel(MSG::DEBUG) )   debug()<<"Trying to compute ZViso BDT on basic particles (Muons or Pions) ==> needs a tau or D"<<endreq;
    }
    else if(part)
    {
      StatusCode scInTracks =  Initialize_tracksVF_ZVtop(); //initialize all the tracks that will be used for VF and ZVtop. Then we call the VFiso and the ZVtop algorithm
      StatusCode scIsoTopo  =  IsoTopo2Body(part);//, prefix, tuple);
      StatusCode scGMPiso   =  GMPiso(part);//, prefix, tuple);
      StatusCode scZViso    =  ZViso();
      if ( msgLevel(MSG::DEBUG) ) debug()<<"ZVisoBDT_value : "<<m_ZViso<<endmsg;
      if((!scIsoTopo)||(!scInTracks)||(!scGMPiso)||(!scZViso)) {return StatusCode::FAILURE;}	
      
      Float_t P_charge = 0.0;
      bool flag_OS = true;
      bool flag_tau_mu = false;
      for (LHCb::Particle::ConstVector::const_iterator i_daug_2 = Daughters.begin(); i_daug_2 != Daughters.end() ; i_daug_2++){
        const LHCb::Particle* part2 = *i_daug_2;
        if(i_daug_2 !=i_daug){
	        if((part->charge())*(part2->charge())>0) flag_OS=false;
        	/*if((part2->p())>(part->p()))*/ P_charge = (part->p())-(part2->p());// false; 
          if((part->isBasicParticle())||(part2->isBasicParticle())) flag_tau_mu=true;
        	if ( msgLevel(MSG::DEBUG) ) debug()<<"part2->p() : "<<part->p()<<"   ,    part2->p() : "<<part2->p()<<endmsg;
        }
      }
      
      if ( msgLevel(MSG::DEBUG) ) debug()<<"P_charge : "<<P_charge <<"   ,   flag_OS : "<<flag_OS<<"   ,    flag_tau_mu : "<<flag_tau_mu<<endmsg;
      
      if((flag_OS==true)||(flag_tau_mu==true)){
        if(part->charge()>0){m_ZViso_TauP=m_ZViso;}
        if(part->charge()<0){m_ZViso_TauM=m_ZViso;}
      }else if(flag_OS==false){// i.e. it's same sign
        if(P_charge>0){m_ZViso_TauP=m_ZViso;}
        else if(P_charge<0){m_ZViso_TauM=m_ZViso;}
      }

      
      
    }
  }

  




  if ( msgLevel(MSG::DEBUG) ) debug()<<"ZVisoBDT_value TauP : "<<m_ZViso_TauP<<" , ZVisoBDT_value TauM : "<<m_ZViso_TauM<<endmsg;

  m_map.clear();
  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {
    float value = 0;
    switch (*ikey) {
    case  RelatedInfoNamed::ZVISOTAUP : value = m_ZViso_TauP; break;//
    case  RelatedInfoNamed::ZVISOTAUM : value = m_ZViso_TauM; break;//
    }
    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;
    m_map.insert( std::make_pair( *ikey, value) );
  }


  return StatusCode::SUCCESS;

}

LHCb::RelatedInfoMap* RelInfoBstautauZVisoBDT::getInfo(void)
{
  return &m_map;
}

std::string RelInfoBstautauZVisoBDT::infoPath(void) {
  std::stringstream ss;
  ss << std::string("Particle2BsTauTauZViso");
  return ss.str();
}

//=============================================================================
//  Fill VERT   try of ZVTop
//=============================================================================

StatusCode RelInfoBstautauZVisoBDT::Initialize_tracksVF_ZVtop(){

  double ipsall/*, ipall*/;
  std::vector<const LHCb::Track*> tracks;
  std::vector<const LHCb::Track*> tracksVF;
  std::vector<const LHCb::Track*> tracksEmpty;
  std::vector<const LHCb::Track*> tracksVFEmpty;

  LHCb::Particles* m_allparts = get<LHCb::Particles>("/Event/Phys/StdAllNoPIDsPions/Particles"); //we start with all the tracks
  for ( LHCb::Particles::const_iterator ipp = m_allparts->begin(); ipp != m_allparts->end() ; ++ipp) { //it makes a loop on the tracks
    const LHCb::Track* it_ptrk = (*ipp)->proto()->track();
    ipsall=100000.;
    //  ipall=100000.;
    double imp, impchi2;
    RecVertex::Container* verts = NULL;
    if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){ //it reads the list of PVs
      verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
    }
    else {
      if ( msgLevel(MSG::DEBUG) ) Warning("No primary vertex at location");
    }
    RecVertex::Container::const_iterator iv;
    for ( iv = verts->begin(); iv != verts->end(); iv++) {//Here he's looking at the minimum value of the actual track in the loop wrt all the reconstructed PV
      StatusCode sc2 = m_Geom->distance((it_ptrk),(*iv),imp,impchi2);
      if (sc2.isFailure() )
      {
        Warning("m_Geom problems");
        continue;
      }
      if (impchi2<ipsall) {
        ipsall = impchi2;
        //   ipall = imp;
      }
    }

    double ghostProb;
    ghostProb = (*it_ptrk).ghostProbability();
    if ( msgLevel(MSG::DEBUG) ) debug()<< (it_ptrk) <<endmsg;

    if (ipsall>m_cut_ips && ghostProb <m_cut_ghost &&  (*it_ptrk).momentum().rho()>350.) {  //if the minimum is greater than a treshold cut the track is saved
      tracks.push_back(it_ptrk);
      if ( msgLevel(MSG::DEBUG) )debug() <<"IPS_pass "<<ipsall<<endmsg;
    }
    if (ipsall>m_cut_ips_VF && ghostProb <m_cut_ghost ) {
      tracksVF.push_back(it_ptrk);
      if ( msgLevel(MSG::DEBUG) )  debug() <<"IPS_VF "<<ipsall<<endmsg;
    }

  }//end loop on the tracks

  if ( msgLevel(MSG::DEBUG) ) debug() <<"track size" << tracks.size()<<endmsg;

  double cut_ntrk = m_cut_ntrk;
  if (tracks.size()>cut_ntrk){
    m_tracksZVTop=tracksEmpty;
    m_tracksVF=tracksVFEmpty;
  }
  else {
    m_tracksZVTop=tracks;
    m_tracksVF=tracksVF;
  }

  return StatusCode::SUCCESS;
}



///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////ISOLATION OF MATHIEU/////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////


StatusCode RelInfoBstautauZVisoBDT::IsoTopo2Body(const LHCb::Particle *part)//Isolation of Mathieu - uses only the Vertex Function
{
  //declare the vector to store the Vf scan info
  std::vector<double> v_Vf, v_Position;
  std::vector<double> v_Vf_Scan_Max, v_Position_Scan_Max;
  std::vector<double> v_Vf_Scan_Max_DwS, v_Position_Scan_Max_DwS;
  std::vector<double> v_Vf_Scan_Max_UpS, v_Position_Scan_Max_UpS;//

  //  cout <<"*B***"<<endl;


  std::vector<const LHCb::Track*> tracksVF;
  tracksVF=m_tracksVF;

  m_vertextool->setTracks(tracksVF);

  Gaudi::XYZPoint Cand_Pos = part->endVertex()->position(); //compute Vf at B position
  double zB = Cand_Pos.Z();
  float Vf_cand = m_vertextool->computeValueAt(Cand_Pos);
  m_Vf_B=Vf_cand;
  //Gaudi::XYZPoint Part_DecVect = part->endVertex()->position();//-part->beginVertex()->position();
  //Float_t Part_DecDist=sqrt(Part_DecVect.X()*Part_DecVect.X()+Part_DecVect.Y()*Part_DecVect.Y()+Part_DecVect.Z()*Part_DecVect.Z());//

  //compute daughter particles isolation
  LHCb::Particle::ConstVector Daughters = m_descend->descendants(part);
  LHCb::Particle::ConstVector::const_iterator i_daug; //the tracks I'm scanning along
  LHCb::Particle::ConstVector::const_iterator i_daug_rm, i_daug_rm_b; //the sister tracks I want to remove

  m_nb_peak=0;
  m_nb_peak_UpS=0;
  m_nb_peak_DwS=0;
  for ( i_daug = Daughters.begin(); i_daug != Daughters.end(); i_daug++){

    std::vector<const LHCb::Track*> tracksDaughters = tracksVF;

    for ( i_daug_rm_b = Daughters.begin(); (i_daug_rm_b != Daughters.end())&&(i_daug_rm_b != i_daug )                       ; i_daug_rm_b++){//   AA
      for ( i_daug_rm = Daughters.begin(); (i_daug_rm   != Daughters.end())&&(i_daug_rm!=i_daug_rm_b)&&(i_daug_rm != i_daug); i_daug_rm++  ){// A
        const LHCb::Particle*  Daug_rm = *i_daug_rm;
        const LHCb::Track* Trk_rm = Daug_rm->proto()->track();
        std::vector<const LHCb::Track*>::iterator it = std::find(tracksDaughters.begin(),tracksDaughters.end(),Trk_rm);//
        if (it != tracksDaughters.end()) tracksDaughters.erase(it);
        else Warning("The track was not in the preselected tracks!!!");
      }//A   end of for on i_daug_rm
    }//AA    end of for on i_daug_rm_b

    const LHCb::Particle*  Daug = *i_daug;
    m_vertextool->setTracks(tracksDaughters);
    m_nb_peak = m_nb_peak + VfProfile(zB,Daug, &v_Vf, &v_Position);
    //max
    int index_max = std::distance(v_Vf.begin(), max_element(v_Vf.begin(),v_Vf.end()));
    v_Vf_Scan_Max.push_back(v_Vf[index_max]);
    v_Position_Scan_Max.push_back(v_Position[index_max]);

    std::vector<double>::iterator i_0 = v_Vf.begin();
    std::advance(i_0,m_n_step_m+1);

    //max downstream the B
    index_max = std::distance(v_Vf.begin(), max_element(v_Vf.begin(),i_0));
    v_Vf_Scan_Max_DwS.push_back(v_Vf[index_max]);
    v_Position_Scan_Max_DwS.push_back(v_Position[index_max]);

    //max upstream the B
    index_max = std::distance(v_Vf.begin(), max_element(i_0,v_Vf.end()));
    v_Vf_Scan_Max_UpS.push_back(v_Vf[index_max]);
    v_Position_Scan_Max_UpS.push_back(v_Position[index_max]);

    tracksDaughters.clear();

  }//end of for on i_daug

  //get the max of the two muon vertex max
  int index_max = std::distance(v_Vf_Scan_Max.begin(), max_element(v_Vf_Scan_Max.begin(),v_Vf_Scan_Max.end()));
  m_Vf_Muons_Max = v_Vf_Scan_Max[index_max];
  m_Position_Muons_Max = v_Position_Scan_Max[index_max];

  //downstream the B
  index_max = std::distance(v_Vf_Scan_Max_DwS.begin(), max_element(v_Vf_Scan_Max_DwS.begin(),v_Vf_Scan_Max_DwS.end()));
  m_Vf_Muons_Max_DwS = v_Vf_Scan_Max_DwS[index_max];
  m_Position_Muons_Max_DwS = v_Position_Scan_Max_DwS[index_max];

  //upstream the B
  index_max = std::distance(v_Vf_Scan_Max_UpS.begin(), max_element(v_Vf_Scan_Max_UpS.begin(),v_Vf_Scan_Max_UpS.end()));
  m_Vf_Muons_Max_UpS = v_Vf_Scan_Max_UpS[index_max];
  m_Position_Muons_Max_UpS = v_Position_Scan_Max_UpS[index_max];


  return StatusCode::SUCCESS;
}



//=============================================================================
//  VfProfile
//=============================================================================
int RelInfoBstautauZVisoBDT::VfProfile(double zB,const LHCb::Particle* Part, std::vector<double> *v_Vf, std::vector<double>* v_Position){//
  v_Vf->clear();
  v_Position->clear();
  double dist = 0;
  double Vf = 0;
  std::vector<double> pcn;
  pcn.push_back(0);
  pcn.push_back(0);
  pcn.push_back(0);
  //double Vf_maxsum(0.);
  float z_first_max(-1000);
  m_z_first_max=-1000;
  int n_max = 0;

  m_sum_VF_maxs=m_sum_VF_maxs_UpStr=0.0;
  Float_t Delta_z_first_max;
  for( int i = -m_n_step_m; i<m_n_step_p;i++){
    dist = i*m_step;
    Vf = VfAlongTrack(zB,Part, dist, &z_first_max);
    //if(TMath::Abs(i)%200==0)couty<<"VF "<<Vf<<endl;
    Delta_z_first_max=z_first_max-zB;
    v_Position->push_back(dist);
    v_Vf->push_back(Vf);
    pcn.insert(pcn.begin(),Vf);
    pcn.pop_back();
    if((i>(-m_n_step_m+4))&&(i<(m_n_step_p-4))&&(pcn[0]<pcn[1])&&(pcn[1]>pcn[2])&&(pcn[1]>m_cut)/* && Delta_z_first_max<0*/)//
    {
      n_max =n_max +1;
      if (Delta_z_first_max>0) m_nb_peak_UpS=m_nb_peak_UpS+1;
      if (Delta_z_first_max<0) m_nb_peak_DwS=m_nb_peak_DwS+1;
      m_sum_VF_maxs=m_sum_VF_maxs+pcn[1];
      if(i>0) m_sum_VF_maxs_UpStr=m_sum_VF_maxs_UpStr+pcn[1];
      if(n_max==1) m_z_first_max=z_first_max-zB ;
    }
  }
  pcn.clear();
  return n_max;
}



//=============================================================================
//  VfAlongTrackTrans
//=============================================================================
double RelInfoBstautauZVisoBDT::VfAlongTrack(double zB, const LHCb::Particle* Part, double delta, float* z_f_m ){//
  Gaudi::XYZVector Slope = Part->slopes();
  double zNew = zB + delta * Slope.Z()/Slope.R();
  *z_f_m=zNew;
  LHCb::Particle transPart;
  Gaudi::XYZPoint newPosition;
  StatusCode sc  = m_transporter->transport(Part,zNew,transPart);
  if (sc.isFailure()){
    Warning("Problem in Part Transport");
    newPosition = Part->referencePoint();
  }
  else newPosition = transPart.referencePoint();
  return m_vertextool->computeValueAt(newPosition);
}







///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
//////////////////////ISOLATION OF GIAMPIERO///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////

StatusCode RelInfoBstautauZVisoBDT::GMPiso(const LHCb::Particle *part)//, const std::string prefix, Tuples::Tuple& tuple )//
{
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }

  const RecVertex* sIPSPVrefit =  dynamic_cast<const RecVertex*> (dva->bestVertex(part));//
  Particle::ConstVector vdaugh = m_descend->descendants(part);
  //    cout<<"number of daughters "<<vdaugh.size()<<endl;


  m_par_zv_ipsall=1000;
  m_par_zv_ipall=1000;
  m_par_zv_proba=1000;
  m_par_zv_proba_close=1000;
  m_par_zv_chi2 =1000;
  m_par_zv_dist=1000;
  m_par_zv_ntrkvtx=1000;
  m_zv_ipsall=0;
  m_zv_ipall=0;
  m_zv_proba=0;
  m_zv_proba_close=0;
  m_zv_chi2=0;
  m_zv_dist =0;
  m_zv_ntrkvtx=0;
  m_min_zv_ipsall=1000;
  m_min_zv_ipall=1000;
  m_min_zv_proba=1000;
  m_min_zv_proba_close=1000;
  m_min_zv_chi2 =1000;
  m_min_zv_dist=1000;
  m_min_zv_ntrkvtx=1000;
  m_max_zv_ipsall=0;
  m_max_zv_ipall=0;
  m_max_zv_proba=0;
  m_max_zv_proba_close=0;
  m_max_zv_chi2 =0;
  m_max_zv_dist=0;
  m_max_zv_ntrkvtx=0;


  Particle::ConstVector::const_iterator ip_vdau;
  ip_vdau = vdaugh.begin();
  const Particle* vdau1 = *(ip_vdau);
  ip_vdau++;
  const Particle* vdau2 = *(ip_vdau);
  ip_vdau++;
  const Particle* vdau3 = *(ip_vdau);

  std::vector<LHCb::RecVertex*> vertices = zvtop(vdau1,vdau2,vdau3);//,prefix,tuple); //it gives back a cleaned list of vertices wrt the daughter particles' tracks
  bool test=true;
  //    cout<<"debug   ###################"<<endl;
  //    cout<<"debug   "<<m_zv_proba<<endl;

  zvtop_iso(vdau1,vertices,sIPSPVrefit);
  m_zv_ipsall      =   m_zv_ipsall      +   m_par_zv_ipsall      ;
  m_zv_ipall       =   m_zv_ipall       +   m_par_zv_ipall       ;
  m_zv_proba       =   m_zv_proba       +   m_par_zv_proba       ;
  m_zv_proba_close =   m_zv_proba_close +   m_par_zv_proba_close ;
  m_zv_chi2        =   m_zv_chi2        +   m_par_zv_chi2        ;
  m_zv_dist        =   m_zv_dist        +   m_par_zv_dist        ;
  m_zv_ntrkvtx     =   m_zv_ntrkvtx     +   m_par_zv_ntrkvtx     ;
  if (m_par_zv_ipsall      <   m_min_zv_ipsall      ) m_min_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       <   m_min_zv_ipall       ) m_min_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       <   m_min_zv_proba       ) m_min_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close <   m_min_zv_proba_close ) m_min_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        <   m_min_zv_chi2        ) m_min_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        <   m_min_zv_dist        ) m_min_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     <   m_min_zv_ntrkvtx     ) m_min_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//
  if (m_par_zv_ipsall      >   m_max_zv_ipsall      ) m_max_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       >   m_max_zv_ipall       ) m_max_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       >   m_max_zv_proba       ) m_max_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close >   m_max_zv_proba_close ) m_max_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        >   m_max_zv_chi2        ) m_max_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        >   m_max_zv_dist        ) m_max_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     >   m_max_zv_ntrkvtx     ) m_max_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//


  zvtop_iso(vdau2,vertices,sIPSPVrefit);
  m_zv_ipsall      =   m_zv_ipsall      +   m_par_zv_ipsall      ;
  m_zv_ipall       =   m_zv_ipall       +   m_par_zv_ipall       ;
  m_zv_proba       =   m_zv_proba       +   m_par_zv_proba       ;
  m_zv_proba_close =   m_zv_proba_close +   m_par_zv_proba_close ;
  m_zv_chi2        =   m_zv_chi2        +   m_par_zv_chi2        ;
  m_zv_dist        =   m_zv_dist        +   m_par_zv_dist        ;
  m_zv_ntrkvtx     =   m_zv_ntrkvtx     +   m_par_zv_ntrkvtx     ;
  if (m_par_zv_ipsall      <   m_min_zv_ipsall      ) m_min_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       <   m_min_zv_ipall       ) m_min_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       <   m_min_zv_proba       ) m_min_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close <   m_min_zv_proba_close ) m_min_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        <   m_min_zv_chi2        ) m_min_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        <   m_min_zv_dist        ) m_min_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     <   m_min_zv_ntrkvtx     ) m_min_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//
  if (m_par_zv_ipsall      >   m_max_zv_ipsall      ) m_max_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       >   m_max_zv_ipall       ) m_max_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       >   m_max_zv_proba       ) m_max_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close >   m_max_zv_proba_close ) m_max_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        >   m_max_zv_chi2        ) m_max_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        >   m_max_zv_dist        ) m_max_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     >   m_max_zv_ntrkvtx     ) m_max_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//


  zvtop_iso(vdau3,vertices,sIPSPVrefit);
  m_zv_ipsall      =   m_zv_ipsall      +   m_par_zv_ipsall      ;
  m_zv_ipall       =   m_zv_ipall       +   m_par_zv_ipall       ;
  m_zv_proba       =   m_zv_proba       +   m_par_zv_proba       ;
  m_zv_proba_close =   m_zv_proba_close +   m_par_zv_proba_close ;
  m_zv_chi2        =   m_zv_chi2        +   m_par_zv_chi2        ;
  m_zv_dist        =   m_zv_dist        +   m_par_zv_dist        ;
  m_zv_ntrkvtx     =   m_zv_ntrkvtx     +   m_par_zv_ntrkvtx     ;
  if (m_par_zv_ipsall      <   m_min_zv_ipsall      ) m_min_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       <   m_min_zv_ipall       ) m_min_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       <   m_min_zv_proba       ) m_min_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close <   m_min_zv_proba_close ) m_min_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        <   m_min_zv_chi2        ) m_min_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        <   m_min_zv_dist        ) m_min_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     <   m_min_zv_ntrkvtx     ) m_min_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//
  if (m_par_zv_ipsall      >   m_max_zv_ipsall      ) m_max_zv_ipsall      =   m_par_zv_ipsall      ;//
  if (m_par_zv_ipall       >   m_max_zv_ipall       ) m_max_zv_ipall       =   m_par_zv_ipall       ;//
  if (m_par_zv_proba       >   m_max_zv_proba       ) m_max_zv_proba       =   m_par_zv_proba       ;//
  if (m_par_zv_proba_close >   m_max_zv_proba_close ) m_max_zv_proba_close =   m_par_zv_proba_close ;//
  if (m_par_zv_chi2        >   m_max_zv_chi2        ) m_max_zv_chi2        =   m_par_zv_chi2        ;//
  if (m_par_zv_dist        >   m_max_zv_dist        ) m_max_zv_dist        =   m_par_zv_dist        ;//
  if (m_par_zv_ntrkvtx     >   m_max_zv_ntrkvtx     ) m_max_zv_ntrkvtx     =   m_par_zv_ntrkvtx     ;//








  if(!test) return StatusCode::FAILURE;
  return StatusCode::SUCCESS;

}

std::vector<LHCb::RecVertex*> RelInfoBstautauZVisoBDT::zvtop(const LHCb::Particle* dau1,//
                                                             const LHCb::Particle* dau2,
                                                             const LHCb::Particle* dau3){//
  //const LHCb::Particle *part,
  //const std::string prefix,
  //                              Tuples::Tuple& tuple){//
  //  int  zv_nvtx;
  std::vector<const LHCb::Track*> tracks;
  std::vector<const LHCb::Track*> tracksVF;

  tracksVF=m_tracksVF;
  tracks=m_tracksZVTop;
  StatusCode  sc1 = m_topotool->findVertices(tracks,tracksVF);//Zvtop looks for secondary vertex
  std::vector<LHCb::RecVertex*> vertices = m_topotool->vertices(); // and we store them
  m_zv_nvtx = vertices.size(); //it saves the number of vertices found by zvtop
  //  bool test = tuple->column( prefix+"_ZVtop_nvtx", m_zv_nvtx);
  m_vertextool->setTracks(tracksVF); //Vf initializzation with tracks...it will be called in the zvtop_iso function which follows



  //  int zv_same(0);
  int removeit(0);
  int invtx(0),invtxb(0);

  for(std::vector<LHCb::RecVertex*>::iterator i = vertices.begin() ; vertices.end()!=i ; ++i ) {  //loop over secondary vertexes
    int invtxtemp(0), invtxtempb(0);
    for (SmartRefVector< LHCb::Track >::const_iterator it_ptrk=(*i)->tracks().begin(); it_ptrk!=(*i)->tracks().end(); ++it_ptrk){ //loop over the tracks attached to the current secondary vertex
      if ((dau1->proto()&&dau1->proto()->track()==(*it_ptrk))||(dau2->proto()&&dau2->proto()->track()==(*it_ptrk))||(dau3->proto()&&dau3->proto()->track()==(*it_ptrk))) {//if the current track is one of the daughter...
        if ((*i)->tracks().size()>3) invtxtemp++; //...and the vertex contains more than 2 tracks, it increases this variable//3 tracks from a >3-tracks vertex
        if ((*i)->tracks().size()==3) removeit++; //..while if the vertex contains only two tracks it increases this other variable//3 tracks from a 3-tracks vertex
        if ((*i)->tracks().size()==2) invtxtempb++; //2 tracks from a 2-tracks vertex
      }
    }//end loop over tracks attached to the secondary vertex
    if (invtx<invtxtemp) invtx=invtxtemp;
    if (invtxb<invtxtemp) invtxb=invtxtempb;

    if (removeit==3)
    {
      vertices.erase(i);
      break;
    }//if the vertex contains all the three tracks it's removed
  }//end loop over secondary ZVtop reconstructed vertexes

  if (removeit==3) m_zv_same = 1; //all the 3 tracks from a pure 3-tracks ZVtop-vertex
  else if (invtx==3) m_zv_same = 2; //all 3 tracks from a >3-tracks ZVtop-vertex
  else if (invtxb==2) m_zv_same = 3; //only 2 of the 3 tracks from a pure 2-tracks ZVtop-vertex
  else if (removeit==2) m_zv_same = 4;//only 2 of the 3 tracks from a pure 3-tracks ZVtop-vertex
  else if (invtx==2) m_zv_same = 5; //only 2 of the 3 tracks from a >3-tracks ZVtop-vertex


  else m_zv_same = 0;
  //  test &=tuple->column(prefix+"_zv_same", m_zv_same);


  /* /////////////////as it was before to produce the ntuple /lhcb/users/morda/cmtuser/DaVinci_v33r1p1/Phys/DaVinci/options/DTtuple_zvsametest.root

  //  int zv_same(0);
  int removeit(0);
  int invtx(0),invtxb(0);

  for(std::vector<LHCb::RecVertex*>::iterator i = vertices.begin() ; vertices.end()!=i ; ++i ) {  //loop over secondary vertexes
  int invtxtemp(0), invtxtempb(0);
  for (SmartRefVector< LHCb::Track >::const_iterator it_ptrk=(*i)->tracks().begin(); it_ptrk!=(*i)->tracks().end(); ++it_ptrk){ //loop over the tracks attached to the current secondary vertex
  if ((dau1->proto()&&dau1->proto()->track()==(*it_ptrk))||(dau2->proto()&&dau2->proto()->track()==(*it_ptrk))||(dau3->proto()&&dau3->proto()->track()==(*it_ptrk))) {//if the current track is one of the daughter...
  if ((*i)->tracks().size()>3) invtxtemp++; //...and the vertex contains more than 2 tracks, it increases this variable
  if ((*i)->tracks().size()<3) invtxtempb++; //...and the vertex contains more than 2 tracks, it increases this variable
  if ((*i)->tracks().size()==3) removeit++; //..while if the vertex contains only two tracks it increases this other variable
  }
  }//end loop over tracks attached to the secondary vertex
  if (invtx<invtxtemp) invtx=invtxtemp;
  if (invtxb<invtxtempb) invtxb=invtxtempb;
  if (removeit==3)
  {
  vertices.erase(i);
  break;
  }//if the vertex contains all the three tracks it's removed
  }//end loop over secondary ZVtop reconstructed vertexes

  if (removeit==3) m_zv_same = 1; //all tracks from te same ZVtop vertex
  else if (invtx<3) m_zv_same = 2; //tracks coming from a ZVtop vertex with more than 3 tracks
  else if (invtxb<3) m_zv_same = 3;//tracks coming from a ZVtop vertex with less than 3 tracks
  else m_zv_same = 0;

  test &=tuple->column(prefix+"_zv_same", m_zv_same);

  */

  return  vertices;

}






void  RelInfoBstautauZVisoBDT::zvtop_iso(const LHCb::Particle* dau,//
                                         const std::vector<LHCb::RecVertex*> vertices,//
                                         const LHCb::VertexBase* primary)//,
// const std::string prefix,
//Tuples::Tuple& tuple)//
{

  double cut_zv_dist(1.);
  double zv_ipall, zv_ipsall;
  double zv_proba(-10.), zv_proba_close(-10.), zv_chi2(-10.),zv_dist(-10.);
  int zv_ntrkvtx(-1);



  zv_ipsall=100000.;
  zv_ipall=100000.;

  for(RecVertex::Container::const_iterator iVtx = vertices.begin() ; vertices.end()!=iVtx ; ++iVtx ) {//loop on ZVtop reconstructed vertex different from the one in common among the two muons (just the "cleaned" list I gave him in the arguments of the function
    if((*iVtx)->tracks().size()>0){//if the current vertex in the loop contains tracks it goes ahead
      //            m_Geom->distance(&(*dau),(*iVtx),imp,impchi2);
      Gaudi::XYZPoint vtx((*iVtx)->position().x(),(*iVtx)->position().y(),(*iVtx)->position().z()); //it reads the vertex position
      double imp, impchi2;
      m_Geom->distance(&(*dau),vtx,imp,impchi2); //it computes IP and IPchi2 of the tracks (dau) with respect the current vertex in the loop (vtx)
      if (impchi2<zv_ipsall) { // now it looks for the closest ZVtop reconstructed vertex to the daughter track (dau) in order to understand if this tracks could come from other vertexes wrt the one in common with the other muon
        Gaudi::XYZVector v(vtx-primary->position()); //distance from the current vertex in the loop and the PV
        if (v.R() > cut_zv_dist) {//in order to eliminate ZVtop reconstructed vtx generated by prompt background we require a minimum distance from the ZVTopVtx in the loop and the PV
          zv_dist = v.R();
          zv_ipsall = impchi2;
          zv_ipall = imp;
          zv_proba = (*iVtx)->info(1,-1.);
          zv_chi2 = (*iVtx)->chi2()/(*iVtx)->nDoF();
          zv_ntrkvtx = (*iVtx)->tracks().size();
          Gaudi::XYZPoint close = close_point(vtx,dau->proto()->track()->position(),dau->slopes());//
          zv_proba_close = m_vertextool->computeValueAt(close); //VF al punto di massimo avvicinamento della tracciaValue of the VF at the point of closest approach of the tracks wrt the vertex in the loop
        }//end check on distance vtx-PV
      } //end check on the closest vertex to the track
    }//end check on the number of tracks in the vertex in the loop
  }//end loop on Zvtop vertex

  m_par_zv_ipsall       = zv_ipsall ;
  m_par_zv_ipall        = zv_ipall;
  m_par_zv_proba        = zv_proba;
  m_par_zv_proba_close  = zv_proba_close;
  m_par_zv_chi2         = zv_chi2;
  m_par_zv_dist         = zv_dist;
  m_par_zv_ntrkvtx      = zv_ntrkvtx;





}


StatusCode RelInfoBstautauZVisoBDT::ZViso(void){
  m_varmap.clear();

  m_varmap.insert("Tau_Vf",m_Vf_B);
  m_varmap.insert("Tau_MPT_iso",m_nb_peak);
  m_varmap.insert("Tau_MPT_iso_UpS",m_nb_peak_UpS);
  m_varmap.insert("Tau_MPT_iso_DwS",m_nb_peak_DwS);
  m_varmap.insert("Tau_Vf_Max",m_Vf_Muons_Max);
  m_varmap.insert("Tau_Vf_Max_UpS",m_Vf_Muons_Max_UpS);
  m_varmap.insert("Tau_Vf_Max_DwS",m_Vf_Muons_Max_DwS);
  m_varmap.insert("Tau_Pos_Max",m_Position_Muons_Max);
  m_varmap.insert("Tau_Pos_Max_UpS",m_Position_Muons_Max_UpS);
  m_varmap.insert("Tau_Pos_Max_DwS",m_Position_Muons_Max_DwS);
  m_varmap.insert("Tau_sum_Vf_Maxs",m_sum_VF_maxs);
  m_varmap.insert("Tau_sum_Vf_Maxs_UpS",m_sum_VF_maxs_UpStr);
  m_varmap.insert("Tau_zv_same",m_zv_same);
  m_varmap.insert("Tau_zv_ipall",m_zv_ipall);
  m_varmap.insert("Tau_zv_dist",m_zv_dist);
  m_varmap.insert("Tau_zv_ntrkvtx",m_zv_ntrkvtx);
  m_varmap.insert("Tau_zv_max_ipall",m_max_zv_ipall);
  m_varmap.insert("Tau_zv_max_dist",m_max_zv_dist);
  m_varmap.insert("Tau_zv_max_ntrkvtx",m_max_zv_ntrkvtx);
  m_varmap.insert("Tau_zv_min_ipall",m_min_zv_ipall);
  m_varmap.insert("Tau_zv_min_dist",m_min_zv_dist);
  m_varmap.insert("Tau_zv_min_ntrkvtx",m_min_zv_ntrkvtx);


  m_tmva(m_varmap,m_out) ;
  m_ZViso = m_out[m_transformName];


  return StatusCode::SUCCESS;

}





Gaudi::XYZPoint RelInfoBstautauZVisoBDT::close_point(Gaudi::XYZPoint o,
                                                     Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                                     p_mu)
{
  Gaudi::XYZVector v(o - o_mu);
  Gaudi::XYZVector u = p_mu/sqrt(p_mu.Dot(p_mu));

  Gaudi::XYZVector m = (u.Dot(v))*u;
  Gaudi::XYZVector d(m - v);
  Gaudi::XYZPoint close(o + d);
  return close;

}



