// $Id: Rich1DTabProperty.h,v 1.1 2004-06-17 12:00:47 cattanem Exp $
#ifndef RICHUTILS_RICH1DTABPROPERTY_H
#define RICHUTILS_RICH1DTABPROPERTY_H 1

// Base class
#include "RichKernel/Rich1DTabFunc.h"

// Gaudi
#include "DetDesc/TabulatedProperty.h"

/** @class Rich1DTabProperty Rich1DTabProperty.h RichKernel/Rich1DTabProperty.h
 *
 *  A derived class from Rich1DTabFunc for tabulated properties
 *
 *  @author Antonis Papanestis   Antonis.Papanestis@cern.ch
 *  @date   2002-08-05
 *  @author Chris Jones          Christopher.Rob.Jones@cern.ch
 *  @date   2003-08-13
 */

class Rich1DTabProperty : public Rich1DTabFunc {

public:

  /// Constructor from tabulated property and gsl interpolator type
  explicit Rich1DTabProperty( const TabulatedProperty * tab,
                              const gsl_interp_type * interType = gsl_interp_linear );

  /// Destructor
  virtual ~Rich1DTabProperty( ) { clearInterpolator(); }

  /// Returns a pointer to the underlying tabulated property
  const TabulatedProperty * tabProperty() const;

private: // data

  /// Pointer to the underlying TabulatedProperty
  const TabulatedProperty * m_tabProp;

};

inline const TabulatedProperty * Rich1DTabProperty::tabProperty() const
{
  return m_tabProp;
}


#endif // RICHUTILS_RICH1DTABPROPERTY_H
