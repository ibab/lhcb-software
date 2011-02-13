// $Id: $
#ifndef CHARGEDPROTOANNPIDTUPLETOOL_H
#define CHARGEDPROTOANNPIDTUPLETOOL_H 1

// base class
#include "ChargedProtoANNPIDToolBase.h"

// interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTupleTool.h"

// Event
#include "Event/MCParticle.h"

// boost
#include "boost/assign/list_of.hpp"

namespace ANNGlobalPID
{

  /** @class ChargedProtoANNPIDTupleTool ChargedProtoANNPIDTupleTool.h
   *
   *  Tool to fill the ANN PID variables into a tuple
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2011-02-04
   */

  class ChargedProtoANNPIDTupleTool : public ChargedProtoANNPIDToolBase,
                                      virtual public IChargedProtoANNPIDTupleTool
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDTupleTool( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent ) ;

    /// Destructor
    virtual ~ChargedProtoANNPIDTupleTool( );

    virtual StatusCode initialize();   ///< Algorithm initialization

  public:

    // Fill the tuple tool with information for the given ProtoParticle
    virtual StatusCode fill( const LHCb::ProtoParticle * proto,
                             const LHCb::ParticleID pid = LHCb::ParticleID() ) const;

  private:

    StringInputs m_variablesS;  ///< ProtoParticle variables as strings to add to the ntuple
    IntInputs    m_variablesI;  ///< ProtoParticle variables as ints to add to the ntuple

    /// Use RICH tool to get MCParticle associations for Tracks (To avoid Linker details)
    const Rich::Rec::MC::IMCTruthTool * m_truth;

  };

}

#endif // CHARGEDPROTOANNPIDTUPLETOOL_H
