// $Id: IdealStateCreator.h,v 1.12 2007-06-09 10:46:02 mneedham Exp $
#ifndef TRACKMCTOOLS_IDEALSTATECREATOR_H
#define TRACKMCTOOLS_IDEALSTATECREATOR_H 1

// Include files

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from MCInterfacces
#include "MCInterfaces/IIdealStateCreator.h"

// from TrackInterfacces
#include "TrackInterfaces/ITrackExtrapolator.h"

// incident service
#include "GaudiKernel/IIncidentListener.h"

// from Event/LinkerEvent
#include "Linker/LinkedFrom.h"

#include <string>
#include <vector>

namespace LHCb
{ 
  class State;
  class MCParticle;
  class MCHit;
}

class IMagneticFieldSvc;

/** @class IdealStateCreator IdealStateCreator.h "TrackMCTools/IdealStateCreator.h"
 * 
 *  An IdealStateCreator is an IIdealStateCreator tool that creates a
 *  State. There are two methods: one creates a state at a certain 
 *  z-position using the closest two extry/exit points from a MCParticle, 
 *  and a track extrapolator. The other creates a state at the vertex, using 
 *  the properties of the MCParticles and MCVertex.
 *
 *  The diagonal elements of the covariance matrix are set with
 *  the job options. Note that "eP" is dp/p.
 *
 *  Moved to LHCb v20r0. Adapted code to use updated Det packages.
 *  @author Edwin Bos
 *  @date   2006-02-02
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-06
 *
 *  @author Rutger van der Eijk, Jeroen van Tilburg
 *  @date   3-7-2002
 */

class IdealStateCreator: public GaudiTool,
                         virtual public IIdealStateCreator,  
                         virtual public IIncidentListener {
public:

  /// Standard constructor
  IdealStateCreator( const std::string& type,
                     const std::string& name,
                     const IInterface* parent );

  /// Destructor
  virtual ~IdealStateCreator();

  /// Tool initialization
  virtual StatusCode initialize();

  /** This method creates a state at z position from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  zRec   The z-position at which the state will be created
   *  @param  pState The ref to the State which is created.
   */
  virtual StatusCode createState( const LHCb::MCParticle* mcPart,
                                  double zRec,
                                  LHCb::State& pState ) const;


  /** This method creates a state at z position from a MCHit
   *  using the entry/exit points of the MCHit.
   *  @return StatusCode
   *  @param  mcHit The MCHit from which the state will be created
   *  @param  zRec   The z-position at which the state will be created
   *  @param  pState The ref to the State which is created.
   */
  virtual StatusCode createState( const LHCb::MCHit* mcHit,
                                  double zRec,
                                  LHCb::State& pState ) const;

  /** This method creates a state at the origin vertex from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  pState The ref to the State which is created.
   */
  virtual StatusCode createStateVertex( const LHCb::MCParticle* mcPart,
                                        LHCb::State& pState ) const;


  
  /** This method creates a state at z position from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  zRec   The z-position at which the state will be created
   *  @param  pVec The StateVector which is created.
   */
  virtual StatusCode createStateVector( const LHCb::MCParticle* mcPart,
                                  double zRec,
                                  LHCb::StateVector& pVec ) const;


  /** This method creates a state at z position from a MCHit
   *  using the entry/exit points of the MCHit.
   *  @return StatusCode
   *  @param  mcHit The MCHit from which the state will be created
   *  @param  zRec   The z-position at which the state will be created
   *  @param pVec StateVector which is created.
   */
  virtual StatusCode createStateVector( const LHCb::MCHit* mcHit,
                                  double zRec,
                                  LHCb::StateVector& pVec ) const;

  /** This method creates a state at the origin vertex from a MCParticle
   *  using the entry/exit points of the MCHits.
   *  @return StatusCode
   *  @param  mcPart The MCParticle from which the state will be created
   *  @param  pVec the StateVector which is created.
   */
  virtual StatusCode createStateVectorVertex( const LHCb::MCParticle* mcPart,
                                        LHCb::StateVector& pVec ) const;


  
  /** Implement the handle method for the Incident service.
  *  This is used to inform the tool of software incidents.
  *
  *  @param incident The incident identifier
  */
  void handle( const Incident& incident );

private:

  void initEvent() const;

  typedef LinkedFrom<LHCb::MCHit,LHCb::MCParticle> HitLinks;

  /** Find the z-closest MCHit associated to an MCParticle
      looping over the hits in all the tracking detectors
  */
  LHCb::MCHit* findClosestHit(const LHCb::MCParticle* mcPart,
                              const double zRec ) const;
 
  double  qOverP(const LHCb::MCHit* mcHit) const;

  double  qOverP(const LHCb::MCParticle* mcPart) const;

  /// Correct slopes for magnetic field given an MCHit and a MCParticle
  void correctSlopes( const LHCb::MCHit* mcHit,
                      double& tx, double& ty ) const;

  IMagneticFieldSvc*      m_magSvc;       ///< Pointer to magnetic field service

  ITrackExtrapolator*      m_extrapolator; ///< Extrapolator Tool
  std::string m_extrapolatorName;  ///< Name of track state extrapolator

  std::vector<std::string> m_dets;
  mutable std::vector<HitLinks> m_links;

  double m_eX2;                    ///< Error^2 on x
  double m_eY2;                    ///< Error^2 on y
  double m_eTx2;                   ///< Error^2 on slope x
  double m_eTy2;                   ///< Error^2 on slope y
  double m_eP;                     ///< dp/p

  bool m_correctSlopes;            ///< Correct for the magnetic field effect
  mutable bool m_configured;

};

#endif // TRACKMCTOOLS_IDEALSTATECREATOR_H
