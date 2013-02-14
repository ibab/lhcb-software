// $Id: TupleToolJetTag.cpp,v 1.1 2011-05-16 14:14:14 aphan Exp $

//-----------------------------------------------------------------------------
//
// Implementation file for class TupleToolJetTag
//
// Runs one jet tagging algorithm and writes the value out to the tuple
//
//-----------------------------------------------------------------------------

//=============================================================================
// Setup
#include "Event/Particle.h"
#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "GaudiKernel/ToolFactory.h"
#include "Kernel/IJets2Jets.h"

#include "TupleToolJetTag.h"

DECLARE_TOOL_FACTORY(TupleToolJetTag);


//=============================================================================
// Constructor
TupleToolJetTag::TupleToolJetTag(const std::string& type,
				                        const std::string& name,
				                        const IInterface* parent)
  : TupleToolJetsBase(type, name , parent)
  , m_TagTool(0)
{
  declareInterface<IParticleTupleTool>(this);

  declareProperty("tagToolName",
                  m_tagToolName,
                  "name of jet tagger");

  declareProperty("tagToolLabel",
                  m_tagToolLabel,
                  "label of jet tagger for tuple");

  declareProperty( "VerboseTag", m_verboseTag=false );


}


//=============================================================================
// Initalise
StatusCode TupleToolJetTag::initialize()
{
  if(!TupleToolBase::initialize())
    return StatusCode::FAILURE;

  m_TagTool = tool<IJetTagTool>(m_tagToolName,m_tagToolName, this);
  if(m_TagTool == 0)
    return Error("Couldn't get requested jet tag tool", StatusCode::SUCCESS);

  return StatusCode::SUCCESS;
}


//=============================================================================
// Execute
StatusCode TupleToolJetTag::fill(const LHCb::Particle*,
				 const LHCb::Particle* jet,
				 const std::string& head,
				 Tuples::Tuple& tuple)
{
  const std::string prefix = fullName(head);
  bool result = true;
  if(jet)
    {
      // Run jet tag tool
      std::map<std::string,double> property;
      
      m_TagTool->calculateJetProperty(jet, property);
      
      if(property.empty())
	if(msgLevel(MSG::ERROR)) 	debug() << "Write tag to ntuple --- not found "<< endmsg;
      
      
      typedef std::map<std::string, double>::iterator it_type;
      for( std::map<std::string, double>::iterator  it = property.begin();  it != property.end(); it++) {
	if(it->first != "extraInfo")
	  result &= (tuple)->column(prefix + "_" + m_tagToolLabel +"_" + (it->first) , (it->second));
	
	
	
      }
    }
  
  return StatusCode(result);
}

