// $Id: SimAttribute.h,v 1.3 2009-10-15 12:41:20 silviam Exp $
#ifndef SIMATTRIBUTE_H 
#define SIMATTRIBUTE_H 1

// Include files

/** @class SimAttribute SimAttribute.h
 *  This encapsulated the definition of simulation attributes for
 *  a logical volume. Included attributes are :
 *     - the maximum number of steps
 *     - the maximum track length
 *     - the maximum time
 *     - the minimal kinetic energy
 *     - the minimal remaining range
 *
 *  @author Sebastien Ponce
 */
class SimAttribute {

public:
  /// Default constructor
  SimAttribute () :
    m_maxAllowedStep(-1.0),
    m_maxTrackLength(-1.0),
    m_maxTime(-1.0),
    m_minEkine(-1.0),
    m_minRange(-1.0)
  {};

  /// Standard constructor
  SimAttribute (double maxAllowedStep,
                double maxTrackLength,
                double maxTime,
                double minEkine,
                double minRange) :
    m_maxAllowedStep(maxAllowedStep),
    m_maxTrackLength(maxTrackLength),
    m_maxTime(maxTime),
    m_minEkine(minEkine),
    m_minRange(minRange)
  {};  

  /**
   * Get accessor to member m_maxAllowedStep
   * @return the current value of m_maxAllowedStep
   */
  double maxAllowedStep () const {
    return m_maxAllowedStep;
  }

  /**
   * Get accessor to member m_maxTrackLength
   * @return the current value of m_maxTrackLength
   */
  double maxTrackLength () const {
    return m_maxTrackLength;
  }

  /**
   * Get accessor to member m_maxTime
   * @return the current value of m_maxTime
   */
  double maxTime () const {
    return m_maxTime;
  }

  /**
   * Get accessor to member m_minEkine
   * @return the current value of m_minEkine
   */
  double minEkine () const {
    return m_minEkine;
  }

  /**
   * Get accessor to member m_minRange
   * @return the current value of m_minRange
   */
  double minRange () const {
    return m_minRange;
  }

private:
  double m_maxAllowedStep;
  double m_maxTrackLength;
  double m_maxTime;
  double m_minEkine;
  double m_minRange;
};
#endif // SIMATTRIBUTE_H
