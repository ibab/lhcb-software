
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

#ifndef TrackFitEvent_ITMeasurement_H
#define TrackFitEvent_ITMeasurement_H 1

// Include files
#include "Event/Measurement.h"
#include "Event/ITCluster.h"
#include "STDet/DeSTDetector.h"


// Forward declarations

/** @class ITMeasurement ITMeasurement.h
 *
 * ITMeasurement is a measurement made from a IT cluster. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Apr  7 19:37:00 2005
 * 
 */

class ITMeasurement: public Measurement
{
public:

  /// Constructor from an ITCluster
  ITMeasurement(ITCluster& itCluster,
                DeSTDetector& det);

  /// Default Constructor
  ITMeasurement() : m_cluster() {}

  /// Default Destructor
  virtual ~ITMeasurement() {}

  /// Clone the ITMeasurement
  ITMeasurement* clone() const;

  /// Retrieve const  Pointer to the ITCluster
  const ITCluster* cluster() const;

  /// Retrieve  Pointer to the ITCluster
  ITCluster* cluster();

  /// Update  Pointer to the ITCluster
  void setCluster(ITCluster* value);

protected:

private:

  ITCluster* m_cluster; ///< Pointer to the ITCluster

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const ITCluster* ITMeasurement::cluster() const 
{
  return m_cluster;
}

inline ITCluster* ITMeasurement::cluster() 
{
  return m_cluster;
}

inline void ITMeasurement::setCluster(ITCluster* value) 
{
  m_cluster = value;
}

inline ITMeasurement* ITMeasurement::clone() const 
{

  ITMeasurement* me = new ITMeasurement(); *me = *this; return me;
        
}


#endif ///TrackFitEvent_ITMeasurement_H
