// $Id: $
#ifndef RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H
#define RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Event Model
#include "Event/ProtoParticle.h"
#include "Event/Particle.h"

static const InterfaceID IID_IChargedProtoANNPIDTupleTool ( "IChargedProtoANNPIDTupleTool", 1, 0 );

namespace ANNGlobalPID
{

  /** @class IChargedProtoANNPIDTupleTool RecInterfaces/IChargedProtoANNPIDTupleTool.h
   *
   *  Interface to tool to fill the ANN PID variables into a tuple
   *
   *  @author Chris Jones
   *  @date   2011-02-04
   */
  class IChargedProtoANNPIDTupleTool :
    virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IChargedProtoANNPIDTupleTool; }

  public:

    /** Fill the tuple tool with information for the given ProtoParticle
     *  @param proto Pointer to the ProtoParticle to fill into the tuple
     *  @param pdgCode The PID to assume for this ProtoParticle
     *  @return StatusCode indicating if the ProtoParticle information was successfully filled
     */
    virtual StatusCode fill( const LHCb::ProtoParticle * proto,
                             const LHCb::ParticleID pid = LHCb::ParticleID() ) const = 0;

    /** Fill the tuple tool with information for the given Particle
     *  @param part Pointer to the Particle to fill into the tuple
     *  @return StatusCode indicating if the Particle information was successfully filled
     */
    inline StatusCode fill( const LHCb::Particle * part ) const
    {
      return ( part ? fill( part->proto(), part->particleID() ) : StatusCode::SUCCESS );
    }

  };

}

#endif // RECINTERFACES_ICHARGEDPROTOANNPIDTUPLETOOL_H
