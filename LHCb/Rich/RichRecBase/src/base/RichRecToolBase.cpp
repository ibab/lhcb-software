// $Id: RichRecToolBase.cpp,v 1.4 2003-07-03 13:08:36 jonesc Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichRecBase/RichRecToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichRecToolBase
//
// 2002-07-26 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichRecToolBase::RichRecToolBase( const std::string& type,
                                  const std::string& name,
                                  const IInterface* parent )
  : AlgTool ( type, name, parent ),
    m_toolReg(0),
    m_msgLevel(0),
    m_toolList() {
  
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );

}

StatusCode RichRecToolBase::initialize() {

  // Get the current message service printout level
  IntegerProperty msgLevel;
  IProperty* algIProp;
  this->queryInterface( IID_IProperty, (void**)&algIProp );
  msgLevel.assign( algIProp->getProperty( "OutputLevel" ) );
  m_msgLevel = msgLevel;
  algIProp->release();

  // Get pointer to Rich Tool Registry
  if ( !toolSvc()->retrieveTool( "RichToolRegistry",  m_regName, m_toolReg) ) {
    MsgStream msg( msgSvc(), name() );
    msg << MSG::ERROR << "RichToolRegistry not found" << endreq;
    return StatusCode::FAILURE;
  }
  
  return StatusCode::SUCCESS;
}

StatusCode RichRecToolBase::finalize() {

  // Release all tools
  for ( ToolList::iterator it = m_toolList.begin();
        it != m_toolList.end(); ++it ) {
    if ( *it ) { toolSvc()->releaseTool(*it); (*it) = NULL; }
  }
  if ( m_toolReg ) { toolSvc()->releaseTool( m_toolReg ); m_toolReg = NULL; }
  
  return StatusCode::SUCCESS;
}
