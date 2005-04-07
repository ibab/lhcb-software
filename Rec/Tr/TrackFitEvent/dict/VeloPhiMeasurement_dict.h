
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

#ifndef TrackFitEvent_VeloPhiMeasurement_H
#define TrackFitEvent_VeloPhiMeasurement_H 1

// Include files
#include "Event/Measurement.h"
#include "Event/VeloCluster.h"
#include "VeloDet/DeVelo.h"


// Forward declarations

/** @class VeloPhiMeasurement VeloPhiMeasurement.h
 *
 * VeloPhiMeasurement is a measurement made from a VeloPhiCluster. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Apr  7 19:45:17 2005
 * 
 */

class VeloPhiMeasurement: public Measurement
{
public:

  friend class VeloPhiMeasurement_dict;

  /// Constructor from an VeloPhiCluster
  VeloPhiMeasurement(VeloCluster& cluster,
                     DeVelo& det);

  /// Default Constructor
  VeloPhiMeasurement() : m_cluster() {}

  /// Default Destructor
  virtual ~VeloPhiMeasurement() {}

  /// Clone the VeloPhiMeasurement
  VeloPhiMeasurement* clone() const;

  /// Retrieve const  Pointer to the VeloPhiCluster
  const VeloCluster* cluster() const;

  /// Retrieve  Pointer to the VeloPhiCluster
  VeloCluster* cluster();

  /// Update  Pointer to the VeloPhiCluster
  void setCluster(VeloCluster* value);

protected:

private:

  VeloCluster* m_cluster; ///< Pointer to the VeloPhiCluster

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const VeloCluster* VeloPhiMeasurement::cluster() const 
{
  return m_cluster;
}

inline VeloCluster* VeloPhiMeasurement::cluster() 
{
  return m_cluster;
}

inline void VeloPhiMeasurement::setCluster(VeloCluster* value) 
{
  m_cluster = value;
}

inline VeloPhiMeasurement* VeloPhiMeasurement::clone() const 
{

  VeloPhiMeasurement* me = new VeloPhiMeasurement(); *me = *this; return me;
        
}


#endif ///TrackFitEvent_VeloPhiMeasurement_H
