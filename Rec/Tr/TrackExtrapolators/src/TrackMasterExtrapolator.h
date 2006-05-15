#ifndef TRACKMASTEREXTRAPOLATOR_H
#define TRACKMASTEREXTRAPOLATOR_H 1

// Include files

// local
#include "TrackExtrapolator.h"
#include "DetDesc/ILVolume.h"

// Forward declarations
class ITransportSvc;
class Material;
class ITrackExtraSelector;

using namespace Gaudi;
using namespace LHCb;

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
  virtual StatusCode propagate( State& state,
                                double z,
                                ParticleID partId = ParticleID(211) );

 private:
   
  /// apply thick scatter state
  void thinScatter( State& state, double radLength );

  /// apply thick scatter state
  void thickScatter( State& state, double tWall, double radLength );

  /// apply energy loss state
  void energyLoss( State& state, double tWall, const Material* aMaterial );

  ///  electron energy loss state
  void electronEnergyLoss( State& state, double radLength );

  /// update transport matrix
  void updateTransportMatrix( const TrackMatrix& newStepF );

  /// z scatter
  double zScatter(const double z1,
		  const double z2 ) const;

  /// convert from transport service ticks to mm in the LHCb frame
  void transformToGlobal( const double zStep, const double zStart,
                         ILVolume::Intersections& intersept );

  /// extra selector
  ITrackExtraSelector* m_extraSelector;

  /// Pointer to the transport service
  ITransportSvc* m_transportSvc;

  bool m_upStream;                 ///< Flag to distinguish between up/downstr.
  double m_tMax ;                  ///< max rad length - avoid underflow on NT
  double m_eMax;                   ///< max energy loss is dE/dx corr

  // job options
  std::string m_extraSelectorName; ///< extrapolator selector
  bool   m_applyMultScattCorr;     ///< turn on/off multiple scattering correctn
  double m_fms2;                   ///< factor for inflating scattering errors
  double m_thickWall;              ///< thick wall
  bool   m_applyEnergyLossCorr;    ///< turn on/off dE/dx correction
  double m_energyLoss;             ///< tuneable energy loss correction      
  double m_maxStepSize;            ///< maximum length of a step
  double m_minRadThreshold;        ///< minimal thickness of a wall
  double m_maxSlope;               ///< maximum slope of state vector
  double m_maxTransverse;          ///< maximum x,y position of state vector  

  /// turn on/off electron energy loss correction
  bool   m_applyElectronEnergyLossCorr;
  double m_startElectronCorr;   ///< z start for electron energy loss
  double m_stopElectronCorr;    ///< z start for electron energy loss 

};

inline void TrackMasterExtrapolator::updateTransportMatrix
( const TrackMatrix& newStepF )
{
  // update F - after transport step
  TrackMatrix tempF( m_F );
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

#endif // TRACKMASTEREXTRAPOLATOR_H
