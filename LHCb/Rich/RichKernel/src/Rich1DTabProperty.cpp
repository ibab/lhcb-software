// $Id: Rich1DTabProperty.cpp,v 1.1 2004-06-17 12:00:48 cattanem Exp $

// local
#include "RichKernel/Rich1DTabProperty.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Rich1DTabProperty
//
// 2002-08-05 : Papanestis - First version
// 2003-08-13 : C. Jones   - Update to use base class
//-----------------------------------------------------------------------------

// Constructor from a tabulated property pointer
Rich1DTabProperty::Rich1DTabProperty( const TabulatedProperty * tab,
                                      const gsl_interp_type * interType ) :
  m_tabProp( tab )
{

  // copy data to internal vectors
  for ( TabulatedProperty::Table::const_iterator it = tab->table().begin();
        it != tab->table().end(); ++it ) {
    m_data[ (*it).first ] = (*it).second;
  }

  m_OK = initInterpolator( interType );
}

//============================================================================
