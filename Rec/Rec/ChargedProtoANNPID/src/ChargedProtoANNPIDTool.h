// $Id: $
#ifndef CHARGEDPROTOANNPIDTOOL_H
#define CHARGEDPROTOANNPIDTOOL_H 1

// STL
#include <unordered_map>
#include <memory>

// base class
#include "ChargedProtoANNPIDToolBase.h"

// interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTool.h"

// Event
#include "Event/MCParticle.h"

namespace ANNGlobalPID
{

  /** @class ChargedProtoANNPIDTool ChargedProtoANNPIDTool.h
   *
   *  Tool to provide the ANN PID variable for a given object
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2014-06-27
   */

  class ChargedProtoANNPIDTool : public ChargedProtoANNPIDToolBase,
                                 virtual public IChargedProtoANNPIDTool
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDTool( const std::string& type,
                            const std::string& name,
                            const IInterface* parent ) ;

    /// Destructor
    virtual ~ChargedProtoANNPIDTool( );

    virtual StatusCode finalize();   ///< Algorithm finalization

  public:

    // Compute the ANNPID value
    virtual IChargedProtoANNPIDTool::RetType 
    annPID( const LHCb::ProtoParticle * proto,
            const LHCb::ParticleID& pid,
            const std::string& annPIDTune ) const;

  private:

    /// Access the NetConfig object for a given configuration
    const NetConfig * getANN( const std::string & trackType,
                              const std::string & pidType,
                              const std::string & netVersion ) const;

  private:

    /// Mapping type to link confgurations to NetConfig instances
    typedef std::unordered_map< std::string, std::unique_ptr<NetConfig> > Networks;

    /// Networks for a given ANNPID configuration
    mutable Networks m_annNets;

    /// Suppress all printout from the ANN experts
    bool m_suppressANNPrintout;

  };

}

#endif // CHARGEDPROTOANNPIDTOOL_H
