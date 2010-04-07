
//-----------------------------------------------------------------------------
/** @file IITHitCreator.h
 *
 *  Header file for class : Tf::IITHitCreator
 *
 *  $Id: IITHitCreator.h,v 1.5 2010-04-07 05:18:00 wouter Exp $
 *
 *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
 *  @date   2007-06-01
 */
//-----------------------------------------------------------------------------

#ifndef TFKERNEL_IITHitCreator_H
#define TFKERNEL_IITHitCreator_H 1

#include "GaudiKernel/IAlgTool.h"
#include "TfKernel/STHit.h"
#include "TfKernel/Region.h"
#include "TfKernel/TfIDTypes.h"

namespace Tf
{
  /// Static ID object
  static const InterfaceID IID_IITHitCreator( "Tf::IITHitCreator", 0, 0 );

  /** @class IITHitCreator IITHitCreator.h
   *
   *  Interface to ST data provider. Creates, on demand, the low level 
   *  Tf::STHit data objects and provides access to these objects by region.
   *
   *  Methods are provided to return the hits in a selected part of the ST detectors.
   *
   *  E.g. for IT :-
   *
   *  @code
   *  // Get all the hits in the IT stations
   *  Tf::STHitRange range = hitMan->itHits();
   *
   *  // Get all the IT hits in one specific T station
   *  Tf::TStationID sta = ...;
   *  Tf::STHitRange range = hitMan->hits(sta);
   *
   *  // Get all the IT hits in one specific layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::STHitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the IT hits in a specific 'region' of one layer of one T station
   *  Tf::TStationID sta = ...;
   *  Tf::TLayerID   lay = ...;
   *  Tf::ITRegionID reg = ...;
   *  Tf::STHitRange range = hitMan->hits(sta,lay,reg);
   *  @endcode
   *
   *  Or, for TT :-
   *
   *  @code
   *  // Get all the hits in the TT stations
   *  Tf::STHitRange range = hitMan->ttHits();
   *
   *  // Get all the TT hits in one specific TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::STHitRange range = hitMan->hits(sta);
   *
   *  // Get all the TT hits in one specific layer of one TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::TTLayerID   lay = ...;
   *  Tf::STHitRange range = hitMan->hits(sta,lay);
   *
   *  // Get all the TT hits in a specific 'region' of one layer of one TT station
   *  Tf::TTStationID sta = ...;
   *  Tf::TTLayerID   lay = ...;
   *  Tf::TTRegionID  reg = ...;
   *  Tf::STHitRange range = hitMan->hits(sta,lay,reg);
   *  @endcode
   *
   *  In all cases the returned Range object acts like a standard vector or container :-
   *
   *  @code
   *   // Iterate over the returned range
   *  for ( Tf::STHitRange::const_iterator iR = range.begin();
   *        iR != range.end(); ++iR )
   *  {
   *    // do something with the hit
   *  }
   *  @endcode
   *
   *  This interface also provides access to the Tf::IITHitCreator::STRegion object, that can be used to
   *  get detailed information about the geomterical properties of an ST region
   *
   *  @code
   *  // Get region information for IT
   *  Tf::TStationID  itsta = ...;
   *  Tf::TLayerID    itlay = ...;
   *  Tf::ITRegionID  itreg = ...;
   *  Tf::IITHitCreator::STRegion* region = hitMan->region(itsta,itlay,itreg);
   *  // Get region information for TT
   *  Tf::TTStationID ttsta = ...;
   *  Tf::TTLayerID   ttlay = ...;
   *  Tf::TTRegionID  ttreg = ...;
   *  Tf::IITHitCreator::STRegion* region = hitMan->region(ttsta,ttlay,ttreg);
   *  @endcode
   *
   *  See the Tf::IITHitCreator::STRegion and Tf::EnvelopeBase classes for more details.
   *
   *  @author S. Hansmann-Menzemer, W. Hulsbergen, C. Jones, K. Rinnert
   *  @date   2007-06-01
   */

  class IITHitCreator : virtual public IAlgTool
  {
  public:

    /// The region type for TT hit
    typedef Tf::Region<STHit> STRegion ;

    /// Retrieve interface ID
    static const InterfaceID& interfaceID() { return IID_IITHitCreator ; }

    /* Reset used flags of hits */
    virtual void resetUsedFlagOfHits() const = 0;
    
    /** Load all the IT hits
     *  @return Range object for the hits ion the selected region of interest
     */
    virtual STHitRange hits() const = 0 ;

    /** Load the IT hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer) const = 0 ;

    /** Load the IT hits for a given region of interest
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
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion) const = 0 ;

    /** Load the IT hits for a given region of interest
     *
     * @attention This call may lead to loading on demand and
     *            may invalidate iterators returned by a previous call.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *  @param[in] xmin     Minimum local x value for region of interest
     *  @param[in] xmax     Maximum local x value for region of interest
     *
     *  @return Range object for the hits in the selected region of interest
     */
    virtual STHitRange hitsLocalXRange(const TStationID iStation,
				       const TLayerID iLayer,
				       const ITRegionID iRegion,
				       const double xminT,
				       const double xmaxT) const = 0 ;

    /** Load the IT hits for a given region of interest
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
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion,
                            const double xmin,
                            const double xmax) const = 0 ;

    /** Load the IT hits for a given region of interest
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
    virtual STHitRange hits(const TStationID iStation,
                            const TLayerID iLayer,
                            const ITRegionID iRegion,
                            const double xmin,
                            const double xmax,
                            const double ymin,
                            const double ymax) const = 0 ;

    /** Retrieve the IT STRegion for a certain region ID. The region
     *   knows its 'size' and gives access to its hits.
     *
     *  @param[in] iStation Station ID
     *  @param[in] iLayer   Station layer ID
     *  @param[in] iRegion  Region within the layer
     *
     *  @return Pointer to the STRegion object
     */
    virtual const STRegion* region(const TStationID iStation,
                                   const TLayerID   iLayer,
                                   const ITRegionID iRegion) const = 0 ;

    
    /** create a single STHit from an STChannelID. note: not very fast.
     * @param[in] stid STChannelID
     * @ return a Tf::STHit
     */
    virtual Tf::STHit hit(LHCb::STChannelID stid) const = 0 ;
    
  };
}

#endif // TFKERNEL_IITHitCreator_H
