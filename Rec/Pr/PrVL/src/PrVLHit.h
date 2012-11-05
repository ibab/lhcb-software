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
      m_sensor(0), m_strip(0), m_zone(0), m_z(0.), 
      m_rLocal(0.), m_rGlobal(0.), 
      m_xGlobal(0.), m_yGlobal(0.), m_dGlobal(0.), 
      m_weight(0.), m_phiWeight(0.), m_nUsed(0),
      m_a(0.), m_b(0.), m_c(999.),
      m_xStripCentre(0.), m_yStripCentre(0.),
      m_xSensorCentre(0.), m_ySensorCentre(0.) {

  }
  /// Destructor
  virtual ~PrVLHit() {}

  void setHit(const LHCb::VLLiteCluster cluster, const unsigned int zone, 
              const double z, const double r, const double weight) {
    m_id = cluster.channelID();
    m_sensor = cluster.channelID().sensor();
    m_strip = cluster.channelID().strip();
    m_interStripFraction = cluster.interStripFraction();
    m_zone = zone;
    m_z = float(z);
    m_rLocal = m_rGlobal = float(r);
    m_xGlobal = m_yGlobal = 0.; 
    m_dGlobal = 0.;
    m_weight = m_phiWeight = float(weight);
    m_nUsed = 0;
    m_a = m_b = 0.;
    m_c = 1.;
    m_xStripCentre = m_yStripCentre = 0.;
    m_xSensorCentre = m_ySensorCentre = 0.;
  }

  LHCb::LHCbID lhcbID() const {return m_id;}
  double rLocal() const {return m_rLocal;}
  double rGlobal() const {return m_rGlobal;}
  double xGlobal() const {return m_xGlobal;}
  double yGlobal() const {return m_yGlobal;}
  double dGlobal() const {return m_dGlobal;}
  double weight() const {return m_weight;}
  double z() const {return m_z;}
  unsigned int sensor() const {return m_sensor;}
  unsigned int strip() const {return m_strip;}
  unsigned int zone() const {return m_zone;}
  double interStripFraction() const {return m_interStripFraction;}
  unsigned int nUsed() const {return m_nUsed;}

  void setUsed()   {++m_nUsed;}
  void clearUsed() {--m_nUsed;}
  void setGlobalRadius(const double r) {m_rGlobal = float(r);}
  void setGlobalPosition(const double x, const double y) { 
    m_xGlobal = float(x); 
    m_yGlobal = float(y);
  }
  void setGlobalDistance(const double d) {m_dGlobal = float(d);}
  void setZ(const double z) {m_z = float(z);}
  void setSensorCentre(const double x, const double y) {
    m_xSensorCentre = float(x); 
    m_ySensorCentre = float(y);
  }

  double a() const {return m_a;}
  double b() const {return m_b;}
  double c() const {return m_c;}
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

  double xStripCentre() const {return m_xStripCentre;}
  double yStripCentre() const {return m_yStripCentre;}

  void setStartingPoint(const double x, const double y) {
    float dx = float(x) - m_xSensorCentre;
    float dy = float(y) - m_ySensorCentre;
    float norm = sqrt(dx * dx + dy * dy);
    m_a = dx / norm;
    m_b = dy / norm;
    float xOnStrip = m_xSensorCentre + m_a * m_rLocal;
    float yOnStrip = m_ySensorCentre + m_b * m_rLocal;
    m_c = - (xOnStrip * m_a + yOnStrip * m_b);
  }

  void setPhiWeight(const double r) {
    m_weight = m_phiWeight / (r * r);
  }
  void setPhiWeight(const double x, const double y) {
    const double dx = x - m_xSensorCentre;
    const double dy = y - m_ySensorCentre;
    m_weight = m_phiWeight / (dx * dx + dy * dy); 
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
  struct IncreasingByGlobalDistance {
    bool operator() (const PrVLHit* lhs, const PrVLHit* rhs) const {
      return lhs->dGlobal() < rhs->dGlobal();
    }
  };

private:

  LHCb::LHCbID m_id;
  unsigned int m_sensor;
  unsigned int m_strip;
  unsigned int m_zone;
  float m_interStripFraction;
  float m_z;
  float m_rLocal;
  float m_rGlobal;
  float m_xGlobal;
  float m_yGlobal;
  float m_dGlobal;
  float m_weight;
  float m_phiWeight;
  unsigned int m_nUsed;
  float m_a, m_b, m_c;
  float m_xStripCentre;
  float m_yStripCentre;
  float m_xSensorCentre;
  float m_ySensorCentre;
};

typedef std::vector<PrVLHit*> PrVLHits;

#endif
