
//-----------------------------------------------------------------------------
/** @file RichToolRegistry.cpp
 *
 * Implementation file for class : RichToolRegistry
 *
 * CVS Log :-
 * $Id: RichToolRegistry.cpp,v 1.12 2006-12-01 13:13:13 cattanem Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 14/01/2002
 */
//-----------------------------------------------------------------------------

#include "GaudiKernel/ToolFactory.h"
// Suppress "debug information truncated" warnings on Windows
#include "GaudiKernel/Kernel.h"

// local
#include "RichToolRegistry.h"

DECLARE_TOOL_FACTORY( RichToolRegistry );

// Standard constructor
RichToolRegistry::RichToolRegistry( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent )
  : GaudiTool( type, name, parent )
{
  // declare interface
  declareInterface<IRichToolRegistry>(this);

  // job option for mapping between nickname and class name
  declareProperty( "Tools", m_names );
}

StatusCode RichToolRegistry::initialize()
{
  // Execute the base class initialize
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  // setup tool registry
  for ( ToolList::const_iterator it = m_names.begin();
        it != m_names.end(); ++it )
  {
    const int slash = (*it).find_first_of( "/" );
    addEntry( ( slash>0 ? (*it).substr(slash+1) : *it ),
              ( slash>0 ? (*it).substr(0,slash) : *it ) );
  }

  return sc;
}

const std::string &
RichToolRegistry::toolType( const std::string & nickname ) const
{
  // Test nickname is valid
  if ( nickname.empty() )
  {
    // Empty nick name !
    Exception( "Received empty tool nickname" );
  }

  // test instance name is defined
  if ( m_myTools[nickname].empty() )
  {
    // Don't allow any missing tool entries
    // Exception( "Unknown RICH tool nickname '" + nickname + "'" );
    // or... just assume same as nickname and issue a Warning
    Warning( "Unknown nickname '" + nickname + "' -> Assuming same class name",
             StatusCode::SUCCESS );
    addEntry( nickname, nickname );
  }

  // All OK, so return instance name for this nickname
  return m_myTools[nickname];
}

const std::string 
RichToolRegistry::toolName( const std::string & nickname ) const
{
  return ( context().empty() ? nickname : context()+"."+nickname );
}

void RichToolRegistry::addEntry( const std::string & nickname,
                                 const std::string & type ) const
{
  if ( !m_myTools[nickname].empty() && type != m_myTools[nickname] )
  {
    Warning( "Nickname '" + nickname + "' mapping changed : '" 
             + m_myTools[nickname] + "' to '" + type + "'", StatusCode::SUCCESS );
  }
  if ( msgLevel(MSG::DEBUG) )
  {
    debug() << " Tool nickname '" << nickname
            << "' maps to type '" << type << "'" << endreq;
  }
  m_myTools[nickname] = type;
}

const std::string RichToolRegistry::getContext() const
{
  return context();
}
