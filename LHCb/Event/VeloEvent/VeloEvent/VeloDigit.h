#ifndef LHCBEVENT_VELODIGIT_H
#define LHCBEVENT_VELODIGIT_H 1

/// Includes
#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/ContainedObject.h"

/// Externals
static const CLID CLID_VeloDigit = 4001;

/**
 * Vertex detector digitised hit base class.
 * This class is the base for a digitised \f$R\f$ or \f$\phi\f$ hit from 
 * the vertex detector (VELO). The hit may be constructed from the 
 * digitisation of Monte Carlo produced data or from the decoding of raw 
 * data from the LHCb detector.
 */

class VeloDigit : public ContainedObject {

public:
  
  /// Default constructor
  VeloDigit() : 
    m_stripNumber(0),
    m_adcValue(0.),
    m_rawAdcValue(0),
    m_pedestal(0.),
    m_commonModeNoise(0.),
    m_noise(0.),
    m_chipNumber(0),
    m_chipChannel(0),
    m_fadcNumber(0),
    m_fadcChannel(0),
    m_station(0),
    m_detector(0),
    m_sector(0),
    m_subSector(0) {
    
  }

  /// Destructor
  virtual ~VeloDigit() { 

  }

  /// Strip number (starting at 0 and increasing monotonically 
  /// with \f$R\f$ or \f$\phi\f$ within a given sub-sector)
  virtual long stripNumber() const { 
    return m_stripNumber;
  }

  /// Set strip number 
  virtual void setStripNumber(long stripNumber) {
    m_stripNumber = stripNumber;
  }

  /// ADC value with common mode noise and pedestal subtracted
  virtual double adcValue() const {
    return m_adcValue;
  }

  /// Set ADC value with common mode noise and pedestal subtracted
  virtual void setAdcValue(double adcValue) {
    m_adcValue = adcValue;
  }

  /// ADC value without common mode noise or pedestal subtracted
  virtual long rawAdcValue() const {
    return m_rawAdcValue;
  }

  /// Set ADC value without common mode noise or pedestal subtracted
  virtual void setRawAdcValue(long rawAdcValue) {
    m_rawAdcValue = rawAdcValue;
  }

  /// Pedestal for this channel
  virtual double pedestal() {
    return m_pedestal;
  }

  /// Set pedestal for this channel
  virtual void setPedestal(double pedestal) {
    m_pedestal = pedestal;
  }

  /// Common mode noise for this channel
  virtual double commonModeNoise() {
    return m_commonModeNoise;
  }

  /// Set common mode noise for this channel
  virtual void setCommonModeNoise(double commonModeNoise) {
    m_commonModeNoise = commonModeNoise;
  }

  /// Noise for this channel
  virtual double noise() {
    return m_noise;
  }

  /// Set noise for this channel
  virtual void setNoise(double noise) {
    m_noise = noise;
  }

  /// Signal to noise ratio
  virtual double signalToNoise() {
    return m_adcValue/m_noise;
  }

  /// Number of the chip to which this strip is connected
  virtual long chipNumber() {
    return m_chipNumber;
  }

  /// Set number of the chip to which this strip is connected
  virtual void setChipNumber(long chipNumber) {
    m_chipNumber = chipNumber;
  }

  /// Number of the channel on the chip to which this strip is connected
  virtual long chipChannel() {
    return m_chipChannel;
  }

  /// Set number of the channel on the chip to which this strip is connected
  virtual void setChipChannel(long chipChannel) {
    m_chipChannel = chipChannel;
  }

  /// Number of the FADC to which this strip is connected
  virtual long fadcNumber() {
    return m_fadcNumber;
  }

  /// Set number of the FADC to which this strip is connected
  virtual void setFadcNumber(long fadcNumber) {
    m_fadcNumber = fadcNumber;
  }

  /// Number of the FADC channel to which this strip is connected
  virtual long fadcChannel() {
    return m_fadcChannel;
  }

  /// Set number of the FADC channel to which this strip is connected
  virtual void setFadcChannel(long fadcChannel) {
    m_fadcChannel = fadcChannel;
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

  /// Sector number (sector in \f$\phi\f$ within a detector: 0-2 in increasing
  /// \f$\phi\f$ for \f$60^{\circ}\f$ sectors)
  virtual unsigned char sector() {
    return m_sector;
  }

  /// Set sector number
  virtual void setSector(unsigned char sector) {
    m_sector = sector;
  }

  /// Sub-sector number (sub-sector within a sector: 0-3 in increasing
  /// \f$R\f$ and \f$\phi\f$)
  virtual unsigned char subSector() {
    return m_subSector;
  }

  /// Set sub-sector number
  virtual void setSubSector(unsigned char subSector) {
    m_subSector = subSector;
  }

  /// Retrieve pointer to class defininition structure
  const CLID& clID() const { 
    return VeloDigit::classID(); 
  }
  static const CLID& classID() { 
    return CLID_VeloDigit; 
  }

private:

  /// Strip number (starting at 0 and increasing monotonically 
  /// with \f$R\f$ or \f$\phi\f$ within a given sub-sector)
  long m_stripNumber;
  /// ADC value with common mode noise and pedestal subtracted
  double m_adcValue;
  /// ADC value without common mode noise or pedestal subtracted
  long m_rawAdcValue;
  /// Pedestal for this channel
  double m_pedestal;
  /// Common mode noise for this channel
  double m_commonModeNoise;
  /// Noise for this channel
  double m_noise;
  /// Number of the chip to which this strip is connected
  long m_chipNumber;
  /// Number of the channel on the chip to which this strip is connected
  long m_chipChannel;
  /// Number of the FADC to which this strip is connected
  long m_fadcNumber;
  /// Number of the FADC channel to which this strip is connected
  long m_fadcChannel;
  /// Station number (0-24 in increasing \f$z\f$)
  unsigned char m_station;
  /// Detector number (0-3 in increasing \f$z\f$)
  unsigned char  m_detector;
  /// Sector number (sector in \f$\phi\f$ within a detector: 0-2 in increasing
  /// \f$\phi\f$ for \f$60^{\circ}\f$ sectors)
  unsigned char m_sector;
  /// Sub-sector number (sub-sector within a sector: 0-3 in increasing
  /// \f$R\f$ and \f$\phi\f$)
  unsigned char m_subSector;
};

#endif  // LHCBEVENT_VELODIGIT_H










