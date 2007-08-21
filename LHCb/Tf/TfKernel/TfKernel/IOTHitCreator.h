
//-----------------------------------------------------------------------------
/** @file IOTHitCreator.h
 *
 *  Header file for class : Tf::IOTHitCreator
 *
 *  $Id: IOTHitCreator.h,v 1.7 2007-08-21 22:44:51 jonrob Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_IOTHitCreator_H
#define TFKERNEL_IOTHitCreator_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/Region.h"
#include "TfKernel/TfIDTypes.h"

namespace Tf
{
  /// Static ID object
  static const InterfaceID IID_IOTHitCreator( "Tf::IOTHitCreator", 0, 0 );

  /** @class IOTHitCreator IOTHitCreator.h
   *
   *  Interface to OT data provider. Creates, on demand, the low level 
   *  Tf::OTHit data objects and provides access to these objects by region.
   *
   *  Methods are provided to return the hits in a selected part of the OT detectors.
   *  E.g.
   *
   *  @code
   *  // Get all the hits in the IT stations
   *  Tf::OTHitRange range = hitMan->hits();
   *
   *  // Get all the IT hits in one specific T station
   *  Tf::TStationID sta = ...;
   *  Tf::OTHitRange range = hitMan->hits(sta);
   *
   *  // Get all the IT hits in one specific layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::OTHitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the IT hits in a specific 'region' of one layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::OTRegionID reg = ...;
   *  Tf::OTHitRange range = hitMan->hits(sta,lay,reg);
   *  @endcode
   * 
   *  In all cases the returned Range object acts like a standard vector or container :-
   *
   *  @code
   *   // Iterate over the returned range
   *  for ( Tf::OTHitRange::const_iterator iR = range.begin();
   *        iR != range.end(); ++iR )
   *  {
   *    // do something with the hit
   *  }
   *  @endcode
   *
   *  This interface also provides access to the Tf::IOTHitCreator::OTRegion object, that can be used to
   *  get detailed information about the geomterical properties of an OT region.
   *
   *  @code
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::OTRegionID reg = ...;
   *  Tf::IOTHitCreator::OTRegion* region = hitMan->region(sta,lay,reg);
   *  @endcode
   *
   *  See the Tf::IOTHitCreator::OTRegion and Tf::EnvelopeBase classes for more details.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class IOTHitCreator : virtual public IAlgTool
  {

  public:

    /// OT Hit Region type
    typedef Tf::Region<OTHit> OTRegion ;

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IOTHitCreator ; }

    /** Load all the OT hits
     *  @return Range object for the hits ion the selected region of interest
     */
    virtual OTHitRange hits() const = 0 ;

    /** Load the hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual OTHitRange hits(const TStationID iStation, 
                            const TLayerID iLayer) const = 0 ;

    /** Load the hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual OTHitRange hits(const TStationID iStation, 
                            const TLayerID iLayer, 
                            const OTRegionID iRegion) const = 0 ;

    /** Load the hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *  @param[in] xmin     Minimum x value for region of interest
     *  @param[in] xmax     Maximum x value for region of interest
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual OTHitRange hits(const TStationID iStation, 
                            const TLayerID iLayer, 
                            const OTRegionID iRegion, 
                            const float xmin, 
                            const float xmax) const = 0 ;

    /** Load the hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *  @param[in] xmin     Minimum x value for region of interest
     *  @param[in] xmax     Maximum x value for region of interest
     *  @param[in] ymin     Minimum y value for region of interest
     *  @param[in] ymax     Maximum y value for region of interest
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual OTHitRange hits(const TStationID iStation, 
                            const TLayerID iLayer, 
                            const OTRegionID iRegion,
                            const float xmin, 
                            const float xmax, 
                            const float ymin,
                            const float ymax) const = 0 ;

    /** Retrieve the OTRegion for a certain region ID. The region
     *  knows its 'size' and gives access to its hits.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Pointer to the OTRegion object
     */
    virtual const OTRegion* region(const TStationID iStation, 
                                   const TLayerID iLayer, 
                                   const OTRegionID iRegion) const = 0 ;

  };
}

#endif // TFKERNEL_IOTHitCreator_H
