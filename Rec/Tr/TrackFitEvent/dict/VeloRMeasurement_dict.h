
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

#ifndef TrackFitEvent_VeloRMeasurement_H
#define TrackFitEvent_VeloRMeasurement_H 1

// Include files
#include "Event/Measurement.h"
#include "Event/VeloCluster.h"
#include "VeloDet/DeVelo.h"


// Forward declarations

/** @class VeloRMeasurement VeloRMeasurement.h
 *
 * VeloRMeasurement is a measurement made from a VeloRCluster. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Apr  7 19:43:18 2005
 * 
 */

class VeloRMeasurement: public Measurement
{
public:

  friend class VeloRMeasurement_dict;

  /// Constructor from an VeloRCluster
  VeloRMeasurement(VeloCluster& cluster,
                   DeVelo& det,
                   double phi = 999.0);

  /// Default Constructor
  VeloRMeasurement() : m_phi(0.0),
                       m_cluster() {}

  /// Default Destructor
  virtual ~VeloRMeasurement() {}

  /// Clone the VeloRMeasurement
  VeloRMeasurement* clone() const;

  /// Retrieve const  the phi of the Velo R measurement
  double phi() const;

  /// Update  the phi of the Velo R measurement
  void setPhi(double value);

  /// Retrieve const  Pointer to the VeloRCluster
  const VeloCluster* cluster() const;

  /// Retrieve  Pointer to the VeloRCluster
  VeloCluster* cluster();

  /// Update  Pointer to the VeloRCluster
  void setCluster(VeloCluster* value);

protected:

private:

  double       m_phi;     ///< the phi of the Velo R measurement
  VeloCluster* m_cluster; ///< Pointer to the VeloRCluster

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline double VeloRMeasurement::phi() const 
{
  return m_phi;
}

inline void VeloRMeasurement::setPhi(double value) 
{
  m_phi = value;
}

inline const VeloCluster* VeloRMeasurement::cluster() const 
{
  return m_cluster;
}

inline VeloCluster* VeloRMeasurement::cluster() 
{
  return m_cluster;
}

inline void VeloRMeasurement::setCluster(VeloCluster* value) 
{
  m_cluster = value;
}

inline VeloRMeasurement* VeloRMeasurement::clone() const 
{

  VeloRMeasurement* me = new VeloRMeasurement(); *me = *this; return me;
        
}


#endif ///TrackFitEvent_VeloRMeasurement_H
