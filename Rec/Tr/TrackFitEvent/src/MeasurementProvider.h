// $Id: MeasurementProvider.h,v 1.1 2005-04-14 17:27:31 hernando Exp $
#ifndef MEASUREMENTPROVIDER_H 
#define MEASUREMENTPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

#include "Event/Track.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "VeloDet/DeVelo.h"

#include "Event/VeloCluster.h"
#include "Event/ITCluster.h"
#include "Event/OTTime.h"

static const InterfaceID IID_MeasurementProvider ( "MeasurementProvider", 1, 0 );

/** @class MeasurementProvider MeasurementProvider.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2005-04-14
 */
class MeasurementProvider : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_MeasurementProvider; }

  /// Standard constructor
  MeasurementProvider( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~MeasurementProvider( ); ///< Destructor

  StatusCode initialize();

  StatusCode load();

  const Measurement& measurement(const LHCbID&, 
                                 double par = 0, double par = 0.);

  StatusCode load(Track& track);  

protected:

  // Geometry information
  DeOTDetector* m_otDet;
  DeSTDetector* m_itDet;
  DeVelo*       m_veloDet;  // Geometry information

  std::string   m_otDetPath;   ///< Name of the OT XML geom path
  std::string   m_itDetPath;   ///< Name of the IT XML geom path
  std::string   m_veloDetPath;        ///< Name of the Velo XML geom path

  OTTimes*   m_otTimes;
  ITClusters*   m_itClusters;
  VeloClusters* m_veloClusters;

  Measurements* m_meas;

  std::string m_measLocation;
  
};
#endif // MEASUREMENTPROVIDER_H
