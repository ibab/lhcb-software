#ifndef VELOLITE_INTERNALCLUSTER_H
#define VELOLITE_INTERNALCLUSTER_H 1

#include <vector>
#include <utility>

class VeloLiteInternalCluster {

  public:
    /// Constructor
    VeloLiteInternalCluster() {}
    /// Destructor
    virtual ~VeloLiteInternalCluster() {}
    /// Number of strips in cluster
    unsigned int size() const {return m_stripSignals.size();}
    /// ADC value of strips in cluster
    double adcValue(unsigned int num) const {
      return m_stripSignals[num].second;
    }
    /// Strip number of strip in cluster
    unsigned int strip(unsigned int num) const {
      return m_stripSignals[num].first;
    }
    /// Sensor number
    unsigned int sensor() const {return m_sensor;}
    /// Set the sensor number (overrides default)
    void setSensor(unsigned int num) {
      m_sensor = num;
    }
    /// Retrieve strip numbers and their signals
    const std::vector<std::pair<int, unsigned int> >& stripSignals() const {
      return m_stripSignals;
    }
    /// Update strip numbers and their signals
    void setStripSignals(const std::vector<std::pair<int, unsigned int> >& value) {
      m_stripSignals = value;
    }
    bool hasHighThreshold() const {return m_hasHighThreshold;}
    void setHasHighThreshold(bool value) {
      m_hasHighThreshold = value;
    }
  
  private:

    /// Sensor number
    unsigned int m_sensor;
    /// Strip numbers and their signals
    std::vector<std::pair<int, unsigned int> > m_stripSignals;
    /// Flag if cluster is over spillover threshold
    bool m_hasHighThreshold; 

};

#endif 
