
//-----------------------------------------------------------------------------
/** @file RichCommonConstructors.cpp
 *
 *  Implementation of templated constructors for Rich::CommonBase
 *
 *  CVS Log :-
 *  $Id: RichCommonConstructors.cpp,v 1.5 2008-03-25 15:55:03 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-09-18
 */
//-----------------------------------------------------------------------------

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichCommonBase.h"

//=============================================================================
// Implementation of RichCommonBase for GaudiAlgorithm
//=============================================================================
template <>
Rich::CommonBase<GaudiAlgorithm>::CommonBase( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator             ),
    m_toolReg      ( NULL                          ),
    m_jos          ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiAlgorithm>::CommonBase( const std::string& /* type */,
                                              const std::string& /* name */,
                                              const IInterface* /* parent */ )
  : GaudiAlgorithm ( "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiAlgorithm> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of RichCommonBase for GaudiHistoAlg
//=============================================================================
template <>
Rich::CommonBase<GaudiHistoAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiHistoAlg  ( name, pSvcLocator             ),
    m_toolReg      ( NULL                          ),
    m_jos          ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiHistoAlg>::CommonBase( const std::string& /* type */,
                                             const std::string& /* name */,
                                             const IInterface* /* parent */ )
  : GaudiHistoAlg ( "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiHistoAlg> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiTupleAlg
//=============================================================================
template <>
Rich::CommonBase<GaudiTupleAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiTupleAlg  ( name, pSvcLocator             ),
    m_toolReg      ( NULL                          ),
    m_jos          ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiTupleAlg>::CommonBase( const std::string& /* type */,
                                             const std::string& /* name */,
                                             const IInterface* /* parent */ )
  : GaudiTupleAlg ( "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiTupleAlg> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiTool
//=============================================================================
template <>
Rich::CommonBase<GaudiTool>::CommonBase( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : GaudiTool   ( type, name, parent            ),
    m_toolReg   ( NULL                          ),
    m_jos       ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiTool>::CommonBase( const std::string& /* name */,
                                         ISvcLocator* /* pSvcLocator */ )
  : GaudiTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiTool> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiHistoTool
//=============================================================================
template <>
Rich::CommonBase<GaudiHistoTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiHistoTool ( type, name, parent            ),
    m_toolReg      ( NULL                          ),
    m_jos          ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiHistoTool>::CommonBase( const std::string& /* name */,
                                              ISvcLocator* /* pSvcLocator */ )
  : GaudiHistoTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiHistoTool> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of RichCommonBase for GaudiTupleTool
//=============================================================================
template <>
Rich::CommonBase<GaudiTupleTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTupleTool ( type, name, parent            ),
    m_toolReg      ( NULL                          ),
    m_jos          ( NULL                          ),
    m_regName      ( context().empty() ?
                     "RichToolRegistry" : context()+".RichToolRegistry" )
{
  declareProperty( "ToolRegistryName", m_regName );
}
template <>
Rich::CommonBase<GaudiTupleTool>::CommonBase( const std::string& /* name */,
                                              ISvcLocator* /* pSvcLocator */ )
  : GaudiTupleTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::CommonBase<GaudiTupleTool> constructor",
                        "Rich::CommonBase", StatusCode::FAILURE );
}
//=============================================================================
