#ifndef LHCBEVENT_VELORCLUSTER_H
#define LHCBEVENT_VELORCLUSTER_H 1

/// Includes
#include "VeloEvent/VeloClusterBase.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/SmartRefVector.h"

/// Externals 
static const CLID CLID_VeloRCluster = 4032;

/**
 * Vertex detector R clusters.
 * This class represents a cluster of hits reconstructed in the Velo from a set
 * of \f$R\f$ hits.
 *
 * An R cluster is specified by a radius and the extent of the cluster in phi.
 * To get r(phi) requires access to the associated DetectorElement. Also
 * stored are the cluster width in number of strips or in and the cluster
 * z position. There should also be a method z(phi) to allow for misalignment,
 * requiring access to the associated DetectorElement. The error on the
 * cluster position can be given for the average angle or for a specified
 * angle for a better estimate.
 */

class VeloRCluster : public VeloClusterBase {

public:

  /// Default constructor
  VeloRCluster() {

  }

  /// Destructor
  virtual ~VeloRCluster() {

  }

  /// R coordinate at centre of cluster
  virtual double r() const {
    return m_r;
  }

  /// Set R coordinate at centre of cluster
  virtual void setR(double r) {
    m_r = r;
  }

  /// R as a function of phi
  virtual double r(const double& phi) {
    /// From some calculation of the DetectorElement
    return 0.;
  }

  /// Error on R coordinate for average track angle using cluster width
  virtual double errorR() const {
    /// Picked up from a database somewhere
    return m_errorR;
  }

  /// Error on R coordinate for a specified track angle
  virtual double errorR(const Hep3Vector& track) {
    /// Get some constants from a database and make some
    /// calculation based on the angle the track makes with 
    /// the normal to the tangent of the strip pointing radially 
    /// inwards from the point the track enters the silicon plane
    return 0.;
  }

  /// Z coordinate at centre of cluster
  virtual double z() const {
    return m_z;
  }

  /// Set Z coordinate at centre of cluster
  virtual void setZ(double z) {
    m_z = z;
  }

  /// Z as a function of phi
  virtual double z(const double& phi) {
    /// From some calculation of the DetectorElement
    return 0.;
  }

  /// Lesser phi coordinate
  virtual double lesserPhi() const {
    return m_lesserPhi;
  }

  /// Set lesser phi coordinate
  virtual void setLesserPhi(double lesserPhi) {
    m_lesserPhi = lesserPhi;
  }

  /// Greater phi coordinate
  virtual double greaterPhi() const {
    return m_greaterPhi;
  }

  /// Set greater phi coordinate
  virtual void setGreaterPhi(double greaterPhi) {
    m_greaterPhi = greaterPhi;
  }
  
  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloRCluster::classID(); 
  }
  static const CLID& classID()        { 
    return CLID_VeloRCluster; 
  }


  /// Access the digitized hits used to build this cluster
  SmartRefVector<VeloRDigit>& digits();
  const SmartRefVector<VeloRDigit>& digits() const;
	
private:
  
  /// R coordinate at centre of cluster
  double m_r;
  /// Error on R coordinate for average track angle
  double m_errorR;
  /// Lesser phi coordinate
  double m_lesserPhi;
  /// Greater phi coordinate
  double m_greaterPhi;
  /// Z coordinate at centre of cluster
  double m_z;
  /// References to digitised hits
  SmartRefVector<VeloRDigit> m_digits;
  /// Pointer to the detector element
  SmartRef<VeloRDetectorElement> m_det;

};

#include "VeloEvent/VeloRDigit.h"

/// Access to digitised hits
inline SmartRefVector<VeloRDigit>& VeloRCluster::digits() { 
  return m_digits;  
}

/// Access to digitised hits
inline const SmartRefVector<VeloRDigit>& VeloRCluster::digits() const { 
  return m_digits;  
}

#endif // LHCBEVENT_VELORCLUSTER_H








