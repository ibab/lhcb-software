#include "GaudiKernel/ToolFactory.h"
#include "RelInfoVertexIsolationBDT.h"
#include "Kernel/RelatedInfoNamed.h"
//-----------------------------------------------------------------------------
// Implementation file for class : RelInfoVertexIsolationBDT
// Converted from RelInfoVertexIsolationBDT (from Greg Ciezerak and Albert Puig)
// by Alex Shires
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_TOOL_FACTORY( RelInfoVertexIsolationBDT )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RelInfoVertexIsolationBDT::RelInfoVertexIsolationBDT( const std::string& type,
                                                      const std::string& name,
                                                      const IInterface* parent)
: GaudiTool ( type, name , parent )
  , m_pVertexFit(0)
  , m_pPVReFitter(0)
{
  declareInterface<IRelatedInfoTool>(this);
  declareProperty("InputParticles", m_inputParticles,
                  "List of containers to check for extra particle vertexing") ;
  declareProperty("WeightsFile", m_weightsName = "IsolationTools_VertexIsolationHard_v1r4.xml" );
  declareProperty("Type", m_bdttype = "Hard", "Use BDT trained on Hard or Soft tracks" );
  declareProperty("Name", m_transformName = "BDTvalue" );
  //
  // optional variables to store
  m_keys.clear();
  /*std::vector<std::string>::const_iterator ivar;
    for (ivar = m_variables.begin(); ivar != m_variables.end(); ivar++) {
    short int key = RelatedInfoNamed::indexByName( *ivar );
    if (key != RelatedInfoNamed::Unknown) {
    m_keys.push_back( key );
    } else {
    Warning( std::string("Unknown variable ") + *ivar + ", skipping" ) ;
    }
    }*/

}

//=============================================================================
// Destructor
//=============================================================================
RelInfoVertexIsolationBDT::~RelInfoVertexIsolationBDT() {}

//=============================================================================
// Initialize
//=============================================================================
StatusCode RelInfoVertexIsolationBDT::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // Get vertex fitter
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ; 
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  m_pVertexFit= dva->vertexFitter() ;
  if ( !m_pVertexFit ) { return Error("Unable to retrieve the IVertexFit tool"); }

  //Get PV refitter? Is this the correct way?
  m_pPVReFitter = tool<IPVReFitter>("LoKi::PVReFitter", this );
  if ( !m_pVertexFit ) { return Error("Unable to retrieve the IPVReFitter tool"); }

  // If no input particle container is specified, put StdNoPIDsPions by default
  if ( m_inputParticles.empty() )
  {
    m_inputParticles.push_back("/Event/Phys/StdNoPIDsPions") ;
    //m_inputParticles.push_back("/Event/Phys/StdNoPIDsUpPions");
    //m_inputParticles.push_back("Phys/StdNoPIDsVeloPions");
  }

  //configure keys
  if ( m_bdttype.compare("Hard") == 0 ) {
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTHARDFIRSTVALUE );
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTHARDSECONDVALUE );
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTHARDTHIRDVALUE );
  }
  else {
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTSOFTFIRSTVALUE );
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTSOFTSECONDVALUE );
    m_keys.push_back( RelatedInfoNamed::VTXISOBDTSOFTTHIRDVALUE );
    //if using default BDT name and want soft BDT:
    if ( m_weightsName.compare("IsolationTools_VertexIsolationHard_v1r4.xml") == 0 ) {
      m_weightsName = "IsolationTools_VertexIsolationSoft_v1r4.xml";
    }
  }


  //configure MVA
  m_optmap["Name"] = m_transformName ;
  m_optmap["KeepVars"] = "0" ;
  m_optmap["XMLFile"] = System::getEnv("TMVAWEIGHTSROOT") + "/data/" + m_weightsName ;
  m_tmva.Init( m_optmap, debug().stream(), msgLevel(MSG::DEBUG) ) ;


  /*m_Reader->AddSpectator( "Track_TYPE",&m_var_type);
    m_Reader->AddVariable( "Track_MINIPCHI2",&m_var_minipchi2);
    m_Reader->AddVariable( "Track_PT",&m_var_trackpt);
    m_Reader->AddVariable( "Track_OPENING",&m_var_opening);
    m_Reader->AddVariable( "Track_IPCHI2",&m_var_trackipchi2);
    m_Reader->AddVariable( "Track_FLIGHT",&m_var_newfdchi2);
    m_Reader->AddVariable( "Track_DELTAFLIGHT",&m_var_deltafd);
  */
  //m_Reader->BookMVA( "BDT method", m_weightsName );
  //if( !m_Reader ){
  //    Error("Unable to configure the BDT");
  //    return StatusCode::FAILURE;
  //}

  return sc;
}

//=============================================================================
// Fill VertexIsolation information
//=============================================================================
StatusCode RelInfoVertexIsolationBDT::calculateRelatedInfo( const LHCb::Particle *top,
                                                            const LHCb::Particle *part )
{

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Fill" << endmsg;

  // Check the particle
  if ( top->isBasicParticle() || isPureNeutralCalo(top) )
  {
    return Error("Cannot calculate isolation for basic or calorimetric particles!") ;
  }
  if( !part )
  {
    return Warning( "Found an invalid particle" );
  }
  // Check the vertex of the particle
  const LHCb::Vertex* vtx = top->endVertex() ;
  if ( !vtx )
  {
    return Error("Can't retrieve the vertex!") ;
  }
  //const double originalVtxChi2 = vtx->chi2() ;

  // Put all basic particles we are considering in m_particlesToVertex
  // -- Clear the vector with the particles in the specific decay
  m_particlesToVertex.clear();
  if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << top->particleID().pid() << endmsg;
  // -- Add the mother (prefix of the decay chain) to the vector
  //  m_particlesToVertex.push_back( top );
  // -- Save all basic particles that belong to the mother vertex in the vector m_particlesToVertex
  findDaughters2Vertex( top );

  // -- Get vector of particles excluding the signal
  LHCb::Particle::ConstVector partsToCheck ;
  for ( auto iLocation = m_inputParticles.begin(), iLocationEnd = m_inputParticles.end();
        iLocation != iLocationEnd;
        ++iLocation )
  {
    // Get the particles
    LHCb::Particle::Range particles = getIfExists<LHCb::Particle::Range>(*iLocation+"/Particles") ;
    if (msgLevel(MSG::DEBUG))
      debug() << "Got " << particles.size() << " particles from " << *iLocation << endreq ;
    if ( particles.empty() ) { continue; }

    // Loop over the particles and take the ones we can use for vertexing
    for ( auto iParticle = particles.begin(), iParticleEnd = particles.end() ;
          iParticle != iParticleEnd;
          ++iParticle )
    {
      // Ignore if no proto
      if ( !(*iParticle)->proto() )        continue ;
      // Also ignore if neutral
      if ( isPureNeutralCalo(*iParticle) ) continue ;
      // Check that the proto of the particle does not match the signal
      bool isSignal = false ;
      for ( auto iSignal = m_particlesToVertex.begin(), iSignalEnd = m_particlesToVertex.end();
            iSignal != iSignalEnd;
            ++iSignal )
      {
        if ( (*iSignal)->proto() == (*iParticle)->proto() ) { isSignal = true ; break ; }
      }
      if ( isSignal ) continue ;
      // Check for duplicated particles
      bool isAlreadyIn = false ;
      for ( auto iSavedParticle = partsToCheck.begin(), iSavedParticleEnd = partsToCheck.end();
            iSavedParticle != iSavedParticleEnd;
            ++iSavedParticle )
      {
        if ( (*iSavedParticle)->proto() == (*iParticle)->proto() ) { isAlreadyIn = true ; break ; }
      }
      if ( isAlreadyIn ) continue ;
      partsToCheck.push_back( *iParticle ) ;
    }
  }
  if ( msgLevel(MSG::DEBUG) )
    debug() << "Number of particles to check excluding signal, particles with same proto and gammas = "
            << partsToCheck.size() << endreq;

  //save PVs for minipchi2 calc
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ; 
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  const LHCb::RecVertex::Range PVs = dva->primaryVertices();
  if ( msgLevel(MSG::VERBOSE) )
    verbose() << "Primary vertices with size : " << PVs.size() << " and empty? " << PVs.empty() << endmsg ;

  // Now let's do some vertexing
  if ( msgLevel(MSG::VERBOSE) ) verbose() << "Filling isolation variables" << endmsg;

  m_bdt1 = -1;
  m_bdt2 = -1;
  m_bdt3 = -1;
  //BDT isolation code
  getIsolation(part,partsToCheck, vtx, PVs ) ;
  if (msgLevel(MSG::DEBUG)) debug() << "BDTvals : " <<  m_bdt1 << '\t' << m_bdt2 << '\t' << m_bdt3 << endmsg ;
  // Save values
  //
  m_map.clear();

  std::vector<short int>::const_iterator ikey;
  for (ikey = m_keys.begin(); ikey != m_keys.end(); ikey++) {

    float value = 0;
    switch (*ikey) {
    case RelatedInfoNamed::VTXISOBDTHARDFIRSTVALUE : value = m_bdt1; break;
    case RelatedInfoNamed::VTXISOBDTHARDSECONDVALUE  : value = m_bdt2; break;
    case RelatedInfoNamed::VTXISOBDTHARDTHIRDVALUE   : value = m_bdt3; break;
    case RelatedInfoNamed::VTXISOBDTSOFTFIRSTVALUE : value = m_bdt1; break;
    case RelatedInfoNamed::VTXISOBDTSOFTSECONDVALUE  : value = m_bdt2; break;
    case RelatedInfoNamed::VTXISOBDTSOFTTHIRDVALUE   : value = m_bdt3; break;

    }

    if (msgLevel(MSG::DEBUG)) debug() << "  Inserting key = " << *ikey << ", value = " << value << " into map" << endreq;
    m_map.insert( std::make_pair( *ikey, value) );
  }

  // We're done!
  return StatusCode::SUCCESS ;
}


//rel infor methods

LHCb::RelatedInfoMap* RelInfoVertexIsolationBDT::getInfo(void) {
  return &m_map;
}


//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
bool RelInfoVertexIsolationBDT::getIsolation( const LHCb::Particle *part
                                              , LHCb::Particle::ConstVector &extraParticles
                                              , const LHCb::Vertex* v
                                              , const LHCb::RecVertex::Range PVs
                                              )
{
  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); } 
  //LHCb::Particle *bestParticle         = NULL ;
  //variables
  double newfdchi2(0), oldfdchi2(0), minipchi2(0), opening(0), ghostprob(0);
  double deltafd(0), pt(0) , ip(0), ipchi2(0), bdtval(0) ;
  int type ;
  for ( auto iExtraPart = extraParticles.begin(), iExtraPartEnd = extraParticles.end() ;
        iExtraPart != iExtraPartEnd ;
        ++iExtraPart )
  {
    const LHCb::Particle* newpart = (*iExtraPart);
    LHCb::Vertex vtxWithExtraTrack ;
    // Temporarily add the extra track to the partcles to vertex vector
    m_particlesToVertex.push_back(newpart) ;
    // Fit
    const StatusCode sc = m_pVertexFit->fit(vtxWithExtraTrack, m_particlesToVertex) ;
    // Remove the extra track
    m_particlesToVertex.pop_back() ;
    if ( !sc )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Failed to fit vertex" << endmsg ;
    }
    else
    {
      //second check:.... sufficiently small
      if ( vtxWithExtraTrack.chi2() < 1E-5 ) continue;

      ghostprob = newpart->proto()->track()->ghostProbability();
      //ghost prob restrictuoin
      if(ghostprob > 0.5){
        if (msgLevel(MSG::VERBOSE))
          verbose() << "skipping track with type " << ghostprob << endmsg ;
        continue;
      }
      //BDT variables
      // 1 : Calculate opening angle
      opening = getopening(newpart->proto()->track(),part);
      // 6 : Track type
      type = newpart->proto()->track()->type();

      // min requirements on pointing
      if( ( type == 3 && (opening < 0.994 )) ///long
          || (type == 4 && (opening < 0.98)) //
          || (type == 1 && (opening < 0.98)) )
      {
        if (msgLevel(MSG::VERBOSE))
          verbose() << "skipping track with type " << type << " and angle " << opening  << endmsg ;
        continue;
      }
      // 2 : Calculate MINIPChi2
      minipchi2 = getminipchi(newpart, PVs);
      // 3 : Calculate new FDChi2
      newfdchi2 = getfdchi2(newpart->proto()->track(),vtxWithExtraTrack, PVs);
      // 4 : Calculate old FDChi2
      oldfdchi2 = getfdchi2(newpart->proto()->track(),*v, PVs);
      //oldfdchi2 = getfdchi2(newpart->proto()->track(),vtxWithExtraTrack, PVs);
      // 5 : Delta FD Chi2
      debug() << "New FD: " << newfdchi2 << " Old FD: " << oldfdchi2 << endmsg ;
      deltafd = log10(fabs(newfdchi2-oldfdchi2)) -7  ;
      if (newfdchi2-oldfdchi2 < 0) deltafd *= -1.;
      //take log of fdchi2
      newfdchi2 = log10(newfdchi2);
      // 7 : Track PT
      if(newpart->proto()->track()->type() == 1) pt = newpart->proto()->track()->momentum().z();
      else pt = newpart->proto()->track()->pt();
      // 8 : Track IP chi2 ??
      StatusCode distsc = dva->distanceCalculator()->distance(newpart,v,ip,ipchi2);
      //debug() << ghostprob << '\t' << type << '\t' << opening << endmsg ;
      //assign
      m_var_type = type ;
      m_var_minipchi2 = minipchi2 ;
      m_var_trackpt = pt ;
      m_var_opening = opening ;
      m_var_newfdchi2 = newfdchi2 ;
      m_var_trackipchi2 = ipchi2 ;
      m_var_deltafd = deltafd ;
      //BDT value


      m_varmap.clear();
      m_varmap.insert( "Track_TYPE", m_var_type ) ;
      m_varmap.insert( "Track_MINIPCHI2", m_var_minipchi2 ) ;
      m_varmap.insert( "Track_PT", m_var_trackpt ) ;
      m_varmap.insert( "Track_OPENING", m_var_opening ) ;
      m_varmap.insert( "Track_IPCHI", m_var_trackipchi2 ) ;
      m_varmap.insert( "Track_FLIGHT", m_var_newfdchi2 ) ;
      m_varmap.insert( "Track_DELTALFLIGHT", m_var_deltafd ) ;


      if (msgLevel(MSG::VERBOSE)) {
        verbose() << "track type " <<'\t'<< type << endmsg ;
        verbose() << "track minipchi2" <<'\t'<< minipchi2 << endmsg ;
        verbose() << "track py" <<'\t'<< pt << endmsg ;
        verbose() << "track opening" <<'\t' << opening << endmsg ;
        verbose() << "track newfdchi2"<<'\t' << newfdchi2 << endmsg ;
        verbose() << "track ipchi2" <<'\t'  << ipchi2 << endmsg ;
        verbose() << "delta fd " << '\t' << deltafd << endmsg ;
      }
      //check map

      //
      m_tmva(m_varmap,m_out) ;
      bdtval = m_out[m_transformName];


      if (msgLevel(MSG::DEBUG)) debug() << m_transformName << " : " << bdtval << endmsg ;

      //pick best and swap
      if (bdtval > m_bdt1 ) {
        m_bdt3 = m_bdt2 ;
        m_bdt2 = m_bdt1 ;
        m_bdt1 = bdtval ;
      }
      else if ( bdtval > m_bdt2 ) {
        m_bdt3 = m_bdt2 ;
        m_bdt2 = bdtval ;
      }
      else if ( bdtval > m_bdt3 ) {
        m_bdt3 = bdtval ;
      }
    }
  }
  return true ;
}

//=============================================================================
// Get (recursively) the particles to vertex in the decay chain
//=============================================================================
void RelInfoVertexIsolationBDT::findDaughters2Vertex( const LHCb::Particle *top )
{
  // -- Get the daughters of the top particle
  const LHCb::Particle::ConstVector & daughters = top->daughtersVector();
  // -- Fill all the daugthers in m_particlesToVertex
  for( auto idau = daughters.begin(), idauEnd = daughters.end() ;
       idau != idauEnd ;
       ++idau )
  {
    // -- If the particle is stable, save it in the vector, or...
    if( (*idau)->isBasicParticle() )
    {
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      if ( (*idau)->proto() && !isPureNeutralCalo(*idau) ) m_particlesToVertex.push_back( (*idau) );
    }
    else
    { // -- if it is not stable, call the function recursively
      //   m_particlesToVertex.push_back( (*idau) );
      if ( msgLevel(MSG::DEBUG) ) debug() << "Filling particle with ID " << (*idau)->particleID().pid() << endmsg;
      findDaughters2Vertex( (*idau) );
    }
  }
}

//=============================================================================
// MINIPCHI2 for a track
//=============================================================================
double RelInfoVertexIsolationBDT::getminipchi(const LHCb::Particle* track, const LHCb::RecVertex::Range PVs) const {

  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ; 
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  double minchi2 = -1 ;
  if ( !PVs.empty() ){
    for ( LHCb::RecVertex::Range::const_iterator pv = PVs.begin() ; pv!=PVs.end() ; ++pv){
      double ip, chi2;
      StatusCode test2 = dva->distanceCalculator()->distance ( (const LHCb::Particle*)track, *pv, ip, chi2 );
      if ((chi2<minchi2) || (minchi2<0.))
      {
        LHCb::RecVertex newPV(**pv);
        StatusCode scfit = m_pPVReFitter->remove(track, &newPV);
        LHCb::RecVertex* newPVPtr = (LHCb::RecVertex*)&newPV;
        test2 = dva->distanceCalculator()->distance ((LHCb::Particle *)track, (LHCb::VertexBase*) newPVPtr, ip, chi2 );
        minchi2 = chi2 ;
      }
    }

  }
  else{ Warning( "IP Chi2 NOT FOUND PV" ) ; }

  return minchi2;
}

double RelInfoVertexIsolationBDT::getfdchi2(const LHCb::Track* track
                                            , LHCb::Vertex Vtx
                                            , const LHCb::RecVertex::Range PVs
                                            ) const
{

  IDVAlgorithm* dva = Gaudi::Utils::getIDVAlgorithm( contextSvc() ) ;
  if ( !dva ) { return Error("Could not get parent DVAlgorithm"); }
  double minchi2 = -1 ;
  double fdchi2 = -1;
  double fd;
  if ( !PVs.empty() ){
    for ( LHCb::RecVertex::Range::const_iterator pv = PVs.begin() ; pv!=PVs.end() ; ++pv){
      double ip, chi2;
      StatusCode test2 = dva->distanceCalculator()->distance ( (const LHCb::Track *)track, *pv, ip, chi2 );
      if ((chi2<minchi2) || (minchi2<0.))
      {
        minchi2 = chi2 ;
        StatusCode test2 = dva->distanceCalculator()->distance ( *pv, &Vtx, fd, fdchi2 );
      }
    }

  }
  else{ Warning( "FC Chi2 NOT FOUND PV" ) ;}
  return fdchi2;
}



//=============================================================================
// Opening angle for a track and particle
//=============================================================================
double RelInfoVertexIsolationBDT::getopening(const LHCb::Track* track,const  LHCb::Particle* P) const {
  Gaudi::XYZVector A = P->momentum().Vect();
  Gaudi::XYZVector B = track->momentum();
  double cosopening = A.Dot( B ) / std::sqrt( A.Mag2()*B.Mag2() );
  return cosopening;
}
