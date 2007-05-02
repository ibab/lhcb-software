
//---------------------------------------------------------------------------------
/** @file IRichSmartIDTool.h
 *
 *  Header file for tool interface : Rich::ISmartIDTool
 *
 *  $Id: IRichSmartIDTool.h,v 1.21 2007-05-02 13:29:46 cattanem Exp $
 *
 *  @author Antonis Papanestis  a.papanestis@rl.ac.uk
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
     *  global LHCb coordinates
     *
     *  @param[in]  smartid      The RichSmartID channel identifier
     *  @param[out] detectPoint  The detection point in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    virtual StatusCode globalPosition ( const LHCb::RichSmartID smartid,
                                        Gaudi::XYZPoint& detectPoint ) const = 0;

    /** Finds the average position of a cluster of RichSmartIDs, in global LHCb coordinates
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

    /** @brief Converts an HPD RichSmartID identification into a position in
     *  global LHCb coordinates.
     *
     *  Return coordinate is the centre of the HPD wafer in global LHCb coordinates.
     *
     *  @param[in]  hpdid      The RichSmartID HPD identifier
     *  @param[out] hpdPoint   The HPD position in global coordinates
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS The conversion to a global coordinate was successful
     *  @retval StatusCode::FAILURE The conversion to a global coordinate failed
     */
    virtual StatusCode hpdPosition ( const LHCb::RichSmartID hpdid,
                                     Gaudi::XYZPoint& hpdPoint ) const = 0;

    /** Computes the global position coordinate for a given position in local
     *  HPD panel coordinates and RICH detector and panel identifiers.
     *
     *  @param[in] localPoint   The local coordinate position
     *  @param[in] rich         The RICH detector
     *  @param[in] side         The RICH HPD panel
     *
     *  @return The position in global coordinates
     */
    virtual Gaudi::XYZPoint globalPosition ( const Gaudi::XYZPoint& localPoint,
                                             const Rich::DetectorType rich,
                                             const Rich::Side side ) const = 0;

    /** Converts a position in global coordinates to the corresponding
     *  RichSmartID identifier.
     *
     *  @param  globalPoint The global coordinate to convert
     *  @param  smartid     The RichSmartID channel identifier to fill
     *
     *  @return The status of the conversion
     *  @retval StatusCode::SUCCESS Conversion was successful (position in HPD acceptance)
     *  @retval StatusCode::FAILURE Conversion was not successful (position not in HPD acceptance)
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
     *  of the appropriate HPD panel
     *
     *  The panel Rich::Left has positive x and panel Rich::Right has negative
     *  x coordinates. Only the x and y coordinates are valid. z coordinate
     *  is set to 0.
     *
     *  @param[in] globalPoint  The global coordinate to convert
     *
     *  @return The local position on the HPD panel
     */
    virtual Gaudi::XYZPoint globalToPDPanel ( const Gaudi::XYZPoint& globalPoint ) const = 0;

  };

}

#endif // RICHKERNEL_IRICHSMARTIDTOOL_H
