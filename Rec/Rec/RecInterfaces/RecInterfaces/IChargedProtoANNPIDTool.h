// $Id: $
#ifndef RECINTERFACES_ICHARGEDPROTOANNPIDTOOL_H
#define RECINTERFACES_ICHARGEDPROTOANNPIDTOOL_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event Model
#include "Event/ProtoParticle.h"
//#include "Event/Particle.h"

static const InterfaceID IID_IChargedProtoANNPIDTool ( "IChargedProtoANNPIDTool", 1, 0 );

namespace ANNGlobalPID
{

  /** @class IChargedProtoANNPIDTool RecInterfaces/IChargedProtoANNPIDTool.h
   *
   *  Interface to tool to fill the ANN PID variables into a tuple
   *
   *  @author Chris Jones
   *  @date   2011-02-04
   */
  class IChargedProtoANNPIDTool : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IChargedProtoANNPIDTool; }

  public:

    /** Fill the tuple tool with information for the given ProtoParticle
     *  @param proto Pointer to the ProtoParticle to fill into the tuple
     *  @param pdgCode The PID to assume for this ProtoParticle
     *  @return StatusCode indicating if the ProtoParticle information was successfully filled
     */
    virtual double annPID( const LHCb::ProtoParticle * proto,
                           const LHCb::ParticleID pid,
                           const std::string& annPIDTune ) const = 0;
    
    
  };
  
}

#endif // RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H
