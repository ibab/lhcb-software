
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef TrackFitEvent_OTMeasurement_H
#define TrackFitEvent_OTMeasurement_H 1

// Include files
#include "Event/Measurement.h"
#include "Event/OTTime.h"
#include "OTDet/DeOTDetector.h"


// Forward declarations

/** @class OTMeasurement OTMeasurement.h
 *
 * OTMeasurement is a measurement made from a OTTime. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Apr  7 19:42:10 2005
 * 
 */

class OTMeasurement: public Measurement
{
public:

  /// Constructor from an OTTime
  OTMeasurement(OTTime& otTime,
                DeOTDetector& det,
                int ambiguity,
                double tu = 999.0);

  /// Default Constructor
  OTMeasurement() : m_ambiguity(0),
                    m_tu(0.0),
                    m_time() {}

  /// Default Destructor
  virtual ~OTMeasurement() {}

  /// Clone the OTMeasurement
  OTMeasurement* clone() const;

  /// Retrieve const  the ambiguity (+1/-1) of the measurement
  int ambiguity() const;

  /// Update  the ambiguity (+1/-1) of the measurement
  void setAmbiguity(int value);

  /// Retrieve const  the slope in u of the measurement
  double tu() const;

  /// Update  the slope in u of the measurement
  void setTu(double value);

  /// Retrieve const  Pointer to the OTTime
  const OTTime* time() const;

  /// Retrieve  Pointer to the OTTime
  OTTime* time();

  /// Update  Pointer to the OTTime
  void setTime(OTTime* value);

protected:

private:

  int     m_ambiguity; ///< the ambiguity (+1/-1) of the measurement
  double  m_tu;        ///< the slope in u of the measurement
  OTTime* m_time;      ///< Pointer to the OTTime

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline int OTMeasurement::ambiguity() const 
{
  return m_ambiguity;
}

inline void OTMeasurement::setAmbiguity(int value) 
{
  m_ambiguity = value;
}

inline double OTMeasurement::tu() const 
{
  return m_tu;
}

inline void OTMeasurement::setTu(double value) 
{
  m_tu = value;
}

inline const OTTime* OTMeasurement::time() const 
{
  return m_time;
}

inline OTTime* OTMeasurement::time() 
{
  return m_time;
}

inline void OTMeasurement::setTime(OTTime* value) 
{
  m_time = value;
}

inline OTMeasurement* OTMeasurement::clone() const 
{

  OTMeasurement* me = new OTMeasurement(); *me = *this; return me;
        
}


#endif ///TrackFitEvent_OTMeasurement_H
