#ifndef DDEXAMPLE_DECALORIMETER_H
#define DDEXAMPLE_DECALORIMETER_H 1


// Include files
#include "DetDesc/DetectorElement.h"


// External declarations
extern const CLID& CLID_DECalorimeter;


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
   * set method for the coding member
   * @param coding the new coding value
   */
  void setCoding (int coding);

  /**
   * gets the value of the coding member
   * @return the coding for this object
   */
  int coding();

  /**
   * This method initializes the detector element. It should be overridden
   * and used for computation purposes. This is a kind of hook for adding
   * user code easily in the initialization of a detector element.
   */
  virtual StatusCode initialize();

private:

  /// the coding for this object
  int m_coding;
};

#endif    // DDEXAMPLE_DECALORIMETER_H
