// $Id: OrderedTaggingTool.cpp,v 1.2 2002-05-24 21:14:49 gcorti Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/MsgStream.h" 

// local
#include "OrderedTaggingTool.h"

//-----------------------------------------------------------------------------
// Implementation file for class : OrderedTaggingTool
//
// 24/04/2002 : Olivier Dormond
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<OrderedTaggingTool>          s_factory ;
const        IToolFactory& OrderedTaggingToolFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OrderedTaggingTool::OrderedTaggingTool( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
  : AlgTool ( type, name , parent ), m_taggingTools_names(0), m_taggingTools(0)
{
  declareInterface<IFlavourTaggingTool>(this);

  declareProperty( "TaggingTools", m_taggingTools_names );
}

StatusCode OrderedTaggingTool::initialize()
{
  MsgStream log(msgSvc(), name());

  //IToolSvc* toolsvc = NULL;
  //if( serviceLocator() )
  //  serviceLocator()->service("ToolSvc", toolsvc);
  //if( !toolsvc )
  //{
  //  log << MSG::FATAL << "Unable to locate Tool Service" << endreq;
  //  return StatusCode::FAILURE;
  //}
  StatusCode sc;
  std::vector<std::string>::iterator ti;
  for( ti=m_taggingTools_names.begin(); ti!=m_taggingTools_names.end(); ti++ )
  {
    IFlavourTaggingTool *tool;
    std::string::size_type type_len = ti->find('/');
    std::string type(*ti,0,type_len);
    std::string name;
    if( type_len < ti->length() )
      name = ti->substr(type_len+1);
    log << MSG::DEBUG << "Retrieving tool named '" << name << "' of type '"
        << type << "'" << endreq;
    sc = toolSvc()->retrieveTool(type, name, tool, this);
    if( !sc.isSuccess() )
    {
      log << MSG::FATAL << "Unable to find the requested tagging tool '"
          << *ti << "'" << endreq;
      return StatusCode::FAILURE;
    }
    m_taggingTools.push_back(tool);
  }
  return StatusCode::SUCCESS;
}

OrderedTaggingTool::~OrderedTaggingTool()
{}

void OrderedTaggingTool::tagThisB( const Particle &theB,
                                   const Particles &theEvent,
                                   const Vertex &thePrimVtx,
                                   FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  std::vector<std::string>::iterator tni;
  std::vector<IFlavourTaggingTool*>::iterator ti;
  for( ti=m_taggingTools.begin(), tni=m_taggingTools_names.begin();
       ti!=m_taggingTools.end() && (theTag.decision()==FlavourTag::none);
       ti++, tni++ )
  {
    log << MSG::DEBUG << "Calling 'tagThisB' on '" << *tni << "'" << endreq;
    (*ti)->tagThisB( theB, theEvent, thePrimVtx, theTag );
    if( theTag.decision() != FlavourTag::none )
      log << MSG::DEBUG << *tni << " result: "
          << theTag.tagger()->particleID().pid() << endreq;
  }
}

void OrderedTaggingTool::tagFromList( const Particle &theB,
                                      const ParticleVector &theCandidates,
                                      const Vertex &thePrimVtx,
                                      FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  std::vector<std::string>::iterator tni;
  std::vector<IFlavourTaggingTool*>::iterator ti;
  for( ti=m_taggingTools.begin(), tni=m_taggingTools_names.begin();
       ti!=m_taggingTools.end() && (theTag.decision()==FlavourTag::none);
       ti++, tni++ )
  {
    log << MSG::DEBUG << "Calling 'tagThisB' on '" << *tni << "'" << endreq;
    (*ti)->tagFromList( theB, theCandidates, thePrimVtx, theTag );
  }
}

void OrderedTaggingTool::tagExcludingFromList(const Particle &theB,
                                              const Particles &theEvent,
                                              const ParticleVector &theExcluded,
                                              const Vertex &thePrimVtx,
                                              FlavourTag &theTag )
{
  MsgStream log(msgSvc(), name());

  std::vector<std::string>::iterator tni;
  std::vector<IFlavourTaggingTool*>::iterator ti;
  for( ti=m_taggingTools.begin(), tni=m_taggingTools_names.begin();
       ti!=m_taggingTools.end() && (theTag.decision()==FlavourTag::none);
       ti++, tni++ )
  {
    log << MSG::DEBUG << "Calling 'tagThisB' on '" << *tni << "'" << endreq;
    (*ti)->tagExcludingFromList( theB,theEvent,theExcluded,thePrimVtx,theTag );
  }
}

//=============================================================================
