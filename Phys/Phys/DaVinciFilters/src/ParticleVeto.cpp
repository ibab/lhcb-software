// $Id: $
// Include files 

// local
#include "ParticleVeto.h"

//-----------------------------------------------------------------------------
// Implementation file for class : ParticleVeto
//
//
//
//  Check overlap between a particle P and any particles from a given container
//
//   - Scan the decay Tree for composite particles
//        - Compare proto origin of charged particles (as in CheckOverlap)
//        - Compare cluster seed of neutral objects (e.g. photon and mergedPi0 are different proto but can be same cluster)
//
//             
//
//
// *  main methods :
//
//  foundOverlap( P , containerName , mode)
//  foundOverlap( P , vector<containerName>, mode)
//
//
// mode = 0 (default) : check any overlap between P and any particule p from the container(s) 
//                            - e.g. Pi0 VETO for photon :  check a photon P is not used to form any pi0 in the container
//
// mode = 1           : require P and p to have the same PID 
//                            - use-case : compare overlap between two different selections of same particles
//
// mode = 2           : do not consider overlap when P and p are strictly identical (only 'partial' overlap are checked)
//
// mode = 3           : mode 1 && mode 2 
//                            - use-case : check overlap between  P and any *other* particle from the same container
//                            - pi0 VETO for pi0 : check both photons from pi0->gg cannot form another pi0 in the same container
//
// other public method : 
//
//  foundOverlap( P , p , mode)
//
//  check overlap between particle P and p 
//
// 2012-04-25 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( ParticleVeto )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ParticleVeto::ParticleVeto( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_cont(""){
  declareInterface<IParticleVeto>(this);

}
//=============================================================================
// Destructor
//=============================================================================
ParticleVeto::~ParticleVeto() {} 

//=============================================================================


StatusCode ParticleVeto::initialize(){
     StatusCode sc = GaudiTool::initialize();
     if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
     if( UNLIKELY( msgLevel(MSG::DEBUG) ) )debug() << "==> Initialize " << name() << endmsg;
     IIncidentSvc* inc = incSvc() ;
     if ( 0 != inc )inc -> addListener  ( this , IncidentType::BeginEvent ) ;
     m_ppsvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;
     return sc;
}
StatusCode ParticleVeto::finalize(){
  IIncidentSvc* inc = incSvc() ;
  if ( 0 != inc ) { inc -> removeListener  ( this ) ; }
  return GaudiTool::finalize();
}


bool ParticleVeto::foundOverlap(const LHCb::Particle* particle, std::vector<std::string> containers,int mode){
  if( NULL == particle)return false;
  if( containers.empty())return false;
  for( std::vector<std::string>::iterator i=containers.begin();i!=containers.end();++i){
    if( foundOverlap( particle, *i,mode) )return true;
  }
  return false;
}

bool ParticleVeto::foundOverlap(const LHCb::Particle* particle, std::string container,int mode)
{
  if( NULL == particle)return false;
  if( container != m_cont )
  {
    m_cont = container;
    if ( !exist<LHCb::Particle::Range>( m_cont )){
      counter("Container " + container + " not found")+=1;
      return false;
    }
    m_parts = get<LHCb::Particle::Range> ( m_cont);
  }
  int i1=container.find("Phys/");
  int i2=container.find("/Particles");
  if( i1 != (int)std::string::npos)i1 +=5;
  if( i2 != (int)std::string::npos)i2 -=12;
  std::string cont = container.substr(i1,i2);

  bool found = false;
  for( LHCb::Particle::Range::const_iterator i = m_parts.begin();m_parts.end() != i;++i){
    if( NULL == *i )return false;
    if( (mode&0x1)!=0 && (*i)->particleID().pid() != particle->particleID().pid() )continue;// same PID is requested
    if( (mode&0x2)!=0 && *i == particle )continue;// no overlaping with fully identical object is requested

    found = foundOverlap( getTree(particle) , getTree(*i) , mode);
    if( found ){
      if (msgLevel(MSG::DEBUG))debug() << "Overlap found input particle (" << particle->particleID().pid() 
                                       << ") with particle (" << (*i)->particleID().pid() << ") from "<< cont <<endmsg;
      break;
    } 
  }
  const LHCb::ParticleProperty* pp=m_ppsvc->find(particle->particleID());
  std::string tag = (pp) ? pp->name() : "pid="+Gaudi::Utils::toString( particle->particleID().pid());
  tag += ( (mode&0x2)==0 ) ? " overlap w/ " : " overlap w/ other "+ tag;
  counter(tag+" ("+cont+")")+=(int)found;
  return found; 
}


bool ParticleVeto::foundOverlap(std::vector<const LHCb::Particle*> vp1, std::vector<const LHCb::Particle*> vp2, int mode){
  // vp1/vp2 MUST BE VECTORS OF BASIC PARTICLES (from getTree)


  if( (mode & 0x2) != 0 && vp1.size() == vp2.size()){
    // no overlap with identical object is requested
    bool ok=true;
    int k=-1;
    for(std::vector<const LHCb::Particle*>::iterator i=vp1.begin();i!=vp1.end();++i){
      k++;
      if( *i == NULL)continue;
      if(vp2[k] == NULL)continue;
      const LHCb::ProtoParticle* pp1 = (*i)->proto();
      const LHCb::ProtoParticle* pp2 = vp2[k]->proto();  
      if( pp1 == NULL || pp2 == NULL )continue;
      if( pp1 != pp2 ){ok=false;break;}
    }
    if( ok ) return false;  
  }  

  for( std::vector<const LHCb::Particle*>::const_iterator i = vp1.begin();vp1.end() != i;++i){    
    for( std::vector<const LHCb::Particle*>::const_iterator j = vp2.begin();vp2.end() != j;++j){
      if( foundOverlap( *i , *j) )return true; // DO NOT PROPAGATE 'mode' here
    }
  }
  return false;
}

bool ParticleVeto::foundOverlap(const LHCb::Particle* p1, const LHCb::Particle* p2,int mode){

  // composite particles
  if( ! p1->proto() || ! p2->proto() ){
    return foundOverlap( getTree( p1) , getTree( p2 ), mode ); // warning : infinite loop if getTree does not provide a vector of basic particles
  }  

  // basic particles
  const LHCb::ProtoParticle* pp1 = p1->proto();
  const LHCb::ProtoParticle* pp2 = p2->proto();

  if( (mode & 0x1) !=0  && p1->particleID().pid()!=p2->particleID().pid())return false;// same PID is requested
  if( (mode & 0x2) !=0  && p1 == p2 )return false;// no overlap for identical particle (mode useless here (basic particles))

  if( isPureNeutralCalo(p1) && isPureNeutralCalo(p2) ){
    // check neutral calo overlap
    int id1 = (int)pp1->info(LHCb::ProtoParticle::CaloNeutralID,0);
    int id2 = (int)pp2->info(LHCb::ProtoParticle::CaloNeutralID,0);
    return ( id1 == id2 && 0 != id1  ) ? true : false;
  }else if( !isPureNeutralCalo(p1) && !isPureNeutralCalo(p2) ){
    // check charged proto overlap
    return ( pp1 == pp2 ) ? true : false;
  }else if ( isPureNeutralCalo(p1) && !isPureNeutralCalo(p2) ){
    // mixed configuration - use-case  :  check photon is not used as electron bremStrahlung
    if( (pp2->calo()).empty() )return false;
    int id1 = (int)pp1->info(LHCb::ProtoParticle::CaloNeutralID,0);
    int id2 = (int)pp2->info(LHCb::ProtoParticle::CaloNeutralID,0);
    return ( id1 == id2 && 0 != id1  ) ? true : false;
  }
  // other configurations have no overlap (incl. electrons sharing a brem photon ! or electron tracks pointing the same ecal cluster)
  
  return false;
}


std::vector<const LHCb::Particle*> ParticleVeto::getTree(const LHCb::Particle* P){
  std::vector<const LHCb::Particle*> tree;
  if( P->proto() ){
    tree.push_back( P );
    return tree;
  }else{
    const LHCb::Particle::ConstVector& daughters = P->daughtersVector();
    for( LHCb::Particle::ConstVector::const_iterator d=daughters.begin();daughters.end()!=d;++d){
      if( (*d)->proto() )tree.push_back( *d );
      else{        
        const std::vector<const LHCb::Particle*>& t=getTree( *d );
        for (std::vector<const LHCb::Particle*>::const_iterator i=t.begin();t.end()!=i;++i){
          tree.push_back( *i );
        } 
      }
    }
  }
  return tree;
}


