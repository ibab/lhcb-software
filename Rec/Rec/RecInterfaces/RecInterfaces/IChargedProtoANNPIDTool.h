// $Id: $
#ifndef RECINTERFACES_ICHARGEDPROTOANNPIDTOOL_H
#define RECINTERFACES_ICHARGEDPROTOANNPIDTOOL_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event Model
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

static const InterfaceID IID_IChargedProtoANNPIDTool ( "IChargedProtoANNPIDTool", 1, 0 );

namespace ANNGlobalPID
{

  /** @class IChargedProtoANNPIDTool RecInterfaces/IChargedProtoANNPIDTool.h
   *
   *  Interface to tool to access the ANN PID information
   *
   *  @author Chris Jones
   *  @date   2014-06027
   */
  class IChargedProtoANNPIDTool : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IChargedProtoANNPIDTool; }

  public:

    /** Fill the tuple tool with information for the given ProtoParticle
     *  @param proto Pointer to the ProtoParticle to fill into the tuple
     *  @param pid The PID to assume for this ProtoParticle
     *  @param annPIDTune The ANNPID tune to use
     *  @return The ANNPID value
     */
    virtual double annPID( const LHCb::ProtoParticle * proto,
                           const LHCb::ParticleID& pid,
                           const std::string& annPIDTune ) const = 0;

    /** Fill the tuple tool with information for the given Particle
     *  @param part Pointer to the Particle to fill into the tuple
     *  @param pid The PID to assume for this ProtoParticle
     *  @param annPIDTune The ANNPID tune to use
     *  @return The ANNPID value
     */
    inline double annPID( const LHCb::Particle * part,
                          const LHCb::ParticleID& pid,
                          const std::string& annPIDTune ) const
    {
      return ( part ? annPID(part->proto(),pid,annPIDTune) : -1 );
    }
    
  };
  
}

#endif // RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H
