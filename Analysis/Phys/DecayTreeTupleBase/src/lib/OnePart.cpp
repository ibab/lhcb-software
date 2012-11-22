// $Id: OnePart.cpp,v 1.5 2009-01-20 18:31:30 pkoppenb Exp $
// Include files

#include "Kernel/IParticleTupleTool.h"
#include "Kernel/IMCParticleTupleTool.h"
// local
#include "DecayTreeTupleBase/OnePart.h"

using namespace LHCb;
//-----------------------------------------------------------------------------
// Implementation file for class : OnePart
//
// 2009-01-20 : Patrick Koppenburg
//-----------------------------------------------------------------------------
Decays::OnePart::OnePart( const std::string& realname,
                          const std::string& head )
  : m_head( head )
  , m_realname(realname)
  , m_mother(0)
{
}
// -----------------------------------------------------
Decays::OnePart::~OnePart(){}
// -----------------------------------------------------
std::string Decays::OnePart::headName(){return m_head;}
void Decays::OnePart::headName( const std::string& h ){m_head=h;}
std::string Decays::OnePart::getRealName(){return m_realname;}
const std::string& Decays::OnePart::getRealName() const{return m_realname;}
std::string Decays::OnePart::info() const{return m_realname + " (" + m_head + ")";}
void Decays::OnePart::setMother( const OnePart* mother ){m_mother = mother;}
const Decays::OnePart* Decays::OnePart::getMother() const{return m_mother;}
void Decays::OnePart::addDaughter( OnePart* d ){m_daughters.push_back( d );}
int Decays::OnePart::depth() const{
  if( m_mother ) return m_mother->depth()+1;
  return 0;
}
// -----------------------------------------------------
void Decays::OnePart::addTool( IParticleTupleTool* tool ){m_tools.push_back( tool );}
void Decays::OnePart::addTool( IMCParticleTupleTool* tool ){m_mctools.push_back( tool );}
std::vector< IParticleTupleTool* >& Decays::OnePart::tools( ){return m_tools;}
std::vector< IMCParticleTupleTool* >& Decays::OnePart::mctools( ){return m_mctools;}
void Decays::OnePart::clearTools()
{
  m_tools.clear();
  m_mctools.clear();
}
std::vector<std::string> Decays::OnePart::toolList() const{
  std::vector<std::string> v;
  v.reserve( m_tools.size() );
  for( std::vector<IParticleTupleTool*>::const_iterator it=m_tools.begin();
       m_tools.end()!=it; ++ it )
  {
    v.push_back( (*it)->type() );
  }
  return v;
}
std::vector<std::string> Decays::OnePart::mctoolList() const{
  std::vector<std::string> v;
  v.reserve( m_mctools.size() );
  for( std::vector<IMCParticleTupleTool*>::const_iterator it=m_mctools.begin();
       m_mctools.end()!=it; ++ it )
  {
    v.push_back( (*it)->type() );
  }
  return v;
}
// -----------------------------------------------------
void Decays::OnePart::printStructure( MsgStream & os, bool verbose ) const
{
  const int dd = depth();
  std::string i;
  if( dd ) i = std::string( 3*dd, ' ' );
  i.append( info() );
  if ( !verbose ) { os << i << endmsg; }
  else 
  {
    std::vector<std::string> l = toolList();
    os << "  " << i << std::setw( 30-i.size() ) << " : ";
    os << Decays::join( l.begin(), l.end(), ", ", "none" ) << endmsg;
  }
  if ( m_daughters.empty() ) return;
  for( std::vector<const OnePart*>::const_iterator cit = m_daughters.begin();
       cit != m_daughters.end(); ++cit )
  {
    (*cit)->printStructure( os, verbose );
  }
}
