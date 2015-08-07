// Include files

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
  : base_class ( type, name , parent )
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
  m_jos.reset();
  return GaudiTool::finalize();
}

//=============================================================================

SmartIF<IJobOptionsSvc>& MapperToolBase::joSvc() const
{
  if ( !m_jos ) { m_jos = svc<IJobOptionsSvc>("JobOptionsSvc"); }
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
