#include "GaudiKernel/ToolFactory.h"
#include "RelInfoTrackIsolationBDT.h"
#include "Kernel/RelatedInfoNamed.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoTrackIsolationBDT
// Converted from ConeVariables by A. Shires
//
// 2014-06-18 : Alex Shires
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoTrackIsolationBDT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoTrackIsolationBDT::RelInfoTrackIsolationBDT( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent) 
: GaudiTool ( type, name , parent )
{
  declareInterface<IRelatedInfoTool>(this);

  declareProperty( "Variables", m_variables = -1,
                   "Set of variables to store (0,1,2,3,4)");
  declareProperty
    ( "MVATransform" , m_transformName ,
      "path/name of the DictTransform tool");

  declareProperty
    ( "WeightsFile" , m_weightsName = "BsMuMu_TrackIsolationBDT6varsA_v1r4.xml" ,
      "weights parameter file");

  declareProperty( "PVInputLocation"
                   , m_PVInputLocation = LHCb::RecVertexLocation::Primary
                   , " PV input location"
                   );


  declareProperty("ParticlePath", m_ParticlePath="/Event/Phys/StdAllNoPIDsPions/Particles");

  declareProperty( "TrackType", m_trackType = 3,
                   "Set the type of tracks considered for the isoBDT (default = 3)");


  //sort these keys out by adding all
  m_keys.clear();
  m_keys.push_back( RelatedInfoNamed::TRKISOBDTFIRSTVALUE );
  m_keys.push_back( RelatedInfoNamed::TRKISOBDTSECONDVALUE );
  m_keys.push_back( RelatedInfoNamed::TRKISOBDTTHIRDVALUE );

}

//=============================================================================
// Destructor
//=============================================================================
RelInfoTrackIsolationBDT::~RelInfoTrackIsolationBDT() {}


//=============================================================================
// Initialize
//=============================================================================
StatusCode RelInfoTrackIsolationBDT::initialize()
{
  StatusCode sc = GaudiTool::initialize() ;
  if ( sc.isFailure() ) return sc ;

  m_optmap["Name"] = m_transformName ;
  m_optmap["KeepVars"] = "0" ;
  m_optmap["XMLFile"] = System::getEnv("TMVAWEIGHTSROOT") + "/data/" + m_weightsName ;
  m_tmva.Init( m_optmap , debug().stream(), msgLevel(MSG::DEBUG) ) ; //

  return sc;
}

//=============================================================================
// Fill Extra Info structure
//=============================================================================
StatusCode
RelInfoTrackIsolationBDT::calculateRelatedInfo( const LHCb::Particle *top,
                                                const LHCb::Particle *part )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "Calculating TrackIso extra info" << endmsg;
  m_bdt1 = -1 ;
  m_bdt2 = -1 ;
  m_bdt3 = -1 ;

  // -- The vector m_decayParticles contains all the particles that belong to the given decay
  // -- according to the decay descriptor.


  if ( ! part->isBasicParticle() ) {
    if ( msgLevel(MSG::DEBUG) ) debug() << "Running track isolation on non-final state particle, skipping" << endmsg;
    return StatusCode::SUCCESS ;
  }

  // -- Get all tracks in the event
  LHCb::Particle::Range parts = get<LHCb::Particle::Range>(m_ParticlePath);
  if (!parts) {
    Warning( "Could not retrieve particles. Skipping" );
    return StatusCode::SUCCESS;
  }

  bool test = true;

  //set PV and SV of the mother
  //
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  const LHCb::VertexBase* PV = dva->bestVertex(top);
  const LHCb::VertexBase *SV = top->endVertex();

  m_vertices = get<LHCb::RecVertex::Range>(m_PVInputLocation);

  if( part )
  {

    if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling variables with particle " << part << endmsg;

    // -- process -- iterate over tracks
    //
    //assign top three bdt values
    calcBDTValue( part, parts, PV, SV ) ;
    if ( msgLevel(MSG::DEBUG) ) debug() << m_bdt1 << '\t'  << m_bdt2 << '\t' << m_bdt3 << endmsg ;
    //
    //store
    m_map.clear();

    std::vector<short int>::const_iterator ikey;
    for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {

      float value = 0;
      switch (*ikey) {
      case RelatedInfoNamed::TRKISOBDTFIRSTVALUE : value = m_bdt1; break;
      case RelatedInfoNamed::TRKISOBDTSECONDVALUE  : value = m_bdt2; break;
      case RelatedInfoNamed::TRKISOBDTTHIRDVALUE   : value = m_bdt3; break;
      }
      if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;

      m_map.insert( std::make_pair( *ikey, value) );
    }

  }

  else
  {
    if ( msgLevel(MSG::WARNING) ) Warning( "The particle in question is not valid" ) ;
    return StatusCode::FAILURE;
  }

  return StatusCode(test);
}


//rel infor methods

LHCb::RelatedInfoMap* RelInfoTrackIsolationBDT::getInfo(void) {
  return &m_map;
}







///============================================================================
/// Track isolation method
///============================================================================



bool RelInfoTrackIsolationBDT::calcBDTValue( const LHCb::Particle * part
                                             , const LHCb::Particle::Range particles
                                             , const LHCb::VertexBase * PV
                                             , const LHCb::VertexBase * SV
                                             )
{
  double bdtval = 0 ;
  double doca   = 0;
  double angle  = 0;
  double fc     = 0;
  double pvDistGeometric(0) ;
  double svDistGeometric(0) ;

  //MR: additional variables for new isoBDTs
  double log_trk_ips = 0;//log(IP significance of the track)
  double trk_gho = 0;//track ghost probability
  double trk_chi = 0;//track chi2
  double trk_dphi = 0;//abs(trk_phi - muon_phi)
  double trk_deta = 0;//abs(trk pseudorapidity - muon pseudorapidity)
  double trk_pt = 0;//track pT
  double trk_ch = 0;//charge(track)*charge(muon)
  double trk_nnmu = 0;//track ProbNN(mu)
  double trk_nnpi = 0;//track ProbNN(pi)
  double trk_nnk = 0;//track ProbNN(K)
  //end MR

  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }

  LHCb::Particle::Range::const_iterator part_it, part_it_end( particles.end() ) ;
  for ( part_it = particles.begin() ; part_it != part_it_end ; ++part_it )
  {
    const LHCb::Track * track = (*part_it)->proto()->track() ;
    //pointer comparison
    if ( track == part->proto()->track() ) continue ;
    //track type comparison
    if(track->type()!=m_trackType) continue;

    const Gaudi::XYZVector& trackMomentum = track->momentum();
    const Gaudi::XYZPoint& trackPosition = track->position();
    //exception handling?
    const Gaudi::XYZVector& partMomentum = part->proto()->track()->momentum();
    const Gaudi::XYZPoint& partPosition = part->proto()->track()->position();

    //MR: compute the additional variables
    log_trk_ips = std::log(calcIPToAnyPV(track) );//Note:I've commented out var_ipchisqany below
    trk_gho = track->ghostProbability();
    trk_chi = track->chi2PerDoF();
    trk_dphi = fabs(track->momentum().Phi() - part->proto()->track()->momentum().Phi());
    trk_deta = fabs(track->momentum().Eta() - part->proto()->track()->momentum().Eta());
    trk_pt = track->pt();
    trk_ch = (track->charge())*(part->proto()->track()->charge());
    //particle for propbnn variables
    trk_nnmu = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNmu,-9999);
    if (trk_nnmu<=0) trk_nnmu=-0.1;
    trk_nnpi = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNpi,-9999);
    trk_nnk = (*part_it)->proto()->info(LHCb::ProtoParticle::ProbNNk,-9999);
    //end MR

    // -- BDT takes five variables
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
    StatusCode sc_doca  = dva->distanceCalculator()->distance(track,part->proto()->track(),doca);
    if(!sc_doca)  return StatusCode(sc_doca);
    double pvDist,pvDistChi2 ;
    StatusCode sc_pv    = dva->distanceCalculator()->distance(PV, vertex_mu_track, pvDist, pvDistChi2);
    pvDistGeometric     = calcVertexDist(vertex_mu_track, PV);
    //if(!sc_pv)  return StatusCode(sc_pv);
    double svDist, svDistChi2 ;
    StatusCode sc_sv    = dva->distanceCalculator()->distance(SV, vertex_mu_track, svDist, svDistChi2);
    svDistGeometric     = calcVertexDist(vertex_mu_track, SV);
    //   if(!sc_sv)  return StatusCode(sc_sv);
    //var_ipchisqany = std::log(calcIPToAnyPV(track) );
    var_fc = fc ;
    var_angle = angle ;
    var_log_doca = std::log(doca) ;
    var_PVdist = pvDistGeometric ;
    var_SVdist = svDistGeometric ;
    //
    m_varmap.clear()    ;
    if (m_variables>=0) //Laura's variables
    {
      m_varmap.insert( "PVdist", var_PVdist ) ;
      m_varmap.insert( "SVdist", var_SVdist ) ;
      m_varmap.insert( "angle", var_angle ) ;
      m_varmap.insert( "log_doca", var_log_doca ) ;
      m_varmap.insert( "fc", var_fc ) ;
      m_varmap.insert( "log_trk_ips", log_trk_ips ) ; // (a)
    }
    if (m_variables>=2)
    {
      m_varmap.insert( "trk_dphi", trk_dphi ) ; // (d)
      m_varmap.insert( "trk_deta", trk_deta ) ; // (e)
      m_varmap.insert( "trk_pt", trk_pt ) ;     // (f)
    }
    if (m_variables>=3)
    {
      m_varmap.insert( "trk_ch", trk_ch ) ;     // (g)
      m_varmap.insert( "trk_nnmu", trk_nnmu ) ; // (h)
      m_varmap.insert( "trk_nnpi", trk_nnpi ) ; // (h)
      m_varmap.insert( "trk_nnk", trk_nnk ) ;   // (h)
    }
    if (m_variables>=4)
    {
      m_varmap.insert( "trk_gho", trk_gho ) ;   // (b)
      m_varmap.insert( "trk_chi", trk_chi ) ;   // (c)
    }

    if (msgLevel(MSG::VERBOSE)) {
      verbose() << "PVdist"<<'\t' << var_PVdist <<  endmsg;
      verbose() << "SVdist"<<'\t' << var_SVdist <<  endmsg;
      verbose() << "angle"<<'\t' << var_angle <<  endmsg;
      verbose() << "log_doca"<<'\t' << var_log_doca <<  endmsg;
      verbose() << "fc value"<<'\t' << var_fc <<  endmsg;
      if (m_variables>=1) {
        verbose() << "log_trk_ips"<<'\t' << log_trk_ips <<  endmsg;
      }
      if (m_variables>=2) {
        verbose() << "trk_dphi"<<'\t' << trk_dphi <<  endmsg;
        verbose() << "trk_deta"<<'\t' << trk_deta <<  endmsg;
        verbose() << "trk_pt"<<'\t' << trk_pt <<  endmsg;
      }
      if (m_variables>=3) {
        verbose() << "trk_ch"<<'\t' << trk_ch <<  endmsg;
        verbose() << "trk_nnmu"<<'\t' << trk_nnmu <<  endmsg;
        verbose() << "trk_nnpi"<<'\t' << trk_nnpi <<  endmsg;
        verbose() << "trk_nnk"<<'\t' << trk_nnk <<  endmsg;
      }
      if (m_variables>=4) {
        verbose() << "trk_gho"<<'\t' << trk_gho <<  endmsg;
        verbose() << "trk_chi"<<'\t' << trk_chi <<  endmsg;
      }
    }

    m_tmva(m_varmap,m_out) ;
    bdtval = m_out[m_transformName];

    if (msgLevel(MSG::DEBUG)) debug() << m_transformName << " : " << bdtval << endmsg ;
    //is this really the most efficient??
    if (bdtval > m_bdt1) {
      m_bdt3 = m_bdt2 ;
      m_bdt2 = m_bdt1 ;
      m_bdt1 = bdtval ;
    }
    else if (bdtval > m_bdt2) {
      m_bdt3 = m_bdt2 ;
      m_bdt2 = bdtval ;
    }
    else if (bdtval > m_bdt3) {
      m_bdt3 = bdtval ;
    }
  }
  return true ;
}

///============================================================================
/// helper functions
///============================================================================


/// ------------------
/// calc ips to any PV
/// ------------------

double RelInfoTrackIsolationBDT::calcIPToAnyPV( const LHCb::Track * track )
{
  LHCb::RecVertex::Range::const_iterator iv;
  double ips(-1),imp(-1),impchi2(-1);
  ips = 6.0e5;
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  for(iv = m_vertices.begin();iv!=m_vertices.end();iv++){
    StatusCode sc_ips = dva->distanceCalculator()->distance(track,(*iv),imp,impchi2);
    if(!sc_ips) return StatusCode(sc_ips);
    if(ips>impchi2) ips = impchi2;
  }
  return std::sqrt(ips);
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
void RelInfoTrackIsolationBDT::getPerpFeet(Gaudi::XYZPoint track_pos,
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

double RelInfoTrackIsolationBDT::calcFC( Gaudi::XYZVector track_mom,
                                         Gaudi::XYZVector mu_mom,
                                         Gaudi::XYZPoint mu_track_vertex,
                                         const LHCb::VertexBase* PV)
{
  double fc   = -1;
  Gaudi::XYZPoint  pv              = PV->position();
  Gaudi::XYZVector track_plus_mu_p( track_mom + mu_mom );
  Gaudi::XYZVector pv_to_tmuVertex( mu_track_vertex - pv);
  double angle      = enclosedAngle(track_plus_mu_p, pv_to_tmuVertex);
  double fc_num     = track_plus_mu_p.R() * angle;
  double fc_denom   = track_plus_mu_p.R() * angle + track_mom.Rho() + mu_mom.Rho() ;
  if(fc_denom != 0 ) fc = fc_num / fc_denom ;
  return fc;
}

///--------------------------------------------
/// calculates angle between two vectors
///--------------------------------------------
double RelInfoTrackIsolationBDT::enclosedAngle(Gaudi::XYZVector p1,Gaudi::XYZVector p2) {
  double den      = p1.R()*p2.R();
  double cosAngle = p1.Dot(p2)/den;
  double angle    = acos(fabs(cosAngle));
  if (cosAngle < 0 ) {
    angle = ROOT::Math::Pi() - angle;
  }
  return angle;
}

double RelInfoTrackIsolationBDT::calcVertexDist(Gaudi::XYZPoint muTrack, const LHCb::VertexBase* v){
  if (v==NULL) return 0 ;
  Gaudi::XYZPoint vertex = v->position();
  return ( (muTrack.z()-vertex.z())/fabs(muTrack.z()-vertex.z())*(muTrack-vertex).R() );
}

