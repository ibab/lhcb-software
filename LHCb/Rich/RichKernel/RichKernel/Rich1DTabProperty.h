/** @file Rich1DtabProperty.h
 *
 *  Header file for utility class : Rich1DtabProperty
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.h,v 1.3 2004-07-26 17:53:17 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

#ifndef RICHKERNEL_RICH1DTABPROPERTY_H
#define RICHKERNEL_RICH1DTABPROPERTY_H 1

// Base class
#include "RichKernel/Rich1DTabFunc.h"

// Gaudi
#include "DetDesc/TabulatedProperty.h"

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
   *  @param tab        Pointer to a tabulated proper
   *  @param interType  GSL Interpolator type
   */
  explicit Rich1DTabProperty( const TabulatedProperty * tab, 
                              const gsl_interp_type * interType = gsl_interp_linear );

  /// Destructor
  virtual ~Rich1DTabProperty( ) { clearInterpolator(); }

  /** The underlying tabulated property used to initialise the interpolator
   *
   *  @return Pointer to the tabulated property
   */
  inline const TabulatedProperty * tabProperty() const
  {
    return m_tabProp;
  }

private: // data

  /// Pointer to the underlying TabulatedProperty
  const TabulatedProperty * m_tabProp;

};

#endif // RICHKERNEL_RICH1DTABPROPERTY_H
