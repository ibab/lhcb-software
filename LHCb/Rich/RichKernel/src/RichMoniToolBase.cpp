
//============================================================================
/** @file RichMoniToolBase.cpp
 *
 *  Implementation file for tool base class : RichMoniToolBase
 *
 *  CVS Log :-
 *  $Id: RichMoniToolBase.cpp,v 1.1 2005-01-13 13:43:02 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.4  2004/07/26 17:53:17  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//============================================================================

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/GaudiException.h"

// local
#include "RichKernel/RichMoniToolBase.h"

// Standard constructor, initializes variables
RichMoniToolBase::RichMoniToolBase( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTupleTool ( type, name, parent ),
    m_toolReg ( 0 )
{
  declareProperty( "ToolRegistryName", m_regName = "RichToolRegistry" );
}

StatusCode RichMoniToolBase::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();

  // Printout from initialization
  debug() << "Initialize" << endreq;

  // return status
  return sc;
}

StatusCode RichMoniToolBase::finalize()
{
  // Printout from finalization
  debug() << "Finalize" << endreq;

  // Finalise base class and return
  return GaudiTool::finalize();
}
