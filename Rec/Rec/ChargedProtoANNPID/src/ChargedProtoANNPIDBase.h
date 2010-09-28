
//-----------------------------------------------------------------------------
/** @file ChargedProtoANNPIDBase.h
 *
 * Header file for algorithm ChargedProtoANNPIDBase
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2010-03-09
 */
//-----------------------------------------------------------------------------

#ifndef ChargedProtoANNPID_ChargedProtoANNPIDBase_H
#define ChargedProtoANNPID_ChargedProtoANNPIDBase_H 1

// STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiAlg/GaudiTupleAlg.h"

// Event Model
#include "Event/ProtoParticle.h"

//-----------------------------------------------------------------------------
/** @namespaceANNGlobalPID
 *
 *  General namespace for Global PID ANN software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   2010-03-09
 */
//-----------------------------------------------------------------------------

namespace ANNGlobalPID
{

  //-----------------------------------------------------------------------------
  /** @class ChargedProtoANNPIDBase ChargedProtoANNPIDBase.h
   *
   *  Base class for all ProtoParticle ANN based PID algorithms
   *
   *  @author Chris Jones
   *  @date   2010-03-09
   */
  //-----------------------------------------------------------------------------

  class ChargedProtoANNPIDBase : public GaudiTupleAlg
  {

  public:

    /// Standard constructor
    ChargedProtoANNPIDBase( const std::string& name,
                            ISvcLocator* pSvcLocator );

    virtual ~ChargedProtoANNPIDBase( ); ///< Destructor

  protected:

    /// Type for list of inputs
    typedef std::vector<std::string> StringInputs;

    /// Type for list of inputs
    typedef std::vector<int> IntInputs;

  public:

    /// Convert a variable name string to ID number
    int variableID( const std::string & name ) const;

    /// Convert variable names string to ID numbers
    IntInputs variableIDs( const StringInputs & names ) const;

    /// Get the value for the given input int ID
    double getInput( const LHCb::ProtoParticle * proto,
                     const int input ) const;

    /// Get the value for the given input string ID
    inline double getInput( const LHCb::ProtoParticle * proto,
                            const std::string& input ) const
    {
      return getInput( proto, variableID(input) );
    }

  protected:

    std::string m_protoPath; ///< Location in TES of ProtoParticles

  };

}

#endif // ChargedProtoANNPID_ChargedProtoANNPIDBase_H
