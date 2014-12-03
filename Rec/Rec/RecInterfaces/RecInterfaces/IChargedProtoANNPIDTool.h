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

    /// The return type for the tool. Result with status code.
    class RetType
    {
    public:
      RetType( const bool   _s = false, 
               const double _v = -3 ) : status(_s), value(_v) { }
    public:
      bool status;
      double value;
    };

  public:

    /** Access the ANNPID value for a given ProtoParticle, tune and PID type
     *  @param proto Pointer to the ProtoParticle to fill into the tuple
     *  @param pid The PID to assume for this ProtoParticle
     *  @param annPIDTune The ANNPID tune to use
     *  @return The ANNPID result (status code and value)
     */
    virtual RetType annPID( const LHCb::ProtoParticle * proto,
                            const LHCb::ParticleID& pid,
                            const std::string& annPIDTune ) const = 0;

    /** Access the ANNPID value for a given ProtoParticle, tune and PID type
     *  @param part Pointer to the Particle to fill into the tuple
     *  @param pid The PID to assume for this ProtoParticle
     *  @param annPIDTune The ANNPID tune to use
     *  @return The ANNPID result (status code and value)
     */
    inline RetType annPID( const LHCb::Particle * part,
                           const LHCb::ParticleID& pid,
                           const std::string& annPIDTune ) const
    {
      return ( part ? annPID(part->proto(),pid,annPIDTune) : -1 );
    }

  };

}

#endif // RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H
