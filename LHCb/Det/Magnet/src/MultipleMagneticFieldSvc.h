#ifndef MULTIPLEMAGNETICFIELDSVC_H
#define MULTIPLEMAGNETICFIELDSVC_H 1

// Include files
#include "GaudiKernel/Service.h"
#include "Kernel/ILHCbMagnetSvc.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Point3DTypes.h"

// Forward declarations
template <class TYPE> class SvcFactory;


/** @class MultipleMagneticFieldSvc MultipleMagneticFieldSvc.h
 *  A service for wrapping multiple MagneticFieldSvc
 *
 *  @author Magnus Lieng
 *  @date   2008-07-03
 */

class MultipleMagneticFieldSvc : public Service,
                         virtual public ILHCbMagnetSvc {

protected:
  
  /// Standard Constructor.
  /// @param  name   String with service name
  /// @param  svc    Pointer to service locator interface
  MultipleMagneticFieldSvc( const std::string& name, ISvcLocator* svc );
  
  /// Virtual destructor.
  virtual ~MultipleMagneticFieldSvc();

public:

  /// Initialise the service (Inherited Service overrides) 
  virtual StatusCode initialize();
  
  /// Finalise the service (Inherited Service overrides) 
  virtual StatusCode finalize();
    
  /** Query the available interfaces.
   * @param riid Requested interface ID
   * @param ppvInterface Pointer to requested interface
   * @return StatusCode indicating SUCCESS or FAILURE.
   */
  virtual StatusCode queryInterface( const InterfaceID& riid, 
                                     void** ppvInterface      );
  
  /// Service type.
  //virtual const InterfaceID& type() const { return IID_IMagneticFieldSvc; };

  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @param[out] fvec Magnectic field vector.
   * @return StatusCode SUCCESS if calculation was performed.
   */
  virtual StatusCode fieldVector( const Gaudi::XYZPoint&  xyz, 
				  Gaudi::XYZVector& fvec ) const;
  
  /** Implementation of IMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @return fvec Magnectic field vector.
   */
  virtual ROOT::Math::XYZVector fieldVector( const Gaudi::XYZPoint&  xyz ) const {
    ROOT::Math::XYZVector bf ;
    fieldVector(xyz,bf).ignore() ;
    return bf ;
  }

  // These should be incorperated in a better way...
  bool   useRealMap() const; ///< True is using real map
  double scaleFactor() const; ///< accessor to m_scaleFactor
  int polarity() const; ///< Polarity (only if they are all the same!) 
  const LHCb::MagneticFieldGrid* fieldGrid() const { return 0 ; }

private:

  /// Allow SvcFactory to instantiate the service.
  friend class SvcFactory<MultipleMagneticFieldSvc>;

  std::vector<std::string> m_names;
  std::vector<ILHCbMagnetSvc*> m_magneticFieldSvcs; ///< Magnetic field services.
};

#endif  // MULTIPLEMAGNETICFIELDSVC_H
