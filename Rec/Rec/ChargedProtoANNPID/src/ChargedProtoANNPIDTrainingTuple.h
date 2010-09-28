
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDBase.h
 *
 * Header file for algorithm ChargedProtoANNPIDTrainingTuple
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H
#define ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H 1

// STL
#include <sstream>

// local
#include "ChargedProtoANNPIDBase.h"

// Tool interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event Model
#include "Event/ProtoParticle.h"
#include "Event/MCParticle.h"

// boost
#include "boost/assign/list_of.hpp"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDTrainingTuple ChargedProtoANNPIDTrainingTuple.h
   *
   *  Makes an ntuple for PID ANN training
   *
   *  @author Chris Jones
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDTrainingTuple : public ChargedProtoANNPIDBase
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDTrainingTuple( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDTrainingTuple( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  private:

    StringInputs m_variablesS;  ///< ProtoParticle variables as strings to add to the ntuple
    IntInputs    m_variablesI;  ///< ProtoParticle variables as ints to add to the ntuple

    /// Use RICH tool to get MCParticle associations for Tracks (To avoid Linker details)
    const Rich::Rec::MC::IMCTruthTool * m_truth;

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDTrainingTuple_H
