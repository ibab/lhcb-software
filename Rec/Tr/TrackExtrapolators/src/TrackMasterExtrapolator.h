#ifndef TRACKMASTEREXTRAPOLATOR_H
#define TRACKMASTEREXTRAPOLATOR_H 1

// Include files

// local
#include "TrackExtrapolator.h"
#include "TrackTransportStep.h"
#include "DetDesc/ILVolume.h"

// Forward declarations
class ITransportSvc;
class Material;

/** @class TrackMasterExtrapolator TrackMasterExtrapolator.h \
 *         "TrackMasterExtrapolator.h"
 *
 *  A TrackMasterExtrapolator is a ITrackExtrapolator
 *  which calls the other extrapolators to do the extrapolating.
 *  It takes into account:
 *  @li Detector Material (multiple scattering , energy loss)
 *  @li Magnetic field (based on start/stop z)
 *  @li most of the work done by  m_freeFieldExtrapolator, \
 *       m_shortFieldExtrapolator, m_longFieldExtrapolator
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
                                const IInterface* parent);

  /// destructor
  virtual ~TrackMasterExtrapolator();

  /// intialize
  virtual StatusCode initialize();
 

  /// Propagate a state to a given z-position
  virtual StatusCode propagate( State& state,
				double z,
				ParticleID partId = ParticleID(211) );

  // Propagate a state to the intersection point with a given plane
  StatusCode propagate( State& state,
                        const HepPlane3D& plane,
                        ParticleID pid = ParticleID(211) );

 private:
 
  /// make transport steps 
  StatusCode createTransportSteps( double zStart, 
                                   double zTarget, 
                                   std::list<TrackTransportStep>& transList );
  
  /// apply thick scatter state
  StatusCode thinScatter( State& state ,
                          double radLength );

  /// apply thick scatter state
  StatusCode thickScatter( State& state ,
                           double tWall, 
                           double radLength );

  /// apply energy loss state
  StatusCode energyLoss( State& state, 
                         double tWall, 
                         const Material* aMaterial );

  ///  electron energy loss state
  StatusCode electronEnergyLoss( State& state, 
                                 double radLength );

  /// choose Extrapolator to use in field free region
  ITrackExtrapolator* chooseMagFieldExtrapolator( const double zStart,
                                                  const double zTarget );

  bool m_upStream;
  int  m_particleType;
  double m_tMax ;     ///< max radiation length - avoid underflow on NT
  double m_eMax;      /// max energy loss is dE/dx corr  
  
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
 
  ITransportSvc* m_transportSvc;  ///< Pointer to the transport service

  /// update transport matrix
  void updateTransportMatrix( const HepMatrix& newStepF );

  /// z scatter
  double zScatter(const double z1,
		  const double z2 ) const;


  void transformToGlobal(const double zStep, const double zStart,
                         ILVolume::Intersections& intersept);


};

inline ITrackExtrapolator* 
TrackMasterExtrapolator::chooseMagFieldExtrapolator( const double zStart,
                                                     const double zTarget )
{
  //choose which extrapolator to use in magnetic field - simplifies the code
  return (fabs(zTarget-zStart)< m_shortDist ? m_shortFieldExtrapolator : m_longFieldExtrapolator);
}

inline void TrackMasterExtrapolator::updateTransportMatrix
(const HepMatrix& newStepF)
{
  //update F - after transport step
  m_F = newStepF* m_F;
}

inline void TrackMasterExtrapolator::transformToGlobal( const double zStep, const double zStart,
                                                        ILVolume::Intersections& intersept) {

  // convert from transport service ticks to mm in the LHCb frame
  for (unsigned int iW = 0 ;  intersept.size() > iW ; ++iW ) {
    intersept[iW].first.first  = (intersept[iW].first.first*zStep)+zStart;
    intersept[iW].first.second = (intersept[iW].first.second*zStep)+zStart;
  } // iW
}

#endif // TRACKMASTEREXTRAPOLATOR_H
