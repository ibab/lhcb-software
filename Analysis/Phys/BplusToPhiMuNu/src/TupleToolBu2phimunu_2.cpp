#include "GaudiKernel/ToolFactory.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/Particle.h"
#include "Kernel/IPVReFitter.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "DaVinciP2VVAngles.h"
#include "Kernel/IP2VVPartAngleCalculator.h"
#include <Kernel/DaVinciFun.h> 
#include "Kernel/IP2VVAngleCalculator.h"
#include "TMath.h"
#include "IsoBDT_Bs.h"
#include "TMVAClassification_Bs2KmuNu.class.h"
#include "TupleToolBu2phimunu_2.h"
#include <utility>
#include <iostream>     
#include <algorithm>    

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolBu2phimunu_2
//
// Author: Basem KHANJI 
//-----------------------------------------------------------------------------
using namespace LHCb ;
using namespace DaVinci;
using namespace P2VVAngles ;

using namespace DaVinci::P2VVAngles;
using namespace Gaudi::Units;
using namespace ROOT::Math;
//Declaration of Factory
DECLARE_TOOL_FACTORY( TupleToolBu2phimunu_2 )  //commented to adapt to DecayTreeTuple
//============================================================================
TupleToolBu2phimunu_2::TupleToolBu2phimunu_2(const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : TupleToolBase ( type, name , parent ),
  m_dva(0), 
  m_dist(0),
  m_descend(0),
  m_util(0),
  //m_pvReFitter(0),
  m_pvReFitterName( "LoKi::PVReFitter:PUBLIC" ),
  m_vtxfitter(0),
  m_read_BDT_muontrack(0),
  m_read_BDT_kaontrack(0),
  m_read_BDT_kaon2track(0),
  m_read_BDT_muontrack_New(0),
  m_read_BDT_kaontrack_New(0),
  m_read_BDT_kaon2track_New(0)
 
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty("ParticlePath",
                  m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles"
                  );
  declareProperty("VarSuffix",
                  aux_name  = "_Long"
                  );
  
}
// =============================================================================
StatusCode TupleToolBu2phimunu_2::initialize() {

  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  
  std::vector<std::string> BDT_Var_names;

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (!m_dva) return Error("Couldn't get parent DVAlgorithm");
  
  m_dist = m_dva->distanceCalculator();
  
  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE; }
  
  m_util = tool<ITaggingUtilsChecker> ( "TaggingUtilsChecker", this );
  if( ! m_util ) {fatal() << "Unable to retrieve TaggingUtilsChecker tool "<< endreq;    
    return StatusCode::FAILURE;}
  
  m_pvReFitter = tool<IPVReFitter>( m_pvReFitterName, this );
  if ( !m_pvReFitter )
  {
    return Error( "Unable to retrieve IPVReFitter instance" );
  }
    
  m_vtxfitter = tool<IVertexFit>("LoKi::VertexFitter");
  if(!m_vtxfitter ) 
  {
    fatal() << "Unable to retrieve Loki VertexFitter" << endreq;
    return StatusCode::FAILURE;
  }
  
  BDTvariableNames_bb_bkg_training(m_inNames_muon );
  BDTvariableNames_bb_bkg_training(m_inNames_kaon );
  BDTvariableNames_bb_bkg_training(m_inNames_kaon2 );
  BDTvariableNames_Bplus_training(m_inNames_muon_New );
  BDTvariableNames_Bplus_training(m_inNames_kaon_New );
  BDTvariableNames_Bplus_training(m_inNames_kaon2_New );
 
  m_read_BDT_muontrack = new ReadBDT_bS(m_inNames_muon);
  m_read_BDT_kaontrack = new ReadBDT_bS(m_inNames_kaon);
  m_read_BDT_kaon2track = new ReadBDT_bS(m_inNames_kaon2);
  
  m_read_BDT_muontrack_New = new ReadBDTG_isoBDT_Bplus(m_inNames_muon_New);
  m_read_BDT_kaontrack_New = new ReadBDTG_isoBDT_Bplus(m_inNames_kaon_New);
  m_read_BDT_kaon2track_New = new ReadBDTG_isoBDT_Bplus(m_inNames_kaon2_New);
  
  m_input_muon = new std::vector<double>;
  m_input_kaon = new std::vector<double>;
  m_input_kaon2 = new std::vector<double>;

  m_input_muon_New = new std::vector<double>;
  m_input_kaon_New = new std::vector<double>;
  m_input_kaon2_New = new std::vector<double>;
 
  return sc;
}
//=============================================================================
StatusCode TupleToolBu2phimunu_2::fill(const LHCb::Particle* mother 
                                     , const LHCb::Particle* P
                                     , const std::string& head
                                     , Tuples::Tuple& tuple)
{
  Assert( P && m_dva
          , "No mother or particle, or tools misconfigured." );
  
  const std::string prefix=fullName(head);
  bool test = true;
  if( !P ) return StatusCode::FAILURE;
  if( !( P->particleID().hasBottom() )) return StatusCode::SUCCESS;
  
  int iso_muon = 0;     double minSumBDT = -5 ;        
  int iso_kaon = 0;     double minMinBDT_New  = 5.5 ;  
  int iso_kaon2 = 0;    

  double minMinBDT_New_muon  = 5.5 ;  
  double minMinBDT_New_kaon  = 5.5 ;  
  double minMinBDT_New_kaon2  = 5.5 ;  
  //double MinBDT_New    = -2.5;
  //double SumBDT        = -2.5;
  int    best_track_charge = 0 ;
  double MuMu_M_best        = -1 ;
  double MuMu_M_best_fromiso= -1 ;
  
  double Best_MuMu_VtxChi2  = -1 ;
  double Best_MuMu_distance = -1 ;
  double Best_MuMu_distance_chi2  = -1 ;
  double Sum_of_best_MuMu_d_chi2s = -1;
    
  double Kh_M_best          = -1 ;
  double Kh_M_best_fromiso  = -1 ;
  
  double Best_Kh_VtxChi2  = -1 ;
  double Best_Kh_distance = -1 ;
  double Best_Kh_distance_chi2   = -1 ;
  double Sum_of_best_Kh_d_chi2s   = -1;
    
  double K2h_M_best          = -1 ;
  double K2h_M_best_fromiso  = -1 ;
  
  double Best_K2h_VtxChi2  = -1 ;
  double Best_K2h_distance = -1 ;
  double Best_K2h_distance_chi2   = -1 ;
  double Sum_of_best_K2h_d_chi2s   = -1;
    
  XYZTVector NonIsolatingMuon4vect ;
  XYZTVector NonIsolatingKaon4vect ;
  XYZTVector NonIsolatingKaon24vect ;
  
  XYZTVector lepton_plus;
  XYZTVector kaon_plus;
  XYZTVector kaon2_plus;
  XYZTVector lepton_minus;
  XYZTVector kaon_minus;
  XYZTVector kaon2_minus;
  
  const VertexBase* aPV = NULL;
  aPV = m_dva->bestVertex(mother);//bestVertex
    
  Gaudi::XYZPoint PosPV(0,0,200000) ;
  if(!aPV){ return StatusCode::SUCCESS; }
  if(aPV){PosPV = aPV->position(); }
  
  const Vertex* endv = mother->endVertex();
  double Bd_mass = mother->momentum().M();
  
  Particle::ConstVector parts_cand = m_descend->descendants(mother);
  
  const Particle* muon   = findID(13 , parts_cand, 1);
  const Particle* kaon   = findID(321, parts_cand, 1);
  const Particle* kaon2   = findID(321, parts_cand, 2); // added the second kaon
  
  int muon_charge = muon->particleID().pid();
  int Kaon_charge = kaon->particleID().pid();
  int Kaon2_charge = kaon2->particleID().pid(); //Added second kaon
 
  if(!muon)   return StatusCode::SUCCESS;
  if(!kaon)   return StatusCode::SUCCESS;
  if(!kaon2)   return StatusCode::SUCCESS;  //Added second kaon
  
  Gaudi::XYZPoint PosSV(0,0,200000);
  if( !endv ) { return StatusCode::FAILURE; }
  if(  endv ) { PosSV= mother->endVertex()->position(); } 
  
  // Get info from B to estimate the function used in the isolation BDT:
  Gaudi::XYZVector A = mother->momentum().Vect();
  Gaudi::XYZVector B = endv->position() - aPV->position();
  double cosPFD = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
  double Bd_eta = mother->momentum().Eta();
  double Bd_phi = mother->momentum().Phi();
  double Bd_pT  = mother->pt();
  double Sum_of_trackpt = 0;
  double Bd_PointingAngle  = std::acos(cosPFD);
  
  // Estimate corrected mass
  double pNuT        = sin(Bd_PointingAngle)*std::sqrt( A.Mag2());
  double Bs_corrMass    = sqrt(Bd_mass*Bd_mass +pNuT*pNuT) + pNuT;
  
  XYZTVector Muon4vect = muon->momentum();
  XYZTVector Kaon4vect = kaon->momentum();
  XYZTVector Kaon24vect = kaon2->momentum(); //Added second kaon
 
  XYZTVector KMu_mom  = Muon4vect + Kaon4vect + Kaon24vect ; //Added second kaon
  double KMu_corrMass = sqrt(KMu_mom.M2() +pNuT*pNuT) + pNuT;

  double minchi2_MuMu = 99999;
  double minchi2_Kh   = 99999;
  double minchi2_K2h   = 99999;
  double minMuTM = 99999; 
 
  double MuMu_VtxChi2 = -1;
  double Kh_VtxChi2   = -1;
  double K2h_VtxChi2   = -1;
  
  LHCb::Particle::Range allparts ;
  if (exist<LHCb::Particle::Range>(m_ParticlePath))
  { allparts = get<LHCb::Particle::Range>(m_ParticlePath); }
  else return Error("Nothing found at "+ m_ParticlePath , StatusCode::SUCCESS,1);
  
  LHCb::Particle::Range::const_iterator im;
  for(im = allparts.begin() ; im != allparts.end() ; ++im)
  {
    const Particle * axp  = (*im);
    const Track* track = axp->proto()->track();
        
    bool isInDecay = false;
    Particle::ConstVector::const_iterator it_p;
    // Get the decendants
    //Particle::ConstVector parts_cand = m_descend->descendants(mother);
    
    for( it_p=parts_cand.begin(); it_p!=parts_cand.end(); it_p++){
      if(!(*it_p)->isBasicParticle()) continue;
      const LHCb::Track* Track_decay = (*it_p)->proto()->track();
      //if(!track || !Track_decay) continue;
      if(Track_decay && track){
        //debug()<<"track of decay particle exist!"<<endreq;
        if(Track_decay == track){
          //debug()<<"This track is in decay .. remove it"<<endreq;
          isInDecay = true;
        }  
      } 
    }
    
    
    if(isInDecay) continue;
        
    const double track_minIPchi2 = get_MINIPCHI2(axp);
    double track_pt  = axp->pt();
    double track_eta = track->momentum().eta();
    double track_phi = track->momentum().phi();
    double track_Matchchi2 = track->info(LHCb::Track::FitMatchChi2,-1);
    
    XYZTVector track4vect = axp->momentum();
    int track_charge = (std::abs(axp->particleID().pid()))/(axp->particleID().pid());
    //std::cout<<"TRACK CHARGE IS " << track_charge <<std::endl;
    
    //Get the Mu + track Mass
    
    double MuTMass = (track4vect + Muon4vect).M();
    
    
    // Get the 4 momentum of K mu system: this is important for control/normalization channels as they may be composed of more particles 
    
    XYZTVector MuMu_mom = Muon4vect + track4vect;
    XYZTVector Kh_mom   = Kaon4vect  + track4vect;
    XYZTVector K2h_mom  = Kaon24vect  + track4vect; //Add K2+Track
    double Kh_mass   = std::abs(std::sqrt(Kh_mom.M2())   );
    double K2h_mass   = std::abs(std::sqrt(K2h_mom.M2())   );//Add mass of K2+track
    double MuMu_mass = std::abs(std::sqrt(MuMu_mom.M2()) );
    
    Gaudi::XYZPoint pos_track(track->position());
    Gaudi::XYZVector mom_track(track->momentum());
    Gaudi::XYZPoint pos_muon(muon->proto()->track()->position() );
    Gaudi::XYZVector mom_muon(muon->proto()->track()->momentum());
    Gaudi::XYZPoint pos_kaon(kaon->proto()->track()->position() );
    Gaudi::XYZVector mom_kaon(kaon->proto()->track()->momentum());
    Gaudi::XYZPoint pos_kaon2(kaon2->proto()->track()->position() );  //More lines for Kaon2
    Gaudi::XYZVector mom_kaon2(kaon2->proto()->track()->momentum());  //More lines for Kaon2
    
    // Calculate the input of ISO variable :
    Gaudi::XYZPoint vtx_muon_track(0.,0.,0.);
    Gaudi::XYZPoint vtx_kaon_track(0.,0.,0.);
    Gaudi::XYZPoint vtx_kaon2_track(0.,0.,0.); //More kaon2
       
    double doca_muon(-1.) , angle_muon(-1.) ; 
    double doca_kaon(-1.) , angle_kaon(-1.) ; 
    double doca_kaon2(-1.) , angle_kaon2(-1.) ; 
    
    InCone(pos_muon   , mom_muon , pos_track , mom_track , vtx_muon_track  , doca_muon, angle_muon);
    InCone(pos_kaon   , mom_kaon , pos_track , mom_track , vtx_kaon_track  , doca_kaon, angle_kaon);
    InCone(pos_kaon2   , mom_kaon2 , pos_track , mom_track , vtx_kaon2_track  , doca_kaon2, angle_kaon2);   //More kaon2
   
    double PVdis_muon_track   = ( vtx_muon_track.z() -PosPV.z() ) / fabs(vtx_muon_track.z()-PosPV.z())*(vtx_muon_track-PosPV).R() ;
    double SVdis_muon_track   = ( vtx_muon_track.z() -PosSV.z() ) / fabs(vtx_muon_track.z()-PosSV.z())*(vtx_muon_track-PosSV).R() ;
    double fc_mu = pointer(vtx_muon_track -PosPV ,mom_track , mom_muon );
    
    double PVdis_kaon_track   = ( vtx_kaon_track.z() -PosPV.z() ) / fabs(vtx_kaon_track.z()-PosPV.z())*(vtx_kaon_track-PosPV).R() ;
    double SVdis_kaon_track   = ( vtx_kaon_track.z() -PosSV.z() ) / fabs(vtx_kaon_track.z()-PosSV.z())*(vtx_kaon_track-PosSV).R() ;
    double fc_k = pointer(vtx_kaon_track -PosPV ,mom_track , mom_kaon );
    
    double PVdis_kaon2_track   = ( vtx_kaon2_track.z() -PosPV.z() ) / fabs(vtx_kaon2_track.z()-PosPV.z())*(vtx_kaon2_track-PosPV).R() ;
    double SVdis_kaon2_track   = ( vtx_kaon2_track.z() -PosSV.z() ) / fabs(vtx_kaon2_track.z()-PosSV.z())*(vtx_kaon2_track-PosSV).R() ;
    double fc_k2 = pointer(vtx_kaon2_track -PosPV ,mom_track , mom_kaon2 );
    
    m_input_muon->clear();
    m_input_muon->reserve(6);
    m_input_muon->push_back(track_minIPchi2);
    m_input_muon->push_back(PVdis_muon_track);
    m_input_muon->push_back(SVdis_muon_track);
    m_input_muon->push_back(doca_muon);
    m_input_muon->push_back(angle_muon);
    m_input_muon->push_back(fc_mu);

    m_input_kaon->clear();
    m_input_kaon->reserve(6);
    m_input_kaon->push_back(track_minIPchi2);
    m_input_kaon->push_back(PVdis_kaon_track);
    m_input_kaon->push_back(SVdis_kaon_track);
    m_input_kaon->push_back(doca_kaon);
    m_input_kaon->push_back(angle_kaon);
    m_input_kaon->push_back(fc_k);
    
    m_input_kaon2->clear();
    m_input_kaon2->reserve(6);
    m_input_kaon2->push_back(track_minIPchi2);
    m_input_kaon2->push_back(PVdis_kaon2_track);
    m_input_kaon2->push_back(SVdis_kaon2_track);
    m_input_kaon2->push_back(doca_kaon2);
    m_input_kaon2->push_back(angle_kaon2);
    m_input_kaon2->push_back(fc_k2);

    double BDT_val_muon =0;
    double BDT_val_kaon =0;
    double BDT_val_kaon2 =0;
    BDT_val_muon =  m_read_BDT_muontrack->GetMvaValue( *m_input_muon );
    BDT_val_kaon =  m_read_BDT_kaontrack->GetMvaValue( *m_input_kaon );
    BDT_val_kaon2 =  m_read_BDT_kaon2track->GetMvaValue( *m_input_kaon2 );
       
    // New BDT tunning:
    // Sisters for slpion and muon
    m_input_muon_New->clear();
    m_input_muon_New->reserve(10);
    m_input_muon_New->push_back(track_minIPchi2);
    m_input_muon_New->push_back(track_eta);
    m_input_muon_New->push_back(track_phi);
    m_input_muon_New->push_back(track_pt);
    m_input_muon_New->push_back(track_Matchchi2);
    m_input_muon_New->push_back(PVdis_muon_track);
    m_input_muon_New->push_back(SVdis_muon_track);
    m_input_muon_New->push_back(doca_muon);
    m_input_muon_New->push_back(angle_muon);
    m_input_muon_New->push_back(fc_mu);
    
    m_input_kaon_New->clear();
    m_input_kaon_New->reserve(9);
    m_input_kaon_New->push_back(track_minIPchi2);
    m_input_kaon_New->push_back(track_eta);
    m_input_kaon_New->push_back(track_phi);
    m_input_kaon_New->push_back(track_pt);
    m_input_kaon_New->push_back(track_Matchchi2);
    m_input_kaon_New->push_back(PVdis_kaon_track);
    m_input_kaon_New->push_back(SVdis_kaon_track);
    m_input_kaon_New->push_back(doca_kaon);
    m_input_kaon_New->push_back(angle_kaon);
    m_input_kaon_New->push_back(fc_k);
     
    m_input_kaon2_New->clear();
    m_input_kaon2_New->reserve(9);
    m_input_kaon2_New->push_back(track_minIPchi2);
    m_input_kaon2_New->push_back(track_eta);
    m_input_kaon2_New->push_back(track_phi);
    m_input_kaon2_New->push_back(track_pt);
    m_input_kaon2_New->push_back(track_Matchchi2);
    m_input_kaon2_New->push_back(PVdis_kaon2_track);
    m_input_kaon2_New->push_back(SVdis_kaon2_track);
    m_input_kaon2_New->push_back(doca_kaon2);
    m_input_kaon2_New->push_back(angle_kaon2);
    m_input_kaon2_New->push_back(fc_k2);
   
    double BDT_val_muon_New =0;
    double BDT_val_kaon_New =0;
    double BDT_val_kaon2_New =0;
    BDT_val_muon_New =  m_read_BDT_muontrack_New->GetMvaValue( *m_input_muon_New );
    BDT_val_kaon_New =  m_read_BDT_kaontrack_New->GetMvaValue( *m_input_kaon_New );
    BDT_val_kaon2_New =  m_read_BDT_kaon2track_New->GetMvaValue( *m_input_kaon2_New );
   // Get the mumu mass corresponding to best non-isolating track
    double track_MM_sign = muon->charge()/( axp->charge());
    double track_Kh_sign = kaon->charge()/( axp->charge());
    double track_K2h_sign = kaon2->charge()/( axp->charge());
    //std::cout<<"track_MM_sign = " << track_MM_sign << std::endl;
    //std::cout<<"track_Kh_sign = " << track_Kh_sign << std::endl;
    if(minMinBDT_New_muon > BDT_val_muon_New)
    {
      minMinBDT_New_muon = BDT_val_muon_New ;
      //  if(muon_charge*track_charge>0){ // verify that the tracks has the opposite sign of that if the kaon
      if( track_MM_sign<0 ){ // verify that the tracks has the opposite sign of that if the kaon
        { 
          MuMu_M_best_fromiso = MuMu_mass;
          NonIsolatingMuon4vect = track4vect;
        }
      }
    }
    
    // notice that the set of underlying tracks considered in the case of the kaon is orthogonal to that of the muon (as the two signs must be opposite to eaach other by construction, this is not true in the case of SS data)  
    if(minMinBDT_New_kaon > BDT_val_kaon_New)
    {
      minMinBDT_New_kaon = BDT_val_kaon_New ; 
      if( track_Kh_sign <0){ // verify that the tracks has the opposite sign of that if the kaon
        { 
          
          Kh_M_best_fromiso = Kh_mass;  
          NonIsolatingKaon4vect = track4vect;
        }
      }
    }
    // notice that the set of underlying tracks considered in the case of the kaon is orthogonal to that of the muon (as the two signs must be opposite to eaach other by construction, this is not true in the case of SS data)  
    if(minMinBDT_New_kaon2 > BDT_val_kaon2_New)//This is all for kaon2
    {
      minMinBDT_New_kaon2 = BDT_val_kaon2_New ; 
      if( track_Kh_sign <0){ // verify that the tracks has the opposite sign of that if the kaon
        { 
          
          K2h_M_best_fromiso = K2h_mass;  
          NonIsolatingKaon24vect = track4vect;
        }
      }
    }    
    
    double MinBDT_New = std::min(BDT_val_muon_New , BDT_val_kaon_New );
    MinBDT_New = std::min(MinBDT_New, BDT_val_kaon2_New);
    
    if(  minMinBDT_New > MinBDT_New  ) {
      minMinBDT_New = MinBDT_New;
      //std::cout<<"CHARGE OF WORST ISOLATED TRACK W.R.T B VERTEX IS " << track_charge <<std::endl;
      best_track_charge = track_charge;
      minMuTM = MuTMass;
    }
    
    double SumBDT = BDT_val_muon + BDT_val_kaon + BDT_val_kaon2;
    
    if( minSumBDT<SumBDT ) {
      minSumBDT = SumBDT;
    }
    
        
    // Apply giampie's iso cut:
    if ( angle_muon < 0.27 && fc_mu<0.60 && doca_muon<0.13 && track_minIPchi2 > 3.0 &&
         SVdis_muon_track >-0.15 && SVdis_muon_track<30. && PVdis_muon_track>0.5 && PVdis_muon_track<40.
      ) {
      iso_muon += 1;
      
    }
    
    if ( angle_kaon < 0.27 && fc_k<0.60 && doca_kaon<0.13 && track_minIPchi2 > 3.0 &&
         SVdis_kaon_track >-0.15 && SVdis_kaon_track<30. && PVdis_kaon_track>0.5 && PVdis_kaon_track<40.
         ) {
      iso_kaon += 1;
      
    }
    
    if ( angle_kaon2 < 0.27 && fc_k2<0.60 && doca_kaon2<0.13 && track_minIPchi2 > 3.0 &&
         SVdis_kaon2_track >-0.15 && SVdis_kaon2_track<30. && PVdis_kaon2_track>0.5 && PVdis_kaon2_track<40.
         ) {
      iso_kaon2 += 1;
      
    }    
    // calculate the Jpsi mass :
        
    LHCb::Vertex vtx_MuMu; LHCb::Vertex vtx_Kh; LHCb::Vertex vtx_K2h;
        
    double MuMu_distance =  0 ; double MuMu_chi2 =  0 ;
    double Kh_distance   =  0 ; double Kh_chi2 =  0;
    double K2h_distance   =  0 ; double K2h_chi2 =  0;
   
    StatusCode ok_mumu =  m_dist->distance( muon , axp , MuMu_distance, MuMu_chi2 );
    StatusCode ok_kk   =  m_dist->distance( kaon , axp , Kh_distance  , Kh_chi2   );
    StatusCode ok_k2k2 =  m_dist->distance( kaon2 , axp , K2h_distance  , K2h_chi2   );
       
    StatusCode MuMu_Vtx_fit = m_vtxfitter ->fit( vtx_MuMu, *muon, *axp );
    if ( !MuMu_Vtx_fit ) { MuMu_VtxChi2 = -1; }
    else {
      MuMu_VtxChi2 = vtx_MuMu.chi2()/(double)vtx_MuMu.nDoF();
      if ( minchi2_MuMu>0 && minchi2_MuMu> MuMu_VtxChi2 && track_MM_sign < 0)
      {
        minchi2_MuMu       = MuMu_VtxChi2   ;
        Best_MuMu_VtxChi2  = MuMu_VtxChi2   ;
        Best_MuMu_distance =  MuMu_distance ;
        Best_MuMu_distance_chi2 =  MuMu_chi2 ;
        Sum_of_best_MuMu_d_chi2s +=  MuMu_chi2 ;
        MuMu_M_best = MuMu_mass;
      }
    }
    
    StatusCode Kh_Vtx_fit   = m_vtxfitter ->fit( vtx_Kh, *kaon, *axp );
    if ( !Kh_Vtx_fit ) { Kh_VtxChi2 = -1; }
    else {
      Kh_VtxChi2 = vtx_Kh.chi2()/(double)vtx_Kh.nDoF();
      if ( minchi2_Kh > Kh_VtxChi2 && track_Kh_sign < 0)
      {
        minchi2_Kh = Kh_VtxChi2 ;
        Best_Kh_VtxChi2 = Kh_VtxChi2;
        Best_Kh_distance =  Kh_distance;
        Best_Kh_distance_chi2   = Kh_chi2;
        Sum_of_best_Kh_d_chi2s += Kh_chi2;
        Kh_M_best = Kh_mass;
      }
    }
    
    StatusCode K2h_Vtx_fit   = m_vtxfitter ->fit( vtx_K2h, *kaon2, *axp );
    if ( !K2h_Vtx_fit ) { K2h_VtxChi2 = -1; }
    else {
      K2h_VtxChi2 = vtx_K2h.chi2()/(double)vtx_K2h.nDoF();
      if ( minchi2_K2h > K2h_VtxChi2 && track_K2h_sign < 0)
      {
        minchi2_K2h = K2h_VtxChi2 ;
        Best_K2h_VtxChi2 = K2h_VtxChi2;
        Best_K2h_distance =  K2h_distance;
        Best_K2h_distance_chi2   = K2h_chi2;
        Sum_of_best_K2h_d_chi2s += K2h_chi2;
        K2h_M_best = K2h_mass;
      }
    }   
    double cone_radius = std::sqrt(std::pow((Bd_eta - track_eta) , 2)  + 
                                   std::pow((Bd_phi - track_phi) , 2)  ) ;
    
    if ( cone_radius <1){
      Sum_of_trackpt += axp->pt() ;
    }
  }
  
  double Bd_CDF3 = Bd_pT/(Bd_pT+Sum_of_trackpt);

  if (muon_charge>0 ) { 
    lepton_plus  = Muon4vect;
    lepton_minus = NonIsolatingMuon4vect; 
    kaon_plus    = NonIsolatingKaon4vect ;
    kaon_minus   = Kaon4vect ;
    kaon2_plus    = NonIsolatingKaon24vect ;
    kaon2_minus   = Kaon24vect ;
  }
  else { 
    lepton_plus  = NonIsolatingMuon4vect ; 
    lepton_minus = Muon4vect ; 
    kaon_plus    = Kaon4vect ; 
    kaon_minus   = NonIsolatingKaon4vect ; 
    kaon2_plus    = Kaon24vect ; 
    kaon2_minus   = NonIsolatingKaon24vect ; 
  }
  
  double Theta_tr_angle = DaVinci::P2VVAngles::calculateThetaTr(lepton_plus,lepton_minus,kaon_plus,kaon_minus);
  double Phi_tr_angle   = DaVinci::P2VVAngles::calculatePhiTr(lepton_plus,lepton_minus,kaon_plus,kaon_minus);
  double phi_plane_angle= DaVinci::P2VVAngles::calculatePlaneAngle(lepton_plus,lepton_minus,kaon_plus,kaon_minus);
  double ThetaK_angle   = DaVinci::P2VVAngles::calculateHelicityPolarAngle(kaon_plus,kaon_minus,lepton_plus,lepton_minus);
  double ThetaV_angle   = DaVinci::P2VVAngles::calculateHelicityPolarAngle(lepton_plus,lepton_minus,kaon_plus,kaon_minus); 
  double Theta_tr_angle2 = DaVinci::P2VVAngles::calculateThetaTr(lepton_plus,lepton_minus,kaon2_plus,kaon2_minus);
  double Phi_tr_angle2   = DaVinci::P2VVAngles::calculatePhiTr(lepton_plus,lepton_minus,kaon2_plus,kaon2_minus);
  double phi_plane_angle2= DaVinci::P2VVAngles::calculatePlaneAngle(lepton_plus,lepton_minus,kaon2_plus,kaon2_minus);
  double ThetaK_angle2   = DaVinci::P2VVAngles::calculateHelicityPolarAngle(kaon2_plus,kaon2_minus,lepton_plus,lepton_minus);
  double ThetaV_angle2   = DaVinci::P2VVAngles::calculateHelicityPolarAngle(lepton_plus,lepton_minus,kaon2_plus,kaon2_minus); 
 
  
  /*
    if (track_type == 1 ) aux_name = "_VELO" ;
    if (track_type == 3 ) aux_name = "_Long" ;
    if (track_type == 4 ) aux_name = "_Upstream" ;
  */
  
  test &= tuple->column(prefix+"_IsoTrack_Charge" +aux_name, best_track_charge );
  test &= tuple->column(prefix+"_KKMu_CorrMass" , KMu_corrMass );
  test &= tuple->column(prefix+"_Bs_CorrMass" , Bs_corrMass);
  test &= tuple->column(prefix+"_MuTrackMass" , minMuTM); 
  test &= tuple->column(prefix+"_iso_muon"+aux_name    , iso_muon);
  test &= tuple->column(prefix+"_iso_Kaon"+aux_name     , iso_kaon);
  test &= tuple->column(prefix+"_iso_Kaon2"+aux_name     , iso_kaon2);
  test &= tuple->column(prefix+"_iso_SumBDT"+aux_name   , minSumBDT);
  test &= tuple->column(prefix+"_iso_MinBDT"+aux_name   , minMinBDT_New);
  test &= tuple->column(prefix+"_ConeIso"+aux_name      , Bd_CDF3);
  test &= tuple->column(prefix+"_MuMu_M_iso"+aux_name  , MuMu_M_best_fromiso);
  test &= tuple->column(prefix+"_Kh_M_iso"+aux_name           , Kh_M_best_fromiso);
  test &= tuple->column(prefix+"_K2h_M_iso"+aux_name           , K2h_M_best_fromiso);

  test &= tuple->column(prefix+"_Theta_tr" + aux_name  , Theta_tr_angle );
  test &= tuple->column(prefix+"_Phi_tr" + aux_name    , Phi_tr_angle );
  test &= tuple->column(prefix+"_phi_plane" + aux_name , phi_plane_angle );
  test &= tuple->column(prefix+"_ThetaK" + aux_name    , ThetaK_angle );
  test &= tuple->column(prefix+"_ThetaV" + aux_name    , ThetaV_angle );
 
  test &= tuple->column(prefix+"_Theta_tr2" + aux_name  , Theta_tr_angle2 );
  test &= tuple->column(prefix+"_Phi_tr2" + aux_name    , Phi_tr_angle2 );
  test &= tuple->column(prefix+"_phi_plane2" + aux_name , phi_plane_angle2 );
  test &= tuple->column(prefix+"_ThetaK2" + aux_name    , ThetaK_angle2 );
  test &= tuple->column(prefix+"_ThetaV2" + aux_name    , ThetaV_angle2 );
 
  //test &= tuple->column(prefix+"_MuMu_M_bestchi2"+aux_name  , MuMu_M_best);
  //test &= tuple->column(prefix+"_Best_MuMu_VtxChi2"+aux_name   , Best_MuMu_VtxChi2  );
  //test &= tuple->column(prefix+"_Best_MuMu_distance"+aux_name  , Best_MuMu_distance );
  //test &= tuple->column(prefix+"_Best_MuMu_distance_chi2"+aux_name    , Best_MuMu_distance_chi2 );
  //test &= tuple->column(prefix+"_Sum_of_best_MuMu_d_chi2s"+aux_name  , Sum_of_best_MuMu_d_chi2s);
  //test &= tuple->column(prefix+"_Kh_M_bestchi2"+aux_name           , Kh_M_best);
  //test &= tuple->column(prefix+"_Best_Kh_VtxChi2"+aux_name    , Best_Kh_VtxChi2 );
  //test &= tuple->column(prefix+"_Best_Kh_distance"+aux_name    , Best_Kh_distance );
  //test &= tuple->column(prefix+"_Best_Kh_distance_chi2"+aux_name    , Best_Kh_distance_chi2 );
  //test &= tuple->column(prefix+"_Sum_of_best_Kh_d_chi2s"+aux_name  , Sum_of_best_Kh_d_chi2s);
  
  return StatusCode(test);
}

const Particle* TupleToolBu2phimunu_2::findID(unsigned int id,
                                     Particle::ConstVector& v,
                                     int opts )
{
  const Particle* p=0;
  int count = 0;
  for( Particle::ConstVector::const_iterator ip=v.begin(); ip!=v.end(); ip++)
  {
    if((*ip)->particleID().abspid() == id) 
    {
/*      if(id==211 && opts=="slow")
      {
        const Particle* mater = m_util->motherof(*ip, v);
        if(mater->particleID().abspid()!=413) continue;
      }
      if(id==211 && opts=="fast")
      {
        const Particle* mater = m_util->motherof(*ip, v);
        if(mater->particleID().abspid()==413) continue;
      }*/
      count++ ;
      if ( opts == count ){
		p = (*ip);  
		break;
	  }
    }
  }
 if(!p) 
 {
   return NULL;
 }
 return p;
}

//=============================================================================
double TupleToolBu2phimunu_2::get_MINIPCHI2(const Particle* p){  
  double minchi2 = -1 ;
  
  const RecVertex::Range PV = m_dva->primaryVertices();
  if ( !PV.empty() ){
    for (RecVertex::Range::const_iterator pv = PV.begin(); pv!=PV.end(); ++pv){
      double ip, chi2;
      
      RecVertex newPV(**pv);
      StatusCode scfit = m_pvReFitter->remove(p, &newPV);
      if(!scfit) { err()<<"ReFitter fails!"<<endreq; continue; }
      
      LHCb::VertexBase* newPVPtr = (LHCb::VertexBase*)&newPV; 
      bool test2  =  m_dist->distance ( p, newPVPtr, ip, chi2 );
      if( test2 ) {
        if ((chi2<minchi2) || (minchi2<0.)) minchi2 = chi2 ;        
      }
    }
  }
  return minchi2;
}

//========================================================================
double TupleToolBu2phimunu_2::get_IPCHI2wrtDcyVtx(const Particle* B,
                                                const Particle* P){
  
  const VertexBase *decayVtxB = B->endVertex();
  double chi2 = 0 ;
  
  if ( 0==decayVtxB )
  { chi2 = -999. ;}
  else 
  {
    double dist = 0;
    bool ok =  m_dist->distance(P , decayVtxB , dist , chi2 );
    if (!ok) return -999.;
  }
  return chi2;
}
//=============================================================================
void TupleToolBu2phimunu_2::InCone(Gaudi::XYZPoint o1,
                                   Gaudi::XYZVector p1,Gaudi::XYZPoint o2,
                                   Gaudi::XYZVector p2,
                                   Gaudi::XYZPoint& vtx, double&
                                   doca, double& angle) const
{

  Gaudi::XYZPoint rv;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);
  closest_point(o1,p1,o2,p2,close,close_mu,vtx, fail);
  if (fail) {
    doca =-1.;
    angle=-1.;
  }
  else {
    doca = (close-close_mu).R();
    angle = arcosine(p1,p2);
  }
}

//============================================================================
double TupleToolBu2phimunu_2::pointer (Gaudi::XYZVector vertex,
                                  Gaudi::XYZVector p, Gaudi::XYZVector p_mu) const
{
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double  num=ptot.R()*sin(arcosine(vertex,ptot));
  double  den=(num+pt);
  double fc = num/den;
  return fc;
}
//============================================================================
void TupleToolBu2phimunu_2::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
                                     Gaudi::XYZPoint o_mu,Gaudi::XYZVector
                                     p_mu, Gaudi::XYZPoint& close1,
                                     Gaudi::XYZPoint& close2,
                                     Gaudi::XYZPoint& vertex, bool&
                                     fail) const
{
  Gaudi::XYZVector v0(o - o_mu);
  Gaudi::XYZVector v1(p.unit());
  Gaudi::XYZVector v2(p_mu.unit());
  Gaudi::XYZPoint temp1(0.,0.,0.);
  Gaudi::XYZPoint temp2(0.,0.,0.);
  fail = false;

  double  d02 = v0.Dot(v2);
  double  d21 = v2.Dot(v1);
  double  d01 = v0.Dot(v1);
  double  d22 = v2.Dot(v2);
  double  d11 = v1.Dot(v1);
  double  denom = d11 * d22 - d21 * d21;
  if (fabs(denom) <= 0.) {
    close1 = temp1;
    close2 = temp2;
    fail = true;
  }
  else {
    double numer = d02 * d21 - d01 * d22;
    double mu1 = numer / denom;
    double mu2 = (d02 + d21 * mu1) / d22;
    close1 = o+v1*mu1;
    close2 = o_mu+v2*mu2;
  }
  vertex = (close1+(close2-close1)*0.5);
}
//============================================================================
double TupleToolBu2phimunu_2::arcosine(Gaudi::XYZVector p1,
                                 Gaudi::XYZVector p2) const
{

  double num=(p1.Cross(p2)).R();
  double den=p1.R()*p2.R();
  double seno = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;
  }
  return alpha; 
}
//============================================================================
void TupleToolBu2phimunu_2::BDTvariableNames_bb_bkg_training(std::vector<std::string>& inNames) const {
  inNames.clear();
  inNames.push_back("track_minIPchi2");
  inNames.push_back("track_pvdis_mu");
  inNames.push_back("tracksvdis_mu" );
  inNames.push_back("track_doca_mu" );
  inNames.push_back("track_angle_mu");
  inNames.push_back("track_fc_mu"   );
  return; 
}
//============================================================================
//============================================================================
void TupleToolBu2phimunu_2::BDTvariableNames_Bplus_training(std::vector<std::string>& inNames) const {
  inNames.clear();
  inNames.push_back("track_minIPchi2" );
  inNames.push_back("track_eta" );
  inNames.push_back("track_phi" );
  inNames.push_back("track_pt" );
  inNames.push_back("track_MatchChi2" );
  inNames.push_back("track_pvdis_mu");
  inNames.push_back("tracksvdis_mu" );
  inNames.push_back("track_doca_mu" );
  inNames.push_back("track_angle_mu");
  inNames.push_back("track_fc_mu");
  return; 
}
//====================================================================================

