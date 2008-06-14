
/** @file Rich1DTabProperty.h
 *
 *  Header file for utility class : Rich::TabulatedProperty1D
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.h,v 1.7 2008-06-14 20:43:56 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

#ifndef RICHDET_RICH1DTABPROPERTY_H
#define RICHDET_RICH1DTABPROPERTY_H 1

// Base class
#include "RichDet/Rich1DTabFunc.h"

// Gaudi
#include "DetDesc/TabulatedProperty.h"

class ISvcLocator;
class IUpdateManagerSvc;

namespace Rich
{

  /** @class TabulatedProperty1D Rich1DTabProperty.h RichDet/Rich1DTabProperty.h
   *
   *  A derived class from RichTabulatedFunction1D for tabulated properties
   *
   *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
   *  @date   2003-08-13
   *
   *  @todo Figure out why releasing the services during destruction causes a crash
   */

  class TabulatedProperty1D : public Rich::TabulatedFunction1D
  {

  public:

    /** Constructor from tabulated property and gsl interpolator type
     *
     *  @param tab         Pointer to a tabulated proper
     *  @param registerUMS Flag to indicate if this interpolator should register
     *                     itself to the UMS, so that it is automatically updated
     *                     when the underlying TabulatedProperty is updated
     *  @param interType   GSL Interpolator type. See
     *                     http://www.gnu.org/software/gsl/manual/gsl-ref_26.html#SEC389
     */
    explicit TabulatedProperty1D( const TabulatedProperty * tab,
                                  const bool registerUMS = false,
                                  const gsl_interp_type * interType = gsl_interp_linear );

    /// Destructor
    virtual ~TabulatedProperty1D( );

    /** The underlying tabulated property used to initialise the interpolator
     *
     *  @return Pointer to the tabulated property
     */
    inline const TabulatedProperty * tabProperty() const
    {
      return m_tabProp;
    }

    /** @brief The UMS update method
     *
     *  Running this triggers a re-initialisation of the interpolator from
     *  the underlying tabulated property.
     *
     *  Can either be called automatically, if configured to do so at
     *  construction, or can be called "by hand" by the user
     *
     *  @return StatusCode indicating if the update was successfully or not
     */
    StatusCode updateTabProp();

  private: // methods

    /// Service locator
    ISvcLocator* svcLocator();

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

}

/** backwards compatibility
 *  @todo remove this typedef */
typedef Rich::TabulatedProperty1D RichTabulatedProperty1D;

#endif // RICHDET_RICH1DTABPROPERTY_H
