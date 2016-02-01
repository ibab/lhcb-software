// $Id: $
#ifndef CHARGEDPROTOANNPIDTUPLETOOL_H
#define CHARGEDPROTOANNPIDTUPLETOOL_H 1

// STL
#include <unordered_map>
#include <memory>

// base class
#include "ChargedProtoANNPIDToolBase.h"

// interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"
#include "RecInterfaces/IChargedProtoANNPIDTupleTool.h"

// Event
#include "Event/MCParticle.h"

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

    virtual StatusCode initialize(); ///< Algorithm initialization

    virtual StatusCode finalize();   ///< Algorithm finalization

  public:

    // Fill the tuple tool with information for the given ProtoParticle
    virtual StatusCode fill( Tuples::Tuple& tuple,
                             const LHCb::ProtoParticle * proto,
                             const LHCb::ParticleID pid = LHCb::ParticleID() ) const;

  private:

    StringInputs m_variables;  ///< ProtoParticle variables as strings to add to the ntuple

    /// Use RICH tool to get MCParticle associations for Tracks (To avoid annoying Linkers)
    const Rich::Rec::MC::IMCTruthTool * m_truth = nullptr;

    /// map of accessor objects for each variable by name
    typedef std::unordered_map< std::string, ChargedProtoANNPIDToolBase::Input::SharedPtr > Inputs;
    // variables to fill
    Inputs m_inputs;

  };

}

#endif // CHARGEDPROTOANNPIDTUPLETOOL_H
