// $Id: TupleToolSubMass.cpp,v 1.12 2010-04-23 12:01:26 jhe Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
// local
#include "TupleToolSubMass.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include <boost/algorithm/string.hpp>
//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolSubMass
//
// 2011-02-10 Patrick Koppenburg
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolSubMass )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolSubMass::TupleToolSubMass( const std::string& type,
                                      const std::string& name,
                                      const IInterface* parent )
  : TupleToolBase ( type, name , parent ){
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "SetMax", m_max=0);
  declareProperty( "Substitution", m_subst);
  declareProperty( "DoubleSubstitution", m_subst2);
}

//=============================================================================

StatusCode TupleToolSubMass::initialize()
{
  const StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;

  // get property service
  m_ppsvc = svc<LHCb::IParticlePropertySvc>("LHCb::ParticlePropertySvc",true) ;

  // consistentcy checks
  for( std::vector<std::string>::const_iterator is = m_subst.begin(); m_subst.end() !=is ; ++is){
    const std::pair<std::string,std::string> fromto=parseSubst(*is);
    const std::string from = fromto.first;
    const std::string to   = fromto.second;
    if( NULL ==  property(from) )return Warning("Cannot substitute from unknown particle " + from, StatusCode::FAILURE);
    if( NULL ==  property(to)   )return Warning("Cannot substitute to unknown particle " + to, StatusCode::FAILURE);
  }
  for( std::vector<std::string>::const_iterator is = m_subst2.begin(); m_subst2.end() !=is ; ++is){
    const std::pair<std::string,std::string> fromto=parseSubst(*is);
    const std::string from = fromto.first;
    const std::string to   = fromto.second;
    if( parseID(from).first == "NONE" )
      return Warning("Cannot substitute from unknown particles pair " + from, StatusCode::FAILURE);
    if( parseID(to).first == "NONE" )
      return Warning("Cannot substitute to unknown particles pair " + from, StatusCode::FAILURE);
  }
  return sc ;
}

//=============================================================================
StatusCode TupleToolSubMass::fill( const LHCb::Particle*
                                   , const LHCb::Particle* P
                                   , const std::string& head
                                   , Tuples::Tuple& tuple ){
  const std::string prefix=fullName(head);
  bool fill = true;
  if( !P ) return StatusCode::FAILURE;
  if( P->isBasicParticle() ) return StatusCode::SUCCESS;

  // get the final state tree
  m_ancestor = P; // cache the ancestor for cc-independant tree-sorting
  std::vector<const LHCb::Particle*> tree=getTree( P );

  //== check sorting
  std::string decay = property( P->particleID())->name() + " -> [";
  for( unsigned int pos =0; pos < tree.size() ; pos++){
    if( pos >= tree.size())return Warning("Unexpected error",StatusCode::FAILURE);
    if(msgLevel(MSG::DEBUG))debug() << " Ancestor : " << P->particleID().pid() << " daughter " << pos << " : " << tree[pos]->particleID().pid() << endmsg;
    std::string sep = ( pos != 0) ? "," : "";
    decay += sep +  property( tree[pos]->particleID() )->name();
  }
  decay += "]";
  counter(decay)+=1;
  

  //=== Get all combinaisons
  unsigned int nfs= tree.size();;
  unsigned int max = ( m_max > 1) ? m_max : nfs;
  if( max > nfs)max=nfs;

  for(unsigned int level=2;level<=max;++level){
    for( unsigned int d=0; d < max-level+1; ++d){
      std::vector<int> v;
      std::vector<std::vector<int> > vv;
      v.push_back( d );
      vv.push_back(v);    
      // produce the combinations tables
      std::vector<std::vector<int> > out=combine( vv , level, nfs);
      // loop over combinations tables
      for( std::vector<std::vector<int> >::const_iterator ivv = out.begin(); ivv != out.end(); ++ivv){
        const std::vector<int> map = *ivv;

        //====  Mass for the current combination (no substition)
        std::pair<std::string,double> fMass = getMass( tree, map );

        if( fMass.first != "NONE")fill &= tuple->column( prefix + "_M"+fMass.first, fMass.second  );

        //==== Try single substitution when requested
        for( std::vector<std::string>::const_iterator is = m_subst.begin(); m_subst.end() !=is ; ++is){
          const std::pair<std::string,std::string> fromto=parseSubst(*is);
          const std::string from = fromto.first;
          const std::string to   = fromto.second;
          unsigned int fromID = property(from)->pid().abspid() ;
          double toMass = property(to)->mass();
          for( std::vector<int>::const_iterator im = map.begin() ; map.end() != im ; ++im){
            int pos = *im;
            const LHCb::Particle* part = tree[ pos ];
            if( part->particleID().abspid() == fromID){
              std::pair<std::string,double> sMass = getMass( tree, map, pos , toMass );
              if( sMass.first != "NONE")fill &= tuple->column( prefix + "_M" 
                                                               + sMass.first + "_" + getFlag(from,to,pos), sMass.second  );
            }
          }
        }
        //==== Try double substitutions when requested
        for( std::vector<std::string>::const_iterator iss = m_subst2.begin(); m_subst2.end() != iss ; ++iss){
          const std::pair<std::string,std::string> fromto=parseSubst(*iss);
          const std::string from = fromto.first;
          const std::string to   = fromto.second;
          const std::string from1 = parseID(from).first;
          const std::string from2 = parseID(from).second;
          const std::string to1 = parseID(to).first;
          const std::string to2 = parseID(to).second;
          int fromID1 = property(from1)->pid().pid() ;
          int fromID2 = property(from2)->pid().pid() ;
          double toMass1 = property(to1)->mass();
          double toMass2 = property(to2)->mass();
          unsigned int size=map.size();
          for( unsigned int i1 = 0; i1 < size ; i1++){
            for( unsigned int i2 = i1+1; i2 < size ; i2++){
              int pos1 = map[i1];
              int pos2 = map[i2];
              const LHCb::Particle* part1 = tree[ pos1 ];
              const LHCb::Particle* part2 = tree[ pos2 ];
              int pid1  = part1->particleID().pid();
              int apid1 = (part1->charge() == 0 ) ? pid1 : -pid1 ;
              int pid2  = part2->particleID().pid();
              int apid2 = (part2->charge() == 0 ) ? pid2 : -pid2 ;
              if( ( pid1 == fromID1 && pid2 == fromID2 ) || ( apid1 == fromID1 && apid2 == fromID2 ) ){
                std::pair<std::string,double> sMass = getMass( tree, map, pos1 , toMass1, pos2, toMass2 );
                if( sMass.first != "NONE"){
                  fill &= tuple->column( prefix + "_M"+ sMass.first + "_" + getFlag(from1,to1,pos1,from2,to2,pos2),sMass.second);
                }
              }else if( ( pid1 == fromID2 && pid2 == fromID1 ) || ( apid2 == fromID1 && apid1 == fromID2 )){
                std::pair<std::string,double> sMass = getMass( tree, map, pos1 , toMass2, pos2, toMass1 );
                if( sMass.first != "NONE"){
                  fill &= tuple->column( prefix + "_M"+ sMass.first + "_" + getFlag(from1,to1,pos1,from2,to2,pos2),sMass.second);
                } 
              }
            }   
          }
        }        
      }  
    }
  }  
  return StatusCode(fill); 
} 

// ------ parse substitution string
std::pair<std::string,std::string> TupleToolSubMass::parseSubst(std::string subst){
  int i = subst.find_last_of("=>");
  std::string from = subst.substr(0,i-1);
  std::string to   = subst.substr(i+1,std::string::npos);
  boost::erase_all(from," ");
  boost::erase_all(to," ");
  return std::make_pair(from,to);
}


//------- particleID parser for double substitution map
std::pair<std::string,std::string> TupleToolSubMass::parseID(std::string PID){
  // syntax a/b
  if( PID.find_last_of("/") != std::string::npos){
    int i = PID.find_last_of("/");
    std::string pid1 = PID.substr(0,i);
    std::string pid2 = PID.substr(i+1,std::string::npos);
    if( property(pid1) != NULL && property(pid2) != NULL)return std::make_pair(property(pid1)->name(),property(pid2)->name());
  }else{
    // try another syntax : ab
    for( unsigned int i=1 ; i<PID.size();++i){
      std::string pid1 = PID.substr(0,i);
      std::string pid2 = PID.substr(i,std::string::npos);
      if( property(pid1) != NULL && property(pid2) != NULL)return std::make_pair(property(pid1)->name(),property(pid2)->name());
    }
  }
  // parsing failed
  return std::make_pair("NONE","NONE");
}

//------- produce the ntuple entry flag  : 
std::string TupleToolSubMass::getFlag(std::string from,std::string to,int pos,std::string from2,std::string to2,int pos2){
  boost::replace_all(from,"+",""); 
  boost::replace_all(from,"-",""); 
  boost::replace_all(to,"+",""); 
  boost::replace_all(to,"-",""); 
  boost::replace_all(from2,"+",""); 
  boost::replace_all(from2,"-",""); 
  boost::replace_all(to2,"+",""); 
  boost::replace_all(to2,"-",""); 
  if( pos2 < 0)return "Subst"+Gaudi::Utils::toString(pos)+"_"+from+"2"+to;
  return "Subst"+Gaudi::Utils::toString(pos)+Gaudi::Utils::toString(pos2)+"_"+from+from2+"2"+to+to2;
}

//------- compute momentum (possibly with PID substitution)
Gaudi::LorentzVector TupleToolSubMass::sMomentum(const LHCb::Particle* part , double sMass) {
  Gaudi::LorentzVector momentum = part ->momentum();
  if( isPureNeutralCalo( part ) ){// photon (E is measured) - use-case :  gamma <->pi0 substitution
    if( sMass < momentum.E() ){
      double P = sqrt( momentum.E()*momentum.E() - sMass*sMass); 
      double px = momentum.px()/momentum.P()*P; 
      double py = momentum.py()/momentum.P()*P; 
      double pz = momentum.pz()/momentum.P()*P; 
      momentum.SetPx(px); 
      momentum.SetPy(py); 
      momentum.SetPz(pz); 
    }
  }else{// tracks (p is measured) 
    double E = sqrt(momentum.P()*momentum.P() + sMass*sMass); 
    momentum.SetE( E ); 
  }
  return momentum; 
} 

//-------- compute the combination mass (possibly with PID substitution(s))
std::pair<std::string,double> TupleToolSubMass::getMass(std::vector<const LHCb::Particle*> tree, const std::vector<int> map,
                                                        int sPos, double sMass,int sPos2, double sMass2){
  Gaudi::LorentzVector sum;
  std::string flag;
  bool doSub1 = false;
  bool doSub2 = false;
  for( std::vector<int>::const_iterator iv = map.begin(); iv != map.end(); ++iv){
    int pos = *iv;
    const LHCb::Particle* part = tree[ pos ];
    
    if( pos == sPos ){
      doSub1 = true;
      sum += sMomentum(part,sMass);
    }else if( pos == sPos2 ){
      doSub2 = true;
      sum += sMomentum(part,sMass2);
    }else
      sum += part->momentum();
    flag += Gaudi::Utils::toString( pos );
  }
  if( (!doSub1 && sPos >=0) || (!doSub2 && sPos2 >=0))return std::make_pair("NONE",0.);
  return std::make_pair(flag,sum.M());
}



//-------- get sorted tree (cc-independant)
std::vector<const LHCb::Particle*> TupleToolSubMass::getTree( const LHCb::Particle* P ){
  std::vector<const LHCb::Particle*> tree;
  if( P->isBasicParticle() ){
    tree.push_back( P );
  }else{
    SmartRefVector<LHCb::Particle> daughters = P->daughters(); // local copy to sort
    std::stable_sort   ( daughters.begin(),daughters.end(),SortDaughtersByPID(m_ancestor,m_ppsvc));
    for (SmartRefVector<LHCb::Particle>::const_iterator idau = daughters.begin(); idau!= daughters.end();++idau){
      std::vector<const LHCb::Particle*> temp=getTree( *idau );
      for( std::vector<const LHCb::Particle*>::iterator dd=temp.begin();temp.end() !=dd; ++dd){
        tree.push_back( *dd );
      }
    }
  }
  return tree;
}

//-------- get the combinations tables
std::vector<std::vector<int> > TupleToolSubMass::combine( std::vector<std::vector<int> >  vec, unsigned int level, int nfs){
  if( (vec.back()).size() == level )return vec;  
  std::vector<std::vector<int> > cc ; // local vector
  for( std::vector<std::vector<int> >::iterator iv = vec.begin(); iv != vec.end(); ++iv){
    if( iv->size() == level )continue;
    int min=iv->back();
    for(int i=min+1 ; i<nfs;++i){
      std::vector<int> c = *iv;
      c.push_back(i);
      cc.push_back( c );
    };
  }
  return combine( cc, level, nfs);
}
