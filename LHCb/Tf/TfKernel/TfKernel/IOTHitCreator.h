// $Id: IOTHitCreator.h,v 1.1.1.1 2007-08-13 11:13:58 jonrob Exp $
#ifndef _TF_IOTDataSvc_H
#define _TF_IOTDataSvc_H

#include "GaudiKernel/IAlgTool.h"
#include "TfKernel/OTHit.h"
#include "TfKernel/Region.h"

namespace Tf
{
  /// Static ID object
  static const InterfaceID IID_IOTHitCreator( "Tf::IOTHitCreator", 0, 0 );

  /** @class IOTHitCreator IOTHitCreator.h
   *
   *  Interface to OT data provider
   *
   *  @author S. Hansmann-Menzemer, W. Houlsbergen, C. Jones, K. Rinnert
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
    virtual OTHitRange hits(int iStation, int iLayer) const = 0 ;

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
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion) const = 0 ;

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
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion, float xmin, float xmax) const = 0 ;

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
    virtual OTHitRange hits(int iStation, int iLayer, int iRegion,
                            float xmin, float xmax, float ymin, float ymax) const = 0 ;

    /** Retrieve the OTRegion for a certain region ID. The region
     *  knows its 'size' and gives access to its hits.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return OTRegion object
     */
    virtual const OTRegion* region(int iStation, int iLayer, int iRegion) const = 0 ;

  };
}

#endif
