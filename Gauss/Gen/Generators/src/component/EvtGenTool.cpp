// $Id: EvtGenTool.cpp,v 1.23 2010-05-05 19:38:36 robbep Exp $
// Header file
#include "EvtGenTool.h"


#include<iostream>

// from Gaudi
#include "GaudiKernel/System.h"
#include "GaudiKernel/GaudiException.h"
#include "GaudiKernel/DeclareFactoryEntries.h"

// from EvtGen
#include "EvtGen/EvtGen.hh"
#include "EvtGenBase/EvtParticleFactory.hh"
#include "EvtGenBase/EvtDecayTable.hh"
#include "EvtGenBase/EvtDecayBase.hh"

#include "Generators/StreamForGenerator.h"

//-----------------------------------------------------------------------------
// Implementation file for class : EvtGenTool
//
// 2003-10-15 : Alex Shires
//-----------------------------------------------------------------------------
// Declaration of the Tool Factory

DECLARE_TOOL_FACTORY( EvtGenTool );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EvtGenTool::EvtGenTool( const std::string& type,
                          const std::string& name,
                          const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_gen(0),
    m_init(false) 
{
    declareInterface<IEvtGenTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
EvtGenTool::~EvtGenTool( ) { ; }
//=============================================================================
// Initialize method
//=============================================================================
StatusCode EvtGenTool::initialize( ) {
  StatusCode sc = GaudiTool::initialize( ) ;
  if ( sc.isFailure() ) return sc ;

  //MsgStream * msg = new MsgStream( msgSvc() , name() ) ;
  //StreamForGenerator::getStream() = msg ;

  debug() << "EvtGenTool initialized" << endmsg ;
  
  return StatusCode::SUCCESS ;
}

//=============================================================================
// Finalize method
//=============================================================================
StatusCode EvtGenTool::finalize() {
  if ( 0 != m_gen ) { 
    delete m_gen ;
    m_gen = 0 ;
  }

  m_init = false ;

  if ( ! msgLevel( MSG::DEBUG ) ) {
      debug() << "EvtGenTool finalized" << endmsg ;
  }
  
  return GaudiTool::finalize( ) ;
}
