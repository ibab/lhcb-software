/** @file IRichSmartIDTool.h
 *
 *  Header file for tool interface : IRichSmartIDTool
 *
 *  CVS Log :-
 *  $Id: IRichSmartIDTool.h,v 1.6 2004-10-27 14:29:31 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.5  2004/07/29 09:30:32  jonrob
 *  Fix various typos + minor updates
 *
 *  Revision 1.4  2004/07/26 17:53:17  jonrob
 *  Various improvements to the doxygen comments
 * 
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2003-10-28
 */

#ifndef RICHKERNEL_IRICHSMARTIDTOOL_H
#define RICHKERNEL_IRICHSMARTIDTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// CLHEP
class HepPoint3D;
class HepVector3D;

// Rich Kernel
class RichSmartID;

/// Static Interface Identification
static const InterfaceID IID_IRichSmartIDTool( "IRichSmartIDTool", 1, 0 );

/** @class IRichSmartIDTool IRichSmartIDTool.h RichKernel/IRichSmartIDTool.h
 *
 *  Interface to tools providing manipulation of RichSmartID channel identifiers
 *
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @date   2003-10-28
 */
class IRichSmartIDTool : virtual public IAlgTool {

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichSmartIDTool; }

  /** Converts a RichSmartID channel indentification into a position in
   *  global LHCb coordinates
   *
   *  @param smartid   The RichSmartID channel indentifier
   *  @param position  The global position coordinates
   *
   *  @return The status of the conversion
   *  @retval StatusCode::SUCCESS Conversion was successful (valid RichSmartID)
   *  @retval StatusCode::FAILURE Conversion was not possible (in-valid RichSmartID)
   */
  virtual HepPoint3D globalPosition ( const RichSmartID& smartid ) const = 0;

  /** Computes the global position coordinate for a given position in local
   *  HPD panel coordinates and RICH detector and panel indentifiers.
   *
   *  @param localPoint  The local coordinate position
   *  @param rich        The RICH detector 
   *  @param side        The RICH HPD panel
   *
   *  @return The global coordinate
   */
  virtual HepPoint3D globalPosition ( const HepPoint3D& localPoint, 
                                      const Rich::DetectorType rich,
                                      const Rich::Side side ) const = 0;

  /** Converts a position in global coordinates to the corresponding 
   *  RichSmartID indentifier.
   *
   *  @param  globalPoint The global coordinate to convert
   *  @param  smartid     The RichSmartID channel identifier to fill
   *
   *  @return The status of the conversion
   *  @retval StatusCode::SUCCESS Conversion was successful (position in HPD acceptance)
   *  @retval StatusCode::FAILURE Conversion was not successful (position not in HPD acceptance)
   */
  virtual StatusCode smartID ( const HepPoint3D& globalPoint, 
                               RichSmartID& smartid ) const = 0;

  /** Supplies a vector of all currently active and valid channels in the RICH detectors
   *
   *  @param readoutChannelList  Vector of RichSmartIDs to fill with the valid channels
   *
   *  @return status of the return vector of channel identifiers
   *  @retval StatusCode::SUCCESS Request was successful, RichSmartID vector is valid
   *  @retval StatusCode::FAILURE Request was successful, RichSmartID vector is not valid
   */
  virtual StatusCode readoutChannelList ( std::vector<RichSmartID>& readoutChannels ) const = 0;
  
  /** Converts a position in global coordinates to the local coordinate system
   *  of the appropriate HPD panel
   *
   *  @param globalPoint The global coordinate to convert
   *
   *  @return The coordinate in local HPD panel coordinates
   */
  virtual HepPoint3D globalToPDPanel ( const HepPoint3D& globalPoint ) const = 0;

};

#endif // RICHKERNEL_IRICHSMARTIDTOOL_H
