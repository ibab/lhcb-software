
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

#ifndef TrackFitEvent_FitTrack_H
#define TrackFitEvent_FitTrack_H 1

// Include files
#include "Event/Track.h"
#include "Event/Node.h"
#include "Event/Measurement.h"
#include "GaudiKernel/KeyedContainer.h"
#include <vector>


// Forward declarations
class Plane3D;


// Class ID definition
static const CLID& CLID_FitTrack = 10020;

// Namespace for locations in TDS
namespace FitTrackLocation {
  static const std::string& Default = "Rec/FitTrack/Best";
  static const std::string& Velo = "Rec/FitTrack/Velo";
  static const std::string& Seed = "Rec/FitTrack/Seed";
  static const std::string& Match = "Rec/FitTrack/Match";
  static const std::string& Forward = "Rec/FitTrack/Forward";
  static const std::string& Follow = "Rec/FitTrack/Follow";
  static const std::string& VeloTT = "Rec/FitTrack/VeloTT";
  static const std::string& KsTrack = "Rec/FitTrack/KsTrack";
}


/** @class FitTrack FitTrack.h
 *
 * FitTrack is the class for tracking software. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Apr  7 15:08:13 2005
 * 
 */

class FitTrack: public Track
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
             Unique=2,
             IPSelected=4,
             MuSelected=8 };

  /// Default Constructor
  FitTrack() : m_nodes(),
               m_measurements() {}

  /// Default Destructor
  virtual ~FitTrack() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// number of measurements on track
  unsigned int nMeasurements() const;

  /// add a measurement to the list associated to the track
  void addToMeasurements(Measurement* meas);

  /// sort the measurements by increasing z
  void sortMeasurements();

  /// Retrieve the reference to the state closest to the given z-position
  virtual State & closestState(double z);

  /// Retrieve the reference to the state closest to the given z-position
  virtual const State & closestState(double z) const;

  /// Retrieve the reference to the state closest to the given plane
  virtual State & closestState(const HepPlane3D& plane);

  /// Retrieve the reference to the state closest to the given plane
  virtual const State & closestState(const HepPlane3D& plane) const;

  /// check the existence of a state at a certain predefined location. See State class.
  bool hasStateAt(const State::Location& value) const;

  /// Retrieve the pointer to the state at a certain predefined location. See State class.
  virtual State* stateAt(const State::Location& value);

  /// Retrieve the pointer to the state at a certain predefined location. See State class.
  virtual const State* stateAt(const State::Location& value) const;

  /// Clear the fit track before re-use
  virtual void reset();

  /// Clone the fit track
  virtual FitTrack* clone() const;

  /// Check whether the fit track was produced by a given algorithm
  virtual bool producedByAlgo(const HistoryFlag& value) const;

  /// Update the name of the algorithm that produced the fit track
  virtual void setProducedByAlgo(const HistoryFlag& value);

  /// Retrieve const  container of Nodes
  const std::vector<Node*>& nodes() const;

  /// Retrieve  container of Nodes
  std::vector<Node*>& nodes();

  /// Update  container of Nodes
  void setNodes(const std::vector<Node*>& value);

  /// Retrieve const  container of Measurements
  const std::vector<Measurement*>& measurements() const;

  /// Retrieve  container of Measurements
  std::vector<Measurement*>& measurements();

  /// Update  container of Measurements
  void setMeasurements(const std::vector<Measurement*>& value);

protected:

  std::vector<Node*>        m_nodes;        ///< container of Nodes
  std::vector<Measurement*> m_measurements; ///< container of Measurements

private:

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations
#include "CLHEP/Geometry/Plane3D.h"


inline const CLID& FitTrack::clID() const
{
  return FitTrack::classID();
}

inline const CLID& FitTrack::classID()
{
  return CLID_FitTrack;
}

inline const std::vector<Node*>& FitTrack::nodes() const 
{
  return m_nodes;
}

inline std::vector<Node*>& FitTrack::nodes() 
{
  return m_nodes;
}

inline void FitTrack::setNodes(const std::vector<Node*>& value) 
{
  m_nodes = value;
}

inline const std::vector<Measurement*>& FitTrack::measurements() const 
{
  return m_measurements;
}

inline std::vector<Measurement*>& FitTrack::measurements() 
{
  return m_measurements;
}

inline void FitTrack::setMeasurements(const std::vector<Measurement*>& value) 
{
  m_measurements = value;
}

inline unsigned int FitTrack::nMeasurements() const 
{

  return m_measurements.size();
        
}

inline void FitTrack::addToMeasurements(Measurement* meas) 
{

  m_measurements.push_back( meas );
        
}

inline void FitTrack::reset() 
{

  m_chi2PerDoF   = 0;
  m_nDoF         = 0;
  m_flags        = 0;
  m_physicsState.reset();
  m_states.clear();
  m_lhcbIDs.clear();
  m_nodes.clear();
  m_measurements.clear();
        
}

// Definition of Keyed Container for FitTrack
typedef KeyedContainer<FitTrack, Containers::HashMap> FitTracks;



#endif ///TrackFitEvent_FitTrack_H
