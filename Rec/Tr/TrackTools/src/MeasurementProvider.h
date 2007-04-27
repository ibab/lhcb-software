// $Id: MeasurementProvider.h,v 1.13 2007-04-27 20:41:26 polye Exp $
#ifndef TRACKTOOLS_MEASUREMENTPROVIDER_H 
#define TRACKTOOLS_MEASUREMENTPROVIDER_H 1

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"
#include "Kernel/ISTClusterPosition.h"
#include "Kernel/IVeloClusterPosition.h"

// from TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/Track.h"

// from DigiEvent
#include "Event/VeloCluster.h"
#include "Event/STCluster.h"
#include "Event/OTTime.h"

// from XxxDet
#include "VeloDet/DeVelo.h"
#include "OTDet/DeOTDetector.h"
#include "STDet/DeSTDetector.h"
#include "MuonDet/DeMuonDetector.h"

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
  StatusCode load( LHCb::Track& track );  
  
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
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
                                  double par = 0. );
  
protected:
  // Interfaces
  ISTClusterPosition* m_ttPositionTool;  ///< ST cluster position tool for TT
  ISTClusterPosition* m_itPositionTool;  ///< ST cluster position tool for IT
  std::string m_ttPositionToolName; ///< ST cluster position tool name for TT
  std::string m_itPositionToolName; ///< ST cluster position tool name for IT

  IVeloClusterPosition* m_veloPositionTool; ///< Velo cluster position tool
  std::string  m_veloPositionToolName; ///< Velo cluster position tool name

  // Geometry information
  DeVelo*       m_veloDet;
  DeSTDetector* m_ttDet;
  DeSTDetector* m_itDet;
  DeOTDetector* m_otDet;
  DeMuonDetector* m_muDet;

  std::string   m_veloDetPath;        ///< Name of the Velo XML geom path
  std::string   m_ttDetPath;          ///< Name of the TT XML geom path
  std::string   m_itDetPath;          ///< Name of the IT XML geom path
  std::string   m_otDetPath;          ///< Name of the OT XML geom path
  std::string   m_muDetPath;          ///< Name of the Muon XML geom path
  
  LHCb::VeloClusters* m_veloClusters;
  LHCb::STClusters*   m_ttClusters;
  LHCb::STClusters*   m_itClusters;
  LHCb::OTTimes*      m_otTimes;

  bool m_ignoreVelo;    ///< Ignore Velo information
  bool m_ignoreTT;      ///< Ignore TT information
  bool m_ignoreIT;      ///< Ignore IT information
  bool m_ignoreOT;      ///< Ignore OT information
  bool m_ignoreMuon;      ///< Ignore Muon information

};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
