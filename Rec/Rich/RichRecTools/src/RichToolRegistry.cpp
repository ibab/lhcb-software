// $Id: RichToolRegistry.cpp,v 1.4 2003-07-06 09:25:23 jonesc Exp $

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
    m_tools[name] = type;
  }

  return StatusCode::SUCCESS;
}

StatusCode RichToolRegistry::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  return StatusCode::SUCCESS;
}

std::string RichToolRegistry::toolType( std::string tname ) {
  return ( "" == m_tools[tname] ? tname : m_tools[tname] );
}
