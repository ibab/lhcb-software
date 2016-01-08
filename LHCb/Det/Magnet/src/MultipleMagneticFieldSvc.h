#ifndef MULTIPLEMAGNETICFIELDSVC_H
#define MULTIPLEMAGNETICFIELDSVC_H 1

// Include files
#include <vector>
#include "GaudiKernel/Service.h"
#include "Kernel/ILHCbMagnetSvc.h"

/** @class MultipleMagneticFieldSvc MultipleMagneticFieldSvc.h
 *  A service for wrapping multiple MagneticFieldSvc
 *
 *  @author Magnus Lieng
 *  @date   2008-07-03
 */

class MultipleMagneticFieldSvc : public extends<Service, ILHCbMagnetSvc> {

public:
  /// Standard Constructor.
  /// @param  name   String with service name
  /// @param  svc    Pointer to service locator interface
  MultipleMagneticFieldSvc( const std::string& name, ISvcLocator* svc );

  /// Virtual destructor.
  ~MultipleMagneticFieldSvc() override;


  /// Initialise the service (Inherited Service overrides)
  StatusCode initialize() override;

  /// Finalise the service (Inherited Service overrides)
  StatusCode finalize() override;


  using ILHCbMagnetSvc::fieldVector; // avoid hiding compiler  warning...

  /** Implementation of ILHCbMagneticFieldSvc interface.
   * @param[in]  xyz Point at which magnetic field vector will be given
   * @return fvec Magnectic field vector.
   */
  ROOT::Math::XYZVector fieldVector( const Gaudi::XYZPoint&  xyz ) const override;

  // These should be incorperated in a better way...
  bool   useRealMap() const;            ///< True is using real map
  double scaleFactor() const;           ///< accessor to m_scaleFactor
  int polarity() const;                 ///< Polarity (only if they are all the same!)
  bool isDown() const;                  /// sign of the polarity
  double signedRelativeCurrent() const; /// includes scale factor for polarity and current
  const LHCb::MagneticFieldGrid* fieldGrid() const { return 0 ; }


private:

  std::vector<std::string> m_names;
  std::vector<SmartIF<ILHCbMagnetSvc>> m_magneticFieldSvcs; ///< Magnetic field services.
};

#endif  // MULTIPLEMAGNETICFIELDSVC_H
