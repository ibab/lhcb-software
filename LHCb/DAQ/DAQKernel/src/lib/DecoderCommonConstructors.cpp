
//-----------------------------------------------------------------------------
/** @file DAQCommonConstructors.cpp
 *
 *  Implementation of templated constructors for DAQ::CommonBase
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
#include "DAQKernel/DAQCommonBase.h"

namespace DAQ
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiAlgorithm> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiAlgorithm>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiAlgorithm ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiAlgorithm> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiHistoAlg> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
    : GaudiHistoAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiHistoAlg> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  //----------------------------------------------------------------------------
  template <>
  HistoBase<GaudiHistoAlg>::HistoBase( const std::string& name,
                                       ISvcLocator* pSvcLocator )
    : CommonBase<GaudiHistoAlg> ( name, pSvcLocator )
  {  }
  template <>
  HistoBase<GaudiHistoAlg>::HistoBase( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
    : CommonBase<GaudiHistoAlg> ( type, name, parent ) { }
  template <>
  HistoBase<GaudiHistoAlg>::HistoBase( long storage_type,
                                       const CLID& class_type,
                                       ISvcLocator * svc  )
    : CommonBase<GaudiHistoAlg> ( storage_type, class_type, svc ) { }
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTupleAlg> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleAlg>::CommonBase( long /* storage_type */,
                                         const CLID& /* class_type */,
                                         ISvcLocator * /* svc */ )
    : GaudiTupleAlg ( "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTupleAlg> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  //----------------------------------------------------------------------------
  template <>
  HistoBase<GaudiTupleAlg>::HistoBase( const std::string& name,
                                       ISvcLocator* pSvcLocator )
    : CommonBase<GaudiTupleAlg> ( name, pSvcLocator )
  {  }
  template <>
  HistoBase<GaudiTupleAlg>::HistoBase( const std::string& type,
                                       const std::string& name,
                                       const IInterface* parent )
    : CommonBase<GaudiTupleAlg> ( type, name, parent ) { }
  template <>
  HistoBase<GaudiTupleAlg>::HistoBase( long storage_type,
                                       const CLID& class_type,
                                       ISvcLocator * svc  )
    : CommonBase<GaudiTupleAlg> ( storage_type, class_type, svc ) { }
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTool>::CommonBase( long /* storage_type */,
                                     const CLID& /* class_type */,
                                     ISvcLocator * /* svc */ )
    : GaudiTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiHistoTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiHistoTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiHistoTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiHistoTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  //-----------------------------------------------------------------------------
  template <>
  HistoBase<GaudiHistoTool>::HistoBase( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : CommonBase<GaudiHistoTool> ( type, name, parent )
  {  }
  template <>
  HistoBase<GaudiHistoTool>::HistoBase( const std::string& name ,
                                        ISvcLocator* pSvcLocator )
    : CommonBase<GaudiHistoTool> ( name, pSvcLocator ) { }
  template <>
  HistoBase<GaudiHistoTool>::HistoBase( long storage_type,
                                        const CLID& class_type,
                                        ISvcLocator * svc )
    : CommonBase<GaudiHistoTool> ( storage_type, class_type, svc ) { }
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
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTupleTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  template <>
  CommonBase<GaudiTupleTool>::CommonBase( long /* storage_type */,
                                          const CLID& /* class_type */,
                                          ISvcLocator * /* svc */ )
    : GaudiTupleTool ( "ERROR", "ERROR", NULL )
  {
    throw GaudiException( "Invalid DAQ::CommonBase<GaudiTupleTool> constructor",
                          "DAQ::CommonBase", StatusCode::FAILURE );
  }
  //-----------------------------------------------------------------------------
  template <>
  HistoBase<GaudiTupleTool>::HistoBase( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent )
    : CommonBase<GaudiTupleTool> ( type, name, parent )
  {  }
  template <>
  HistoBase<GaudiTupleTool>::HistoBase( const std::string& name ,
                                        ISvcLocator* pSvcLocator )
    : CommonBase<GaudiTupleTool> ( name, pSvcLocator ) { }
  template <>
  HistoBase<GaudiTupleTool>::HistoBase( long storage_type,
                                        const CLID& class_type,
                                        ISvcLocator * svc )
    : CommonBase<GaudiTupleTool> ( storage_type, class_type, svc ) { }
  //=============================================================================

  //=============================================================================

}
