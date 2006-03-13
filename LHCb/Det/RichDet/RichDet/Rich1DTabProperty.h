
/** @file Rich1DTabProperty.h
 *
 *  Header file for utility class : Rich1DTabProperty
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.h,v 1.2 2006-03-13 17:47:42 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

#ifndef RICHKERNEL_RICH1DTABPROPERTY_H
#define RICHKERNEL_RICH1DTABPROPERTY_H 1

// Base class
#include "RichDet/Rich1DTabFunc.h"

// Gaudi
#include "DetDesc/TabulatedProperty.h"

class ISvcLocator;
class IUpdateManagerSvc;

/** @class Rich1DTabProperty Rich1DTabProperty.h RichKernel/Rich1DTabProperty.h
 *
 *  A derived class from Rich1DTabFunc for tabulated properties
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

class Rich1DTabProperty : public Rich1DTabFunc {

public:

  /** Constructor from tabulated property and gsl interpolator type
   *
   *  @param tab         Pointer to a tabulated proper
   *  @param registerUMS Flag to indicate if this interpolator should register
   *                     itself to the UMS, so that it is automatically updated
   *                     when the underlying TabulatedProperty is updated
   *  @param interType   GSL Interpolator type
   */
  explicit Rich1DTabProperty( const TabulatedProperty * tab,
                              const bool registerUMS = true,
                              const gsl_interp_type * interType = gsl_interp_linear );

  /// Destructor
  virtual ~Rich1DTabProperty( );

  /** The underlying tabulated property used to initialise the interpolator
   *
   *  @return Pointer to the tabulated property
   */
  inline const TabulatedProperty * tabProperty() const
  {
    return m_tabProp;
  }

private: // methods

  /// Service locator
  ISvcLocator* svcLocator();

  /// UMS update method
  StatusCode updateTabProp();

  /// Access the UpdateManagerSvc
  IUpdateManagerSvc* updMgrSvc();

  /// Access the message service
  IMessageSvc* msgSvc();

private: // data

  /// Pointer to the underlying TabulatedProperty
  const TabulatedProperty * m_tabProp;

  /// The service locator
  ISvcLocator* m_svcLocator;

  /// The Message service
  IMessageSvc* m_msgSvc;

  /// The Update Manager Service
  IUpdateManagerSvc* m_updMgrSvc;

};

#endif // RICHKERNEL_RICH1DTABPROPERTY_H
