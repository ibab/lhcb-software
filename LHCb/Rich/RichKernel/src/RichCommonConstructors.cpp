
//-----------------------------------------------------------------------------
/** @file RichCommonConstructors.cpp
 *
 *  Implementation of templated constructors for Rich::CommonBase
 *
 *  CVS Log :-
 *  $Id: RichCommonConstructors.cpp,v 1.9 2009-07-07 16:24:53 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-09-18
 */
//-----------------------------------------------------------------------------

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "RichKernel/RichConverter_Imp.h"
#include "RichKernel/RichCommonBase.h"

namespace Rich
{

  //=============================================================================
  // Implementation of RichCommonBase for GaudiAlgorithm
  //=============================================================================
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( const std::string& name,
                                          ISvcLocator* pSvcLocator )
    : GaudiAlgorithm ( name, pSvcLocator             ),
      m_toolReg      ( NULL                          ),
      m_jos          ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty( "ToolRegistryName", m_regName );
  }
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( const std::string& /* type */,
                                          const std::string& /* name */,
                                          const IInterface* /* parent */ )
    : GaudiAlgorithm ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiAlgorithm> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
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
  CommonBase<GaudiHistoAlg>::CommonBase( const std::string& name,
                                         ISvcLocator* pSvcLocator )
    : GaudiHistoAlg  ( name, pSvcLocator             ),
      m_toolReg      ( NULL                          ),
      m_jos          ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty ( "ToolRegistryName", m_regName );
    setProperty     ( "HistoTopDir",  "RICH/" );
  }
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( const std::string& /* type */,
                                         const std::string& /* name */,
                                         const IInterface* /* parent */ )
    : GaudiHistoAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiHistoAlg> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
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
  CommonBase<GaudiTupleAlg>::CommonBase( const std::string& name,
                                         ISvcLocator* pSvcLocator )
    : GaudiTupleAlg  ( name, pSvcLocator             ),
      m_toolReg      ( NULL                          ),
      m_jos          ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty ( "ToolRegistryName", m_regName );
    setProperty     ( "HistoTopDir",  "RICH/" );
    setProperty     ( "NTupleTopDir", "RICH/" );
    setProperty     ( "NTupleLUN",    "RICHTUPLE1" );
  }
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( const std::string& /* type */,
                                         const std::string& /* name */,
                                         const IInterface* /* parent */ )
    : GaudiTupleAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiTupleAlg> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
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
  CommonBase<GaudiTool>::CommonBase( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
    : GaudiTool   ( type, name, parent            ),
      m_toolReg   ( NULL                          ),
      m_jos       ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty( "ToolRegistryName", m_regName );
  }
  template <>
  CommonBase<GaudiTool>::CommonBase( const std::string& /* name */,
                                     ISvcLocator* /* pSvcLocator */ )
    : GaudiTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiTool> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTool>::CommonBase( long /* storage_type */,
                                     const CLID& /* class_type */,
                                     ISvcLocator * /* svc */ )
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
  CommonBase<GaudiHistoTool>::CommonBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : GaudiHistoTool ( type, name, parent            ),
      m_toolReg      ( NULL                          ),
      m_jos          ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty ( "ToolRegistryName", m_regName );
    setProperty     ( "HistoTopDir",  "RICH/" );
  }
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( const std::string& /* name */,
                                          ISvcLocator* /* pSvcLocator */ )
    : GaudiHistoTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiHistoTool> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
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
  CommonBase<GaudiTupleTool>::CommonBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : GaudiTupleTool ( type, name, parent            ),
      m_toolReg      ( NULL                          ),
      m_jos          ( NULL                          ),
      m_regName      ( context().empty() ?
                       "RichToolRegistry" : context()+"_RichToolRegistry" )
  {
    declareProperty ( "ToolRegistryName", m_regName );
    setProperty     ( "HistoTopDir",  "RICH/" );
    setProperty     ( "NTupleTopDir", "RICH/" );
    setProperty     ( "NTupleLUN",    "RICHTUPLE1" );
  }
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( const std::string& /* name */,
                                          ISvcLocator* /* pSvcLocator */ )
    : GaudiTupleTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiTupleTool> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiTupleTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<GaudiTupleTool> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  //=============================================================================

  //=============================================================================
  // Implementation of RichCommonBase for Converter
  //=============================================================================
  template <>
  CommonBase< Rich::Converter_Imp >::CommonBase( const std::string& /* type */,
                                                 const std::string& /* name */,
                                                 const IInterface* /* parent */ )
    : Rich::Converter_Imp ( 0, CLID_NULL, NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<Rich::Converter_Imp> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase< Rich::Converter_Imp >::CommonBase( const std::string& /* name */,
                                                 ISvcLocator* /* pSvcLocator */ )
    : Rich::Converter_Imp ( 0, CLID_NULL, NULL )
  {
    throw GaudiException( "Invalid Rich::CommonBase<Rich::Converter_Imp> constructor",
                          "Rich::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase< Rich::Converter_Imp >::CommonBase( long storage_type,
                                                 const CLID &class_type,
                                                 ISvcLocator *svc )
    : Rich::Converter_Imp ( storage_type, class_type, svc )
  {
    // to do ...
    // declareProperty ( "ToolRegistryName", m_regName );
  }
  //=============================================================================

}
