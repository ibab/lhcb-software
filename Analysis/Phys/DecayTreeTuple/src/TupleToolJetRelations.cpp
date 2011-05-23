#include "TupleToolJetRelations.h"
#include "GaudiKernel/ToolFactory.h"
#include <math.h>
#include <boost/bind.hpp>
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolJetRelations
// Autor: Albert Bursche
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( TupleToolJetRelations )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolJetRelations::TupleToolJetRelations( const std::string& type,
					      const std::string& name,
					      const IInterface* parent )
: TupleToolJetsBase ( type, name , parent )
   
{
  declareInterface<IParticleTupleTool>(this);
  declareProperty( "RelationsContainer",m_RelationsContainer = "",
                   "Where to find the table to get information from." );
  declareProperty( "InvertTable",m_Invert = false,
                   "Invert relation table" );
  declareProperty( "ReverseMatching",m_Reverse = false,
                   "Does best matching correspond to highes or lowest weight?" );
  declareProperty( "MaxMatches",m_MaxMatches=1,
                   "How many matches in the tuple?" );

}

//=============================================================================
StatusCode TupleToolJetRelations::initialize()
{
  if( ! TupleToolBase::initialize() )
    return StatusCode::FAILURE;
  return StatusCode::SUCCESS ;
}
StatusCode TupleToolJetRelations::fill( const LHCb::Particle* /* top */,
                                  const LHCb::Particle* p,
                                  const std::string& head ,
                                  Tuples::Tuple& tuple )
{
  m_tuple = &tuple;
  bool test = true;
  m_p = p;
  m_head = m_extraName+head; 
  if(exist<IJets2Jets::Table>(m_RelationsContainer))
    {
      m_Table = get<IJets2Jets::Table>(m_RelationsContainer);
      if (m_Invert)
	m_Table = new IJets2Jets::Table(*m_Table,1); 
      IJets2Jets::Table::Range r = m_Table->i_relations(p);
      (*m_tuple)->column( m_head+"_numMatches",r.size());
      unsigned int numMatches = 0;
      if (r.size())
	{
	  if(m_Reverse)
	    {
	      IJets2Jets::Table::Range::reverse_iterator it = r.rbegin();
	      for (;numMatches<m_MaxMatches;numMatches++)
		if(it!=r.rend())
		  fill(numMatches,&*it++);
		else
		  fill(numMatches,0);
	    }
	  else
	    {
	      IJets2Jets::Table::Range::iterator it = r.begin();
	      for (;numMatches<m_MaxMatches;numMatches++)
		if(it!=r.end())
		  fill(numMatches,&*it++); 
		else
		  fill(numMatches,0);
	    }
	}
      else 
	for (;numMatches<m_MaxMatches;numMatches++)
	  fill(numMatches,0);
      if (m_Invert)
	delete m_Table; 

    }
  return StatusCode(test);
}

double TupleToolJetRelations::DeltaR(const LHCb::Particle& jet,const LHCb::Particle& jet2)
{  //Delta R
  double dphi = jet.momentum().Phi()- jet2.momentum().Phi();
  while (fabs(dphi)>2*M_PI)
    dphi+=(dphi>0)?-2*M_PI:2*M_PI;
  return sqrt(pow(jet.momentum().Eta()- jet2.momentum().Eta(),2)+pow(dphi,2));
}
#define SAVEPOINT(POINT,FUN)                    \
  (POINT?POINT->FUN:-1.0)
bool TupleToolJetRelations::fill(unsigned int i,const IJets2Jets::Table::Entry *e)
{
  bool test =  true;
  std::string I = boost::lexical_cast<std::string>(i);
  if ( msgLevel(MSG::DEBUG) ) 
    debug() << "Tuple: "<< m_tuple << "\t"+ I +" \tRelation" << e << endmsg;

  test &=(*m_tuple)->column(m_head+"Relation"+I+"weight",SAVEPOINT(e,weight()));
  test &=(*m_tuple)->column(m_head+"Relation"+I+"DeltaR",e==0?-1:DeltaR((*e->from()),*(e->to())));
  if (e&&e->to())
    WriteJetToTuple(e->to(),m_head+"Relation"+I+"Jet");
  else 
    WriteJetToTuple(0,m_head+"Relation"+I+"Jet");
      

  
  return test;
}
