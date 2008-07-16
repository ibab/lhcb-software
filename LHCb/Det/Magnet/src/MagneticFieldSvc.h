// $Id: MagneticFieldSvc.h,v 1.20 2008-07-16 15:08:57 ahicheur Exp $
#ifndef MAGNETICFIELDSVC_H
#define MAGNETICFIELDSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "GaudiKernel/IMagneticFieldSvc.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"
#include "DetDesc/Condition.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

// Forward declarations
template <class TYPE> class SvcFactory;


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
                         virtual public IMagneticFieldSvc {

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

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<MagneticFieldSvc>;

  StatusCode parseFile( );       ///< Reads the field map from file 
  StatusCode parseRealFiles( );  ///< Reads the real field map from files 
  StatusCode i_updateScaling(); ///< Reads current and polarity from conditions
  double GetScale();
  

  
  std::string m_filename;        ///< Magnetic field file name
  std::string m_qfilename[4];    ///< True Magnetic field file names (one for each quadrant
  std::vector<double> m_Q;       ///< Field vector
  std::vector<double> m_Q_quadr[4];      ///< Field vectors for 4 quadrants real map
  double m_Dxyz[3];              ///< Steps in x, y and z
  int    m_Nxyz[3];              ///< Number of steps in x, y and z
  double m_max_FL[3];    
  double m_min_FL[3];

  double m_zOffSet;              ///< The z offset

  bool                m_useConstField;    ///< Job option to use constant field
  std::vector<double> m_constFieldVector; ///< Option for constant field value
  double              m_scaleFactor;      ///< Option for field scaling factor
  bool                m_useRealMap; ///< To use the real map for data
  double              m_nominalCurrent; ///< To set up the running magnet current for rescaling
  std::string m_condPath; ///< Path to access the field conditions from the database.
  Condition *m_condition; ///< Access the conditions
  bool m_UseConditions;
  IUpdateManagerSvc * m_updMgrSvc;
   

};

#endif  // MAGNETICFIELDSVC_H
