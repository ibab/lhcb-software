// $Id: RichToolBase.cpp,v 1.2 2004-07-12 14:51:18 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichKernel/RichToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichToolBase
//
// 2002-07-26 : Chris Jones   Christopher.Rob.Jones@cern.ch
//-----------------------------------------------------------------------------

// Standard constructor, initializes variables
RichToolBase::RichToolBase( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : GaudiTool ( type, name, parent ),
    m_toolReg ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

StatusCode RichToolBase::initialize() 
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();
  
  // Printout from initialization
  debug() << "Initilize" << endreq;
  
  // return status
  return sc;
}

StatusCode RichToolBase::finalize() 
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiTool::finalize();
}
