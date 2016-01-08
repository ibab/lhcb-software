#ifndef MAGNETICFIELDSVC_H
#define MAGNETICFIELDSVC_H 1

// Include files
#include "Kernel/ILHCbMagnetSvc.h"
#include "DetDesc/Condition.h"
#include "DetDesc/MagneticFieldGrid.h"

#include "MagneticFieldGridReader.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Service.h"

#include <vector>
#include <string>

// Forward declarations
class IUpdateManagerSvc;

/** @class MagneticFieldSvc MagneticFieldSvc.h
 *  A service for finding the magnetic field vector at a given
 *  point in space. Based on original implementation by I.Last and
 *  G.Gracia via Gaudi_gufld (modified from SICBMC)
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 *  Updated and further developped - Adlene Hicheur
 */

class MagneticFieldSvc : public extends<Service, ILHCbMagnetSvc>
{

public:
  /// Standard Constructor.
  /// @param  name   String with service name
  /// @param  svc    Pointer to service locator interface
  MagneticFieldSvc( const std::string& name, ISvcLocator* svc );

  /// Virtual destructor.
  ~MagneticFieldSvc() override;

  /// Initialise the service (Inherited Service overrides)
  StatusCode initialize() override;

  /// Finalise the service (Inherited Service overrides)
  StatusCode finalize() override;


  using ILHCbMagnetSvc::fieldVector;
  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @return Magnetic field vector.
   */
  Gaudi::XYZVector fieldVector( const Gaudi::XYZPoint&  xyz ) const override;

  /// Returns the field grid
  const LHCb::MagneticFieldGrid* fieldGrid() const override
  {
    return &m_magFieldGrid ;
  }

  bool useRealMap() const override; ///< True is using real map

  double signedRelativeCurrent() const override
  {
    const int sign = ( isDown() ? -1 : +1 );
    return std::abs(m_magFieldGrid.scaleFactor())*sign;
  }

  // True if the down polarity map is loaded
  bool isDown() const override;

private:

  StatusCode initializeWithCondDB();    ///< default get magnet data from CondDB
  StatusCode initializeWithoutCondDB(); ///< alternative get magnet data from job options
  StatusCode i_updateConditions();       ///< Reads from conditions

private:

  // Properties to configure the service
  bool m_UseConditions;      ///< Get data from CondDB or options. Default CondDB
  bool m_UseSetCurrent;      ///< Use Set or Measured current. Default false
  double m_nominalCurrent;   ///< Nominal magnet current to normalise rescaling
  std::string m_mapFilePath; ///< Directory where field map files are located

  // Special properties to use constant field (and no condDB!)
  bool                m_useConstField;    ///< Job option to use constant field
  std::vector<double> m_constFieldVector = {{ 0., 0., 0. }}; ///< Option for constant field value

  // Properties to over-ride values in CondDB
  std::vector<std::string> m_mapFileNames;
  bool                     m_forcedToUseDownMap = false;
  bool                     m_forcedToUseUpMap = false;
  double                   m_forcedScaleFactor = 9999.;
  bool                     m_mapFromOptions = false;

  // Private data

  Condition* m_mapFilesUpPtr = nullptr;   ///< Pointer to FieldMapFilesUp condition
  Condition* m_mapFilesDownPtr = nullptr; ///< Pointer to FieldMapFilesDown condition
  Condition* m_scaleUpPtr = nullptr;      ///< Pointer to ScaleUp condition
  Condition* m_scaleDownPtr = nullptr;    ///< Pointer to ScaleDown condition
  Condition* m_currentPtr = nullptr;      ///< Pointer to Measured or Set condition

  SmartIF<IUpdateManagerSvc> m_updMgrSvc; ///< Pointer to UpdateManagerSvc

  MagneticFieldGridReader m_magFieldGridReader ;
  LHCb::MagneticFieldGrid m_magFieldGrid ;

  bool m_isDown = false; ///< Cache the field polarity

  // update the cached field polarity
  void cacheFieldPolarity() {
    m_isDown = m_magFieldGrid.fieldVectorClosestPoint({ 0,0,5200}).y() < 0 ;
  }

};

#endif  // MAGNETICFIELDSVC_H
