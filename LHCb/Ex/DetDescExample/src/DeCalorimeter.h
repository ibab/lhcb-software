// $Id: DeCalorimeter.h,v 1.5 2005-12-08 10:02:43 cattanem Exp $
#ifndef DDEXAMPLE_DECALORIMETER_H
#define DDEXAMPLE_DECALORIMETER_H 1


// Include files
#include "DetDesc/DetectorElement.h"

/** @class DeCalorimeter DeCalorimeter.h
 *
 * Example of a specific detector element class.
 * This uses both the userParameters and the override of the initialize method
 * of the DetectorElement class
 *
 * @author Marco Cattaneo
 * @author Sebastien Ponce
 */
class DeCalorimeter: public DetectorElement {

public:
  /**
   * Constructor for this class
   */
  DeCalorimeter();

  /**
   * Default destructor
   */
  ~DeCalorimeter() {};

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  const CLID& clID() const {
    return classID();
  }

  /**
   * Retrieves reference to class identifier
   * @return the class identifier for this class
   */
  static const CLID& classID();

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize();

  /**
   * Set accessor to member m_codingBit
   * @param codingBit the new value for m_codingBit
   */
  void setCodingBit (int codingBit) {
    m_codingBit = codingBit;
  }

  /**
   * Get accessor to member m_codingBit
   * @return the current value of m_codingBit
   */
  int codingBit () {
    return m_codingBit;
  }

  /**
   * Set accessor to member m_etInCenter
   * @param etInCenter the new value for m_etInCenter
   */
  void setEtInCenter (double etInCenter) {
    m_etInCenter = etInCenter;
  }
  
  /**
   * Get accessor to member m_etInCenter
   * @return the current value of m_etInCenter
   */
  double etInCenter () {
    return m_etInCenter;
  }


  /**
   * Set accessor to member m_etSlope
   * @param etSlope the new value for m_etSlope
   */
  void setEtSlope (double etSlope) {
    m_etSlope = etSlope;
  }

  /**
   * Get accessor to member m_etSlope
   * @return the current value of m_etSlope
   */
  double etSlope () {
    return m_etSlope;
  }

  /**
   * Set accessor to member m_adcMax
   * @param adcMax the new value for m_adcMax
   */
  void setAdcMax (int adcMax) {
    m_adcMax = adcMax;
  }

  /**
   * Get accessor to member m_adcMax
   * @return the current value of m_adcMax
   */
  int adcMax () {
    return m_adcMax;
  }

  /**
   * Set accessor to member m_activeToTotal
   * @param activeToTotal the new value for m_activeToTotal
   */
  void setActiveToTotal (double activeToTotal) {
    m_activeToTotal = activeToTotal;
  }

  /**
   * Get accessor to member m_activeToTotal
   * @return the current value of m_activeToTotal
   */
  double activeToTotal () {
    return m_activeToTotal;
  }

  /**
   * Set accessor to member m_zShowerMax
   * @param zShowerMax the new value for m_zShowerMax
   */
  void setZShowerMax (double zShowerMax) {
    m_zShowerMax = zShowerMax;
  }

  /**
   * Get accessor to member m_zShowerMax
   * @return the current value of m_zShowerMax
   */
  double zShowerMax () {
    return m_zShowerMax;
  }

private:
  int m_codingBit;
  double m_etInCenter;
  double m_etSlope;
  int m_adcMax;
  double m_activeToTotal;
  double m_zShowerMax;

};

#endif    // DDEXAMPLE_DECALORIMETER_H
