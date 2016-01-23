
#include "ChargedProtoANNPIDCommonBase.h"

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"
#include "GaudiAlg/GaudiTupleTool.h"

namespace ANNGlobalPID
{

  //=============================================================================
  // Implementation for GaudiTupleAlg
  //=============================================================================

  template <>
  ChargedProtoANNPIDCommonBase<GaudiTupleAlg>::
  ChargedProtoANNPIDCommonBase( const std::string& name,
                                ISvcLocator* pSvcLocator )
    : GaudiTupleAlg ( name, pSvcLocator )
  {
    this -> initCommonConstructor();
  }

  template <>
  ChargedProtoANNPIDCommonBase<GaudiTupleAlg>::
  ChargedProtoANNPIDCommonBase( const std::string& /* type */,
                                const std::string& /* name */,
                                const IInterface* /* parent */ )
    : GaudiTupleAlg ( "ERROR", nullptr )
  {
    throw GaudiException( "Invalid ANNGlobalPID::ChargedProtoANNPIDBase<GaudiTupleAlg> constructor",
                          "ANNGlobalPID::ChargedProtoANNPIDBase", StatusCode::FAILURE );
  }

  //=============================================================================
  // Implementation for GaudiTupleTool
  //=============================================================================

  template <>
  ChargedProtoANNPIDCommonBase<GaudiTupleTool>::
  ChargedProtoANNPIDCommonBase( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
    : GaudiTupleTool ( type, name, parent )
  {
    this -> initCommonConstructor();
  }
  
  template <>
  ChargedProtoANNPIDCommonBase<GaudiTupleTool>::
  ChargedProtoANNPIDCommonBase( const std::string& /* name */,
                                ISvcLocator* /* pSvcLocator */ )
    : GaudiTupleTool ( "ERROR", "ERROR", nullptr )
  {
    throw GaudiException( "Invalid ChargedProtoANNPIDCommonBase<GaudiTupleTool> constructor",
                          "ANNGlobalPID::ChargedProtoANNPIDBase", StatusCode::FAILURE );
  }

}
