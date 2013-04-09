// $Id: $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "MapperToolBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MapperToolBase
//
// 2012-03-26 : Chris Jones
//-----------------------------------------------------------------------------

MapperToolBase::MapperToolBase( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
  : base_class ( type, name , parent ),
    m_jos      ( NULL )
{
}

//=============================================================================

MapperToolBase::~MapperToolBase() {}

//============================================================================

StatusCode MapperToolBase::initialize()
{
  const StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;

  return sc;
}

//============================================================================

StatusCode MapperToolBase::finalize()
{
  if ( m_jos ) { m_jos->release(); m_jos = NULL; }
  return GaudiTool::finalize();
}

//=============================================================================

IJobOptionsSvc * MapperToolBase::joSvc() const
{
  if ( !m_jos )
  {
    m_jos = svc<IJobOptionsSvc>( "JobOptionsSvc" );
  }
  return m_jos;
}

// ============================================================================

std::string MapperToolBase::streamName( const std::string & path ) const
{
  std::string tmp = path;
  if ( path.substr(0,7) == "/Event/" ) { tmp = tmp.substr(7); }
  const std::string::size_type slash = tmp.find_first_of( "/" );
  return tmp.substr(0,slash);
}

//=============================================================================
