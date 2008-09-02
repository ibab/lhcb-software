// $Id: MagneticFieldSvc.h,v 1.25 2008-09-02 09:11:50 cattanem Exp $
#ifndef MAGNETICFIELDSVC_H
#define MAGNETICFIELDSVC_H 1

// Include files
#include "Kernel/ILHCbMagnetSvc.h"
#include "DetDesc/Condition.h"

#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Service.h"

#include <vector>
#include <string>

// Forward declarations
template <class TYPE> class SvcFactory;
class IToolSvc;
class IUpdateManagerSvc;
class IMagFieldTool;

/** @class MagneticFieldSvc MagneticFieldSvc.h
 *  A service for finding the magnetic field vector at a given
 *  point in space. Based on original implementation by I.Last and
 *  G.Gracia via Gaudi_gufld (modified from SICBMC)
 *
 *  @author Edgar De Oliveira
 *  @date   2002-05-21
 *  Updated and further developped - Adlene Hicheur
 */

class MagneticFieldSvc : public Service,
                         virtual public ILHCbMagnetSvc {

protected:
  
  /// Standard Constructor.
  /// @param  name   String with service name
  /// @param  svc    Pointer to service locator interface
  MagneticFieldSvc( const std::string& name, ISvcLocator* svc );
  
  /// Virtual destructor.
  virtual ~MagneticFieldSvc();

public:

  /// Initialise the service (Inherited Service overrides) 
  virtual StatusCode initialize();
  
 
  
  /** Query the available interfaces.
   * @param riid Requested interface ID
   * @param ppvInterface Pointer to requested interface
   * @return StatusCode indicating SUCCESS or FAILURE.
   */
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface      );
  
  /// Service type.
  virtual const InterfaceID& type() const { return IID_IMagneticFieldSvc; };

  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @param[out] fvec Magnectic field vector.
   * @return StatusCode SUCCESS if calculation was performed.
   */
  virtual StatusCode fieldVector( const Gaudi::XYZPoint&  xyz, 
                                  Gaudi::XYZVector& fvec ) const;
  
  bool   useRealMap() const; ///< True is using real map
  double scaleFactor() const { return m_scaleFactor; } ///< accessor to m_scaleFactor

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<MagneticFieldSvc>;
  StatusCode initializeWithCondDB();    ///< default get magnet data from CondDB
  StatusCode initializeWithoutCondDB(); ///< alternative get magnet data from job options

  StatusCode i_updateConditions();       ///< Reads from conditions
  StatusCode updateTool( int polarity ); ///< Steers conditions to appropriate tool

  // Properties to configure the service
  bool m_UseConditions;      ///< Get data from CondDB or options. Default CondDB
  bool m_UseSetCurrent;      ///< Use Set or Measured current. Default false
  double m_nominalCurrent;   ///< Nominal magnet current to normalise rescaling
  std::string m_mapFilePath; ///< Directory where field map files are located
 
  // Special properties to use constant field (and no condDB!)
  bool                m_useConstField;    ///< Job option to use constant field
  std::vector<double> m_constFieldVector; ///< Option for constant field value

  // Properties to over-ride values in CondDB
  std::vector<std::string> m_mapFileNames; ///< Field map file names
  int                      m_polarity;     ///< Polarity
  double                   m_scaleFactor;  ///< Field scaling factor

  // Private data
  bool m_mapFromOptions;        ///< Set if not using condDB for field map.
  bool m_scaleFromOptions;      ///< Set if not using condDB for scale factor.
  
  Condition* m_mapFilesUpPtr;   ///< Pointer to FieldMapFilesUp condition
  Condition* m_mapFilesDownPtr; ///< Pointer to FieldMapFilesDown condition
  Condition* m_scaleUpPtr;      ///< Pointer to ScaleUp condition
  Condition* m_scaleDownPtr;    ///< Pointer to ScaleDown condition
  Condition* m_currentPtr;      ///< Pointer to Measured or Set condition

  IMagFieldTool* m_fieldTool;     ///< Pointer to current map handling tool
  IMagFieldTool* m_DC06FieldUp;   ///< Pointer to tool handling "Up" DC06 map
  IMagFieldTool* m_DC06FieldDown; ///< Pointer to tool handling "Down" DC06 map
  IMagFieldTool* m_RealFieldUp;   ///< Pointer to tool handling "Up" Real map
  IMagFieldTool* m_RealFieldDown; ///< Pointer to tool handling "Down" Real map

  IUpdateManagerSvc* m_updMgrSvc; ///< Pointer to UpdateManagerSvc
  IToolSvc*          m_toolSvc;   ///< Pointer to ToolSvc

  // Following are obsolete, will be removed soon
  bool m_useRealMap; ///< OBSOLETE: To use the real map for data
  std::string m_filename; ///< OBSOLETE: Magnetic field file name
  std::string m_qfilename[4]; ///< OBSOLETE: True Magnetic field file names (one for each quadrant
  std::string m_condPath; ///< OBSOLETE Path to access the field conditions from the database.

};

#endif  // MAGNETICFIELDSVC_H
