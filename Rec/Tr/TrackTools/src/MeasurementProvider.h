// $Id: MeasurementProvider.h,v 1.15 2007-07-23 11:27:37 spozzi Exp $
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
#include "Kernel/IVeloClusterPosition.h"

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
  virtual LHCb::Measurement* measurement( const LHCb::LHCbID& id, const LHCb::StateVector& ref, 
					  bool localY=false) const ;

  /** See interface class */ 
  virtual StatusCode update(  LHCb::Measurement&, const LHCb::StateVector& refvector ) const ;

  /** See interface class */ 
  virtual double nominalZ( const LHCb::LHCbID& id ) const ;

protected:
  // Handles to actual measurement providers
  ToolHandle<IMeasurementProvider> m_veloRProvider ;
  ToolHandle<IMeasurementProvider> m_veloPhiProvider ;
  ToolHandle<IMeasurementProvider> m_ttProvider ;
  ToolHandle<IMeasurementProvider> m_itProvider ;
  ToolHandle<IMeasurementProvider> m_otProvider ;
  ToolHandle<IMeasurementProvider> m_muonProvider ;
  
  bool m_ignoreVelo;    ///< Ignore Velo hits
  bool m_ignoreTT;      ///< Ignore TT hits
  bool m_ignoreIT;      ///< Ignore IT hits
  bool m_ignoreOT;      ///< Ignore OT hits
  bool m_ignoreMuon;    ///< Ignore Muon hits
  bool m_initializeReference; ///< Initialize measurement reference vector with closest state on track

  std::vector<const IMeasurementProvider*> m_providermap ;
};
#endif // TRACKTOOLS_MEASUREMENTPROVIDER_H
