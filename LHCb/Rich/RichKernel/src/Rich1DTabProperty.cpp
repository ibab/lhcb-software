
//-----------------------------------------------------------------------------
/** @file Rich1DTabProperty.cpp
 *
 *  Implementation file for class : Rich1DTabProperty
 *
 *  CVS Log :-
 *  $Id: Rich1DTabProperty.cpp,v 1.4 2004-10-20 16:18:24 jonrob Exp $
 *  $Log: not supported by cvs2svn $
 *  Revision 1.3  2004/07/26 17:53:17  jonrob
 *  Various improvements to the doxygen comments
 *
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-26
 */
//-----------------------------------------------------------------------------

// local
#include "RichKernel/Rich1DTabProperty.h"
#include "RichKernel/RichException.h"

// Constructor from a tabulated property pointer
Rich1DTabProperty::Rich1DTabProperty( const TabulatedProperty * tab,
                                      const gsl_interp_type * interType ) 
  : Rich1DTabFunc (     ),
    m_tabProp     ( tab )
{

  // Check the data is valid
  if ( !tab ) throw RichException("Rich1DTabProperty : Null TabulatedProperty");

  // copy data to internal container
  for ( TabulatedProperty::Table::const_iterator it = tab->table().begin();
        it != tab->table().end(); ++it ) {
    m_data[ (*it).first ] = (*it).second;
  }

  // initialise the underlying GSL interpolator
  m_OK = initInterpolator( interType );

}

