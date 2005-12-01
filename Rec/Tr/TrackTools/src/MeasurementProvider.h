// $Id: MeasurementProvider.h,v 1.4 2005-12-01 18:10:26 erodrigu Exp $
#ifndef TRACKTOOLS_MEASUREMENTPROVIDER_H 
#define TRACKTOOLS_MEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"

// from STTools
#include "STTools/ISTClusterPosition.h"

// from TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"            // Interface

// from TrackEvent
#include "Event/Track.h"

// from XxxEvent
#include "Event/VeloCluster.h"
#include "Event/ITCluster.h"
#include "Event/OTTime.h"

// from XxxDet
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "VeloDet/DeVelo.h"


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

  void load();

  StatusCode load( Track& track );  

  Measurement* measurement( const LHCbID&, 
                            double par0 = 999.,
                            double par1 = 999.);

protected:
  // Interfaces
  ISTClusterPosition* m_stPositionTool;     ///< ST cluster position tool

  // Geometry information
  DeOTDetector* m_otDet;
  DeSTDetector* m_itDet;
  DeVelo*       m_veloDet;

  std::string   m_otDetPath;          ///< Name of the OT XML geom path
  std::string   m_itDetPath;          ///< Name of the IT XML geom path
  std::string   m_veloDetPath;        ///< Name of the Velo XML geom path

  OTTimes*      m_otTimes;
  ITClusters*   m_itClusters;
  VeloClusters* m_veloClusters;

  std::string   m_measLocation;
  
};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
