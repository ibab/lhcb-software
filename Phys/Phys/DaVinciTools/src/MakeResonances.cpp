// $Id: MakeResonances.cpp,v 1.4 2005-05-02 11:57:26 pkoppenb Exp $
// Include files 

#include <algorithm>

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 

// from DaVinci
#include "Kernel/IDecodeSimpleDecayString.h"
#include "Kernel/IFilterCriterion.h"
#include "Kernel/IPlotTool.h"
#include "Kernel/ICheckOverlap.h"

// local
#include "MakeResonances.h"
// #define PKDEBUG

//-----------------------------------------------------------------------------
// Implementation file for class : MakeResonances
//
// 2004-11-29 : Patrick KOPPENBURG
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<MakeResonances>          s_factory ;
const        IAlgFactory& MakeResonancesFactory = s_factory ;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MakeResonances::MakeResonances( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : DVAlgorithm ( name , pSvcLocator )
  ,  m_daughterFilter()
  ,  m_motherFilter()
  ,  m_daughterPlots()
  ,  m_motherPlots()
  ,  m_checkOverlap()
{
  declareProperty( "DaughterFilterName" , m_daughterFilterName = "DaughterFilter" );
  declareProperty( "MotherFilterName" , m_motherFilterName = "MotherFilter");
  declareProperty( "Window", m_massWindow = 1000000. );  
  declareProperty( "UpperWindow", m_upperMassWindow = 1000000. );  
  declareProperty( "LowerWindow", m_lowerMassWindow = 1000000. );  
  declareProperty( "MinMomentum", m_minMomentum = -1. );  
  declareProperty( "MinPt", m_minPt = -1. );
  declareProperty( "KillOverlap", m_killOverlap = true );  
  declareProperty( "DaughterPlotTool", m_daughterPlotTool = "RecursivePlotTool/DaughterPlots" );  
  declareProperty( "MotherPlotTool", m_motherPlotTool = "RecursivePlotTool/MotherPlots" );  
  declareProperty( "DaughterPlotsPath", m_daughterPlotsPath = "" );  
  declareProperty( "MotherPlotsPath", m_motherPlotsPath = "" );  
  declareProperty( "DecayDescriptors", m_decayDescriptors );  
  //  declareProperty( "MakePlots" , m_makePlots = false) ;
  declareProperty( "MotherToNGammas" , m_motherToNGammas = false) ;
}
//=============================================================================
// Destructor
//=============================================================================
MakeResonances::~MakeResonances() {}; 

//#############################################################################
// Initialization
//#############################################################################
StatusCode MakeResonances::initialize() {

  debug() << "==> Initialize" << endmsg;
  info() << "Mass Cuts are " << endmsg;
  info() << ">>>   Upper Mass Window  " << std::min(m_massWindow,m_upperMassWindow) << endmsg;
  info() << ">>>   Lower Mass Window  " << std::min(m_massWindow,m_lowerMassWindow) << endmsg;
  if ( m_minPt > 0.) info() << ">>>   Minimum Pt  " << m_minPt << endmsg;
  if ( m_minMomentum > 0.) info() << ">>>   Minimum P  " << m_minMomentum << endmsg;

  m_daughterFilter = tool<IFilterCriterion>("ByPIDFilterCriterion",m_daughterFilterName , this ) ;
  if ( !m_daughterFilter){
    err() << "Cannot retrieve " << m_daughterFilterName << endmsg ;
    return StatusCode::FAILURE ;    
  }
  m_motherFilter = tool<IFilterCriterion>("ByPIDFilterCriterion", m_motherFilterName, this ) ;
  if ( !m_motherFilter){
    err() << "Cannot retrieve " << m_motherFilterName << endmsg ;
    return StatusCode::FAILURE ;    
  }
  if ( m_killOverlap ){
    m_checkOverlap = tool<ICheckOverlap>("CheckOverlap") ;
    if ( !m_checkOverlap ){
      err() << "Cannot retrieve " << m_motherFilterName << endmsg ;
      return StatusCode::FAILURE ;    
    }
  }
  // histogramming 
  //  GaudiHistoAlg::setProduceHistos( m_makePlots );

  if ( produceHistos() ){
    if ( m_daughterPlotTool != "none" ){
      m_daughterPlots = tool<IPlotTool>(m_daughterPlotTool,this);
      if( !m_daughterPlots ) {
        err() << "Unable to get " << m_daughterPlotTool << endmsg;
        return StatusCode::FAILURE;
      }
      if (m_daughterPlotsPath == "") m_daughterPlotsPath = "I"+name();
      else m_daughterPlots->setPath(m_daughterPlotsPath);
      info() << "Daughter plots will be in " << m_daughterPlotsPath << endmsg ;
    }
    m_motherPlots = tool<IPlotTool>(m_motherPlotTool,this);
    if ( m_motherPlotTool != "none" ){
      if( !m_motherPlots ) {
        err() << "Unable to get " << m_motherPlotTool << endmsg;
        return StatusCode::FAILURE;
      }
      if (m_motherPlotsPath == "") m_motherPlotsPath = "O"+name();
      else m_motherPlots->setPath(m_motherPlotsPath);
      info() << "Mother plots will be in " << m_motherPlotsPath << endmsg ;
    }    
  }

  if(m_motherToNGammas){
    info() << "Will make mother to n gammas" << endmsg;
  }

  StatusCode sc = createDecays();
  if ( sc.isFailure()) return sc ;  

  return StatusCode::SUCCESS;
};
//=============================================================================
// Create decays, Stolen from CombineParticles
//=============================================================================
StatusCode MakeResonances::createDecays(){
  // get string decoder
  IDecodeSimpleDecayString* dsds = tool<IDecodeSimpleDecayString>("DecodeSimpleDecayString",this);
  if ( !dsds ) return StatusCode::FAILURE ;  
  
  if ( m_decayDescriptors.empty() ){
    m_decayDescriptors.push_back(getDecayDescriptor());
  }

  for ( std::vector<std::string>::const_iterator dd = m_decayDescriptors.begin() ;
        dd != m_decayDescriptors.end() ; ++dd ){
    
    debug() << " setting up " << *dd << endmsg;

    // initialize string decode
    StatusCode sc = dsds->setDescriptor(*dd);
    if ( sc.isFailure()) return sc ;  

    std::string mother;
    strings daughters;
    debug() << "Getting strings for " << dsds->getDescriptor() << endmsg;
    sc = dsds->getStrings(mother, daughters);
    if (sc.isFailure()) return sc;  
    std::sort(daughters.begin(),daughters.end()); // helps a lot, and used to avoid duplication of mothers in cc
    debug() << "Sorted daughters to " << daughters << endmsg ;
    sc = createDecay(mother, daughters);
    if (sc.isFailure()) return sc;
    
    // once again for cc if needed
    if ( dsds->is_cc() ){
      // LF : avoid duplication of mothers when using []cc
      strings daughtersBeforecc = daughters; // daughters have been sorted
      debug() << "Sorted before cc daughters to " << daughtersBeforecc << endmsg;
      
      debug() << "Setting up cc for " << dsds->getDescriptor() << endmsg;
      sc = dsds->getStrings_cc(mother, daughters);
      if (sc.isFailure()) return sc;
      std::sort(daughters.begin(),daughters.end()); // needed to compare daughters for original and cc
      debug() << "Sorted cc daughters to " << daughters << endmsg ;

      if(daughtersBeforecc == daughters){
        info() << "Ignoring cc since decay products identical to original decay products" << endmsg;
      }
      else{
        sc = createDecay(mother, daughters);
        if (sc.isFailure()) return sc;
      }
      
    } else verbose() << dsds->getDescriptor() << " is not a cc mode" << endmsg ;
  }
  
  return StatusCode::SUCCESS;
}
//=============================================================================
// Create decays
//=============================================================================
StatusCode MakeResonances::createDecay(const std::string& mother, 
                                       const strings& daughters){
  info() << "Creating " << mother << " -> " << daughters << endmsg;
  
  // mother
  ParticleProperty* pmother = ppSvc()->find(mother);
  if (!pmother){
    err() << "Cannot find particle property for mother " << mother << endmsg ;
    return StatusCode::FAILURE;
  }
  int pid = pmother->pdgID() ;
  
  //daughters
  std::vector<int> dpid ;
  for (strings::const_iterator d=daughters.begin() ; d!=daughters.end() ; ++d){
  // mother
    ParticleProperty* pd = ppSvc()->find(*d);
    if (!pd){
      err() << "Cannot find particle property for mother " << *d << endmsg ;
      return StatusCode::FAILURE;
    }
    dpid.push_back(pd->pdgID()) ;    
  }

  double mass = pmother->mass();
  double minmass = mass - std::min(m_massWindow,m_lowerMassWindow);
  double maxmass = mass + std::min(m_massWindow,m_upperMassWindow);
  verbose() << "Mass cuts are " << minmass << " " << maxmass << endmsg ;

  // decay container
  Decay dk;
  StatusCode sc = dk.initialize(pid,dpid,minmass,maxmass,m_minMomentum,m_minPt,m_checkOverlap);
  if (!sc) {
    err() << "Cannot initialize Decay object with " << pid << " " << dpid << endmsg ;
    return sc;
  }
  m_decays.push_back(dk);
  info() << "Initialized decay # " << m_decays.size() << endmsg ;
  
  return StatusCode::SUCCESS;
}
//#############################################################################
// Execution
//#############################################################################
StatusCode MakeResonances::execute() {

  debug() << "==> Execute" << endmsg;

  setFilterPassed(false);   // Mandatory. Set to true if event is accepted.
  ParticleVector Daughters , Resonances ;
  StatusCode sc = applyFilter(desktop()->particles(),Daughters,m_daughterFilter);
  if (!sc) {
    err() << "Unable to filter daughters" << endmsg;
    return StatusCode::FAILURE ;  
  }
  if (Daughters.empty()) {
    debug() << "No daughters found at all" << endmsg ;
    return StatusCode::SUCCESS;
  }
  sc = makePlots(Daughters,m_daughterPlots);
  if (!sc) return sc;

  // The LOOP ///
  for ( Decays::iterator d = m_decays.begin() ; d != m_decays.end() ; ++d ){
    verbose() << "New Decay loop" << endmsg ;
    if (!d->fillPidParticles(Daughters)){
      debug() << "Not all necessary particles found for decay" << endmsg ;
      continue ;      
    }
    sc = applyDecay(*d,Resonances); // make the resonances
    if (!sc) return sc;
  }
  debug() << "Found " << Resonances.size() << " candidates" << endmsg ;
  // filter
  ParticleVector Final ;
  sc = applyFilter(Resonances,Final,m_motherFilter);
  if (!sc) {
    err() << "Unable to filter mothers" << endmsg;
    return StatusCode::FAILURE ;  
  }  
  sc = makePlots(Final,m_motherPlots);
  if (!sc) return sc;
  debug() << "Saving " << Final.size() << " candidates" << endmsg ;
  sc = desktop()->saveTrees(Final);
  if (!sc) {
    err() << "Unable to save mothers" << endmsg;
    return StatusCode::FAILURE ;  
  }  
  setFilterPassed(!Final.empty());   // Mandatory. Set to true if event is accepted.
  return StatusCode::SUCCESS;
};
//=============================================================================
// Filter particles
//=============================================================================
StatusCode MakeResonances::applyFilter(const ParticleVector& IN, ParticleVector& OUT, 
                                       IFilterCriterion* fc) const{
  if (fc==NULL) { // not possible yet
    debug() << "Null filter criterion" << endmsg ;
    OUT = IN ;
    return StatusCode::SUCCESS;
  }
  for ( ParticleVector::const_iterator p = IN.begin() ; p!=IN.end(); ++p){
    if (fc->isSatisfied(*p)) {
      OUT.push_back(*p);
      debug() << "Particle " << (*p)->key() << " ID=" << (*p)->particleID().pid() << " with momentum " << 
        (*p)->momentum() << " m=" << (*p)->mass() << " passes cuts" << endmsg ;
    } else {    
      verbose() << "Particle "  << (*p)->key() << " ID=" << (*p)->particleID().pid() << " with momentum " 
                << (*p)->momentum() << " m=" << (*p)->mass() << " fails cuts" << endmsg ;
    }
  }  

  return StatusCode::SUCCESS;
}
//=============================================================================
// Apply one decay
//=============================================================================
StatusCode MakeResonances::applyDecay(Decay& d, ParticleVector& Resonances){
  verbose() << "In applyDecay" << endmsg ;
  ParticleVector DaughterVector ;
  bool inloop = d.getFirstCandidates(DaughterVector); // get first daughter vector
  while (inloop){ 
    verbose() << "In while loop " << DaughterVector.size() << endmsg ;
    // Find invariant mass
    HepLorentzVector sum4(0,0,0,0) ;
    for ( ParticleVector::const_iterator p = DaughterVector.begin() ; 
          p!=DaughterVector.end() ; ++p){
      verbose() << "Particle " << (*p)->key() << " ID=" 
                << (*p)->particleID().pid() << " with momentum " << 
        (*p)->momentum() << " m=" << (*p)->mass() << endmsg ;
      sum4 += (*p)->momentum() ;
    }
    verbose() << " -> momentum " << sum4 << " m=" << sum4.m() << endmsg ;
    if (d.goodFourMomentum(sum4)) {
      verbose() << "Found a candidate with mass " << sum4.m() << endmsg ;
      // LF
      // vertex fit or make mother to n gammas!
      Particle* Mother;
      StatusCode sc = makeMother(Mother,DaughterVector,d.getMotherPid());
      if (!sc){
        if(m_motherToNGammas) Warning("Something failed making mother to n gammas");
        else Warning("Something failed in vertex fitting");
      }
      else {
        verbose() << "Getting mother " << Mother->particleID().pid()
                  << " " << Mother->momentum() << endmsg ;
        Resonances.push_back(Mother);
      }
    } // mass cut
    inloop = d.getNextCandidates(DaughterVector);
  }
  debug() << "Found " << Resonances.size() << " candidates with PID " 
          << d.getMotherPid().pid() << endmsg ;
  return StatusCode::SUCCESS ;  
}

//=============================================================================
//  Make Mother Vertex fit
//=============================================================================
StatusCode MakeResonances::makeMother(Particle*& Mother,const ParticleVector& Daughters,
                                     const ParticleID& motherPid) const{
  verbose() << "Will make particle with PID " << motherPid.pid() << endmsg ;

  StatusCode sc = StatusCode::SUCCESS;
  Vertex MotherVertex;

  // LF
  if(m_motherToNGammas){
    // The mother made with gammas is created at the origin
    HepPoint3D zero(0.,0.,0.);
    MotherVertex.setPosition(zero);
    MotherVertex.setType(Vertex::Decay);
   
    // Add products to vertex
    for(ParticleVector::const_iterator iterP = Daughters.begin(); iterP != Daughters.end(); iterP++) {
      MotherVertex.addToProducts(*iterP);
    }
  }
  else{
    sc = vertexFitter()->fitVertex(Daughters,MotherVertex);
    if (!sc){
      Warning("Failed to fit vertex");
      return sc ;
    }
    debug() << "Fit vertex at " << MotherVertex.position()/cm 
            << " with chi^2 " << MotherVertex.chi2() << endmsg;
    // may add a chi^2 cut here
  } 
  
  Particle Candidate;
  sc = particleStuffer()->fillParticle(MotherVertex,Candidate,motherPid);
  if (!sc) {
    err() << "Failed to make particle with pid " << motherPid.pid() << endmsg ;
    return sc;
  }
  verbose() << "Calling desktop()->createParticle" << endmsg ;
  Mother = desktop()->createParticle(&Candidate);
  if (!Mother){
    err() << "Cannot create particle with pid " << motherPid.pid() << endmsg ;
    return StatusCode::FAILURE;
  }
  
  debug() << "Made Particle " << Mother->particleID().pid() << " with momentum "  
          << Mother->momentum() << " m=" << Mother->mass() << endmsg ;
  return StatusCode::SUCCESS;
};
//#############################################################################
// Plotting
//#############################################################################
StatusCode MakeResonances::makePlots(const ParticleVector& PV,IPlotTool* PT) {

  if (!produceHistos()) return StatusCode::SUCCESS;
  if (!PT) return StatusCode::SUCCESS;
  debug() << "Plotting " << endmsg;

  return PT->fillPlots(PV) ;
}
//#############################################################################
// Finalization
//#############################################################################
StatusCode MakeResonances::finalize() {

  debug() << "==> Finalize" << endmsg;

  return  StatusCode::SUCCESS;
}
//#############################################################################
// Decay class
//#############################################################################
StatusCode MakeResonances::Decay::initialize(const int& pid, 
                                             const std::vector<int>& pids,
                                             const double& m1, const double& m2, 
                                             const double& mp, const double& mpt, 
                                             ICheckOverlap* co){ 
  m_motherPid = ParticleID(pid) ;

  for ( std::vector<int>::const_iterator p = pids.begin() ; p != pids.end() ; ++p){
    // check for identical PIDs
    PidParticles PP(*p);
    for ( std::vector<PidParticles>::const_iterator p2 = m_pidParticles.begin() ; 
          p2 != m_pidParticles.end() ; ++p2){
      if ( (*p)==(*p2).getPid() ){
#ifdef PKDEBUG
        std::cout << "   initialize Found identical PIDs " << (*p) << " and " << (*p2).getPid() 
                  << " -> check for ordering" << std::endl ;
#endif       
        m_checkOrder = true ;       
        PP.setSamePid(true);
      }
    }
    m_pidParticles.push_back(PP);
  }  
  m_minMass = m1;
  m_maxMass = m2;
  m_minP    = mp;
  m_minPt   = mpt;
  m_checkP = (( mpt>0.) || (mp>0.));
  m_checkOverlap = co ;

  return StatusCode::SUCCESS ;
}
//=============================================================================
// Fill PidParticles at each event
//=============================================================================
bool MakeResonances::Decay::fillPidParticles(const ParticleVector& PV){

  for ( std::vector<PidParticles>::iterator pp=m_pidParticles.begin() ;
        pp != m_pidParticles.end() ; ++pp ){    
    if (!pp->fillParticles(PV)){ // fill from vector
#ifdef PKDEBUG
      std::cout << "   fillPidParticles: No particles with ID " << pp->getPid() << " found" << std::endl ;
#endif
      return false ;
    }
  }
  return true ;
}

//=============================================================================
// Get first set of daughters candidates 
//=============================================================================
bool MakeResonances::Decay::getFirstCandidates(ParticleVector& DaughterVector){
  return getCandidates(DaughterVector);
}
//=============================================================================
// Get next set of daughters candidates 
//=============================================================================
bool MakeResonances::Decay::getNextCandidates(ParticleVector& DaughterVector){
  if (!iterate()) return false; // first iterate to next iterators
  return getCandidates(DaughterVector);
}
//=============================================================================
// Get set of daughters candidates 
//=============================================================================
bool MakeResonances::Decay::getCandidates(ParticleVector& DaughterVector){
  bool refusedcombination = true ;
  while ( refusedcombination ){    
    DaughterVector.clear();
    refusedcombination = false ;
    for ( std::vector<PidParticles>::const_iterator pp = m_pidParticles.begin() ;
          pp != m_pidParticles.end() ; ++pp ){
      Particle* P = pp->getCandidate() ;
      if (!P) { 
        std::cout << "MakeResonances::Decay::getNextCandidates : This should never happen (1)" << std::endl;
        return false ;
      } else {
        if ( m_checkOrder && pp->samePid() ) {  
          if (*(DaughterVector.rbegin())) {
            if ( (*(DaughterVector.rbegin()))->key() >= P->key()){ // in case of same PID I want one key to be greater
#ifdef PKDEBUG
              std::cout << "   getCandidates skipping combination with keys " 
                        << (*(DaughterVector.rbegin()))->key() << " and " <<  P->key() << std::endl ;
#endif
              refusedcombination = true ;
              break ;
            }
          } else {
            std::cout << "MakeResonances::Decay::getNextCandidates : This should never happen (2)" << std::endl;
            return false ;
          } 
        }
        DaughterVector.push_back(P);
      } // there is a particle
    } // pp loop
    
    if ( (!refusedcombination) && ( m_checkOverlap )) {
      if ( foundOverlap(DaughterVector) ){
#ifdef PKDEBUG
        std::cout << "   getCandidates Found Overlap -> iterating " << std::endl ;
#endif
        refusedcombination = true ;
      }
    }
    if (refusedcombination){
      if (!iterate()) return false; // iterate if possible
    }
    
  } // while
#ifdef PKDEBUG
  std::cout << "   getCandidates returns vector : "  ;
    for ( ParticleVector::const_iterator p = DaughterVector.begin() ;
          p != DaughterVector.end() ; ++p ){ std::cout << (*p)->key() << " " ; }
    std::cout << std::endl ;
#endif
  return true; // it's OK
}
//=============================================================================
// Found Overlap in vector?
//=============================================================================
bool MakeResonances::Decay::foundOverlap(const ParticleVector& DaughterVector){
  ParticleVector Children = DaughterVector ;
#ifdef PKDEBUG2
  std::cout << "   foundOverlap Starting loop " << Children.size() << std::endl ;
#endif
  while ( replaceResonanceByDaughters( Children )){
#ifdef PKDEBUG2
    std::cout << "   foundOverlap ... in endless loop... " << Children.size() << std::endl ;
#endif
  } // endless loop
#ifdef PKDEBUG2
  std::cout << "   foundOverlap Out of loop " << Children.size() << std::endl ;
#endif
  bool found = m_checkOverlap->foundOverlap( Children );
#ifdef PKDEBUG2
  if (found) std::cout << "Overlap found! " << std::endl ;
#endif
  return found ;
}
//=============================================================================
// Replace resonance by daughters in vector
//=============================================================================
bool MakeResonances::Decay::replaceResonanceByDaughters(ParticleVector& Children){
  for ( ParticleVector::iterator c = Children.begin() ; c !=  Children.end() ; ++c ){
    if ( (*c)->endVertex() ){
      SmartRefVector<Particle> daughters = (*c)->endVertex()->products() ;
#ifdef PKDEBUG
      std::cout << "   replaceResonanceByDaughters Erasing     " << (*c)->particleID().pid() << " "
                <<  (*c)->momentum() << std::endl ;
#endif
      Children.erase(c);
      for ( SmartRefVector<Particle>::iterator d = daughters.begin() ; 
            d !=  daughters.end() ; ++d ){
        Particle* p = *d ;
        Children.push_back(p);
#ifdef PKDEBUG
        std::cout << "   replaceResonanceByDaughters Pushed back " << p->key() << " " << p->particleID().pid() << " "
                  <<  p->momentum() << std::endl ;
#endif
      } 
      return true ;
    }
  }
  return false;
}
//=============================================================================
// iterate to next combination
//=============================================================================
bool MakeResonances::Decay::iterate(){
  // now iterate to the next combination
  bool didbreak = false ;
  for ( std::vector<PidParticles>::reverse_iterator pp = m_pidParticles.rbegin() ;
        pp != m_pidParticles.rend() ; ++pp ){ // reverse iterator to first increment vector n 
    if (pp->iterate()) {
      didbreak = true ;
      break ; // if I can go to the next in this vector I can exit the loop
    }
#ifdef PKDEBUG
    std::cout << "   iterate could not iterate for pid " << pp->getPid() << std::endl ;     
#endif
  }
#ifdef PKDEBUG
  std::cout << "   iterate returns " << didbreak << std::endl ;
#endif
  return didbreak ; // If I did not break, then I'm at the end of the loop
}
//#############################################################################
// PidParticles class
//#############################################################################
bool MakeResonances::Decay::PidParticles::fillParticles(const ParticleVector& PV){
  
  m_particles.clear();
  for ( ParticleVector::const_iterator p=PV.begin(); p!=PV.end(); ++p){
    if ( m_pid==(*p)->particleID().pid() ) m_particles.push_back(*p) ;
  }
#ifdef PKDEBUG
  std::cout << "PidParticles::fillParticles Found " <<  m_particles.size() << " particles with pid "
            << m_pid << " : " ;
  for ( ParticleVector::const_iterator p = m_particles.begin(); p!=m_particles.end();++p) {
    std::cout << (*p)->key() << " " ;
  }
  std::cout << std::endl ;
#endif
  m_iterator = m_particles.begin();
  return ( !m_particles.empty() );
  
}
//=============================================================================
// go to next
//=============================================================================
bool MakeResonances::Decay::PidParticles::iterate(){
  ++m_iterator ;
  if ( m_iterator==m_particles.end() ) {
#ifdef PKDEBUG
    std::cout << "   PidParticles::iterate Reached end of loop for pid " << m_pid << std::endl ;
#endif
    m_iterator=m_particles.begin() ;
    return false ;
  } else {
#ifdef PKDEBUG
    std::cout << "   PidParticles::iterate Iterating for pid " << m_pid << std::endl ;    
#endif
    return true ; 
  }
  
}
