// $Id: RichRecToolBase.cpp,v 1.7 2003-08-26 14:37:22 jonrob Exp $

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
    m_toolReg  ( 0 ),
    m_DDS      ( 0 ),
    m_msgLevel ( 0 ) {

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

IDataProviderSvc * RichRecToolBase::detSvc() {

  if ( !m_DDS ) {
    StatusCode sc = service( "DetectorDataSvc", m_DDS, true );
    if ( sc.isFailure() ) {
      throw GaudiException("Service [DetectorDataSvc] not found", name(), sc);
    }
  }

  return m_DDS;
}

StatusCode RichRecToolBase::finalize() {

  // Release all tools and services
  releaseTool( m_toolReg );
  if ( m_DDS ) { m_DDS->release(); m_DDS = 0; }

  return StatusCode::SUCCESS;
}
