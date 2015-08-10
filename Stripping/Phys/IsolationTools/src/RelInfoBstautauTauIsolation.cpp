#include "GaudiKernel/ToolFactory.h"
#include "RelInfoBstautauTauIsolation.h"
#include "Kernel/RelatedInfoNamed.h"
#include "Event/Particle.h"
// kernel
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/PhysicalConstants.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IPVReFitter.h"
//#include "Kernel/IOnOffline.h"
#include "Kernel/IDVAlgorithm.h"
#include <Kernel/GetIDVAlgorithm.h>


#include "TMath.h"
#include "TFile.h"
#include "TString.h"
#include <TROOT.h>
#include <TObject.h>
#include "TH1D.h"
//#include "TMVA/Reader.h"
#include "Kernel/IRelatedPVFinder.h"
#include "Math/Boost.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoBstautauTauIsolation
// Converted from ConeVariables by A. Shires
//
// 2014-07-25 : Giampiero Mancinelli
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoBstautauTauIsolation )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoBstautauTauIsolation::RelInfoBstautauTauIsolation( const std::string& type,
                                                          const std::string& name,
                                                          const IInterface* parent) : GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);

  declareProperty( "tracktype", m_tracktype = 3,
                   "Set the type of tracks which are considered inside the cone (default = 3)");

  setProperty("PVInputLocation",m_PVInputLocation = LHCb::RecVertexLocation::Primary);
  declareProperty ( "TrackContainer",   m_TracksPath = LHCb::TrackLocation::Default); // default is "Rec/Track/Best "
  declareProperty("angle"     , m_angle  = 0.27     ); //
  declareProperty("fc"        , m_fc  = 0.60     ); //
  declareProperty("doca_iso"  , m_doca_iso  = 0.13     ); //
  declareProperty("ips"       , m_ips  = 3.0     ); //
  declareProperty("svdis"     , m_svdis  = -0.15     ); //
  declareProperty("svdis_h"   , m_svdis_h  = 30.     ); //
  declareProperty("pvdis"     , m_pvdis  = 0.5     ); //
  declareProperty("pvdis_h"   , m_pvdis_h  = 40.    ); //
  declareProperty( "Variables", m_variables,
                   "List of variables to store (store all if empty)");
  declareProperty(    "PVInputLocation"
                      , m_PVInputLocation = LHCb::RecVertexLocation::Primary
                      , " PV input location"
                      );

  //sort these keys out by adding all
  m_keys.clear();
  /*    std::vector<std::string>::const_iterator ivar;
        for (ivar = m_variables.begin(); ivar != m_variables.end(); ivar++) {
        short int key = RelatedInfoNamed::indexByName( *ivar );
        if (key != RelatedInfoNamed::Unknown) {
        m_keys.push_back( key );
        } else {
        }
        }
  */
  // m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUE );
  // m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUE );
  m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUETAUP );
  m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUETAUP );
  m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUETAUM );
  m_keys.push_back( RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUETAUM );
  
}

//=============================================================================
// Destructor
//=============================================================================
RelInfoBstautauTauIsolation::~RelInfoBstautauTauIsolation() {}


//=============================================================================
// Initialize
//=============================================================================
StatusCode RelInfoBstautauTauIsolation::initialize() {
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  m_descend = tool<IParticleDescendants> ( "ParticleDescendants", this );
  if( ! m_descend ) {
    fatal() << "Unable to retrieve ParticleDescendants tool "<< endreq;
    return StatusCode::FAILURE;
  }
  return StatusCode::SUCCESS;

}

//=============================================================================
// Fill Extra Info structure
//=============================================================================
StatusCode RelInfoBstautauTauIsolation::calculateRelatedInfo( const LHCb::Particle *top,
                                                              const LHCb::Particle *top_bis )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "Calculating TauIso extra info" << endmsg;
  m_bdt1 = -10000.0 ;
  m_bdt2 = -10000.0 ;
  m_bdt1_TauP = -10000.0 ;
  m_bdt2_TauP = -10000.0 ;
  m_bdt1_TauM = -10000.0 ;
  m_bdt2_TauM = -10000.0 ;

  if ( top->isBasicParticle() || top!=top_bis)
  {
    if ( msgLevel(MSG::DEBUG) ) 
      debug() << "top != top_bis" << endmsg;
    return StatusCode::SUCCESS ;
  }

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.

  // -- Clear the vector with the particles in the specific decay
  m_decayParticles.clear();

  // -- Get all tracks in the event
  LHCb::Particles* tracks = get<LHCb::Particles>("/Event/Phys/StdAllNoPIDsPions/Particles");
  //    LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
  if ( tracks->empty() )
  {
    if ( msgLevel(MSG::WARNING) ) Warning( "Could not retrieve tracks. Skipping" );
    return StatusCode::FAILURE;
  }

  bool test = true;

  //set PV and SV of the mother
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  const LHCb::VertexBase* PV = dva->bestVertex(top);
  const LHCb::VertexBase *SV = top->endVertex();

  if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
    m_vertices = get<LHCb::RecVertex::Container>(m_PVInputLocation);
  }

  LHCb::Particle::ConstVector Daughters = m_descend->descendants(top,1);
  LHCb::Particle::ConstVector::const_iterator i_daug; 
 // Int_t flag(0);
  for ( i_daug = Daughters.begin(); i_daug != Daughters.end(); i_daug++){
    const LHCb::Particle* part = *i_daug;
    if(!part) {
      return Warning( "Found an invalid particle" ,StatusCode::FAILURE,50);
    }else if( part )
      {
        if(part->isBasicParticle())
          {
            if ( msgLevel(MSG::VERBOSE) ) verbose() << "Trying to compute Tau (D) isolation on a muon (pion)"<<endmsg;
          }
        else
          {
            if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling variables with particle " << part << endmsg;
              calcValue( part, tracks, PV, SV );
            //  if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2 << endmsg ;

   bool P_charge = true;
        	bool flag_OS = true;
	        bool flag_tau_mu = false;
	         for (LHCb::Particle::ConstVector::const_iterator i_daug_2 = Daughters.begin(); i_daug_2 != Daughters.end() ; i_daug_2++){
	        const LHCb::Particle* part2 = *i_daug_2;
		if (i_daug_2 !=i_daug ){
	        if((part->charge())*(part2->charge())>0) flag_OS=false;
	        if((part2->p())>(part->p())) P_charge = false; 
		if((part->isBasicParticle())||(part2->isBasicParticle())) flag_tau_mu=true;
	        }
	        }


        	if((flag_OS==true)||(flag_tau_mu==true)){
	        if(part->charge()>0){ 
		m_bdt1_TauP=m_bdt1;
                m_bdt2_TauP=m_bdt2;
              		}
		if(part->charge()<0){ 
		m_bdt1_TauM=m_bdt1;
                m_bdt2_TauM=m_bdt2;
              		}
		}else if(flag_OS==false){// i.e. it's same sign
		if(P_charge){ 
		m_bdt1_TauP=m_bdt1;
                m_bdt2_TauP=m_bdt2;
             		}
		if(!P_charge){ 
		m_bdt1_TauM=m_bdt1;
                m_bdt2_TauM=m_bdt2;
             		}
		}


/*
            if(part->charge()>0&&(flag==0||flag==2))
              {
                if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2 << endmsg ;
                m_bdt1_TauP=m_bdt1;
                m_bdt2_TauP=m_bdt2;
                              flag=1;
              }else if(part->charge()>0&&flag==1)
              {
                if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2 << endmsg ;
                m_bdt1_TauM=m_bdt1;
                m_bdt2_TauM=m_bdt2;
                             }else if(part->charge()<0&&(flag==0||flag==1))
              {
                if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2 << endmsg ;
                m_bdt1_TauM=m_bdt1;
                m_bdt2_TauM=m_bdt2;
                flag=2;
              }else if(part->charge()<0&&flag==2)
              {
                if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2  << endmsg ;
                m_bdt1_TauP=m_bdt1;
                m_bdt2_TauP=m_bdt2;
              }
*/
          }
      }
  }
  
  
  if ( msgLevel(MSG::DEBUG) ) debug()<<" m_bdt1_TauP : "<<m_bdt1_TauP<<"  ,  m_bdt2_TauP : "<<m_bdt2_TauP<<endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug()<<" m_bdt1_TauM : "<<m_bdt1_TauM<<"  ,  m_bdt2_TauM : "<<m_bdt2_TauM<<endmsg;

  
  m_map.clear();
  
  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {
    
    float value = 0;
    switch (*ikey) {
    case RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUETAUP : value = m_bdt1_TauP; break;
    case RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUETAUP : value = m_bdt2_TauP; break;
    case RelatedInfoNamed::BSTAUTAUTAUISOFIRSTVALUETAUM : value = m_bdt1_TauM; break;
    case RelatedInfoNamed::BSTAUTAUTAUISOSECONDVALUETAUM : value = m_bdt2_TauM; break;
    }
    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;
    
    m_map.insert( std::make_pair( *ikey, value) );
  }
  



  return StatusCode(test);
}


//rel infor methods

LHCb::RelatedInfoMap* RelInfoBstautauTauIsolation::getInfo(void) {
  return &m_map;
}

std::string RelInfoBstautauTauIsolation::infoPath(void) {
  std::stringstream ss;
  ss << std::string("Particle2TauIsolationRelations");
  return ss.str();
}

//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoBstautauTauIsolation::saveDecayParticles( const LHCb::Particle *top)
{

  // -- Get the daughters of the top particle
  const SmartRefVector< LHCb::Particle > & daughters = top->daughters();

  // -- Fill all the daugthers in m_decayParticles
  for( SmartRefVector< LHCb::Particle >::const_iterator idau = daughters.begin() ; idau != daughters.end() ; ++idau){

    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() ){
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      m_decayParticles.push_back( (*idau) );
    }else{
      // -- if it is not stable, call the function recursively
      m_decayParticles.push_back( (*idau) );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      saveDecayParticles( (*idau) );
    }

  }

}

//=============================================================================
// Check if the track is already in the decay
//=============================================================================
bool RelInfoBstautauTauIsolation::isTrackInDecay(const LHCb::Track* track){

  bool isInDecay = false;

  for(  std::vector<const LHCb::Particle*>::iterator it = m_decayParticles.begin() ; it != m_decayParticles.end() ; ++it ){

    const LHCb::ProtoParticle* proto = (*it)->proto();
    if(proto){
      const LHCb::Track* myTrack = proto->track();

      if(myTrack){

        if(myTrack == track){
          if ( msgLevel(MSG::DEBUG) ) debug() << "Track is in decay, skipping it" << endmsg;
          isInDecay = true;
        }
      }
    }
  }
  return isInDecay;
}


///============================================================================
/// Tau isolation method
///============================================================================



bool RelInfoBstautauTauIsolation::calcValue( const LHCb::Particle * part
                                             , const LHCb::Particles * tracks
                                             , const LHCb::VertexBase * PV
                                             , const LHCb::VertexBase * SV
                                             )
{
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  //double bdtval = 0 ;
  //double bdtmin = 0 ;
  double doca   = 0;
  double angle  = 0;
  double fc     = 0;
  double pvDistGeometric(0) ;
  double svDistGeometric(0) ;
  double ipchisqany(0);
  int isolation1(0);
  int isolation2(0);

  LHCb::Particles::const_iterator tr_it, tr_it_end( tracks->end() ) ;
  for ( tr_it = tracks->begin() ; tr_it != tr_it_end ; ++tr_it )
  {
    const LHCb::Track * track = (* tr_it)->proto()->track() ;
    bool Indecay = false ;
    //pointer comparison

    if (isTrackInDecay(track)) Indecay = true ;

    Gaudi::XYZVector trackMomentum = track->momentum();
    Gaudi::XYZPoint trackPosition = track->position();
    //exception handling?
    Gaudi::XYZVector partMomentum(part->momentum());
    Gaudi::XYZPoint partPosition = part->endVertex()->position();

    // --  takes seven variables
    //
    angle         = enclosedAngle(trackMomentum, partMomentum);
    Gaudi::XYZPoint perpFoot_track, perpFoot_daughter;

    Gaudi::XYZPoint vertex_mu_track;
    bool failed = false;
    // Get Mu + Track "vertex"
    getPerpFeet(  trackPosition, trackMomentum, partPosition, partMomentum,
                  perpFoot_track, perpFoot_daughter, vertex_mu_track, failed);
    if(failed && msgLevel(MSG::DEBUG) ) debug() << "Vertex calculation ( perpendicular feet ) failed: denom == 0!" << endmsg;
    // FC
    fc = calcFC(  trackMomentum, partMomentum, vertex_mu_track, PV);
    if(fc==-1 && msgLevel(MSG::DEBUG) ) debug() << "FC calculation failed: fc_denom == 0!" << endmsg;
    // DOCA
    StatusCode sc_doca  = dva->distanceCalculator()->distance(part,vertex_mu_track,doca);
    if(!sc_doca)  return StatusCode(sc_doca);
    double pvDist,pvDistChi2 ;
    StatusCode sc_pv    = dva->distanceCalculator()->distance(PV, vertex_mu_track, pvDist, pvDistChi2);
    pvDistGeometric     = calcVertexDist(vertex_mu_track, PV);
    //if(!sc_pv)  return StatusCode(sc_pv);
    double svDist, svDistChi2 ;
    StatusCode sc_sv    = dva->distanceCalculator()->distance(SV, vertex_mu_track, svDist, svDistChi2);
    svDistGeometric     = calcVertexDist(vertex_mu_track, SV);
    //   if(!sc_sv)  return StatusCode(sc_sv);


    ipchisqany = calcIPToAnyPV(track) ;
    if (angle <m_angle && fc<m_fc && (2*doca)<m_doca_iso && ipchisqany>m_ips &&
        svDistGeometric>m_svdis && svDistGeometric<m_svdis_h && pvDistGeometric>m_pvdis && pvDistGeometric<m_pvdis_h
        && track->type()==m_tracktype) {
      isolation2+=1;
      if (Indecay) isolation1+=1;
    }
    
  }
  m_bdt1 = isolation1 ;
  m_bdt2 = isolation2 ;
  
  return true ;
}

///============================================================================
/// helper functions
///============================================================================


/// ------------------
/// calc ips to any PV
/// ------------------

double RelInfoBstautauTauIsolation::calcIPToAnyPV( const LHCb::Track * track )
{
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  LHCb::RecVertex::Container::const_iterator iv;
  double ips(-1),imp(-1),impchi2(-1);
  ips = 6.0e5;
  for(iv = m_vertices->begin();iv!=m_vertices->end();iv++){
    StatusCode sc_ips = dva->distanceCalculator()->distance(track,(*iv),imp,impchi2);
    if(!sc_ips) return StatusCode(sc_ips);
    if(ips>impchi2) ips = impchi2;
  }
  return sqrt(ips);
}



///============================================================================
/// Calculates the perpendicular feet on the track's and muon's trajectory
///
/// The formulae are derived from:
/// Let p1 and p2 be the points where the intersect (p1-p2) is equal to the
/// doca-vector of the track and the muon satisfying:
///    ( p1 - p2 ) * track_mom == 0
/// && ( p1 - p2 ) * mu_mom == 0
/// solve equations for scalars mu1 and mu2 which are the translations
/// along the unit momentum vectors
/// => p1 = track_pos + mu1 * Unit(track_mom) and
///    p2 = mu_pos + mu2 * Unit(mu_mom)
///
/// Additionally, a "vertex" is calculated as the mid-point
/// on the doca line segment
///============================================================================
void RelInfoBstautauTauIsolation::getPerpFeet(Gaudi::XYZPoint track_pos,
                                              Gaudi::XYZVector track_p,
                                              Gaudi::XYZPoint mu_pos,
                                              Gaudi::XYZVector mu_p,
                                              Gaudi::XYZPoint& perpFootTrack,
                                              Gaudi::XYZPoint& perpFootMu,
                                              Gaudi::XYZPoint& vertex, bool& fail)
{
  // def difference of positions
  Gaudi::XYZVector  diffPos(track_pos - mu_pos);
  // Get unit vectors
  Gaudi::XYZVector  unitTrack_p(track_p.unit());
  Gaudi::XYZVector  unitMu_p(mu_p.unit());
  Gaudi::XYZPoint   temp1(0.,0.,0.);
  Gaudi::XYZPoint   temp2(0.,0.,0.);
  fail = false;
  // def used scalar-products
  double  d_DiffTr  = diffPos.Dot( unitTrack_p);
  double  d_DiffMu  = diffPos.Dot( unitMu_p);
  double  d_MuTrack = unitMu_p.Dot( unitTrack_p);
  double  d_TrTr    = unitTrack_p.Dot( unitTrack_p);
  double  d_MuMu    = unitMu_p.Dot( unitMu_p);
  double  denom     = d_MuTrack * d_MuTrack - d_MuMu * d_TrTr;
  if (fabs(denom)<1E-27) {
    perpFootTrack = temp1;
    perpFootMu    = temp2;
    fail = true;
  }
  else {
    double numer  = d_DiffTr * d_MuTrack - d_TrTr * d_DiffMu;
    double mu2    = numer / denom;
    double mu1    = ( mu2 * d_MuTrack - d_DiffTr ) / d_TrTr ;
    perpFootTrack = track_pos + unitTrack_p *mu1;
    perpFootMu    = mu_pos    + unitMu_p    *mu2;
  }
  if ( fail && msgLevel(MSG::DEBUG) ) debug() << d_DiffTr << '\t' << d_DiffMu << '\t' << d_MuTrack << '\t' << d_TrTr << '\t' << denom << endmsg ;
  //
  vertex = ( perpFootTrack + ( perpFootMu - perpFootTrack ) * 0.5 );
}
///============================================================================
/// Calculates fc value which is a combination of momenta, transverse
/// momenta and angles
///
/// For more information see Bs2mumu roadmap or
/// G. Mancinelli & J. Serrano LHCb-INT-2010-011
/// XYZVector::R() equals length of the vector,
/// XYZVector::Rho() equals length projection on X-Y plane -> like
/// transverse momentum
///============================================================================

double RelInfoBstautauTauIsolation::calcFC( Gaudi::XYZVector p,
                                            Gaudi::XYZVector p_mu,
                                            Gaudi::XYZPoint vtx,
                                            const LHCb::VertexBase* PV)
{

  Gaudi::XYZVector rv;
  Gaudi::XYZPoint pv = PV->position();
  double fc = -1.;

  rv = vtx - pv;
  double pt=p.Rho()+p_mu.Rho();
  Gaudi::XYZVector ptot(p+p_mu);
  double temp = arcosine(rv,ptot);
  double  num=ptot.R()*sin(temp);
  double  den=(num+pt);
  return fc = num/den;
}

double RelInfoBstautauTauIsolation::arcosine(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {

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


///--------------------------------------------
/// calculates angle between two vectors
///--------------------------------------------
double RelInfoBstautauTauIsolation::enclosedAngle(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {
  double den      = p1.R()*p2.R();
  double cosAngle = p1.Dot(p2)/den;
  double angle    = acos(fabs(cosAngle));
  if (cosAngle < 0 ) {
    angle = ROOT::Math::Pi() - angle;
  }
  return angle;
}

double RelInfoBstautauTauIsolation::calcVertexDist(Gaudi::XYZPoint muTrack, const LHCb::VertexBase* v){
  if (v==NULL) return 0 ;
  Gaudi::XYZPoint vertex = v->position();
  return ( (muTrack.z()-vertex.z())/fabs(muTrack.z()-vertex.z())*(muTrack-vertex).R() );
}

