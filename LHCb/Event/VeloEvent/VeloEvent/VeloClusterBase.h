#ifndef LHCBEVENT_VELOCLUSTERBASE_H
#define LHCBEVENT_VELOCLUSTERBASE_H 1

///Includes
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"

/// Externals 
static const CLID CLID_VeloClusterBase = 4031;

/**
 * Base class for vertex detector clusters.
 * This class contains the data members and methods which are common to
 * the \f$R\f$ and \f$\phi\f$ clusters.
 */

class VeloClusterBase : public ContainedObject {

public:

  /// Default constructor
  VeloClusterBase() :
    m_station(0),
    m_detector(0),
    m_width(0) {

  }

  /// Destructor
  virtual ~VeloClusterBase() {

  }
  
  /// Station number (0-24 in increasing \f$z\f$)
  virtual unsigned char station() {
    return m_station;
  }

  /// Set station number
  virtual void setStation(unsigned char station) {
    m_station = station;
  }

  /// Detector number (detector within a station: 0-3 in increasing \f$z\f$)
  virtual unsigned char detector() {
    return m_detector;
  }

  /// Set detector number
  virtual void setDetector(unsigned char detector) {
    m_detector = detector;
  }

  /// Width of cluster in strips
  virtual int width() {
    return m_width;
  }

  /// Set width of cluster in strips
  virtual void setWidth(int width) {
    m_width = width;
  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloClusterBase::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloClusterBase; 
  }

private:

  /// Station number (0-24 in increasing \f$z\f$)
  unsigned char m_station;
  /// Detector number (0-3 in increasing \f$z\f$)
  unsigned char  m_detector;
  /// Width of cluster in strips
  int m_width;

};

#endif /// LHCBEVENT_VELOCLUSTERBASE_H






