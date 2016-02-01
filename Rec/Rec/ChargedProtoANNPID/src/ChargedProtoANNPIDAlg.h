
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDAlg.h
 *
 *  Declaration file for ANN Combined PID algorithm ANNGlobalPID::ChargedProtoANNPIDAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/09/2010
 */
//-------------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDAlg_H
#define ChargedProtoANNPID_ChargedProtoANNPIDAlg_H 1

// STL
#include <sstream>
#include <fstream>
#include <memory>

// Gaudi
#include "GaudiKernel/IJobOptionsSvc.h"

// FPE exception protection
#include "Kernel/FPEGuard.h"

// local
#include "ChargedProtoANNPIDAlgBase.h"

// boost
#include "boost/algorithm/string.hpp"
#include "boost/format.hpp"

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDAlg ChargedProtoANNPIDAlg.h
   *
   *  Adds ANN PID information to ProtoParticles
   *
   *  @author Chris Jones
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDAlg : public ChargedProtoANNPIDAlgBase
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDAlg( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDAlg( ); ///< Destructor

    virtual StatusCode initialize();    ///< Algorithm initialization
    virtual StatusCode execute   ();    ///< Algorithm execution

  private:

    /// Track type
    std::string m_trackType;

    /// PID type
    std::string m_pidType;

    /// The version of the PID networks training to use
    std::string m_netVersion;

    /// Suppress all printout from the ANN experts
    bool m_suppressANNPrintout;

    /// Network Configuration
    std::unique_ptr<NetConfig> m_netConfig;

    /// The extra info to fill on the ProtoParticle
    LHCb::ProtoParticle::additionalInfo m_protoInfo;

    /// The track type for this instance
    LHCb::Track::Types m_tkType;

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDAlg_H
