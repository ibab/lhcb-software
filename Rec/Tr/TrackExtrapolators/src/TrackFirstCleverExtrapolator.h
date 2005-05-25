// $Id: TrackFirstCleverExtrapolator.h,v 1.2 2005-05-25 14:24:35 cattanem Exp $
#ifndef TRACKFIRSTCLEVEREXTRAPOLATOR_H
#define TRACKFIRSTCLEVEREXTRAPOLATOR_H 1

// Include files

// local
#include "TrackExtrapolator.h"
#include "TrackTransportStep.h"

/** @class TrackFirstCleverExtrapolator TrackFirstCleverExtrapolator.h \
 *         "TrackFirstCleverExtrapolator.h"
 *
 *  A TrackFirstCleverExtrapolator is a ITrExtrapolator which does a 'FirstClever'
 *  extrapolation of a TrState. It takes account of:
 *   @li Detector Material (multiple scattering , energy loss)
 *   @li Magnetic field (based on start/stop z)
 *   @li most of the work done by  m_freeFieldExtrapolator, \
 *       m_shortFieldExtrapolator, m_longFieldExtrapolator
 *
 *  @author Jose A. Hernando (15-03-05)
 *  @author Matt Needham
 *  @date   22-04-2000
 */

class TrackFirstCleverExtrapolator: public TrackExtrapolator 
{

public:
  /// Constructor
  TrackFirstCleverExtrapolator( const std::string& type, 
                                const std::string& name, 
                                const IInterface* parent);

  /// destructor
  virtual ~TrackFirstCleverExtrapolator();

  /// intialize and finalize
  virtual StatusCode initialize();
 
  /// propagate with Linear state
  virtual StatusCode propagate(State& state, double zNew = 0,
                               ParticleID partId = ParticleID(211));

private:
 
  /// make transport steps 
  StatusCode createTransportSteps( double zStart, 
                                   double zTarget, 
                                   std::list<TrackTransportStep>& transList);
  
  /// apply thick scatter Q/p state
  StatusCode thinScatter( State& state ,
                          double radLength);

  /// apply thick scatter Q/p state
  StatusCode thickScatter( State& state ,
                           double tWall, 
                           double radLength);

  /// apply energy loss P state
  StatusCode energyLoss( State& state, 
                         double tWall, 
                         const Material* aMaterial);

  ///  electron energy loss Q/p state
  StatusCode electronEnergyLoss( State& state, 
                                 double radLength);

  /// choose Extrapolator to use in field free region
  ITrackExtrapolator* chooseMagFieldExtrapolator( const double zStart,
                                                  const double zTarget);

  bool m_upStream;
  int  m_particleType;
  double m_tMax ;     ///< max radiation length - avoid underflow on NT
  double m_eMax;  /// max energy loss is dE/dx corr  
  
  ///job options
  double m_zFieldStart;        ///< start of field
  double m_zFieldStop;         ///< end of field
  double m_shortDist;          ///< min distance to use RungaKutta

  /// extrapolator to use in field free region
  std::string m_freeFieldExtrapolatorName;  
  /// extrapolator to use for short transport in mag field   
  std::string m_shortFieldExtrapolatorName;
  /// extrapolator to use for long transport in mag field 
  std::string m_longFieldExtrapolatorName;

  bool   m_applyMultScattCorr;  ///< turn on/off multiple scattering correction
  double m_fms2;                ///< factor for inflating scattering errors
  double m_thickWall;           ///< thick wall
  bool   m_applyEnergyLossCorr; ///< turn on/off multiple scattering correction
  double m_energyLoss;          ///< tuneable energy loss correction      
  double m_maxStepSize;         ///< maximum length of a step
  double m_minRadThreshold;     ///< minimal thickness of a wall

  /// turn on/off electron energy loss correction
  bool   m_applyElectronEnergyLossCorr;
  double m_startElectronCorr;   ///< z start for electron energy loss
  double m_stopElectronCorr;    ///< z start for electron energy loss

  /// extrapolators
  ITrackExtrapolator* m_freeFieldExtrapolator;
  ITrackExtrapolator* m_shortFieldExtrapolator;
  ITrackExtrapolator* m_longFieldExtrapolator;
 
  ITransportSvc*   m_transportSvc;  ///< Pointer to the transport service

  /// update transport matrix
  void updateTransportMatrix( const HepMatrix& newStepF );

  /// z scatter
  double zScatter(const double z1, const double z2 ) const;

};

inline ITrackExtrapolator* 
TrackFirstCleverExtrapolator::chooseMagFieldExtrapolator
(const double zStart, const double zTarget)
{
  //choose which extrapolator to use in magnetic field - simplifies the code
  return (fabs(zTarget-zStart)< m_shortDist ? m_shortFieldExtrapolator :  m_longFieldExtrapolator);
}

inline void TrackFirstCleverExtrapolator::updateTransportMatrix
(const HepMatrix& newStepF) {
  //update F - after transport step
  m_F = newStepF* m_F;
}
#endif // TRACKFIRSTCLEVEREXTRAPOLATOR_H
