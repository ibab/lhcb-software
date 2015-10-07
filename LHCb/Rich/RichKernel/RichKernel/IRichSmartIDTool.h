
//---------------------------------------------------------------------------------
/** @file IRichSmartIDTool.h
 *
 *  Header file for tool interface : Rich::ISmartIDTool
 *
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-10-28
 */
//---------------------------------------------------------------------------------

#ifndef RICHKERNEL_IRICHSMARTIDTOOL_H
#define RICHKERNEL_IRICHSMARTIDTOOL_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// LHCbKernel
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichSide.h"

// Kernel
#include "Kernel/RichSmartID.h"

// from MathCore
#include "GaudiKernel/Point3DTypes.h"

// forward decs
namespace Rich
{
  class HPDPixelCluster;
}

/// Static Interface Identification
static const InterfaceID IID_IRichSmartIDTool( "Rich::ISmartIDTool", 1, 0 );

namespace Rich
{

  //---------------------------------------------------------------------------------
  /** @class ISmartIDTool IRichSmartIDTool.h RichKernel/IRichSmartIDTool.h
   *
   *  Interface to tools providing manipulation of RichSmartID channel identifiers
   *
   *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   2003-10-28
   */
  //---------------------------------------------------------------------------------

  class ISmartIDTool : virtual public IAlgTool
  {

  public:

    /** static interface identification
     *  @return unique interface identifier
     */
    static const InterfaceID& interfaceID() { return IID_IRichSmartIDTool; }

    /** Converts a RichSmartID channel identification into a position in
     *  global LHCb coordinates, on the PD entrance window.
     *
     *  @param[in]  smartid      The RichSmartID channel identifier
     *  @param[out] detectPoint  The detection point in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    virtual StatusCode globalPosition ( const LHCb::RichSmartID& smartid,
                                        Gaudi::XYZPoint& detectPoint ) const = 0;

    /** Finds the average position of a cluster of RichSmartIDs, in global LHCb coordinates
     *  on the PD entrance window.
     *
     *  @param[in]  cluster      The RichSmartID cluster
     *  @param[out] detectPoint  The detection point in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    virtual StatusCode globalPosition ( const Rich::HPDPixelCluster& cluster,
                                        Gaudi::XYZPoint& detectPoint ) const = 0;

    /** Converts a RichSmartID channel identification into a position in
     *  global LHCb coordinates, on the pixel anode chip.
     *
     *  @param[in]  smartid      The RichSmartID channel identifier
     *  @param[out] detectPoint  The detection point in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    //virtual StatusCode anodeGlobalPosition ( const LHCb::RichSmartID smartid,
    //                                         Gaudi::XYZPoint& detectPoint ) const = 0;

    /** Finds the average position of a cluster of RichSmartIDs, in global LHCb coordinates, 
     *  on the pixel anode chip.
     *
     *  @param[in]  cluster      The RichSmartID cluster
     *  @param[out] detectPoint  The detection point in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    //virtual StatusCode anodeGlobalPosition ( const Rich::HPDPixelCluster& cluster,
    //                                         Gaudi::XYZPoint& detectPoint ) const = 0;

    /** @brief Converts an PD RichSmartID identification into a position in
     *  global LHCb coordinates.
     *
     *  Return coordinate is the centre of the PD entrance window in global LHCb coordinates.
     *
     *  @param[in]  pdid      The RichSmartID PD identifier
     *  @param[out] pdPoint   The PD position in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    virtual StatusCode pdPosition ( const LHCb::RichSmartID& pdid,
                                    Gaudi::XYZPoint& pdPoint ) const = 0;

    /** Computes the global position coordinate (on PD entrance window) for a given 
     *  position in local PD panel coordinates and RICH detector and panel identifiers.
     *
     *  @param[in] localPoint   The local coordinate position
     *  @param[in] rich         The RICH detector
     *  @param[in] side         The RICH PD panel
     *
     *  @return The position in global coordinates
     */
    virtual Gaudi::XYZPoint globalPosition ( const Gaudi::XYZPoint& localPoint,
                                             const Rich::DetectorType rich,
                                             const Rich::Side side ) const = 0;

    /** Converts a position (on the pixel chip) in global coordinates to the corresponding
     *  RichSmartID identifier.
     *
     *  @param  globalPoint The global coordinate to convert
     *  @param  smartid     The RichSmartID channel identifier to fill
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS Conversion was successful (position in PD acceptance)
     *  @retval StatusCode::FAILURE Conversion was not successful (position not in PD acceptance)
     */
    virtual StatusCode smartID ( const Gaudi::XYZPoint& globalPoint,
                                 LHCb::RichSmartID& smartid ) const = 0;

    /** Supplies a vector of all currently active and valid channels in the RICH detectors
     *
     *  @return Vector of RichSmartIDs to fill with the valid channels
     *  @retval Vector size > 0 : Request was successful, RichSmartID vector is valid
     *  @retval Vector size = 0 : Request was UNsuccessful, RichSmartID vector is not valid
     */
    virtual const LHCb::RichSmartID::Vector& readoutChannelList( ) const = 0;

    /** Converts a position in global coordinates to the local coordinate system
     *  of the appropriate PD panel
     *
     *  The panel Rich::Left has positive x and panel Rich::Right has negative
     *  x coordinates. Only the x and y coordinates are valid. z coordinate
     *  is set to 0.
     *
     *  @param[in] globalPoint  The global coordinate to convert
     *
     *  @return The local position on the PD panel
     */
    virtual Gaudi::XYZPoint globalToPDPanel ( const Gaudi::XYZPoint& globalPoint ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHSMARTIDTOOL_H
