// $Id: RichToolBase.cpp,v 1.1 2004-06-17 12:00:48 cattanem Exp $

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
  return GaudiTool::initialize();
}

StatusCode RichToolBase::finalize() 
{
  // Finalise base class
  return GaudiTool::finalize();
}
