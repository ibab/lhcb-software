
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDToolBase.h
 *
 * Header file for algorithm ChargedProtoANNPIDToolBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDToolBase_H
#define ChargedProtoANNPID_ChargedProtoANNPIDToolBase_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleTool.h"

// local
#include "ChargedProtoANNPIDCommonBase.h"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDToolBase ChargedProtoANNPIDToolBase.h
   *
   *  Base class for all ProtoParticle ANN based PID algorithms
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDToolBase : public ANNGlobalPID::ChargedProtoANNPIDCommonBase<GaudiTupleTool>
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDToolBase( const std::string& type,
                                const std::string& name,
                                const IInterface* parent )
      : ANNGlobalPID::ChargedProtoANNPIDCommonBase<GaudiTupleTool> ( type, name, parent ) { }
    
  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDToolBase_H
