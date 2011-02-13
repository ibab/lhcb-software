
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDAlgBase.h
 *
 * Header file for algorithm ChargedProtoANNPIDAlgBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDAlgBase_H
#define ChargedProtoANNPID_ChargedProtoANNPIDAlgBase_H 1

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// local
#include "ChargedProtoANNPIDCommonBase.h"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDAlgBase ChargedProtoANNPIDAlgBase.h
   *
   *  Base class for all ProtoParticle ANN based PID algorithms
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDAlgBase : public ANNGlobalPID::ChargedProtoANNPIDCommonBase<GaudiTupleAlg>
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDAlgBase( const std::string& name, ISvcLocator* pSvcLocator )
      : ANNGlobalPID::ChargedProtoANNPIDCommonBase<GaudiTupleAlg> ( name, pSvcLocator ) { }
    
  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDAlgBase_H
