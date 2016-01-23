
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDTrainingTuple.h
 *
 * Header file for algorithm ChargedProtoANNPIDTrainingTuple
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H
#define ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H 1

// local
#include "ChargedProtoANNPIDAlgBase.h"

// Event Model
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

// Interfaces
#include "RecInterfaces/IChargedProtoANNPIDTupleTool.h"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDTrainingTuple ChargedProtoANNPIDTrainingTuple.h
   *
   *  Makes an ntuple for PID ANN training, starting from ProtoParticles.
   *
   *  @author Chris Jones
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDTrainingTuple : public ChargedProtoANNPIDAlgBase
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDTrainingTuple( const std::string& name, 
                                     ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDTrainingTuple( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  private:

    /// Pointer to the tuple tool
    const IChargedProtoANNPIDTupleTool * m_tuple = nullptr;

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H
