
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef TrEvent_Track_H
#define TrEvent_Track_H 1

// Include files
#include "Event/State.h"
#include "Kernel/LHCbID.h"
#include "CLHEP/Geometry/Plane3D.h"
#include "GaudiKernel/KeyedObject.h"
#include "GaudiKernel/KeyedContainer.h"
#include <vector>


// Forward declarations
class StatusCode;
class Point3D;
class Vector3D;
class SymMatrix;


// Class ID definition
static const CLID& CLID_Track = 10010;

// Namespace for locations in TDS
namespace TrackLocation {
  static const std::string& Default = "Rec/Track/Best";
  static const std::string& Velo = "Rec/Track/Velo";
  static const std::string& Seed = "Rec/Track/Seed";
  static const std::string& Match = "Rec/Track/Match";
  static const std::string& Forward = "Rec/Track/Forward";
  static const std::string& Follow = "Rec/Track/Follow";
  static const std::string& VeloTT = "Rec/Track/VeloTT";
  static const std::string& KsTrack = "Rec/Track/KsTrack";
}


/** @class Track Track.h
 *
 * Track is the base class for offline and online tracks. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Feb 10 18:12:23 2005
 * 
 */

class Track: public KeyedObject<int>
{
public:

  /// enumeration for the track type
  enum TrackType{ Velo=1,
                  VeloR=2,
                  Backward=4,
                  Long=8,
                  Upstream=16,
                  Downstream=32,
                  Ttrack=64 };
  /// enumeration for the track history
  enum HistoryFlag{ LongTrack=1,
                    Seeding=2,
                    TrKshort=4,
                    TrMatching=8,
                    VeloTrack=16,
                    VeloTT=32,
                    TrgForward=64,
                    TrgVelo=128,
                    TrgVeloTT=256 };
  /// enumeration for the track flags
  enum Flag{ Valid=1,
             Unique=2 };

  /// Default Constructor
  Track() : m_chi2PerDoF(0.0),
            m_nDoF(0),
            m_flags(0),
            m_lhcbIDs(),
            m_physicsState(),
            m_states() {}

  /// Track destructor
  ~Track();

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Retrieve the position and momentum vectors and the corresponding 6D covariance matrix (pos:1->3,mom:4-6) of a track closest to the beam-line
  StatusCode positionAndMomentum(HepPoint3D& pos,
                                 HepVector3D& mom,
                                 HepSymMatrix& cov6D) const;

  /// Retrieve the 3D-position (+ errors) of a track closest to the beam-line
  StatusCode position(HepPoint3D& pos,
                      HepSymMatrix& errPos) const;

  /// Retrieve the slopes (dx/dz,dy/dz,1) of a track closest to the beam-line
  StatusCode slopes(HepVector3D& slopes,
                    HepSymMatrix& errSlopes) const;

  /// Retrieve the momentum of a track closest to the beam-line
  double p() const;

  /// Retrieve the transverse momentum of a track closest to the beam-line
  double pt() const;

  /// Retrieve the momentum vector (+ errors) of a track closest to the beam-line
  StatusCode momentum(HepVector3D& mom,
                      HepSymMatrix& errMom) const;

  /// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of a track closest to the beam-line
  StatusCode posMomCovariance(HepSymMatrix& cov6D) const;

  /// Retrieve the charge assigned to the track
  int charge() const;

  /// number of states on track
  unsigned int nStates() const;

  /// Add a State to the list of states associated to the track
  void addToStates(State* value);

  /// Remove a State from the list of states associated to the track
  void removeFromStates(State* value);

  /// Retrieve the pointer to the state closest to the given z-position
  virtual State & closestState(double z);

  /// Retrieve the pointer to the state closest to the given z-position
  virtual const State & closestState(double z) const;

  /// Retrieve the pointer to the state closest to the given plane
  virtual State & closestState(const HepPlane3D& plane);

  /// Retrieve the pointer to the state closest to the given plane
  virtual const State & closestState(const HepPlane3D& plane) const;

  /// check the existence of a state at a certain predefined location. See State class.
  virtual bool hasStateAt(const State::Location& value) const;

  /// Retrieve the pointer to the state at a certain predefined location. See State class.
  virtual State* stateAt(const State::Location& value);

  /// Retrieve the pointer to the state at a certain predefined location. See State class.
  virtual const State* stateAt(const State::Location& value) const;

  /// Retrieve the Chi^2 of the track
  double chi2() const;

  /// Clear the track before re-use
  virtual void reset();

  /// Clone the track
  virtual Track* clone() const;

  /// Add an LHCbID to the list of LHCbIDs associated to the track
  void addToLhcbIDs(const LHCbID& value);

  /// Remove an LHCbID from the list of LHCbIDs associated to the track
  void removeFromLhcbIDs(const LHCbID& value);

  /// Check whether the track was produced by a given algorithm
  virtual bool producedByAlgo(const HistoryFlag& value) const;

  /// Update the name of the algorithm that produced the track
  virtual void setProducedByAlgo(const HistoryFlag& value);

  /// Retrieve const  chi^2 per degree of freedom of the track
  double chi2PerDoF() const;

  /// Update  chi^2 per degree of freedom of the track
  void setChi2PerDoF(double value);

  /// Retrieve const  number of degrees of freedom of the track
  int nDoF() const;

  /// Update  number of degrees of freedom of the track
  void setNDoF(int value);

  /// Retrieve const  the variety of track flags
  unsigned int flags() const;

  /// Update track type
  void setType(const TrackType& value);

  /// Check track type
  bool checkType(const TrackType& value) const;

  /// Update track flags
  void setFlag(const Flag& value);

  /// Check track flags
  bool checkFlag(const Flag& value) const;

  /// Retrieve const  container of LHCbIDs
  const std::vector<LHCbID>& lhcbIDs() const;

  /// Retrieve  container of LHCbIDs
  std::vector<LHCbID>& lhcbIDs();

  /// Update  container of LHCbIDs
  void setLhcbIDs(const std::vector<LHCbID>& value);

  /// Retrieve const  State closest to the beam-line
  const State& physicsState() const;

  /// Retrieve  State closest to the beam-line
  State& physicsState();

  /// Update  State closest to the beam-line
  void setPhysicsState(const State& value);

  /// Retrieve const  container with pointers to all the states
  const std::vector<State*>& states() const;

  /// Retrieve  container with pointers to all the states
  std::vector<State*>& states();

  /// Update  container with pointers to all the states
  void setStates(const std::vector<State*>& value);

protected:

private:

  /// Offsets of bitfield flags
  enum flagsBits{typeBits    = 0,
                 historyBits = 7,
                 flagBits    = 16};

  /// Bitmasks for bitfield flags
  enum flagsMasks{typeMask    = 0x7FL,
                  historyMask = 0xFF80L,
                  flagMask    = 0x30000L};


  double              m_chi2PerDoF;   ///< chi^2 per degree of freedom of the track
  int                 m_nDoF;         ///< number of degrees of freedom of the track
  unsigned int        m_flags;        ///< the variety of track flags
  std::vector<LHCbID> m_lhcbIDs;      ///< container of LHCbIDs
  State               m_physicsState; ///< State closest to the beam-line
  std::vector<State*> m_states;       ///< container with pointers to all the states

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations
#include "GaudiKernel/StatusCode.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "CLHEP/Matrix/SymMatrix.h"


inline Track::~Track() 
{

  for (std::vector<State*>::iterator iter = m_states.begin();
  iter != m_states.end(); iter++)
  delete *iter;
        
}

inline const CLID& Track::clID() const
{
  return Track::classID();
}

inline const CLID& Track::classID()
{
  return CLID_Track;
}

inline double Track::chi2PerDoF() const 
{
  return m_chi2PerDoF;
}

inline void Track::setChi2PerDoF(double value) 
{
  m_chi2PerDoF = value;
}

inline int Track::nDoF() const 
{
  return m_nDoF;
}

inline void Track::setNDoF(int value) 
{
  m_nDoF = value;
}

inline unsigned int Track::flags() const 
{
  return m_flags;
}

inline void Track::setType(const TrackType& value)
{
  unsigned int val = (unsigned int)value;
  m_flags &= ~typeMask;
  m_flags |= ((((unsigned int)val) << typeBits) & typeMask);
}

inline bool Track::checkType(const TrackType& value) const
{
  unsigned int val = (unsigned int)value;  return 0 != ( m_flags & typeMask & val );
}

inline void Track::setFlag(const Flag& value)
{
  unsigned int val = (unsigned int)value;
  m_flags &= ~flagMask;
  m_flags |= ((((unsigned int)val) << flagBits) & flagMask);
}

inline bool Track::checkFlag(const Flag& value) const
{
  unsigned int val = (unsigned int)value;  return 0 != ( m_flags & flagMask & val );
}

inline const std::vector<LHCbID>& Track::lhcbIDs() const 
{
  return m_lhcbIDs;
}

inline std::vector<LHCbID>& Track::lhcbIDs() 
{
  return m_lhcbIDs;
}

inline void Track::setLhcbIDs(const std::vector<LHCbID>& value) 
{
  m_lhcbIDs = value;
}

inline const State& Track::physicsState() const 
{
  return m_physicsState;
}

inline State& Track::physicsState() 
{
  return m_physicsState;
}

inline void Track::setPhysicsState(const State& value) 
{
  m_physicsState = value;
}

inline const std::vector<State*>& Track::states() const 
{
  return m_states;
}

inline std::vector<State*>& Track::states() 
{
  return m_states;
}

inline void Track::setStates(const std::vector<State*>& value) 
{
  m_states = value;
}

inline int Track::charge() const 
{

  double qP = m_physicsState.qOverP();
  return ( qP != 0. ? int(fabs(qP)/qP) : 0 );
        
}

inline unsigned int Track::nStates() const 
{

  return m_states.size();
        
}

inline void Track::addToStates(State* value) 
{

  m_states.push_back( value );
        
}

inline void Track::removeFromStates(State* value) 
{

  std::vector<State*>::iterator iter =
    std::remove( m_states.begin(), m_states.end(), value );
  m_states.erase( iter, m_states.end() );
        
}

inline double Track::chi2() const 
{

  return ( m_chi2PerDoF * double(m_nDoF) );
        
}

inline void Track::reset() 
{

  m_chi2PerDoF   = 0;
  m_nDoF         = 0;
  m_flags        = 0;
  m_physicsState.reset();
  m_states.clear();
  m_lhcbIDs.clear();
        
}

inline void Track::addToLhcbIDs(const LHCbID& value) 
{

  m_lhcbIDs.push_back( value );
        
}

inline void Track::removeFromLhcbIDs(const LHCbID& value) 
{

  std::vector<LHCbID>::iterator iter =
    std::remove( m_lhcbIDs.begin(), m_lhcbIDs.end(), value );
  m_lhcbIDs.erase( iter, m_lhcbIDs.end() );
        
}

// Definition of Keyed Container for Track
typedef KeyedContainer<Track, Containers::HashMap> Tracks;



#endif ///TrEvent_Track_H
