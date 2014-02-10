
//-----------------------------------------------------------------------------
/** @file DAQCommonConstructors.cpp
 *
 *  Implementation of templated constructors for Decoder::CommonBase
 *
 *  @author Rob Lambert    Rob.Lambert@cern.ch
 *  @date   2014-02-10
 */
//-----------------------------------------------------------------------------

// Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "DAQKernel/DecoderCommonBase.h"

namespace Decoder
{

  //=============================================================================
  // Implementation of DAQCommonBase for GaudiAlgorithm
  //=============================================================================
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( const std::string& name,
                                          ISvcLocator* pSvcLocator )
    : GaudiAlgorithm ( name, pSvcLocator )
  {  }
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( const std::string& /* type */,
                                          const std::string& /* name */,
                                          const IInterface* /* parent */ )
    : GaudiAlgorithm ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiAlgorithm> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiAlgorithm ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiAlgorithm> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  //=============================================================================

  //=============================================================================
  // Implementation of DAQCommonBase for GaudiHistoAlg
  //=============================================================================
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( const std::string& name,
                                         ISvcLocator* pSvcLocator )
    : GaudiHistoAlg ( name, pSvcLocator )
  {  }
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( const std::string& /* type */,
                                         const std::string& /* name */,
                                         const IInterface* /* parent */ )
    : GaudiHistoAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiHistoAlg> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
    : GaudiHistoAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiHistoAlg> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }

  //=============================================================================

  //=============================================================================
  // Implementation of CommonBase for GaudiTupleAlg
  //=============================================================================
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( const std::string& name,
                                         ISvcLocator* pSvcLocator )
    : GaudiTupleAlg ( name, pSvcLocator )
  {  }
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( const std::string& /* type */,
                                         const std::string& /* name */,
                                         const IInterface* /* parent */ )
    : GaudiTupleAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTupleAlg> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
    : GaudiTupleAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTupleAlg> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }

  //=============================================================================

  //=============================================================================
  // Implementation of CommonBase for GaudiTool
  //=============================================================================
  template <>
  CommonBase<GaudiTool>::CommonBase( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
    : GaudiTool ( type, name, parent )
  {  }
  template <>
  CommonBase<GaudiTool>::CommonBase( const std::string& /* name */,
                                     ISvcLocator* /* pSvcLocator */ )
    : GaudiTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTool>::CommonBase( long /* storage_type */,
                                     const CLID& /* class_type */,
                                     ISvcLocator * /* svc */ )
    : GaudiTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  //=============================================================================

  //=============================================================================
  // Implementation of CommonBase for GaudiHistoTool
  //=============================================================================
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : GaudiHistoTool ( type, name, parent )
  {  }
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( const std::string& /* name */,
                                          ISvcLocator* /* pSvcLocator */ )
    : GaudiHistoTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiHistoTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiHistoTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiHistoTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }

  //=============================================================================

  //=============================================================================
  // Implementation of DAQCommonBase for GaudiTupleTool
  //=============================================================================
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent )
    : GaudiTupleTool ( type, name, parent )
  {  }
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( const std::string& /* name */,
                                          ISvcLocator* /* pSvcLocator */ )
    : GaudiTupleTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTupleTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiTupleTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid Decoder::CommonBase<GaudiTupleTool> constructor",
                          "Decoder::CommonBase", StatusCode::FAILURE );
  }


  //=============================================================================

}
