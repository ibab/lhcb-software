// $Id: TrackMasterExtrapolator.h,v 1.18 2010-04-07 21:08:38 wouter Exp $
#ifndef TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H
#define TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H 1

// Include files

// local
#include "GaudiKernel/ToolHandle.h" 
#include "TrackExtrapolator.h"
#include "DetDesc/ILVolume.h"

// Forward declarations
class ITransportSvc;
class Material;
class IStateCorrectionTool;
class ITrackExtraSelector;
class IMaterialLocator ;

/** @class TrackMasterExtrapolator TrackMasterExtrapolator.h \
 *         "TrackMasterExtrapolator.h"
 *
 *  A TrackMasterExtrapolator is a ITrackExtrapolator
 *  which calls the other extrapolators to do the extrapolating.
 *  It takes into account:
 *  @li Detector Material (multiple scattering , energy loss)
 *  @li Deals with electrons
 *  @li Checks the input state vector
 *  @li The actual extrapolation is chosen by the extrapolator selector \
 *       m_extraSelector
 *
 *  @author Edwin Bos (added extrapolation methods)
 *  @date   05/07/2005
 *  @author Jose A. Hernando
 *  @date   15-03-05
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackMasterExtrapolator: public TrackExtrapolator 
{

public:
  /// Constructor
  TrackMasterExtrapolator( const std::string& type, 
                           const std::string& name, 
                           const IInterface* parent );

  /// destructor
  virtual ~TrackMasterExtrapolator();

  /// intialize
  virtual StatusCode initialize();

  /// finalize
  virtual StatusCode finalize();

  using TrackExtrapolator::propagate;

  /// Propagate a state vector from zOld to zNew
  /// Transport matrix is calulated when transMat pointer is not NULL
  virtual StatusCode propagate( Gaudi::TrackVector& stateVec,
                                double zOld,
                                double zNew,
                                Gaudi::TrackMatrix* transMat,
                                LHCb::ParticleID pid = LHCb::ParticleID(211) ) const ;

  /// Propagate a state to a given z-position
  /// Transport matrix is calulated when transMat pointer is not NULL
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                Gaudi::TrackMatrix* transMat,
                                LHCb::ParticleID partId = LHCb::ParticleID(211) ) const ;

private:
  /// extra selector
  ToolHandle<ITrackExtraSelector> m_extraSelector;

  /// transport service
  IMaterialLocator* m_materialLocator ;
  
  // job options
  std::string m_materialLocatorname;   ///< name of materialLocator
  bool   m_applyMultScattCorr;     ///< turn on/off multiple scattering correction
  bool   m_applyEnergyLossCorr;    ///< turn on/off dE/dx correction
  double m_maxStepSize;            ///< maximum length of a step
  double m_maxSlope;               ///< maximum slope of state vector
  double m_maxTransverse;          ///< maximum x,y position of state vector  

  /// turn on/off electron energy loss corrections
  bool   m_applyElectronEnergyLossCorr;
  //double m_startElectronCorr;   ///< z start for electron energy loss
  //double m_stopElectronCorr;    ///< z start for electron energy loss 

  bool m_debugLevel;
};

#endif // TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H
