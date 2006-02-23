// $Id: TrajMeasurementProvider.h,v 1.1 2006-02-23 16:55:20 erodrigu Exp $
#ifndef TRACKTOOLS_TRAJMEASUREMENTPROVIDER_H
#define TRACKTOOLS_TRAJMEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"
#include "TrackInterfaces/ITrajectoryProvider.h"

// from TrackEvent
#include "Event/Track.h"

using namespace LHCb;

static const InterfaceID IID_TrajMeasurementProvider ( "TrajMeasurementProvider", 1, 0 );

/** @class TrajMeasurementProvider TrajMeasurementProvider.h
 *  *
 *  @author Jose Angel Hernando Morata, Eduardo Rodrigues
 *  @date 2006-02-20
*/
class TrajMeasurementProvider : public GaudiTool,
                                virtual public IMeasurementProvider {
public:
  /// Standard constructor
  TrajMeasurementProvider( const std::string& type,
                           const std::string& name,
                           const IInterface* parent);
  
  virtual ~TrajMeasurementProvider( ); ///< Destructor
  
  StatusCode initialize();
  
  /** Load the necessary VeloClusters, ITClusters and OTTimes.
   * Note: this method should be called for each event
   *        before any call to load( Track& track )!
   */
  void load();
  
  /** Load (=create) all the Measurements (of TrajMeasurement kind)
   * from the list of LHCbIDs on the input Track
   */
  StatusCode load( Track& track );
  
  /** Construct a Measurement of the type of the input LHCbID
   * Note: this method is not for general use. A user should preferably call
   * the "load( Track& track )" method to load=create "in one go" all the
   * Measurements from the list of LHCbIDs on the Track.
   * This method is in fact called internally by "load( Track& track )".
   * @return Pointer the the Measurement created
   * @param id:  input LHCbID
   * @param par: extra parameter for the XxxMeasurement constructors
   *          (refer to XxxMeasurement.h for details)
   */
  Measurement* measurement( const LHCbID& id,
                            double par = 0. );
  
protected:
  // Interfaces
  ITrajectoryProvider* m_trajProvider;
};
#endif // TRACKTOOLS_TRAJMEASUREMENTPROVIDER_H
