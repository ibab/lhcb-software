// $Id: Rich1DTabProperty.cpp,v 1.2 2004-06-29 19:27:30 jonrob Exp $

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
                                      const gsl_interp_type * interType ) 
  : Rich1DTabFunc (     ),
    m_tabProp     ( tab )
{

  // copy data to internal container
  for ( TabulatedProperty::Table::const_iterator it = tab->table().begin();
        it != tab->table().end(); ++it ) {
    m_data[ (*it).first ] = (*it).second;
  }

  m_OK = initInterpolator( interType );
}

//============================================================================
