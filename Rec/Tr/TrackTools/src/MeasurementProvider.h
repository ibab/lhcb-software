// $Id: MeasurementProvider.h,v 1.19 2009-01-19 11:22:58 dhcroft Exp $
#ifndef TRACKTOOLS_MEASUREMENTPROVIDER_H 
#define TRACKTOOLS_MEASUREMENTPROVIDER_H

// Include files
// -------------
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

// from LHCbKernel
#include "Kernel/LHCbID.h"
#include "Kernel/ISTClusterPosition.h"

// from TrackInterfaces
#include "TrackInterfaces/IMeasurementProvider.h"

// from TrackEvent
#include "Event/Track.h"

class MeasurementProvider : public GaudiTool,
				   virtual public IMeasurementProvider {
public:
  /** standard tool constructor */
  MeasurementProvider( const std::string& type, const std::string& name, const IInterface* parent);
  
  /** destructor */
  virtual ~MeasurementProvider( );

  /** initialize tool */
  StatusCode initialize();

  /** See interface class */
  StatusCode load( LHCb::Track& track ) const ;  
  
  /** See interface class */ 
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY=false) const ;
  
  /** See interface class */ 
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::ZTrajectory& ref, 
                                          bool localY=false) const ;
  
  /** See interface class */ 
  virtual void addToMeasurements( const std::vector<LHCb::LHCbID>& ids,
                                  std::vector<LHCb::Measurement*>& measurements,
                                  const LHCb::ZTrajectory& reftraj) const ;
  
  /** See interface class */ 
  virtual double nominalZ( const LHCb::LHCbID& id ) const ;
  
private:
  // Handles to actual measurement providers
  ToolHandle<IMeasurementProvider> m_veloRProvider ;
  ToolHandle<IMeasurementProvider> m_veloPhiProvider ;
  // extra tools for "lite" velo clusters
  ToolHandle<IMeasurementProvider> m_veloLiteRProvider ;
  ToolHandle<IMeasurementProvider> m_veloLitePhiProvider ;
  // Non velo providers
  ToolHandle<IMeasurementProvider> m_ttProvider ;
  ToolHandle<IMeasurementProvider> m_itProvider ;
  ToolHandle<IMeasurementProvider> m_otProvider ;
  ToolHandle<IMeasurementProvider> m_muonProvider ;

  std::string m_veloRTypeAndName;
  std::string m_veloPhiTypeAndName;
  std::string m_veloLiteRTypeAndName;
  std::string m_veloLitePhiTypeAndName;
  std::string m_ttTypeAndName;
  std::string m_itTypeAndName;
  std::string m_otTypeAndName;
  std::string m_muonTypeAndName;
  
  bool m_ignoreVelo;    ///< Ignore Velo hits
  bool m_veloLite;      ///< Work with lite velo clusters
  bool m_ignoreTT;      ///< Ignore TT hits
  bool m_ignoreIT;      ///< Ignore IT hits
  bool m_ignoreOT;      ///< Ignore OT hits
  bool m_ignoreMuon;    ///< Ignore Muon hits
  bool m_initializeReference; ///< Initialize measurement reference vector with closest state on track

  std::vector<const IMeasurementProvider*> m_providermap ;
};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
