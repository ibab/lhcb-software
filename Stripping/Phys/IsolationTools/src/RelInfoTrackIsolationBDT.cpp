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
        const IInterface* parent) : GaudiTool ( type, name , parent )
{
    declareInterface<IRelatedInfoTool>(this);

    declareProperty( "Variables", m_variables, 
            "List of variables to store (store all if empty)");
    declareProperty
        ( "MVATransform" , m_transformName ,
          "path/name of the DictTransform tool"); 
    declareProperty
        ( "WeightsFile" , m_weightsName = "TrackIsolation.xml" ,
          "weights parameter file"); 
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
            warning() << "Unknown variable " << *ivar << ", skipping" << endmsg; 
        }
    }
*/
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
StatusCode RelInfoTrackIsolationBDT::initialize() {
    StatusCode sc = GaudiTool::initialize() ;
    if ( sc.isFailure() ) return sc ;

    //get from DV algorithm
    m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc() ) ;
    if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", 
            StatusCode::FAILURE);
        m_dist       = tool<IDistanceCalculator>("LoKi::DistanceCalculator",this);
    if( !m_dist ){
        Error("Unable to retrieve the IDistanceCalculator tool");
        return StatusCode::FAILURE;
    }

    /// TMVA configuration
    /*
       m_Reader = new TMVA::Reader( "!Silent" );
       m_Reader->AddVariable("PVdist",&var_PVdist) ;
       m_Reader->AddVariable("SVdist",&var_SVdist) ;
       m_Reader->AddVariable("fc",&var_fc) ; 
       m_Reader->AddVariable("log_doca",&var_log_doca) ;
       m_Reader->AddVariable("angle",&var_angle) ;
       m_Reader->AddVariable("log_TrackIPchi2AnyPV",&var_ipchisqany) ; 
       m_Reader->BookMVA( "BDT method", m_weightsName );
       if( !m_Reader ){
       Error("Unable to retrieve the IVertexFit tool");
       return StatusCode::FAILURE;
       }*/

    m_optmap["Name"] = m_transformName ;
    m_optmap["KeepVars"] = "0" ;
    m_optmap["XMLFile"] = System::getEnv("TMVAWEIGHTSROOT") + "/data/" + m_weightsName ;
    m_tmva.Init( m_optmap , info().stream() ) ; //
    //instance of IParticleDicttool
    //m_varmap = tool<IParticleDictTool>(m_transformname) ;
    //if(m_varmap == NULL) {
    //    Error("Unable to configure IParticleDictTool") ;
    //    return StatusCode::FAILURE;
    //}
    //set up map of variables - doesn't work with GaudiVEctorMap!!!
    //m_varmap["PVdist"]=0.0 ;
    //m_varmap["SVdist"]=0.0 ;
    //m_varmap["angle"]=0.0 ;
    //m_varmap["doca"]=0.0 ;
    //m_varmap["fc"]=0.0 ;
    return StatusCode::SUCCESS;   

}

//=============================================================================
// Fill Extra Info structure
//=============================================================================
StatusCode RelInfoTrackIsolationBDT::calculateRelatedInfo( const LHCb::Particle *top,
        const LHCb::Particle *part )
{

    if ( msgLevel(MSG::DEBUG) ) debug() << "Calculating TrackIso extra info" << endmsg;
    m_bdt1 = -1 ;
    m_bdt2 = -1 ;
    m_bdt3 = -1 ;

    // -- The vector m_decayParticles contains all the particles that belong to the given decay
    // -- according to the decay descriptor.

    // -- Clear the vector with the particles in the specific decay
    m_decayParticles.clear();

    if ( ! part->isBasicParticle() ) { 
        if ( msgLevel(MSG::DEBUG) ) debug() << "Running track isolation on non-final state particle, skipping" << endmsg;
        return StatusCode::SUCCESS ;
    }

    // -- Add the mother (prefix of the decay chain) to the vector
    //if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
    //m_decayParticles.push_back( top );
    // -- Save all particles that belong to the given decay in the vector m_decayParticles
    //saveDecayParticles( top );

    // -- Get all tracks in the event
    LHCb::Tracks* tracks = get<LHCb::Tracks>(LHCb::TrackLocation::Default);
    if ( tracks->empty() )
    {
        if ( msgLevel(MSG::WARNING) ) warning() << "Could not retrieve tracks. Skipping" << endmsg;
        return StatusCode::FAILURE;
    }

    bool test = true;

    //set PV and SV of the mother
    //
    const LHCb::VertexBase* PV = m_dva->bestVertex(top); 
    const LHCb::VertexBase *SV = top->endVertex();

    if(exist<LHCb::RecVertex::Container>(m_PVInputLocation)){
        m_vertices = get<LHCb::RecVertex::Container>(m_PVInputLocation);
    }

    if( part )
    {

        if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling variables with particle " << part << endmsg;

        // -- process -- iterate over tracks
        //
        //assign top three bdt values
        //m_bdt1 = 1 ;
        //m_bdt2 = 2 ;
        //m_bdt3 = 3 ;
        calcBDTValue( part, tracks, PV, SV ) ;
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
        if ( msgLevel(MSG::WARNING) ) warning() << "The particle in question is not valid" << endmsg;
        return StatusCode::FAILURE;
    }

    return StatusCode(test);
}


//rel infor methods

LHCb::RelatedInfoMap* RelInfoTrackIsolationBDT::getInfo(void) {
    return &m_map; 
}

std::string RelInfoTrackIsolationBDT::infoPath(void) {
    std::stringstream ss;
    ss << std::string("Particle2TrackIsolationRelations");
    return ss.str(); 
}

//=============================================================================
// Save the particles in the decay chain (recursive function)
//=============================================================================
void RelInfoTrackIsolationBDT::saveDecayParticles( const LHCb::Particle *top)
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
bool RelInfoTrackIsolationBDT::isTrackInDecay(const LHCb::Track* track){

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
/// Track isolation method
///============================================================================



bool RelInfoTrackIsolationBDT::calcBDTValue( const LHCb::Particle * part
        , const LHCb::Tracks * tracks
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

    LHCb::Tracks::const_iterator tr_it, tr_it_end( tracks->end() ) ;
    for ( tr_it = tracks->begin() ; tr_it != tr_it_end ; ++tr_it )
    {
        LHCb::Track * track = * tr_it ;
        //pointer comparison
        if ( track == part->proto()->track() ) continue ;

        Gaudi::XYZVector trackMomentum = track->momentum();
        Gaudi::XYZPoint trackPosition = track->position();
        //exception handling?
        Gaudi::XYZVector partMomentum = part->proto()->track()->momentum();
        Gaudi::XYZPoint partPosition = part->proto()->track()->position();

        // -- BDT takes five variables 
        //
        var_angle         = enclosedAngle(trackMomentum, partMomentum);
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
        StatusCode sc_doca  = m_dist->distance(track,part->proto()->track(),doca);
        if(!sc_doca)  return StatusCode(sc_doca);
        double pvDist,pvDistChi2 ;
        StatusCode sc_pv    = m_dist->distance(PV, vertex_mu_track, pvDist, pvDistChi2);
        pvDistGeometric     = calcVertexDist(vertex_mu_track, PV);
        //if(!sc_pv)  return StatusCode(sc_pv);
        double svDist, svDistChi2 ;
        StatusCode sc_sv    = m_dist->distance(SV, vertex_mu_track, svDist, svDistChi2);
        svDistGeometric     = calcVertexDist(vertex_mu_track, SV);
        //   if(!sc_sv)  return StatusCode(sc_sv);
        var_ipchisqany = log(calcIPToAnyPV(track) );
        var_fc = fc ;
        var_angle = angle ;
        var_log_doca = log(doca) ;
        var_PVdist = pvDistGeometric ;
        var_SVdist = svDistGeometric ;
        //
        m_varmap.clear()    ;
        //make this more generic??
        m_varmap.insert( "PVdist", var_PVdist ) ;
        m_varmap.insert( "SVdist", var_SVdist ) ;
        m_varmap.insert( "angle", var_angle ) ;
        m_varmap.insert( "doca", var_log_doca ) ;
        m_varmap.insert( "fc", var_fc ) ;

        if (msgLevel(MSG::VERBOSE)) {
            verbose() << "PVdist"<<'\t' << var_PVdist <<  endmsg;
            verbose() << "SVdist"<<'\t' << var_SVdist <<  endmsg;
            verbose() << "anglet"<<'\t' << var_angle <<  endmsg;
            verbose() << "doca"<<'\t' << var_log_doca <<  endmsg;
            verbose() << "fc value"<<'\t' << var_fc <<  endmsg;
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
    LHCb::RecVertex::Container::const_iterator iv;
    double ips(-1),imp(-1),impchi2(-1);
    ips = 6.0e5;
    for(iv = m_vertices->begin();iv!=m_vertices->end();iv++){
        StatusCode sc_ips = m_dist->distance(track,(*iv),imp,impchi2);
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
    if ( fail && msgLevel(MSG::DEBUG) ) info() << d_DiffTr << '\t' << d_DiffMu << '\t' << d_MuTrack << '\t' << d_TrTr << '\t' << denom << endmsg ;
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
};

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
};

