// $Id: RichToolRegistry.cpp,v 1.2 2003-07-03 09:15:07 jonesc Exp $

// local
#include "RichToolRegistry.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichToolRegistry
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichToolRegistry>          s_factory ;
const        IToolFactory& RichToolRegistryFactory = s_factory ;

// Standard constructor
RichToolRegistry::RichToolRegistry( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : AlgTool( type, name, parent ) {

  declareInterface<IRichToolRegistry>(this);

  declareProperty( "Tools", m_names );

}

StatusCode RichToolRegistry::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // setup tool registry
  for ( ToolList::iterator it = m_names.begin();
        it != m_names.end(); ++it ) {
    std::string name = (*it);
    std::string type = (*it);
    int slash = (*it).find_first_of( "/" );
    if ( slash > 0 ) {
      type = (*it).substr( 0, slash );
      name = (*it).substr( slash+1 );
    }
    m_tools[name] = RichToolPair(type,NULL);
    m_refC[name] = 0;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichToolRegistry::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release all acquired tools
  for ( RichToolMap::iterator it = m_tools.begin();
        it != m_tools.end();
        ++it ) { releaseTool( ((*it).second).second ); }

  return StatusCode::SUCCESS;
}

IAlgTool * RichToolRegistry::acquireTool( std::string toolName ) {

  RichToolPair& instance = m_tools[toolName];
  IAlgTool *& pTool = instance.second;
  if ( !pTool ) {
    if ( "" == instance.first ) instance.first = toolName;
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Acquiring tool '"+toolName
        << "' of type '"+instance.first << "'" << endreq;
    if ( !toolSvc()->retrieveTool(instance.first, toolName, pTool) ) {
      pTool = NULL;
      throw GaudiException( "Unable to retrieve tool '" + toolName +
                            "' of type '" + instance.first + "'" ,
                            name(), StatusCode::FAILURE );
    }
  }
  ++m_refC[toolName];

  return pTool;
}

void RichToolRegistry::releaseTool( std::string toolName ) {

  --m_refC[toolName];
  if ( m_refC[toolName] <= 0 ) {
    m_refC[toolName] = 0;
    MsgStream msg( msgSvc(), name() );
    msg << MSG::DEBUG << "Releasing tool '"+toolName
        << "' of type '"+(m_tools[toolName]).first << "'" << endreq;
    releaseTool( (m_tools[toolName]).second );
  }

}

