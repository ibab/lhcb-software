// Include files
#include "GaudiKernel/ToolFactory.h" 
#include "Event/Particle.h"  
// kernel 
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include "Kernel/IParticle2MCAssociator.h"
#include <Kernel/DVAlgorithm.h>
#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
#include "Kernel/IOnOffline.h"
// MC stuff
#include "Event/GenHeader.h" 
#include "Event/MCHeader.h" 

#include "TupleToolMuonVariables.h"
#include "TMath.h"
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolMuonVariables
//
// 2011-08-08 : Fatima Soomro 
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( TupleToolMuonVariables );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolMuonVariables::TupleToolMuonVariables( const std::string& type,
                                                  const std::string& name,
                                                  const IInterface* parent) 
  : TupleToolBase ( type, name , parent )
  , m_dva(0)
  , m_dist(0)

{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");
  
  declareProperty("ParticlePath",   m_ParticlePath="/Event/Phys/StdNoPIDsPions/Particles");
  declareProperty("PVInputLocation",m_PVInputLocation = "");
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty("angle"     , m_angle  = 0.27     ); // 
  declareProperty("fc"        , m_fc  = 0.60     ); // 
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); // 
  declareProperty("ips"       , m_ips  = 3.0     ); // 
  declareProperty("svdis"     , m_svdis  = -0.15     ); // 
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); // 
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); // 
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); // 
  declareProperty("isMC", m_isMC = false); 
  declareProperty( "IP2MCPAssociatorType", m_p2mcAssocType =  "DaVinciSmartAssociator");
  
}
//=============================================================================
// Destructor
//=============================================================================
TupleToolMuonVariables::~TupleToolMuonVariables() {} 

//=============================================================================
// Initialization
//=============================================================================
StatusCode TupleToolMuonVariables::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  
  
  if(m_isMC){
    m_p2mcAssoc = tool<IParticle2MCAssociator>(m_p2mcAssocType, this);
    if (0==m_p2mcAssoc) return Error("Couldn't get MC associator!! ",  StatusCode::FAILURE);  
  }
  
  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
                             StatusCode::FAILURE);  

  if ( "" == m_PVInputLocation){
    const IOnOffline* oo = tool<IOnOffline>("OnOfflineTool",this);
    m_PVInputLocation = oo->primaryVertexLocation();
    debug() << " Will be looking for PVs at " << m_PVInputLocation << endmsg ;
  } 

  m_dist = m_dva->distanceCalculator ();
  if( !m_dist ){
    Error("Unable to retrieve the IDistanceCalculator tool");
    return StatusCode::FAILURE;
  }

  m_pvReFitter = tool<IPVReFitter>("AdaptivePVReFitter", this );
  if(! m_pvReFitter) {
    fatal() << "Unable to retrieve AdaptivePVReFitter" << endreq;
    return StatusCode::FAILURE;
  }
  
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;
  return StatusCode::SUCCESS;

}


//=============================================================================
// Fill the tuple
//=============================================================================
StatusCode TupleToolMuonVariables::fill( const LHCb::Particle *top, const LHCb::Particle *part,
                                          const std::string &  	head, Tuples::Tuple &  	tuple	) {


  const std::string prefix=fullName(head);
  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;
  

  if ( abs(part->particleID().pid()) ==13  )  // only for muons
    {  
      StatusCode scTrackInfo = fillTrackInfo(part, prefix, tuple);
      if(!scTrackInfo) return scTrackInfo;
    }
  
  
  StatusCode scEta = fillEta(part, prefix, tuple);
  if(!scEta) return scEta;
  
  
  const SmartRefVector< LHCb::Particle > Bdaughters = part->daughters();
  int countDaughters =0; bool checkDau=true;
  bool JpsiInDau =false ; 
  
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = Bdaughters.begin() ; idau != Bdaughters.end() ; ++idau){
    if (NULL == (*idau)->proto() )
      {
	checkDau =false;
	debug()<<"daughter no "<<countDaughters<<"  is NULL!! "<<endreq;
	if( abs ((*idau)->particleID().pid() ) == 443) JpsiInDau = true;
      }
    else if (  abs((*idau)->particleID().pid()) == 13 ) countDaughters++;   // has muon as a daughter
    
  } //smartRef
  

  StatusCode fillJpsi =StatusCode::SUCCESS;  
  if(JpsiInDau) {
    fillJpsi = fillFakeVtx(part, prefix, tuple);
  }
  if(!fillJpsi) return fillJpsi;
  
  // Fill these only for B-> 2 body decays
  if  (checkDau && countDaughters ==2 && abs(part->particleID().pid()) >500 )
    {
      
      StatusCode fillIsolation_custom = fillIsolation(part, prefix, tuple);
      if (!fillIsolation_custom) return fillIsolation_custom;
      
      StatusCode scfillIso= CDFIsolation( part, part->daughtersVector().at(0), part->daughtersVector().at(1), prefix, tuple); 
      if (!scfillIso) return scfillIso;
      
      scfillIso= CDFIsolation_2( part, part->daughtersVector().at(0), part->daughtersVector().at(1), prefix, tuple); 
      if (!scfillIso) return scfillIso;
      
      //       StatusCode scfillYuri = CDFIsolation_Yuri( part, part->daughtersVector().at(0), part->daughtersVector().at(1), prefix, tuple); 
      //       if (!scfillYuri) return scfillYuri;
      
      //       StatusCode scfillAtlas = ATLASIsolation( part, prefix, tuple ) ;
      //       if(!scfillAtlas) return scfillAtlas;
      
      StatusCode scfillCosNK = fillCosNK(part, prefix, tuple);
      if(scfillCosNK.isFailure()) return StatusCode::FAILURE;  
      
      StatusCode scfillDoca=fillDoca(part, prefix, tuple);
      if (!scfillDoca) return scfillDoca;
      
    }
  
  
  if(m_isMC){
    StatusCode fillMC = fillMCTruth(part, prefix, tuple);
    if (!fillMC) return fillMC; 
  }
  
  else return StatusCode::SUCCESS;
  
  
  
}


//=============================================================================
//  Fill Eta
//=============================================================================
StatusCode TupleToolMuonVariables::fillEta(const LHCb::Particle *part,
					   const std::string prefix,
					   Tuples::Tuple& tuple ){
  
  bool test = tuple->column( prefix+"_eta", (part)->momentum().Eta()); 
  if(test) return StatusCode::SUCCESS;
  
}



//=============================================================================
// Track info, for muons only
//=============================================================================
StatusCode TupleToolMuonVariables::fillTrackInfo(const LHCb::Particle *part,
						 const std::string prefix,
						 Tuples::Tuple& tuple ){
  if( !part->isBasicParticle() || abs(part->particleID().pid()) != 13 ) return StatusCode::SUCCESS;
  
  const LHCb::ProtoParticle* protop = part->proto();
  if(!protop) return StatusCode::SUCCESS;
  const LHCb::Track* track = protop->track();
  if(!track) return StatusCode::SUCCESS;
  int nIDs(0), nVeloIDs(0), nITIDs(0), nOTIDs(0), nTTIDs(0);
  if(track){
    const std::vector<LHCb::LHCbID>& vids = track->lhcbIDs(); 
    nIDs = vids.size();
    for (std::vector<LHCb::LHCbID>::const_iterator iter = vids.begin() ; iter != vids.end() ; ++iter)
      {
	if( iter->isVelo()) nVeloIDs++;
	if( iter->isTT()) nTTIDs++;
	if( iter->isIT()) nITIDs++;
	if( iter->isOT()) nOTIDs++;
      }
  }// if track
  
  bool test = tuple->column( prefix+"_nVeloIDs", nVeloIDs);
  test &= tuple->column( prefix+"_nITIDs", nITIDs);
  test &= tuple->column( prefix+"_nTTIDs", nTTIDs);
  test &= tuple->column( prefix+"_nOTIDs", nOTIDs);
  
  if(test) return StatusCode::SUCCESS;
  
}



//=============================================================================
// Jpsi vertex distanct chi2 wrt the PV which minimizes the B IPchi2
//=============================================================================
StatusCode TupleToolMuonVariables::fillFakeVtx(const LHCb::Particle *part,
					       const std::string prefix,
					       Tuples::Tuple& tuple ){

  if( part->isBasicParticle() ) return StatusCode::SUCCESS;
  
  const  LHCb::Particle *myJpsi = NULL;
  const LHCb::Particle::ConstVector Bdaughters = part->daughtersVector();
  for (LHCb::Particle::ConstVector::const_iterator ipart= Bdaughters.begin(); ipart!=Bdaughters.end();++ipart) 
    {
      if( fabs ((*ipart)->particleID().pid() ) == 443) myJpsi = *ipart;
      debug()<<"Filling the fake vtx, found the Jpsi in the B daughters! "<<endreq;
      
    }
  
  bool test =true;

  const LHCb::VertexBase* Bvertex = part->endVertex();
  const LHCb::VertexBase* JpsiVtx = myJpsi->endVertex();  
 
  double ip, chi2;
  double ipfinal=-100;
  double chi2final=-100; double  minipchi =-1;
   
  const LHCb::RecVertex::Range PVrange = m_dva->primaryVertices(); 
  if ( !PVrange.empty() )
    {
      if(msgLevel(MSG::VERBOSE)) verbose() << "Filling Jpsi_dist_chi2 but the no of PVs is" << PVrange.size() << endmsg ;
      
      for ( LHCb::RecVertex::Range::const_iterator pv = PVrange.begin() ; pv!=PVrange.end() ; ++pv)
	{
	  StatusCode test2 = m_dist->distance ( *pv, Bvertex, ip, chi2 );
	  if( test2 )
	    {
	      if ( (chi2 < minipchi) || (minipchi<0.) )
		{
		  m_dist->distance ( *pv, JpsiVtx, ip, chi2 );
		  minipchi  = chi2;
		  chi2final = chi2;
		  ipfinal = ip ;
		}
	    }
	}// recVtx
    } // PVrange
  

  test &= tuple ->column ("Jpsi_dist_chi2", chi2final);
  
  const LHCb::VertexBase* goodPV = m_dva->bestPV(part);  
  double dist = -100; double distChi =-100;   
  StatusCode c1 = m_dist->distance( goodPV, JpsiVtx, dist, distChi);   
  if (! c1) distChi = -100;
  test &= tuple ->column ("Jpsi_dist_chi2_r", distChi);
  
  if (test) return StatusCode::SUCCESS;
  else  return StatusCode::FAILURE;
  
}


//-------------------------------------------------------------------------
// isolation variables
//-------------------------------------------------------------------------
StatusCode TupleToolMuonVariables::fillIsolation(const LHCb::Particle *part,
						 const std::string prefix,
						 Tuples::Tuple& tuple ){
  
  m_count_mum = 0;
  m_count_mup = 0;
  
  m_count_mum_f = 0;
  m_count_mup_f = 0;
  const LHCb::VertexBase* goodPV =  m_dva->bestPV(part); //getRelatedPV(part);
  Gaudi::XYZVector PV_pos;
  
  if (NULL==goodPV) {
    warning() << "No best PV for " << part->particleID().pid() << endmsg ;
    return StatusCode::SUCCESS;
  }
  
  PV_pos = goodPV->position();
  
  const LHCb::Particle::ConstVector pv = part->daughtersVector();
  
  int idx = 0;
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) 
    {
      if ( NULL==(*ipart)->proto() ) continue;
      idx++;
    }
  if(idx != 2 )  {
    return StatusCode::SUCCESS;
  }
  

  double p_track;
  double p_part;
  double pt_track;
  double pt_part;
  
  double pt_mu1(-1100.);
  double pt_mu2(-1100.);
  
  Gaudi::XYZVector pos_track;
  Gaudi::XYZVector pos_part;
  
  Gaudi::XYZVector mom_track;
  Gaudi::XYZVector mom_part;
  
  bool test =true; 
  std::string prefix1, prefix2; 
  
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
  
    if(((*ipart)->charge())>0){
      pt_mu1=(*ipart)->proto()->track()->pt();
      prefix1 = prefix+"_D1";
    }
    
    if(((*ipart)->charge())<0) {
      pt_mu2=(*ipart)->proto()->track()->pt();
      prefix2 = prefix+"_D2";
    }
    
  }
  
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
    
    const LHCb::Track*  mytrack= (*ipart)->proto()->track();

    pt_part= mytrack->pt();
    p_part = mytrack->p();
    pos_part = mytrack->position();
    mom_part = mytrack->momentum();
    int fCountIso=0;
    //LOOP SULLE TRACCE
    const LHCb::Tracks* tracks = get<LHCb::Tracks>( m_TracksPath );
    typedef std::vector<const LHCb::Track*> TrackContainer ;
    for( LHCb::Tracks::const_iterator it = tracks->begin() ; it != tracks->end(); ++it) {
     
      if(!((*it)->checkType(LHCb::Track::Long))){  continue; }
      fCountIso++;
      pt_track = (*it)->pt();
      p_track = (*it)->p();
      pos_track = (*it)->position();
      mom_track = (*it)->momentum();
      
      if(pt_track==pt_mu1)continue;
      if(pt_track==pt_mu2)continue; 

      double rapporto = TMath::Abs(pt_track-pt_part)/TMath::Abs(pt_track+pt_part); 
      if(rapporto<0.0001)continue;    
      
      Gaudi::XYZPoint vtx;
      Gaudi::XYZPoint close;
      Gaudi::XYZPoint close_mu;
      Gaudi::XYZPoint o( (*it) ->position());
      Gaudi::XYZPoint o_mu(mytrack->position());
      bool fail_f = false;
      double fc;
      closest_point(o,  mom_track, o_mu, mom_part, close, close_mu, vtx, fail_f); 
      
      Gaudi::XYZPoint rv_2 = vtx - PV_pos;
      Gaudi::XYZVector rv_vec  (rv_2.x(), rv_2.y(), rv_2.z());
      m_doca_tr  = (close-close_mu).R();
      double ZZ = rv_vec.z();
      double pt = mom_track.Rho()+ mom_part.Rho();
      Gaudi::XYZVector ptot(mom_track+mom_part);
      double alpha_2 = arcosine( rv_vec, ptot);
      double num_ff= ptot.R()*(alpha_2);
      double den_ff= num_ff +pt ;
      fc = num_ff/den_ff;
      
      bool flag_m = 0;
      bool flag_p = 0;
      
      if(((*ipart)->charge())>0) flag_p = 1;//mu-
      if(((*ipart)->charge())<0) flag_m = 1;//mu+
      
      if(flag_m && m_doca_tr<0.2 && ZZ>0 && ZZ<30 && fc <0.4 && fc >-1000 && pt_track>2.) m_count_mum++;// _f++;
      if(flag_p && m_doca_tr<0.2 && ZZ>0 && ZZ<30 && fc <0.4 && fc >-1000 && pt_track>2.) m_count_mup++; //_f++;
    
      
      
    }//loop tracks
    
    
  }//loop part
  
  //test &= tuple->column( prefix1+"_isolation", m_count_mup);
  //test &= tuple->column( prefix2+"_isolation", m_count_mum);
  
  //-------------------  Giampi's isolation: -------------------//
  
  int isotype = 5;
  debug()<<" GIAMPI just before calling getIso "<<endreq;
  
  //calling Giampi's tool. setting isotype=5 will output the new isolation variable 
  std::vector<int> iso5 = getIso( part, isotype);  
  debug()<<" GIAMPI just after calling getIso "
	 <<"GIAMPI  iso "<<iso5[0]<<"  "<<iso5[1]<<endreq;
  
  if (iso5[0]==-999) return StatusCode::FAILURE;
  
  if (pv.at(0)->charge() < 0) {
    m_count_mum_Giampi = static_cast<float>( iso5[0] );
    m_count_mup_Giampi = static_cast<float>( iso5[1] );

    m_count_mum_Fatima = static_cast<float>( iso5[2] );
    m_count_mup_Fatima = static_cast<float>( iso5[3] );
  }
  else{
    m_count_mum_Giampi = static_cast<float>( iso5[1] );
    m_count_mup_Giampi = static_cast<float>( iso5[0] );

    m_count_mum_Fatima = static_cast<float>( iso5[3] );
    m_count_mup_Fatima = static_cast<float>( iso5[2] );
  }
  
  debug()<<"filling tuple with Giampi's isolation vars "<<  m_count_mup_Giampi<<"  "<< m_count_mum_Giampi <<endmsg;
  
  test &= tuple->column( prefix1+"_isolation_Giampi", m_count_mup_Giampi);
  test &= tuple->column( prefix2+"_isolation_Giampi", m_count_mum_Giampi);
  
  //-------------------  Giampi's isolation ends -------------------//
  
  if(test) return StatusCode::SUCCESS;
  
}//fillIsolation(part)



//=============================================================================
// method to calculate Giampi's isolation
//=============================================================================
std::vector<int>  TupleToolMuonVariables::getIso(const LHCb::Particle* B, int isotype){

  const LHCb::VertexBase *PV = m_dva->bestPV(B);  /// Fatima, this will be the BPV of the jpsi in some cases!
  const LHCb::VertexBase *SV = B->endVertex();
  const LHCb::Particle::ConstVector parts = B->daughtersVector();  
  
  std::vector<int> iso(2, -9999);
  
  
  if(parts.size() !=2) {
    return iso;
  }
  
  StatusCode sc = StatusCode::SUCCESS;
  
  LHCb::Particles* allparts = get<LHCb::Particles>(m_ParticlePath);
  //LHCb::Particles* allparts = get<LHCb::Particles>(m_TracksPath);
  if (!allparts) {
    error() << " Failed to get particles container "
            <<  m_ParticlePath << endmsg;
    
    return iso;
  }
  
  
  Gaudi::XYZPoint PosPV = PV->position();
  Gaudi::XYZPoint PosSV = SV->position();
  
  int i = 0;
  
  ROOT::Math::SVector<int, 2> iso0;
  iso0[0]=0;
  iso0[1]=0; 
  ROOT::Math::SVector<int, 2> iso5;
  iso5[0]=0;
  iso5[1]=0;
  ROOT::Math::SMatrix<double, 3, 2> o_mu;
  ROOT::Math::SMatrix<double, 3, 2> p_mu;
  ROOT::Math::SVector<double, 2> ptmu;

  //Loop over input particles, get their simple kinematics
  LHCb::Particle::ConstVector::const_iterator ip_part;
  for ( ip_part = parts.begin(); ip_part != parts.end(); ip_part++) {    
    
    const LHCb::ProtoParticle * proto =  (*ip_part)->proto();
    const LHCb::Track* track = proto->track();
    o_mu(0,i) = track->position().x();
    o_mu(1,i) = track->position().y();
    o_mu(2,i) = track->position().z();
    p_mu(0,i) = track->momentum().x();
    p_mu(1,i) = track->momentum().y();
    p_mu(2,i) = track->momentum().z();
    ptmu[i] = sqrt(pow(p_mu(0,i),2)+pow(p_mu(1,i),2));
    i++;
  }
  
  if (i>2) { error()<<"more than 2 daughters of the B" <<endmsg;
    return iso;
  }
  
  
  for ( int i2 = 0; i2 < 2; i2++) {
    bool hltgood = false;
    double fc = 0.;
    Gaudi::XYZPoint omu(o_mu(0,i2),o_mu(1,i2),o_mu(2,i2));
    Gaudi::XYZVector pmu(p_mu(0,i2),p_mu(1,i2),p_mu(2,i2));
    
    //Loop over all particles  
    LHCb::Particles::const_iterator ip;
    int j=0;  // for counting ntracks
    for ( ip = allparts->begin(); ip != allparts->end() ; ++ip) {
      
      const LHCb::ProtoParticle * proto =  (*ip)->proto();
      const LHCb::Track* track = proto->track();
      const LHCb::Particle*  cand = (*ip);
      Gaudi::XYZPoint o(track->position());
      Gaudi::XYZVector p(track->momentum());
      bool isInList = 0;
      double pt = p.Rho();
      double ptot = p.R();

      if (track->type()!=m_tracktype)   continue; 
      j++; 
      
      // skip if other particle is in input list 
      if (ratio(pt, ptmu[0]) < 0.0001 || ratio(pt,ptmu[1]) <0.0001) {  continue; }

      IsHltGood(o, p, omu, pmu ,PosPV, hltgood, fc); 
      
      if (track->type()== m_tracktype) {
	iso0[i2] += hltgood;
	debug()<<"isisold "<< i2 << iso0[i2]<< endreq;
      }
      
      // find doca and angle between input and other tracks
      Gaudi::XYZPoint vtx(0.,0.,0.);
      double doca(-1.);          
      double angle(-1.);
      InCone(omu,pmu, o, p, vtx, doca, angle);
      
      // find impact parameters, distances from secondary and primary vertex
      double imp = 0.;
      double impchi2 = 0.;
      double ips,pvdis,svdis;
      ips=100000.;
      LHCb::RecVertex::Container::const_iterator iv;
      LHCb::RecVertex::Container* verts = NULL;
      
      if(exist<LHCb::RecVertex::Container>(m_PVInputLocation))
	{
	  verts = get<LHCb::RecVertex::Container>(m_PVInputLocation);
	}
      for ( iv = verts->begin(); iv != verts->end(); iv++) {
	m_dist->distance(&(*cand),(*iv),imp,impchi2);
	if (impchi2<ips) ips = impchi2;
      }      
      ips=sqrt(ips);
      
      double deltaZvtx = (vtx.z()-PosPV.z());
      
      pvdis = (vtx.z()-PosPV.z())/fabs(vtx.z()-PosPV.z())*(vtx-PosPV).R();
      svdis = (vtx.z()-PosSV.z())/fabs(vtx.z()-PosSV.z())*(vtx-PosSV).R();
      
      // non-isolating criteria #5
      if (angle <m_angle && fc<m_fc && doca<m_doca_iso && ips>m_ips && 
          svdis>m_svdis && svdis<m_svdis_h && pvdis>m_pvdis && pvdis<m_pvdis_h 
          && track->type()==m_tracktype) {
	iso5[i2] += 1;
      }// if 
      
    }// allparts
    
  } //i2
  
  
  if (isotype==0) {
    iso[0] = iso0[0] ;
    iso[1] = iso0[1] ; 
  }
  else if (isotype==5) {
    iso[0] = iso5[0] ;
    iso[1] = iso5[1] ;
    
    iso[2] = iso0[0] ;
    iso[3] = iso0[1] ;
  }
  
  return iso;

}


//=============================================================================
// IsHLTGood method,used by isolation calculation
//=============================================================================
void  TupleToolMuonVariables::IsHltGood(Gaudi::XYZPoint o,Gaudi::XYZVector p,
					 Gaudi::XYZPoint o_mu,Gaudi::XYZVector 
					 p_mu, Gaudi::XYZPoint PV, bool& hltgood,
					 double& fc) {
  
  Gaudi::XYZVector rv;
  Gaudi::XYZPoint vtx;
  Gaudi::XYZPoint close;
  Gaudi::XYZPoint close_mu;
  bool fail(false);

  closest_point(o,p,o_mu,p_mu,close,close_mu,vtx,fail);

  if (fail) {
    fc = -1.;
    hltgood = -1;
  }
  else {
    double pete = p.Rho();
    rv = vtx - PV;
    double DOCA_b = (close-close_mu).R();
    double ZZ = rv.z();
    fc = pointer(rv,p,p_mu);
    hltgood=( (DOCA_b<0.2) && (ZZ>0.) && (ZZ<30.) && (fc<0.4) && (pete>2.) );
  }
}


double TupleToolMuonVariables::pointer (Gaudi::XYZVector vertex,
                                 Gaudi::XYZVector p, Gaudi::XYZVector
                                 p_mu)  {
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double temp = arcosine(vertex,ptot);
  double  num=ptot.R()*sin(temp);
  double  den=(num+pt);
  double fc = num/den;
  return fc;
}



//=============================================================================
// Fill DOCA:
//=============================================================================
StatusCode TupleToolMuonVariables::fillDoca(const LHCb::Particle *part, 
					     const std::string prefix, 
					     Tuples::Tuple& tuple ){
  bool test = true;
  
  double doca=-999;

  const LHCb::Particle::ConstVector pv = part->daughtersVector();

  const LHCb::Particle *imp, *imm;
  int idx = 0;
  for (LHCb::Particle::ConstVector::const_iterator ipart=pv.begin();ipart!=pv.end();++ipart) {
    
    if ( NULL==(*ipart)->proto() ) continue;
    
    if(idx == 0) {imp = (*ipart);}
    else if(idx == 1) {imm = (*ipart);}
    idx++;
  }
  if(idx != 2)  {
    return StatusCode::SUCCESS;
  }
  
  StatusCode sc_s = m_dist->distance(imp, imm, doca);
  if(sc_s)m_doca=doca;
  
  test &= tuple->column( prefix+"_doca",  m_doca);
  if(test) return StatusCode::SUCCESS;
  
}


//=============================================================================
// Fill cosnk
//=============================================================================

StatusCode TupleToolMuonVariables::fillCosNK(const LHCb::Particle *part, 
					      const std::string prefix, 
					      Tuples::Tuple& tuple ){
  
  //---------------------------------------------------------------------
  // (by Marco)
  // Translation of Yury's python code
  
  bool test = true;
  
  LHCb::Particle::ConstVector vdaugh = part->daughtersVector();
  int idx=0;
  LHCb::Particle::ConstVector::const_iterator it;
  for( it = vdaugh.begin(); vdaugh.end()!=it; ++it ){
    if ( NULL==(*it)->proto() ) continue;
    else idx++;
  }
  
  if(idx != 2)  {
    return StatusCode::SUCCESS;
  }
  
  
  double px_mu_minp;
  double px_part;
  double py_mu_minp;
  double py_part;

  //masses PDG
  double mass_B = 5366.3;
  double mass_mu = 105.658;
  double mmass_B;
  
  
  px_part = part->momentum().px();
  py_part = part->momentum().py();
  mmass_B = part->measuredMass();
  
  debug() <<"Inside cosnk, obtained B quantities" <<endreq;
  
  if ( vdaugh.at(0)->momentum().pt() < vdaugh.at(1)->momentum().pt() ) {
    debug() <<"obtaining daughter quantities" <<endreq;
    px_mu_minp = vdaugh.at(0)->momentum().px();
    py_mu_minp = vdaugh.at(0)->momentum().py();
  } else {
    debug() << "obtaining daughter quantities" <<endreq;
    px_mu_minp = vdaugh.at(1)->momentum().px();
    py_mu_minp = vdaugh.at(1)->momentum().py();
  }
  
  m_cosnk = 2*(py_mu_minp*px_part - px_mu_minp*py_part) / 
    sqrt(px_part*px_part + py_part*py_part) / sqrt(mmass_B*mmass_B - 4*mass_mu*mass_mu);
  
  debug() <<"filling cosnk =   "<<m_cosnk << endreq;
  test &= tuple->column( prefix+"_cosnk",  m_cosnk );

  if(test)   return StatusCode::SUCCESS;  
  
}



//=============================================================================
// Other functions needed by isolation 
//=============================================================================

double TupleToolMuonVariables::getphi(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  double dphi = vdau1->momentum().Phi() - vdau2->momentum().Phi();
  return dphi;
}


//=============================================================================
double TupleToolMuonVariables::gettheta(const LHCb::Particle* vdau1, const LHCb::Particle* vdau2){
  
  double dtheta = vdau1->momentum().Eta() -  vdau2->momentum().Eta();
  return dtheta;
}


//=============================================================================
double TupleToolMuonVariables::ratio( double p1, double p2){  
  return fabs(p1 -p2)*(1./fabs(p1+p2)); 
}



//=============================================================================
double TupleToolMuonVariables::IsClose(const LHCb::Particle* p1,const LHCb::Particle* p2) {
  
  double deta = gettheta(p1,p2);
  double dphi = getphi(p1,p2);
  return sqrt(deta*deta+dphi*dphi);
}


//=============================================================================
void TupleToolMuonVariables::closest_point(Gaudi::XYZPoint o,Gaudi::XYZVector p,
					    Gaudi::XYZPoint o_mu,Gaudi::XYZVector p_mu, 
					    Gaudi::XYZPoint& close1, 
					    Gaudi::XYZPoint& close2, 
					    Gaudi::XYZPoint& vertex, bool& fail) {
  
  
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


double TupleToolMuonVariables::arcosine(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {
  
  double num    = (p1.Cross(p2)).R();
  double den    = p1.R()*p2.R();
  double seno   = num/den;
  double coseno = p1.Dot(p2)/den;
  double alpha  = asin(fabs(seno));
  if (coseno < 0 ) {
    alpha = ROOT::Math::Pi() - alpha;
  }
  return alpha;
}



//=============================================================================
void TupleToolMuonVariables::InCone(Gaudi::XYZPoint o1, 
				     Gaudi::XYZVector p1,Gaudi::XYZPoint o2, 
				     Gaudi::XYZVector p2,
				     Gaudi::XYZPoint& vtx, double& 
				     doca, double& angle){
  
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


//=============================================================================
// To match the CERN definition... should implement this in CDFisolation and remove this
//=============================================================================
StatusCode TupleToolMuonVariables::CDFIsolation_2(const LHCb::Particle* B,
						 const LHCb::Particle* vdau1,
						 const LHCb::Particle* vdau2,
						 const std::string prefix, 
						 Tuples::Tuple& tuple ){
  
  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug()<<"Inside CDFisloation_2, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS;
  }

  const LHCb::Tracks* mytracks = get<LHCb::Tracks>( m_TracksPath);
  if (!mytracks) {
    error() << " Failed to get "<< m_TracksPath <<" tracks container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  
  bool test = true;
  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();
  double pt_bs = B->momentum().rho();
  double p_bs = B->p();
  double ptclose = 0.;
  double ptclose01200 = 0.;
  double ptclose04100 = 0.;
  double iso_giampi =  pt_bs; 
  

  double px_bs = B->momentum().x();
  double py_bs = B->momentum().y();
  double pz_bs = B->momentum().z();
  double pl_bs = TMath::Sqrt( p_bs*p_bs - pt_bs*pt_bs) ;
  
  double etha_bs = 0.5* ( TMath::Log (  (p_bs+pl_bs)/(p_bs-pl_bs ) ) );
  double phi_bs = TMath::ACos( (px_bs/pt_bs) );
  
  if (py_bs<0) {
    etha_bs = - etha_bs;
    phi_bs = -phi_bs;
  }
  

  typedef std::vector<const LHCb::Track*> TrackContainer ;
  for( LHCb::Tracks::const_iterator it = mytracks->begin() ; it != mytracks->end(); ++it) 
    {  
      if( (*it)->type() == m_tracktype)
	{
       
	  const LHCb::Track* atrack = (*it);
	  Gaudi::XYZVector ptrack(atrack->momentum());
	  double pttrack = ptrack.rho();
	  double p_track = atrack->p();
	  double pl_track = TMath::Sqrt( p_track* p_track - pttrack * pttrack );
	  
	  double phi_track = atrack->phi();
	  double delta_phi = TMath::Abs( (phi_bs - phi_track) );
	  if ( delta_phi> TMath::Pi() )  delta_phi = 2*TMath::Pi() -delta_phi;
	  
	  
	  double etha_track = 0.5* (TMath::Log( (p_track + pl_track)/(p_track -pl_track) ) );
	  if (phi_track < 0) etha_track = - etha_track; 
	  double delta_etha = TMath::Abs(etha_bs - etha_track);
	  
	  double rad_cone = TMath::Sqrt(  (delta_phi*delta_phi + delta_etha*delta_etha) );
	  
	  double del1 = TMath::Abs (ptmu1 - pttrack);
	  double del2 = TMath::Abs (ptmu2 - pttrack);
	  if (del1 > del2 ) del1 = del2;
	  
	  if( (del1 > 0.01) && (rad_cone <1.0) ) iso_giampi +=pttrack;
	  
	} // long track
    } // mytracks->begin()

  
  iso_giampi = pt_bs/iso_giampi;
  test &= tuple->column( prefix+"_CDF_iso_CDF", iso_giampi);
  return test;
  
}



//=============================================================================
// compute CDF isolation variable
//=============================================================================
StatusCode TupleToolMuonVariables::CDFIsolation(const LHCb::Particle* B,
						 const LHCb::Particle* vdau1,
						 const LHCb::Particle* vdau2,
						 const std::string prefix, 
						 Tuples::Tuple& tuple ){
  
  
  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug()<<"Inside CDFisloation, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  bool test = true ;
  
  double ptmu1 =  vdau1->proto()->track()->momentum().rho();
  double ptmu2 =  vdau2->proto()->track()->momentum().rho();  
  double ptbs  =  B->momentum().rho(); 
  double ptclose = 0.; double ptclose_check = 0.;
  double isoCDF;
  
  debug()<<" pts:: "<<ptmu1<<" "<<ptmu2<<" "<<ptbs<<endreq;


  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Particles::const_iterator ipp;

  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	Gaudi::XYZVector ptrack(atrack->momentum());
	double pttrack = ptrack.rho();

	if (ratio(pttrack, ptmu1) > 0.0001 && ratio(pttrack,ptmu2) > 0.0001) 
	  { 
	    if ( IsClose(B,(*ipp)) <= 1.0) {
	      ptclose += pttrack;
	    }
	  }
      }// if atrack
    } // if proto
  } // allparts
  
  isoCDF=ptbs/(ptclose+ptbs);
  m_CDFIso = isoCDF;
  test &= tuple->column( prefix+"_CDFiso", m_CDFIso);

  if (test)   return StatusCode::SUCCESS;
  
}



//=============================================================================
// CDFisolation, directly from diego (Flavio)
//=============================================================================

StatusCode TupleToolMuonVariables::CDFIsolation_Yuri(const LHCb::Particle* B,
						      const LHCb::Particle* vdau1,
						      const LHCb::Particle* vdau2,
						      const std::string prefix, 
						      Tuples::Tuple& tuple ){
  
  bool test =true; 
  debug()<<" Computation of Yuri's CDF iso variable "<<endreq;

  if ( (NULL == vdau1 ) || NULL == vdau2) {
    debug() <<"Inside CDFisloation_Yuri, one or both  daughters are NULL"<<endmsg;
    return StatusCode::SUCCESS; 
  }
  
  double PI = TMath::Pi();// 3.1415926;
  double ptmu1 =  vdau1->proto()->track()->pt();
  double pxmu1 =  vdau1->proto()->track()->momentum().x();
  double pymu1 =  vdau1->proto()->track()->momentum().y();
  double pzmu1 =  vdau1->proto()->track()->momentum().z();
  double ptmu2 =  vdau2->proto()->track()->pt();
  double pxmu2 =  vdau2->proto()->track()->momentum().x();  
  double pymu2 =  vdau2->proto()->track()->momentum().y();  
  double pzmu2 =  vdau2->proto()->track()->momentum().z();  

  double ptbs  =  B->pt();  
  double pxbs  =  B->momentum().px();
  double pybs  =  B->momentum().py();  
  double pzbs  =  B->momentum().pz();
  double pbs  =  B->p();
  double plbs  =  sqrt(pbs*pbs+ptbs*ptbs);
  
  Gaudi::XYZVector PV_pos;
  const LHCb::VertexBase* goodPV =  m_dva->bestPV(B); 
  
  if (NULL==goodPV) {
    warning() << "No best PV for " << B->particleID().pid() << endmsg ;
    return StatusCode::SUCCESS;
  }
  PV_pos = goodPV->position();
  
  double ptclose = 0.;
  double isoCDFyuri;
  
  debug()<<" pts:: "<<ptmu1<<" "<<ptmu2<<" "<<ptbs<<endreq;
  
  double ethabs = 0.5*log((pbs+plbs)/(pbs-plbs ));
  if (pybs<0) ethabs = -ethabs;
  
  double phibs = acos(pxbs/ptbs);
  if (pybs>0 && pxbs>0) phibs =  phibs;
  if (pybs<0 && pxbs>0) phibs = -phibs;
  if (pybs<0 && pxbs<0) phibs = -phibs;
  if (pybs>0 && pxbs<0) phibs =  phibs;
  
  double iso_bs  = ptbs;
  
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }
  
  LHCb::Particles::const_iterator ipp;
  
  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	Gaudi::XYZVector ptrack(atrack->momentum());
	double phitrack = ptrack.Phi();
	double deltaphi = fabs(phibs - phitrack);
	if (deltaphi>PI)  deltaphi = 2*PI-deltaphi;
	
	double pltrack = sqrt(atrack->p()*atrack->p()-atrack->pt()*atrack->pt());
	double ethatrack = 0.5*log((atrack->p()+pltrack)/(atrack->p()-pltrack));
	if (phitrack < 0) ethatrack = -ethatrack;
	double deltaetha = abs(ethabs - ethatrack);
	double radcone = sqrt(deltaphi*deltaphi+deltaetha*deltaetha);
	double trpt = atrack->pt();
	
	double trIP = 0;
	m_dist->distance(atrack, goodPV, trIP  );
	
	double mindeltapt = fabs(ptmu1-atrack->pt());
	if (fabs(ptmu2-atrack->pt())<mindeltapt) mindeltapt= fabs(ptmu2-atrack->pt());
	
	bool flag =( mindeltapt>0.01 && radcone<1.);
	debug() <<"radcone:: "<< radcone << " and mindeltapt:: "<< mindeltapt <<"  so the flag is  "<< flag << endreq; 
	
	if (flag && trIP<0.1 && atrack->pt()> 200.) iso_bs += trpt;
	
      } // if(atrack)
    } // if (proto)
  } // parts->begin
  
  
  isoCDFyuri = ptbs/iso_bs;
  debug()<<" iso_bs & ptbs for iso:: "<<iso_bs<<" "<<ptbs<<endreq;
  m_isoCDFyuri = isoCDFyuri;

  test &= tuple->column( prefix+"_CDFiso_yuri",  m_isoCDFyuri);
  if(test)   return StatusCode::SUCCESS;
  
}  // isoCDFYuri




//=============================================================================
// ATLAS isolation 
//=============================================================================

StatusCode TupleToolMuonVariables::ATLASIsolation(const LHCb::Particle* B,
						   const std::string prefix, 
						   Tuples::Tuple& tuple ){
  
  bool test = true;
  
  const LHCb::VertexBase* primary =  m_dva->bestPV(B);
  const LHCb::VertexBase* secondary =  B->endVertex();
  
  double n_incone = 0.;
  
  Gaudi::XYZVector pbs(B->slopes());
  
  LHCb::Particles*  parts = get<LHCb::Particles>(m_ParticlePath);
  if (!parts) {
    error() << " Failed to get particles container " << endmsg;
    return StatusCode::SUCCESS;
  }

  LHCb::Particles::const_iterator ipp;
  for(ipp= parts->begin();ipp!=parts->end();ipp++){
    const LHCb::ProtoParticle *proto = (*ipp)->proto();
    if(proto) {
      const LHCb::Track* atrack = proto->track();
      if(atrack) {
	
	Gaudi::XYZVector ptrack(atrack->momentum());
	Gaudi::XYZPoint otrack(atrack->position());
	
	Gaudi::XYZPoint PosPV = primary->position();
	Gaudi::XYZPoint PosSV = secondary->position();
	Gaudi::XYZPoint vtx(0.,0.,0.);
	double doca(-1.);          
	double angle(-1.);
	
	double imp = 0.;
	double ips = -1.;
	InCone(PosSV,pbs,otrack, ptrack,vtx, doca, angle);
	m_dist->distance((*ipp),primary,imp,ips);
	ips=sqrt(ips);
	
	if (ips > 0. and doca < 0.2 and angle < 0.2618) {
	  // 0.2mm + 15deg
	  n_incone+=1;
	}
      
      }// if (atrack)
    } // if(proto)
  } // parts->begin

  test &= tuple->column( prefix+"_ATLASiso", n_incone);
  if (test) return StatusCode::SUCCESS; 

} // ATLASIsolation 



//==============================
// MC Ancestor Finding Routine:
//==============================
const LHCb::MCParticle* TupleToolMuonVariables::originof( const LHCb::MCParticle* product ) {
  const LHCb::MCParticle* mother = product->mother();
  if ( (!mother) || product->particleID().hasBottom() ) return product;
  else return originof( mother );
}


//=============================================================================
// Fill MC quantities
//=============================================================================
StatusCode TupleToolMuonVariables::fillMCTruth(const LHCb::Particle *part, const std::string prefix, Tuples::Tuple& tuple ){
  
  if(! m_isMC){
    
    debug()<<" Nothing to fill from fillMCTruth "<<endmsg;
    return StatusCode::SUCCESS;
  }
  
  Assert( m_p2mcAssoc 
          , "The DaVinci smart associator hasn't been initialized!");
  
  bool test =true;
  const LHCb::MCParticle* MCp = NULL;
  const LHCb::MCParticle* ancestor =NULL;
  
  MCp = m_p2mcAssoc->relatedMCP(part);
  if (!MCp) {
    debug()<<" Didnt find MCP "<<endmsg;
  }
  
  else {
    ancestor = originof(MCp) ;
    m_mcancestorpid =  ancestor->particleID().pid();
    debug()<<" Filled ancestor PID "<<endreq;      
    
    SmartRefVector< LHCb::MCVertex >::const_iterator iVtx;
    if (0==MCp->endVertices().size())
      {
	debug() << " vertex failed " << endreq;
	m_endVertices  = -100;
      }
    else 
      {
	m_endVertices = MCp->endVertices().size();
      }
  
    
    
   
      m_zOrigin = MCp->originVertex()->position().z();
      m_typeOrigin =  MCp->originVertex()->type();
  
  }// i.e. we have an MCp
   
  m_MCI = -1000; m_MCPV = -1000;
  const LHCb::GenHeader* gh = get<LHCb::GenHeader>(LHCb::GenHeaderLocation::Default);
  if(gh && gh->collisions().size()>0)  
    {
      m_MCI = gh->collisions().size() ;
      verbose() << "retrieved MCInteractions from genHeader:  " << m_MCI<< endmsg;
    }
  
 
  test &= tuple->column( prefix+"_typeOriginVtx", m_typeOrigin);
  test &= tuple->column( prefix+"_nEndVertices",  m_endVertices);
  test &= tuple->column( prefix+"_ancestorpid", m_mcancestorpid);
  test &= tuple->column( prefix+"_MCI", m_MCI);
  
  if(test) return StatusCode::SUCCESS;
  
}
  


//=============================================================================
// Finalize
//=============================================================================
StatusCode TupleToolMuonVariables::finalize(){
  
  return TupleToolBase::finalize();
  

}
