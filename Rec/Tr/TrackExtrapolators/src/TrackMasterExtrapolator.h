#ifndef TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H
#define TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H 1

// Include files

// local
#include "TrackExtrapolator.h"
#include "DetDesc/ILVolume.h"

// Forward declarations
class ITransportSvc;
class Material;
class IStateCorrectionTool;
class ITrackExtraSelector;

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

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( LHCb::State& state,
                                double z,
                                LHCb::ParticleID partId = LHCb::ParticleID(211) );

 private:
  /// update transport matrix
  void updateTransportMatrix( const Gaudi::TrackMatrix& newStepF );

  /// z scatter
  double zScatter(const double z1,
		  const double z2 ) const;

  /// convert from transport service ticks to mm in the LHCb frame
  void transformToGlobal( const double zStep, const double zStart,
                         ILVolume::Intersections& intersept );

  // Multiple scattering tools
  IStateCorrectionTool* m_thinmstool;
  IStateCorrectionTool* m_thickmstool;

  // dE/dx tools
  IStateCorrectionTool* m_dedxtool;
  IStateCorrectionTool* m_elecdedxtool;

  /// extra selector
  ITrackExtraSelector* m_extraSelector;

  /// Pointer to the transport service
  ITransportSvc* m_transportSvc;

  bool m_upStream;                 ///< Flag to distinguish between up/downstr.

  // job options
  std::string m_thinmstoolname;    ///< name of thin MS correction tool
  std::string m_thickmstoolname;   ///< name of thick MS correction tool
  std::string m_dedxtoolname;      ///< name of dE/dx correction tool
  std::string m_elecdedxtoolname;  ///< name of electron's dE/dx correction tool
  std::string m_extraSelectorName; ///< extrapolator selector
  bool   m_applyMultScattCorr;     ///< turn on/off multiple scattering correction
  double m_thickWall;              ///< thick wall
  bool   m_applyEnergyLossCorr;    ///< turn on/off dE/dx correction
  double m_maxStepSize;            ///< maximum length of a step
  double m_minRadThreshold;        ///< minimal thickness of a wall
  double m_maxSlope;               ///< maximum slope of state vector
  double m_maxTransverse;          ///< maximum x,y position of state vector  

  /// turn on/off electron energy loss corrections
  bool   m_applyElectronEnergyLossCorr;
  double m_startElectronCorr;   ///< z start for electron energy loss
  double m_stopElectronCorr;    ///< z start for electron energy loss 

  bool m_debugLevel;
  double m_25m;
};

inline void TrackMasterExtrapolator::updateTransportMatrix
( const Gaudi::TrackMatrix& newStepF )
{
  // update F - after transport step
  Gaudi::TrackMatrix tempF( m_F );
  m_F = newStepF * tempF;
}

inline void 
TrackMasterExtrapolator::transformToGlobal( const double zStep,
                                            const double zStart,
                                            ILVolume::Intersections& intersept)
{
  // convert from transport service ticks to mm in the LHCb frame
  for (unsigned int iW = 0 ;  intersept.size() > iW ; ++iW ) {
    intersept[iW].first.first  = (intersept[iW].first.first*zStep)+zStart;
    intersept[iW].first.second = (intersept[iW].first.second*zStep)+zStart;
  }
}

#endif // TRACKEXTRAPOLATORS_TRACKMASTEREXTRAPOLATOR_H
