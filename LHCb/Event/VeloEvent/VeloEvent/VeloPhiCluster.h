#ifndef LHCBEVENT_VELOPHICLUSTER_H
#define LHCBEVENT_VELOPHICLUSTER_H 1

/// Includes
#include "VeloEvent/VeloClusterBase.h"
#include "CLHEP/Vector/ThreeVector.h"
#include "GaudiKernel/SmartRefVector.h"

/// Externals 
static const CLID CLID_VeloPhiCluster = 4033;

/**
 * Vertex detector Phi clusters.
 * This class represents a cluster of hits reconstructed in the Velo from a set
 * of \f$\phi\f$ hits.
 *
 * A Phi cluster is specified by the azimuthal angle of the cluster at the inner
 * radius of the sub-sector (this would exclude the possibility to build clusters
 * between adjacent sub-sectors) and its extent in r. To get phi(r) requires 
 * access to the associated DetectorElement. It could be that we only require the
 * phi(r) method and remove phi() completely. Also stored are the cluster width 
 * in number of strips and the cluster z position. There should also be a method 
 * z(phi) to allow for misalignment, requiring access to the associated 
 * DetectorElement. The error on the cluster position can be given for the average 
 * angle or for a specified angle for a better estimate - it also requires the 
 * radius as an input value.
 */

class VeloPhiCluster : public VeloClusterBase {

public:

  /// Default constructor
  VeloPhiCluster() {
  }

  /// Destructor
  virtual ~VeloPhiCluster() {
  }

  /// Phi coordinate at inner radius of sub-sector
  virtual double phi() const {
    return m_phi;
  }

  /// Set Phi coordinate at inner radius of sub-sector
  virtual void setPhi(const double& phi) {
    m_phi = phi;
  }

  /// Phi as a function of radius
  virtual double phi(const double& r) {
    /// From some calculation of the DetectorElement
    return 0.;
  }

  /// Error on Phi coordinate for average track angle using cluster width
  virtual double errorPhi(const double& r) const {
    /// Would require getting some number from a database plus accessing
    /// the DetectorElement for the r dependence
    return 0.;
  }

  /// Error on Phi coordinate for a specified track angle
  virtual double errorPhi(const double& r, const Hep3Vector& track) {
    /// Would require getting some number from a database plus accessing
    /// the DetectorElement for the r dependence. We also need to calculate 
    /// the angle the track makes with the normal of the strip in the 
    /// silicon plane
    return 0.;
  }
 
  /// Z coordinate at inner radius of sub-sector
  virtual double z() const {
    return m_z;
  }

  /// Set Z coordinate at inner radius of sub-sector
  virtual void setZ(const double& z) {
    m_z = z;
  }

  /// Z as a function of radius
  virtual double z(const double& radius) {
    /// From some calculation of the DetectorElement
    return 0.;
  }

  /// Inner r coordinate
  virtual double innerR() const {
    return m_innerR;
  }

  /// Set inner r coordinate
  virtual void setInnerR(const double& innerR) {
    m_innerR = innerR;
  }

  /// Outer r coordinate
  virtual double outerR() const {
    return m_outerR;
  }

  /// Set outer r coordinate
  virtual void setOuterR(const double& outerR) {
    m_outerR = outerR;
  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloPhiCluster::classID(); 
  }
  static const CLID& classID()        { 
    return CLID_VeloPhiCluster; 
  }

  /// Access the digitized hits used to build this cluster
  SmartRefVector<VeloPhiDigit>& digits();
  const SmartRefVector<VeloPhiDigit>& digits() const;  
	
private:
  
  /// Phi coordinate at inner radius of sub-sector
  double m_phi;
  /// Inner r coordinate
  double m_innerR;
  /// Outer r coordinate
  double m_outerR;
  /// Z coordinate at inner radius of sub-sector
  double m_z;
  /// References to digitised hits
  SmartRefVector<VeloPhiDigit> m_digits;
  /// Pointer to the detector element
  SmartRef<VeloPhiDetectorElement> m_det;

};

#include "VeloEvent/VeloPhiDigit.h"

/// Access to digitised hits
inline SmartRefVector<VeloPhiDigit>& VeloPhiCluster::digits() { 
  return m_digits;  
}

/// Access to digitised hits
inline const SmartRefVector<VeloPhiDigit>& VeloPhiCluster::digits() const { 
  return m_digits;  
}

#endif // LHCBEVENT_VELOPHICLUSTER_H











