// $Id: MeasurementProvider.h,v 1.3 2005-05-17 10:00:24 erodrigu Exp $
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

  void load();

  StatusCode load( Track& track );  

  Measurement& measurement( const LHCbID&, 
                            double par0 = 999.,
                            double par1 = 999.);

protected:

  // Geometry information
  DeOTDetector* m_otDet;
  DeSTDetector* m_itDet;
  DeVelo*       m_veloDet;  // Geometry information

  std::string   m_otDetPath;   ///< Name of the OT XML geom path
  std::string   m_itDetPath;   ///< Name of the IT XML geom path
  std::string   m_veloDetPath;        ///< Name of the Velo XML geom path

  OTTimes*      m_otTimes;
  ITClusters*   m_itClusters;
  VeloClusters* m_veloClusters;

  std::string m_measLocation;
  
};
#endif // MEASUREMENTPROVIDER_H
