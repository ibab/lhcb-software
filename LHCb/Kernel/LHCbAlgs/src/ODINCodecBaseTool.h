// $Id: ODINCodecBaseTool.h,v 1.3 2009-11-09 18:28:15 marcocle Exp $
#ifndef ODIN_CODEC_BASE_TOOL_H
#define ODIN_CODEC_BASE_TOOL_H
// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiAlg/IGenericTool.h"            // Interface
#include <vector>
#include <string>

// Forward declarations
namespace LHCb {
  class RawBank;
  class ODIN;
}

/** @class ODINCodecBaseTool ODINCodecBaseTool.h
 *
 *  Base tool with the code to encode an ODIN object into an ODIN RawBank and
 *  vice versa.
 *
 *  The properties ODINLocation (location in the Transient Store of the ODIN
 *  object), RawEventLocation (location in the T. S. of the RawEvent) and Force
 *  (whether to overwrite the destination or not) are not used internally, but
 *  they are used by the specializations of this class.
 *
 *  Note that the execute method is not defined in this class, which must be
 *  specialized.
 *
 *  @author Marco Clemencic
 *  @date   2009-02-02
 */
class ODINCodecBaseTool : public GaudiTool, virtual public IGenericTool {
public:
  /// Standard constructor
  ODINCodecBaseTool(const std::string& type,
                 const std::string& name,
                 const IInterface* parent);

  virtual ~ODINCodecBaseTool(); ///< Destructor

  /// Initialize the tool
  virtual inline StatusCode initialize() { return GaudiTool::initialize(); }

protected:
  /// Decode the ODIN RawBank and fill the ODIN object
  /// @param bank ODIN raw bank to decode
  /// @param odin optional pointer to an ODIN object to fill, if null a new object is created (ownership on the caller)
  /// @return pointer to a new ODIN object or the value of the parameter odin
  LHCb::ODIN* i_decode(const LHCb::RawBank* bank, LHCb::ODIN* odin = 0);

  /// Decode the ODIN RawBank and fill the ODIN object
  /// @param odin pointer to the ODIN object to convert
  /// @return     pointer to the newly created RawBank (ownership on the caller)
  LHCb::RawBank* i_encode(const LHCb::ODIN* odin);

  /// If set to true, override the destination object.
  bool m_force;

  /// Flag to indicate if unknown version numbers have to be ignored.
  bool m_ignoreBankVersion;
};

#endif
