
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDCommonConstructors.cpp
 *
 *  Implementation of templated constructors for RiochCommonBase
 *
 *  CVS Log :-
 *  $Id: RichGlobalPIDCommonConstructors.cpp,v 1.1 2008-03-25 16:26:44 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-09-18
 */
//-----------------------------------------------------------------------------

#include "RichRecBase/RichRecAlgBase.h"
#include "RichRecBase/RichRecToolBase.h"
#include "RichGlobalPIDCommonBase.h"

//=============================================================================
// Implementation for algorithms
//=============================================================================
template <>
Rich::Rec::GlobalPID::CommonBase<Rich::Rec::AlgBase>::CommonBase( const std::string& name,
                                                                  ISvcLocator* pSvcLocator )
  : Rich::Rec::AlgBase ( name, pSvcLocator )
{
  this->initConstructor();
}
template <>
Rich::Rec::GlobalPID::CommonBase<Rich::Rec::AlgBase>::CommonBase( const std::string& /* type */,
                                                                  const std::string& /* name */,
                                                                  const IInterface* /* parent */ )
  : Rich::Rec::AlgBase ( "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::Rec::GlobalPID::CommonBase<Rich::Rec::AlgBase> constructor",
                        "Rich::Rec::GlobalPID::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

//=============================================================================
// Implementation for tools
//=============================================================================
template <>
Rich::Rec::GlobalPID::CommonBase<Rich::Rec::ToolBase>::CommonBase( const std::string& type,
                                                                   const std::string& name,
                                                                   const IInterface* parent )
  : Rich::Rec::ToolBase ( type, name, parent )
{
  this->initConstructor();
}
template <>
Rich::Rec::GlobalPID::CommonBase<Rich::Rec::ToolBase>::CommonBase( const std::string& /* name */,
                                                                   ISvcLocator* /* pSvcLocator */ )
  : Rich::Rec::ToolBase ( "ERROR", "ERROR", NULL )
{
  throw GaudiException( "Invalid Rich::Rec::GlobalPID::CommonBase<Rich::Rec::ToolBase> constructor",
                        "Rich::Rec::GlobalPID::CommonBase", StatusCode::FAILURE );
}
//=============================================================================

