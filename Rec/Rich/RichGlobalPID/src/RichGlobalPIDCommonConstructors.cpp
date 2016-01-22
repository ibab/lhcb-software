
//-----------------------------------------------------------------------------
/** @file RichGlobalPIDCommonConstructors.cpp
 *
 *  Implementation of templated constructors for Rich::Rec::GlobalPID::CommonBase
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2004-09-18
 */
//-----------------------------------------------------------------------------

#include "RichRecBase/RichRecHistoAlgBase.h"
#include "RichRecBase/RichRecHistoToolBase.h"
#include "RichGlobalPIDCommonBase.h"

//=============================================================================
// Implementation for algorithms
//=============================================================================
namespace Rich 
{
  namespace Rec 
  {
    namespace GlobalPID 
    {
      
      template <>
      CommonBase<Rich::Rec::HistoAlgBase>::CommonBase( const std::string& name,
                                                       ISvcLocator* pSvcLocator )
        : Rich::Rec::HistoAlgBase ( name, pSvcLocator )
      {
        this->initConstructor();
      }
      template <>
      CommonBase<Rich::Rec::HistoAlgBase>::CommonBase( const std::string& /* type */,
                                                       const std::string& /* name */,
                                                       const IInterface* /* parent */ )
        : Rich::Rec::HistoAlgBase ( "ERROR", nullptr )
      {
        throw GaudiException( "Invalid Rich::Rec::GlobalPID::CommonBase<Rich::Rec::AlgBase> constructor",
                              "Rich::Rec::GlobalPID::CommonBase", StatusCode::FAILURE );
      }
      //=============================================================================

      //=============================================================================
      // Implementation for tools
      //=============================================================================
      template <>
      CommonBase<Rich::Rec::HistoToolBase>::CommonBase( const std::string& type,
                                                        const std::string& name,
                                                        const IInterface* parent )
        : Rich::Rec::HistoToolBase ( type, name, parent )
      {
        this->initConstructor();
      }
      template <>
      CommonBase<Rich::Rec::HistoToolBase>::CommonBase( const std::string& /* name */,
                                                        ISvcLocator* /* pSvcLocator */ )
        : Rich::Rec::HistoToolBase ( "ERROR", "ERROR", nullptr )
      {
        throw GaudiException( "Invalid Rich::Rec::GlobalPID::CommonBase<Rich::Rec::ToolBase> constructor",
                              "Rich::Rec::GlobalPID::CommonBase", StatusCode::FAILURE );
      }

    } // namespace GlobalPID
  } // namespace Rec
} // namespace Rich
//=============================================================================

