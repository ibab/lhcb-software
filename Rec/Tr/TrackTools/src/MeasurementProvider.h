// $Id: MeasurementProvider.h,v 1.8 2006-02-17 17:23:25 erodrigu Exp $
#ifndef TRACKTOOLS_MEASUREMENTPROVIDER_H 
#define TRACKTOOLS_MEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"
#include "Kernel/ISTClusterPosition.h"

// from TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/Track.h"

// from XxxEvent
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// from XxxDet
#include "VeloDet/DeVelo.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"

using namespace LHCb;

static const InterfaceID IID_MeasurementProvider ( "MeasurementProvider", 1, 0 );

/** @class MeasurementProvider MeasurementProvider.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-04-14
 */
class MeasurementProvider : public GaudiTool,
                            virtual public IMeasurementProvider {
public:
  /// Standard constructor
  MeasurementProvider( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~MeasurementProvider( ); ///< Destructor

  StatusCode initialize();

  /** Load the necessary VeloClusters, ITClusters and OTTimes.
   *  Note: this method should be called for each event
   *        before any call to load( Track& track )!
   */
  void load();


  /** Load (=create) all the Measurements from the list of LHCbIDs
   *  on the input Track
   */
  StatusCode load( Track& track );  

  /** Construct a Measurement of the type of the input LHCbID
   *  Note: this method is not for general use. A user should preferably call
   *  the "load( Track& track )" method to load=create "in one go" all the
   *  Measurements from the list of LHCbIDs on the Track.
   *  This method is in fact called internally by "load( Track& track )".
   *  @return Pointer the the Measurement created
   *  @param  id:  input LHCbID
   *  @param  par: extra parameter for the XxxMeasurement constructors
   *          (refer to XxxMeasurement.h for details)
   */
  Measurement* measurement( const LHCbID& id, 
                            double par = 0. );

protected:
  // Interfaces
  ISTClusterPosition* m_stPositionTool;     ///< ST cluster position tool
  std::string         m_stPositionToolName; ///< ST cluster position tool name

  // Geometry information
  DeVelo*       m_veloDet;
  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;
  DeOTDetector* m_otDet;

  std::string   m_veloDetPath;        ///< Name of the Velo XML geom path
  std::string   m_ttDetPath;          ///< Name of the TT XML geom path
  std::string   m_itDetPath;          ///< Name of the IT XML geom path
  std::string   m_otDetPath;          ///< Name of the OT XML geom path

  OTTimes*      m_otTimes;
  STClusters*   m_ttClusters;
  STClusters*   m_itClusters;
  VeloClusters* m_veloClusters;

};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
