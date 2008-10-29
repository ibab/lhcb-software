
//-----------------------------------------------------------------------------
/** @file STCommonConstructors.cpp
 *
 *  Implementation of templated constructors for ST::CommonBase
 *
 *  CVS Log :-
 *  $Id: STCommonConstructors.cpp,v 1.2 2008-10-29 12:50:01 mneedham Exp $
 *
 *  @author Matthew Needham
 *  @date   2008-11-10
 */
//-----------------------------------------------------------------------------

// GaudiAlg
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/GaudiHistoTool.h"

// local
#include "Kernel/STCommonBase.h"

//=============================================================================
// Implementation of STCommonBase for GaudiAlgorithm
//=============================================================================

namespace ST{
template <>
  CommonBase<GaudiAlgorithm>::CommonBase( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator             )
 {
   commonInit();
}
template <>
CommonBase<GaudiAlgorithm>::CommonBase( const std::string& /* type */,
                                              const std::string& /* name */,
                                              const IInterface* /* parent */ )
  : GaudiAlgorithm ( "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiAlgorithm> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of STCommonBase for GaudiHistoAlg
//=============================================================================
template <>
CommonBase<GaudiHistoAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiHistoAlg  ( name, pSvcLocator             )
{
  commonInit();
}
template <>
CommonBase<GaudiHistoAlg>::CommonBase( const std::string& /* type */,
                                             const std::string& /* name */,
                                             const IInterface* /* parent */ )
  : GaudiHistoAlg ( "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiHistoAlg> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiTupleAlg
//=============================================================================
template <>
CommonBase<GaudiTupleAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiTupleAlg  ( name, pSvcLocator             )
{
 commonInit();
}
template <>
CommonBase<GaudiTupleAlg>::CommonBase( const std::string& /* type */,
                                             const std::string& /* name */,
                                             const IInterface* /* parent */ )
  : GaudiTupleAlg ( "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiTupleAlg> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiTool
//=============================================================================
template <>
CommonBase<GaudiTool>::CommonBase( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : GaudiTool   ( type, name, parent            )
{
 commonInit();
}
template <>
CommonBase<GaudiTool>::CommonBase( const std::string& /* name */,
                                         ISvcLocator* /* pSvcLocator */ )
  : GaudiTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiTool> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of CommonBase for GaudiHistoTool
//=============================================================================
template <>
CommonBase<GaudiHistoTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiHistoTool ( type, name, parent            )
{
 commonInit();
}
template <>
CommonBase<GaudiHistoTool>::CommonBase( const std::string& /* name */,
                                              ISvcLocator* /* pSvcLocator */ )
  : GaudiHistoTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiHistoTool> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation of STCommonBase for GaudiTupleTool
//=============================================================================
template <>
CommonBase<GaudiTupleTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTupleTool ( type, name, parent            )
{
 commonInit();
}
template <>
CommonBase<GaudiTupleTool>::CommonBase( const std::string& /* name */,
                                              ISvcLocator* /* pSvcLocator */ )
  : GaudiTupleTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiTupleTool> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================
} // ST namespace
