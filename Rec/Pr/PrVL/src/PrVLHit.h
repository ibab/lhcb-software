#ifndef PRVL_HIT_H
#define PRVL_HIT_H 1

#include "Kernel/LHCbID.h"
#include "Event/VLLiteCluster.h"

/** @class PrVLHit PrVLHit.h
 *
 *  Representation of a hit in the VL for use in the pattern recognition.
 *
 */

class PrVLHit {

public:
  /// Constructor
  PrVLHit() : 
      m_sensor(0), m_strip(0), m_rzone(0), m_phizone(0), 
      m_right(false), m_fraction(0.), m_adcHigh(false), 
      m_x(0.), m_y(0.), m_z(0.), m_r(0.), m_phi(0.), 
      m_weight(0.), m_phiWeight(0.), 
      m_nUsed(0), m_nMaxUsed(1),
      m_a(0.), m_b(0.), m_c(999.),
      m_xStripCentre(0.), m_yStripCentre(0.),
      m_xSensorCentre(0.), m_ySensorCentre(0.) {
  
  }
  /// Destructor
  virtual ~PrVLHit() {}

  void setHit(const LHCb::VLLiteCluster cluster, 
              const unsigned int rzone, const unsigned int phizone, 
              const bool right, const double z, const double r, const double weight) {
    m_id = cluster.channelID();
    m_sensor = cluster.channelID().sensor();
    m_strip = cluster.channelID().strip();
    m_fraction = cluster.interStripFraction();
    m_adcHigh = cluster.highThreshold();
    m_rzone = rzone;
    m_phizone = phizone;
    m_right = right;
    m_r = float(r);
    m_x = m_y = 0.; 
    m_z = float(z);
    m_phi = 0.;
    m_weight = m_phiWeight = float(weight);
    m_nUsed = 0;
    m_a = m_b = 0.;
    m_c = 1.;
    m_xStripCentre = m_yStripCentre = 0.;
    m_xSensorCentre = m_ySensorCentre = 0.;
  }

  LHCb::LHCbID lhcbID() const {return m_id;}
  unsigned int sensor() const {return m_sensor;}
  unsigned int strip() const {return m_strip;}
  unsigned int rZone() const {return m_rzone;}
  unsigned int phiZone() const {return m_phizone;}
  bool right() const {return m_right;}
  double fraction() const {return m_fraction;}
  bool adcHigh() const {return m_adcHigh;}

  double r() const {return m_r;}
  double x() const {return m_x;}
  double y() const {return m_y;}
  double z() const {return m_z;}
  double phi() const {return m_phi;}
  double weight() const {return m_weight;}
  double phiWeight() const {return m_phiWeight;}

  void setR(const double r) {m_r = float(r);}
  void setX(const double x) {m_x = float(x);}
  void setY(const double y) {m_y = float(y);}
  void setZ(const double z) {m_z = float(z);}
  void setPhi(const double phi) {m_phi = float(phi);}
  void setWeight(const double r) {
    m_weight = m_phiWeight / (r * r);
  }
  void setWeight(const double x, const double y) {
    const double dx = x - m_xSensorCentre;
    const double dy = y - m_ySensorCentre;
    m_weight = m_phiWeight / (dx * dx + dy * dy); 
  }
  void setSensorCentre(const double x, const double y) {
    m_xSensorCentre = float(x); 
    m_ySensorCentre = float(y);
  }

  unsigned int nUsed() const {return m_nUsed;}
  void setUsed()   {++m_nUsed;}
  void unsetUsed() {--m_nUsed;}
  void setMaxUsed(const unsigned int n) {m_nMaxUsed = n;}
  unsigned int nMaxUsed() const {return m_nMaxUsed;}

  double a() const {return m_a;}
  double b() const {return m_b;}
  double c() const {return m_c;}
  double xStripCentre() const {return m_xStripCentre;}
  double yStripCentre() const {return m_yStripCentre;}
  void setLineParameters(const double a, const double b, const double c, 
                         const double xs, const double ys) {
    m_a = a;
    m_b = b;
    m_c = c;
    m_xStripCentre = xs;
    m_yStripCentre = ys;
  }

  double distance(const double x, const double y) const {
    return m_a * x + m_b * y + m_c;
  }

  double chi2(const double x, const double y) const {
    const double d = distance(x, y);
    return m_weight * d * d;
  }


  void setStartingPoint(const double x, const double y) {
    float dx = float(x) - m_xSensorCentre;
    float dy = float(y) - m_ySensorCentre;
    float norm = sqrt(dx * dx + dy * dy);
    m_a = dx / norm;
    m_b = dy / norm;
    float xOnStrip = m_xSensorCentre + m_a * m_r;
    float yOnStrip = m_ySensorCentre + m_b * m_r;
    m_c = - (xOnStrip * m_a + yOnStrip * m_b);
  }


  struct DecreasingByZ {
    bool operator() (const PrVLHit* lhs, const PrVLHit* rhs) const {
      return lhs->z() > rhs->z();
    }
  };
  struct IncreasingByZ {
    bool operator() (const PrVLHit* lhs, const PrVLHit* rhs) const {
      return lhs->z() < rhs->z();
    }
  };
  struct IncreasingByR {
    bool operator() (const PrVLHit* lhs, const PrVLHit* rhs) const {
      return lhs->r() < rhs->r();
    } 
  };
  struct IncreasingByPhi {
    bool operator() (const PrVLHit* lhs, const PrVLHit* rhs) const {
      return lhs->phi() < rhs->phi();
    }
  };

private:

  LHCb::LHCbID m_id;
  unsigned int m_sensor;
  unsigned int m_strip;
  unsigned int m_rzone;
  unsigned int m_phizone;
  bool m_right;
  float m_fraction;
  bool m_adcHigh;
  float m_x;
  float m_y;
  float m_z;
  float m_r;
  float m_phi;
  float m_weight;
  float m_phiWeight;
  unsigned int m_nUsed;
  unsigned int m_nMaxUsed;
  float m_a, m_b, m_c;
  float m_xStripCentre;
  float m_yStripCentre;
  float m_xSensorCentre;
  float m_ySensorCentre;
};

typedef std::vector<PrVLHit*> PrVLHits;

#endif
