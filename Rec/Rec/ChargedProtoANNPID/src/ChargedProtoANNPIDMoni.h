
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDMoni.h
 *
 *  Declaration file for ANN Combined PID algorithm ANNGlobalPID::ChargedProtoANNPIDMoni
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   12/02/2012
 */
//-------------------------------------------------------------------------------

#ifndef CHARGEDPROTOANNPIDMONI_H
#define CHARGEDPROTOANNPIDMONI_H 1

// local
#include "ChargedProtoANNPIDAlgBase.h"

namespace ANNGlobalPID
{

  /** @class ChargedProtoANNPIDMoni ChargedProtoANNPIDMoni.h
   *
   *  Monitor for the ANNPID
   *  
   *  @author Chris Jones
   *  @date   2012-01-12
   */

  class ChargedProtoANNPIDMoni : public ChargedProtoANNPIDAlgBase
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDMoni( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDMoni(); ///< Destructor

    virtual StatusCode execute();    ///< Algorithm execution

  };

}

#endif // CHARGEDPROTOANNPIDMONI_H
