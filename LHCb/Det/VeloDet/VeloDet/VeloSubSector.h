#ifndef VELOSUBSECTOR_H
#define VELOSUBSECTOR_H 1

/**
 * Definition of a sub-sector on a Velo detector.
 * This class specifies the boundaries of a sub-sector on a Velo
 * detector. It is described by its extent in \f$R\f$ and \f$\phi\f$
 * and by the strip pitch. For \f$\phi\f$ the strip pitch stored is 
 * the pitch at the innermost radius of the sub-sector.
 */

class VeloSubSector {

public:

  /// Constructor
  VeloSubSector() {

  }

  /// Destructor
  ~VeloSubSector() {

  }
  
  /// Minimum \f$\phi\f$ angle
  double phiMin() const { 
    return m_phiMin; 
  }
  /// Set minimum \f$\phi\f$ angle
  void setPhiMin(double phiMin) {
    m_phiMin = phiMin;
  }

  /// Maximum \f$\phi\f$ angle
  double phiMax() const { 
    return m_phiMax; 
  }
  /// Set maximum \f$\phi\f$ angle
  void setPhiMax(double phiMax) {
    m_phiMax = phiMax;
  }

  /// Minimum \f$R\f$ angle
  double rMin() const { 
    return m_rMin; 
  }
  /// Set minimum \f$R\f$ angle
  void setRMin(double rMin) {
    m_rMin = rMin;
  }

  /// Maximum \f$R\f$ angle
  double rMax() const { 
    return m_rMax; 
  }
  /// Set maximum \f$R\f$ angle
  void setRMax(double rMax) {
    m_rMax = rMax;
  }

  /// Strip pitch
  double stripPitch() const {
    return m_stripPitch;
  }
  /// Set strip pitch
  void setStripPitch(double stripPitch) {
    m_stripPitch = stripPitch;
  }

  /// Stereo angle
  double stereoAngle() const {
    return m_stereoAngle;
  }
  /// Set stereo angle
  void setStereoAngle(double stereoAngle) {
    m_stereoAngle = stereoAngle;
  }

private:

  /// Minimum \f$\phi\f$ angle
  double m_phiMin;
  /// Maximum \f$\phi\f$ angle
  double m_phiMax;
  /// Minimum \f$R\f$ angle
  double m_rMin;
  /// Maximum \f$R\f$ angle
  double m_rMax;
  /// Strip pitch
  double m_stripPitch;
  /// Stereo angle
  double m_stereoAngle;
};

#endif // VELOSUBSECTOR_H


