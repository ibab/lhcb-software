
//---------------------------------------------------------------------------------
/** @file IRichAddBackground.h
 *
 * Header file for tool interface : Rich::IAddBackground
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date   2008-10-10
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHADDBACKGROUND_H
#define RICHKERNEL_IRICHADDBACKGROUND_H 1

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "RichKernel/RichHashMap.h"

namespace Rich
{

  /// Tool Interface ID
  static const InterfaceID IID_IRichAddBackground ( "Rich::IAddBackground", 1, 0 );

  /** @class IAddBackground RichKernel/IRichAddBackground.h
   *
   *  Interface for tools that generate RICH backgrounds
   *
   *  @author Chris Jones
   *  @date   2008-10-10
   */

  class IAddBackground : virtual public IAlgTool
  {

  public:

    /// Return the interface ID
    static const InterfaceID& interfaceID() { return IID_IRichAddBackground; }

  public:

    /// Data structure for returned additional backgrounds
    typedef Rich::HashMap<LHCb::RichSmartID,LHCb::RichSmartID::Vector> HPDBackgrounds;

  public:

    /// Compute a set of background hits
    virtual StatusCode createBackgrounds( HPDBackgrounds & backgrounds,
                                          const bool aliceMode = false ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHADDBACKGROUND_H
