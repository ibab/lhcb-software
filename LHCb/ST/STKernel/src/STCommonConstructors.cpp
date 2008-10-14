
//-----------------------------------------------------------------------------
/** @file STCommonConstructors.cpp
 *
 *  Implementation of templated constructors for ST::CommonBase
 *
 *  CVS Log :-
 *  $Id: STCommonConstructors.cpp,v 1.1 2008-10-14 08:47:06 mneedham Exp $
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
template <>
ST::CommonBase<GaudiAlgorithm>::CommonBase( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : GaudiAlgorithm ( name, pSvcLocator             )
 {
   commonInit();
}
template <>
ST::CommonBase<GaudiAlgorithm>::CommonBase( const std::string& /* type */,
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
ST::CommonBase<GaudiHistoAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiHistoAlg  ( name, pSvcLocator             )
{
  commonInit();
}
template <>
ST::CommonBase<GaudiHistoAlg>::CommonBase( const std::string& /* type */,
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
ST::CommonBase<GaudiTupleAlg>::CommonBase( const std::string& name,
                                             ISvcLocator* pSvcLocator )
  : GaudiTupleAlg  ( name, pSvcLocator             )
{
 commonInit();
}
template <>
ST::CommonBase<GaudiTupleAlg>::CommonBase( const std::string& /* type */,
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
ST::CommonBase<GaudiTool>::CommonBase( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent )
  : GaudiTool   ( type, name, parent            )
{
 commonInit();
}
template <>
ST::CommonBase<GaudiTool>::CommonBase( const std::string& /* name */,
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
ST::CommonBase<GaudiHistoTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiHistoTool ( type, name, parent            )
{
 commonInit();
}
template <>
ST::CommonBase<GaudiHistoTool>::CommonBase( const std::string& /* name */,
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
ST::CommonBase<GaudiTupleTool>::CommonBase( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : GaudiTupleTool ( type, name, parent            )
{
 commonInit();
}
template <>
ST::CommonBase<GaudiTupleTool>::CommonBase( const std::string& /* name */,
                                              ISvcLocator* /* pSvcLocator */ )
  : GaudiTupleTool ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid ST::CommonBase<GaudiTupleTool> constructor",
                        "ST::CommonBase", StatusCode::FAILURE );
}
//=============================================================================
