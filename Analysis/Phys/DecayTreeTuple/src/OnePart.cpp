// $Id: OnePart.cpp,v 1.1 2009-01-20 09:19:40 pkoppenb Exp $
// Include files

#include "Kernel/IParticleTupleTool.h"
#include "Event/Particle.h"
#include "DecayTreeTuple.h" /// @todo Base
// local
#include "OnePart.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : OnePart
//
// 2009-01-20 : Patrick Koppenburg
//-----------------------------------------------------------------------------
Decays::OnePart::OnePart( DecayTreeTuple* parent
                          , const Particle& me
                          , const std::string& head )
  : m_head( head )
    , m_parent( parent )
  , m_mother(0)
{
  if( !m_parent ){
    // should not happen                                                                                      
    std::cout << "Error: parent interface not set!" << std::endl;
    return;
  }
  m_realname = m_parent->ppSvc()->find ( me.particleID() )->particle();
}
// -----------------------------------------------------
Decays::OnePart::~OnePart(){}
// -----------------------------------------------------
std::string Decays::OnePart::headName()
{
  return m_head;
}
void Decays::OnePart::headName( const std::string& h )
{
  m_head=h;
};
std::string Decays::OnePart::getRealName()
{
  return m_realname;
};
const std::string& Decays::OnePart::getRealName() const 
{
  return m_realname;
};
std::string Decays::OnePart::info() const 
{
  return m_realname + " (" + m_head + ")";
};
void Decays::OnePart::setMother( const OnePart* mother )
{
  m_mother = mother;
}
const Decays::OnePart* Decays::OnePart::getMother() const 
{
  return m_mother;
};
DecayTreeTuple* Decays::OnePart::parent()
{
  return m_parent;
};
void Decays::OnePart::addDaughter( OnePart* d )
{
  m_daughters.push_back( d );
}
int Decays::OnePart::depth() const 
{
  if( m_mother ) return m_mother->depth()+1;
  return 0;
}
// -----------------------------------------------------
void Decays::OnePart::addTool( IParticleTupleTool* tool )
{
  m_tools.push_back( tool );
};
std::vector< IParticleTupleTool* >& Decays::OnePart::tools( )
{
  return m_tools;
};
void Decays::OnePart::clearTools()
{
  m_tools.clear();
};
bool Decays::OnePart::fill( Tuples::Tuple& tuple
                                    , const Particle* mother
                                    , const Particle* pp )
{
  bool test = true;
  for( std::vector< IParticleTupleTool* >::iterator it = m_tools.begin();
       m_tools.end()!=it; ++it ){
    bool localTest = (*it)->fill( mother, pp, headName(), tuple );
    test &= localTest;
    if( localTest ){    }
    else {
      m_parent->Warning("Tool '" + (*it)->type() + "' acting on particle '"
                        + headName() + "' returned a failure status." );
    }
  }
  return test;
}
std::vector<std::string> Decays::OnePart::toolList() const{
  std::vector<std::string> v;
  v.reserve( m_tools.size() );
  for( std::vector<IParticleTupleTool*>::const_iterator it=m_tools.begin();
       m_tools.end()!=it; ++ it ){
    v.push_back( (*it)->type() );
  }
  return v;
}
// -----------------------------------------------------   
void Decays::OnePart::printStructure( std::ostream& os, bool verbose ) const{
  int dd = depth();
  std::string i;
  if( dd ) i = std::string( 3*dd, ' ' );
  i.append( info() );
  if( !verbose ) os << i << "\n";
  else {
    std::vector<std::string> l = toolList();
    os << "|" << i << std::setw( 20-i.size() ) <<  " " << ":";
    os << Decays::join( l.begin(), l.end(), ", ", "none" ) << "\n";
  }
  if( m_daughters.empty() ) return;
  for( std::vector<const OnePart*>::const_iterator cit=m_daughters.begin();
       cit != m_daughters.end(); ++cit ){
    (*cit)->printStructure( os, verbose );
  }
}


