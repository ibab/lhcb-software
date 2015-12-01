// $Id: $
// Include files

// local
#include "TupleToolDecayTreeFitter.h"
#include "Kernel/GetIDVAlgorithm.h"
#include "Kernel/IDVAlgorithm.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Event/RecVertex.h"
#include "Event/Particle.h"
#include "Kernel/ParticleProperty.h"
#include "Kernel/IParticleDescendants.h"
#include "Kernel/Escape.h"
#include "DecayTreeFitter/Fitter.h"
#include "Kernel/IParticlePropertySvc.h"
#include "LoKi/ParticleProperties.h"


// boost
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace LHCb;

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolDecayTreeFitter
// Yasmine Amhis, Matt Needham, Patrick Koppenburg
// 30-10-2010, 01-04-2011
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolDecayTreeFitter::TupleToolDecayTreeFitter( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  , m_dva(NULL)
  , m_ppSvc()
  , m_particleDescendants()
  , m_map ()
  , m_substitute()
  , m_stateprovider( "TrackStateProvider" )
  , m_first(true)
{
  declareProperty("daughtersToConstrain", m_massConstraints , "List of particles to contrain to mass");
  declareProperty("constrainToOriginVertex", m_constrainToOriginVertex = false,
                  "Do a refit constraining to Origin Vertex (could be PV)");
  declareProperty( "Substitutions" ,m_map, "PID-substitutions :  { ' decay-component' : 'new-pid' }" ) ;
  declareProperty( "StoreRefittedPVsTwice" ,m_storeAnyway = false,
                   "Store PV even if a refitted version is already the best PV (i.e store twice)" ) ;
  declareProperty( "UpdateDaughters" ,m_updateDaughters = false,
                   "Store updated momenta of tracks in the decay tree" ) ;
  declareProperty( "StateProvider", m_stateprovider ) ;
  declareInterface<IParticleTupleTool>(this);
}

//=============================================================================
StatusCode TupleToolDecayTreeFitter::initialize()
{
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // convert the list of names to a list of pids
  const LHCb::ParticleProperty* prop = 0 ;
  m_ppSvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
  for (std::vector<std::string>::const_iterator iterS = m_massConstraints.begin();
       iterS != m_massConstraints.end(); ++iterS )
  {
    prop = m_ppSvc->find( *iterS );
    if (!prop)  Exception("Unknown PID");
    m_massConstraintsPids.push_back(prop->pdgID());
  } // iterS

  m_dva = Gaudi::Utils::getIDVAlgorithm ( contextSvc(), this ) ;
  if (0==m_dva) return Error("Couldn't get parent IDVAlgorithm", StatusCode::FAILURE);

  m_particleDescendants = tool<IParticleDescendants> ( "ParticleDescendants");

  if ( !m_stateprovider.empty() )
  {
    sc = m_stateprovider.retrieve() ;
    if ( sc.isFailure() ) return sc ;
  }

  if ( m_extraName.empty() )
  {
    const std::string en = name() ; // use tool name as prepended name
    unsigned int d = en.find_last_of(".");
    m_extraName = en.substr(d+1,en.size()-1); // from d to end
    if ( "TupleToolDecayTreeFitter" == m_extraName )  m_extraName = ""; // user has not chanegd instance name
    info() << "All fields will be prepended with ``" << m_extraName << "''" <<endmsg;
  }
  
  if ( m_extraName.empty() )
  {
    return Error( "Extraname is empty. Always give an instance name "
                  "to TupleToolDecayTreeFitter! See doxygen." );
  }

  if ( !m_map.empty() )
  {
    m_substitute = tool<ISubstitutePID>("SubstitutePIDTool",this);
    sc = m_substitute->decodeCode( m_map );
  }
  
  return sc;
}

StatusCode TupleToolDecayTreeFitter::finalize()
{
  StatusCode sc = StatusCode::SUCCESS;
  if ( !m_stateprovider.empty() ) { sc = m_stateprovider.release(); }
  return TupleToolBase::finalize() && sc ;
}

//=============================================================================
//  The fill method implementation
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fill( const LHCb::Particle* mother
                                           , const LHCb::Particle* P
                                           , const std::string& head
                                           , Tuples::Tuple& tuple )
{
  if( !P ) return StatusCode::FAILURE;
  if ( P->isBasicParticle() )
  {
    return Error("Do not call TupleToolDecayTreeFitter for basic particles. Use Branches. See doxygen.");
  }
  const std::string prefix=fullName(head);
  if (msgLevel(MSG::DEBUG)) debug() << "head ''" << head << "'' prefix ''" << prefix 
                                    << "'' extraname ''" << m_extraName << "''" <<endmsg;
  
  const ITrackStateProvider* stateprovider = ( m_stateprovider.empty() ? NULL : &(*m_stateprovider) );

  LHCb::DecayTree tree ( *P ) ;
  // substitute
  if ( m_substitute && !substitute(tree)) return StatusCode::FAILURE ; 
  if ( !m_massConstraintsPids.empty() && !checkMassConstraints(tree)) return StatusCode::FAILURE ;

  // get origin vertices
  std::vector<const VertexBase*> originVtx;
  TupleMap tMap ; // the temporary data map

  if (m_constrainToOriginVertex)
  {
    if (msgLevel(MSG::DEBUG)) {
      debug() << "Constrain the origin vertex" << endmsg;
    }
    // check for origin vertex
    originVtx = originVertex( mother, P );
    if( originVtx.empty() ){return Error("Can't get an origin vertex");}
    if (msgLevel(MSG::DEBUG)) debug() << "PVs: " << originVtx.size() << endmsg;
    for ( std::vector<const VertexBase*>::const_iterator iv = originVtx.begin() ;
          iv != originVtx.end() ; ++iv )
    {
      if (msgLevel(MSG::DEBUG)) debug() << "Creating DecayTreeFitter on "
                                        <<  tree.head() << " " << *iv << endmsg;
      DecayTreeFitter::Fitter fitter(*(tree.head()), *(*iv), stateprovider ) ;
      if (msgLevel(MSG::DEBUG)) debug() << "Created DecayTreeFitter" << endmsg;
      if (!fit(fitter, tree.head(), *iv, prefix, tMap)) return StatusCode::FAILURE ;
    }
  }
  else 
  {
    if (msgLevel(MSG::DEBUG)) debug() << "Do not constrain the origin vertex" << endmsg;
    // Get the fitter
    DecayTreeFitter::Fitter fitter(*(tree.head()), stateprovider ) ;
    if (!fit(fitter, tree.head(), 0, prefix, tMap)) return StatusCode::FAILURE;
  }

  return fillTuple(tMap,tuple,prefix); // the actual filling
}
//=============================================================================
// do filling for a given vertex
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fit(DecayTreeFitter::Fitter& fitter,
                                         const LHCb::Particle* P,
                                         const LHCb::VertexBase* pv,
                                         const std::string& prefix,
                                         TupleMap& tMap) const
{
  if (msgLevel(MSG::VERBOSE)) verbose() << "fit " << P << " " << pv << " " << prefix << endmsg ;
  bool test = true ;
  //add mass contraints
  if ( !m_massConstraintsPids.empty() )
  {
    for ( std::vector<LHCb::ParticleID>::const_iterator iterC = m_massConstraintsPids.begin();
          iterC != m_massConstraintsPids.end(); ++iterC )
    {
      fitter.setMassConstraint(*iterC);
    }
  }
  // fit
  if (msgLevel(MSG::VERBOSE)) verbose() << "calling Fit" << endmsg ;
  fitter.fit();
  if (msgLevel(MSG::VERBOSE)) verbose() << "called Fit" << endmsg ;
  // fill the fit result
  fillDecay(fitter,prefix,tMap );
  fillMomentum(fitter,P,prefix,tMap );
  if (m_constrainToOriginVertex)
  {
    test &= fillPV(pv,prefix,tMap);
    test &= fillLT(fitter,P,prefix,tMap );
  }
  if ( isVerbose() )
  {
    test &= fillDaughters( fitter,P,prefix,tMap );
  }
  if ( m_updateDaughters )
  {
    test &= fillStableDaughters( fitter,P,prefix,tMap );
  }

  return StatusCode(test);
}
//=============================================================================
// Fill standard stuff
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillPV(const LHCb::VertexBase* pv,
                                            const std::string& prefix,
                                            TupleMap& tMap ) const
{
  bool test = true;
  if (msgLevel(MSG::VERBOSE)) verbose() << "FillPV " << prefix << endmsg ;
  if (!pv) Exception("Null PVs cannot happen with ConstrainToOriginVertex!");
  test &= insert( prefix+"_PV_key", pv->key(), tMap );
  if ( isVerbose() )
  {
    test &= insert( prefix+"_PV_X", pv->position().X(), tMap );
    test &= insert( prefix+"_PV_Y", pv->position().Y(), tMap );
    test &= insert( prefix+"_PV_Z", pv->position().Z(), tMap );
  }
  return StatusCode(test);
}
//=============================================================================
// Fill standard stuff
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillDecay(const DecayTreeFitter::Fitter& fitter,
                                               const std::string& prefix,
                                               TupleMap& tMap ) const
{
  bool test = true;
  if (msgLevel(MSG::VERBOSE)) verbose() << "FillDecay " << prefix << endmsg ;

  test &= insert( prefix+"_status", fitter.status(), tMap );
  test &= insert( prefix+"_nDOF", fitter.nDof(), tMap  );
  test &= insert( prefix+"_chi2", fitter.chiSquare(), tMap  );
  test &= insert( prefix+"_nIter", fitter.nIter(), tMap  );

  return StatusCode(test);
}
//=============================================================================
// Fill momentum and mass information
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillMomentum(const DecayTreeFitter::Fitter& fitter,
                                                  const Particle* P,
                                                  const std::string& prefix,
                                                  TupleMap& tMap ) const
{
  bool test = true;

  if (msgLevel(MSG::VERBOSE)) verbose() << "FillMomentum " << prefix << endmsg ;
  //Get the fit parameters
  const Gaudi::Math::ParticleParams* params = fitter.fitParams(P) ;
  const Gaudi::Math::LorentzVectorWithError& momentum = params->momentum() ;

  test &= insert( prefix+"_M",  momentum.m().value(), tMap  );
  test &= insert( prefix+"_MERR", momentum.m().error(), tMap );
  test &= insert( prefix+"_P", momentum.p().value(), tMap );
  test &= insert( prefix+"_PERR", momentum.p().error(), tMap ) ;//MeV

  return StatusCode(test);
}
//=============================================================================
// Fill lifetime information
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillLT(const DecayTreeFitter::Fitter& fitter,
                                            const Particle* P,
                                            const std::string& prefix,
                                            TupleMap& tMap ) const{

  bool test = true;
  if (msgLevel(MSG::VERBOSE)) verbose() << "FillLT " << prefix << endmsg ;
  const Gaudi::Math::ParticleParams* tParams = fitter.fitParams(P);
  const Gaudi::Math::ValueWithError& decayLength = tParams->decayLength();
  const Gaudi::Math::ValueWithError& ctau = tParams->ctau();
  test &= insert( prefix+"_ctau", ctau.value(), tMap  );
  test &= insert( prefix+"_ctauErr", ctau.error(), tMap  );
  test &= insert( prefix+"_decayLength", decayLength.value(), tMap  );
  test &= insert( prefix+"_decayLengthErr", decayLength.error(), tMap  );

  return StatusCode(test);
}
//=============================================================================
// Fill lifetime information for non stable daughters
//=============================================================================
StatusCode TupleToolDecayTreeFitter::fillDaughters( const DecayTreeFitter::Fitter& fitter
                                                    ,const LHCb::Particle* P
                                                    ,const std::string& prefix
                                                    ,TupleMap& tMap ) const
{
  bool test = true;

  if (msgLevel(MSG::VERBOSE)) verbose() << "FillDaughters " << prefix << endmsg ;
  const LHCb::Particle::ConstVector& daughters = m_particleDescendants->descendants(P);
  if (msgLevel(MSG::DEBUG)) debug() << "for id " << P->particleID().pid()
                                    << " daughter size is " << daughters.size() << endmsg;
  if ( daughters.empty() ) return test;
  std::set<std::string> usedNames;
  unsigned int add = 0;
  for ( LHCb::Particle::ConstVector::const_iterator it = daughters.begin(); 
        it != daughters.end(); ++it )
  {
    const LHCb::Particle* particle = *it;
    if ( particle->isBasicParticle() ) continue ;
    const unsigned int pid = abs(particle->particleID().pid());
    const std::string pidName = getName(pid) ;
    std::string name = prefix+"_"+pidName ;
    bool renamed = false;
    while ( usedNames.find(name) != usedNames.end() ) 
    { // fix to bug 88702
      renamed = true;
      if (msgLevel(MSG::VERBOSE)) verbose() << "Found already name " << name 
                                            << " trying next " << endmsg;
      name = prefix + "_" + pidName + "_" + boost::lexical_cast<std::string>(add);
      ++add;
    }
    if ( renamed ) Info("Renaming duplicate to "+name,StatusCode::SUCCESS,1);
    usedNames.insert(name);
    test &= fillMomentum( fitter,particle,name,tMap );
    test &= fillLT( fitter,particle,name,tMap);
  }
  return StatusCode(test);
}
//=============================================================================
// Fill lifetime information for stable daughters
//=============================================================================
StatusCode 
TupleToolDecayTreeFitter::fillStableDaughters( const DecayTreeFitter::Fitter& fitter
                                               ,const LHCb::Particle* P
                                               ,const std::string& prefix
                                               ,TupleMap& tMap ) const
{
  bool test = true;

  if (msgLevel(MSG::VERBOSE)) verbose() << "FillStableDaughters " << prefix << endmsg ;
  const LHCb::Particle::ConstVector& daughters = P->daughtersVector();
  if (msgLevel(MSG::DEBUG)) debug() << "for id " << P->particleID().pid()
                                    << " daughter size is " << daughters.size() << endmsg;
  if ( daughters.empty() ) return test;
  std::set<std::string> usedNames;
  unsigned int add = 0;
  for ( LHCb::Particle::ConstVector::const_iterator it = daughters.begin(); 
        it != daughters.end(); ++it ) 
  {
    const LHCb::Particle* particle = *it;
    if ( !particle->isBasicParticle() ) 
    {
      const unsigned int pid = abs(particle->particleID().pid());
      const std::string pidName = getName(pid) ;
      std::string name = prefix+"_"+pidName ;
      bool renamed = false;
      while ( usedNames.find(name) != usedNames.end() )
      { // fix to bug 88702
        if (msgLevel(MSG::VERBOSE)) verbose() << "Found already name " << name 
                                              << " trying next " << endmsg ;
        renamed = true;
        name = prefix+"_"+pidName+boost::lexical_cast<std::string>(add);
        ++add;
      }
      if ( renamed ) Info("Renaming duplicate to "+name,StatusCode::SUCCESS,1);
      usedNames.insert(name);
      test &= fillStableDaughters( fitter, particle, name, tMap );
    }
    else
    {
      // const int pid = particle->particleID().pid();
      const unsigned int pid = abs(particle->particleID().pid());
      const std::string pidName = getName(pid) ;
      std::string name = prefix+"_"+pidName;
      bool renamed = false;
      while ( usedNames.find(name) != usedNames.end() ) 
      { // fix to bug 88702
        if (msgLevel(MSG::VERBOSE)) verbose() << "Found already name " << name 
                                              << " trying next " << endmsg ;
        renamed = true;
        name = prefix+"_"+pidName+"_"+boost::lexical_cast<std::string>(add);
        ++add;
      }
      if ( renamed ) Info("Renaming duplicate to "+name,StatusCode::SUCCESS,1);
      usedNames.insert(name);
      test &= fillTracksMomentum( fitter, particle, name, tMap );
    }
  }
  return StatusCode(test);
}

//=============================================================================
// Fill updated tracks momentum
//=============================================================================
StatusCode 
TupleToolDecayTreeFitter::fillTracksMomentum( const DecayTreeFitter::Fitter& fitter,
                                              const Particle* P,
                                              const std::string& prefix,
                                              TupleMap& tMap ) const
{
  bool test = true;

  if (msgLevel(MSG::VERBOSE)) verbose() << "FillTracksMomentum " << prefix << endmsg ;

  //Get the fit parameters
  const Gaudi::Math::ParticleParams* params = fitter.fitParams(P) ;
  const Gaudi::Math::LorentzVectorWithError& momentum = params->momentum() ;

  test &= insert( prefix+"_ID", P->particleID().pid(), tMap  );
  test &= insert( prefix+"_PX", momentum.Px(), tMap  );
  test &= insert( prefix+"_PY", momentum.Py(), tMap  );
  test &= insert( prefix+"_PZ", momentum.Pz(), tMap  );
  test &= insert( prefix+"_PE", momentum.E() , tMap  );//MeV

  return StatusCode(test);
}


//=============================================================================
// append data to TupleMap
//=============================================================================
StatusCode TupleToolDecayTreeFitter::insert( const std::string& leaf, 
                                             const double val, 
                                             TupleMap& tMap ) const 
{
  TupleMap::iterator l = tMap.find(leaf);
  if ( l == tMap.end() )
  {  /// first time this is seen. Create
    std::vector<double> vals;
    vals.push_back(val);
    tMap.insert( std::make_pair(leaf,vals) );
  } 
  else
  {
    l->second.push_back(val); /// append a to vector
  }
  if (msgLevel(MSG::VERBOSE)) 
    verbose() << "insert " << leaf << " " << val  
              << " size " << l->second.size() << endmsg ;
  return StatusCode::SUCCESS ;
}

//=============================================================================
// actual filling of the Tuple
//=============================================================================
StatusCode
TupleToolDecayTreeFitter::fillTuple( const TupleMap& tMap, 
                                     Tuples::Tuple& tuple, 
                                     const std::string& prefix ) const 
{
  bool test = true ;
  for ( TupleMap::const_iterator t = tMap.begin() ; t != tMap.end() ; ++t )
  {
    const std::string&         leaf = t->first;
    const std::vector<double>& data = t->second;
    if (msgLevel(MSG::DEBUG))
      debug() << "Filling leaf ``" << leaf << "'' with vector of size " 
              << data.size() << endmsg ;
    if ( m_maxPV < data.size() )
      Exception("Seeing data with too many PVs. Have you set MaxPVs?");
    test &= tuple->farray( leaf, data, prefix+"_nPV", m_maxPV);
  }
  return StatusCode(test);
}

//=============================================================================
// Sort Tracks
//=============================================================================
std::set<const LHCb::Track*> 
TupleToolDecayTreeFitter::sortedTracks(const LHCb::VertexBase* vb) const
{
  const LHCb::RecVertex* pv = dynamic_cast<const LHCb::RecVertex*>(vb);
  if (!pv) Exception("Failed to cast PV");
  std::set<const LHCb::Track*> st ;
  for ( SmartRefVector<LHCb::Track>::const_iterator i = pv->tracks().begin(); 
        i != pv->tracks().end(); ++i )
  {
    st.insert(*i);
  }
  return st ;
}

//=============================================================================
// Compare PVs, check that one PV's tracks is a subset of the other
//=============================================================================
bool TupleToolDecayTreeFitter::samePV( const LHCb::VertexBase* vb1, 
                                       const LHCb::VertexBase* vb2 ) const
{
  // exception checking. See bug https://savannah.cern.ch/bugs/?100933
  if (!vb1 && !vb2){
    Warning("samePV method called with 2 NULL PVs. "
            "The answer is obviously true, but you may want to check the meaning of the question.",
            StatusCode::SUCCESS,1);
    return true ;
  } else if (!vb1 || !vb2){
    Warning("samePV method called with 1 NULL PV. "
            "The answer is obviously false, but you may want to check the meaning of the question.",
            StatusCode::SUCCESS,1);
    return false ;    
  }

  if ( !(vb1->isPrimary()) || !(vb2->isPrimary()) )
  {
    Warning("Non PV VertexBase is being used as PV",1,StatusCode::SUCCESS).ignore();
    return false ;
  }

  std::set<const LHCb::Track*> st1 = sortedTracks(vb1);
  std::set<const LHCb::Track*> st2 = sortedTracks(vb2);

  const bool inc = std::includes(st1.begin(),st1.end(),st2.begin(),st2.end());
  if ( msgLevel(MSG::VERBOSE))
  {
    verbose() << "PV 2 of size " << st2.size() << " is ";
    if (!inc) verbose() << "not ";
    verbose() << "included in PV 1 of size " << st1.size() << endmsg ;
  }
  return inc;
}

//=============================================================================
// get origin vertex
//=============================================================================
std::vector<const VertexBase*>
TupleToolDecayTreeFitter::originVertex( const Particle* mother, const Particle* P ) const 
{
  std::vector<const VertexBase*> oriVx;
  if (mother == P){// the origin vertex is the primary.
    const VertexBase* bpv = m_dva->bestVertex( P );
    if ( bpv ) 
    {
      oriVx.push_back(bpv);
      if ( msgLevel(MSG::VERBOSE) ) 
        verbose() << "Pushed back bpv " << bpv << " from "
                  << tesLocation(bpv) << " at "
                  << bpv->position() << endmsg ;
    }
    else if ( m_constrainToOriginVertex)
    {
      Warning( "NULL bestPV while constraining to origin vertex. Fit will be ignored.",
               0, StatusCode::SUCCESS ).ignore();
    }
    // all the other ones
    /// @todo : keep only the related ones
    for ( LHCb::RecVertex::Range::const_iterator pv = m_dva->primaryVertices().begin();
          pv != m_dva->primaryVertices().end() ; ++pv )
    {
      if ( m_storeAnyway || !samePV(*pv,bpv) )
      {
        oriVx.push_back(*pv);
        if ( msgLevel(MSG::VERBOSE) )
          verbose() << "Pushed back  pv " << *pv << " from "
                    << tesLocation(*pv) << " at "
                    << (*pv)->position() << endmsg ;
      }
      if ( oriVx.size() >= m_maxPV )
      {
        Warning("Truncated number of PVs",0,StatusCode::FAILURE).ignore();
        break ;
      }
    }
  } 
  else 
  {
    const SmartRefVector<LHCb::Particle>& dau = mother->daughters ();
    if( dau.empty() ) return oriVx ;

    for( SmartRefVector<LHCb::Particle>::const_iterator it = dau.begin(); 
         dau.end() != it; ++it )
    {
      if( P == *it )
      {
        oriVx.push_back(mother->endVertex());
        return oriVx ;
      }
    }

    // vertex not yet found, get deeper in the decay:
    for( SmartRefVector<LHCb::Particle>::const_iterator it = dau.begin(); 
         dau.end() != it; ++it )
    {
      if( P != *it && !(*it)->isBasicParticle() )
      {
        oriVx = originVertex( *it, P );
        if( !oriVx.empty() ) 
        {
          return oriVx ;  // found
        }
      }
    }
  }
  return oriVx;
}

//=============================================================================
// Convert pid number in names
//=============================================================================
std::string TupleToolDecayTreeFitter::getName(const int id) const
{
  const LHCb::ParticleProperty* prop = m_ppSvc->find( LHCb::ParticleID(id) );
  if (!prop) Exception("Unknown PID");
  //if (msgLevel(MSG::VERBOSE)) verbose() << "ID " << id << " gets name "
  //                                      << Decays::escape(prop->name()) << endmsg ;
  return Decays::escape(prop->name());
}

//=============================================================================
// Substitute
//=============================================================================
StatusCode TupleToolDecayTreeFitter::substitute(LHCb::DecayTree& tree){
  if (msgLevel(MSG::DEBUG)) debug() << "Calling substitute" << endmsg ;
  unsigned int substituted = m_substitute->substitute ( tree.head() ) ;
  // debugging
  if ( msgLevel(MSG::VERBOSE) || 0==substituted ){
    LHCb::DecayTree::CloneMap mp = tree.cloneMap();
    for ( LHCb::DecayTree::CloneMap::const_iterator i = mp.begin() ; i != mp.end() ; ++i){
      if ( i->first->particleID().pid()==i->second->particleID().pid())
      {
        info() << "A " << getName(i->first->particleID().pid()) << " remains unchanged" << endmsg ;
      } else {
        info() << "A " << getName(i->first->particleID().pid()) << " is substituted by a " 
               << getName(i->second->particleID().pid()) << endmsg ;
      }
    }
    
  } 
  if (0==substituted) 
  {
    return Error( "No particles have been substituted. Check your substitution options." );
  }
  return StatusCode::SUCCESS ;
}
//=============================================================================
// Check Mass Constraints
//=============================================================================
StatusCode TupleToolDecayTreeFitter::checkMassConstraints(const LHCb::DecayTree& tree){
  if (!m_first) return StatusCode::SUCCESS ;  // do that only once
  m_first = false ;
  const LHCb::DecayTree::CloneMap mp = tree.cloneMap();
  for ( std::vector<LHCb::ParticleID>::const_iterator m =  m_massConstraintsPids.begin() ;
        m!= m_massConstraintsPids.end() ; m++){
    bool found = false ;
    for ( LHCb::DecayTree::CloneMap::const_iterator i = mp.begin() ; i != mp.end() ; i++){
      if (m->abspid()==i->second->particleID().abspid()) {
        found = true ;
        break ;
      }
    }
    if (found &&  msgLevel(MSG::VERBOSE)) 
      verbose() << "Constraint " << getName(m->pid()) << " was found in tree" << endmsg ;
    if (!found) {
      std::ostringstream mess;
      mess <<  "Constraint " << getName(m->pid()) 
           << " was not found in tree. Check your options. Maybe also the substitution options.";
      return Error( mess.str() ) ;
    }
  }
  return StatusCode::SUCCESS ;

}

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolDecayTreeFitter )
