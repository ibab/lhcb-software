// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/Lib/Isotope.cpp,v 1.2 2001-01-22 10:55:36 mato Exp $
#include "DetDesc/Isotope.h"

/// RCS Id for identification of object version
///static const char* rcsid = "$Id: Isotope.cpp,v 1.2 2001-01-22 10:55:36 mato Exp $";

// Constructors
Isotope::Isotope( std::string name )
: Material( name ),
  m_A(0.0),
  m_Z(0),
  m_N(0)
{
}

Isotope::Isotope( char* name )
: Material( std::string(name) ),
  m_A(0.0),
  m_Z(0),
  m_N(0)
{
}

Isotope::Isotope( std::string name, double a, double z, double n)
: Material( name ),
  m_A(a),
  m_Z(z),
  m_N(n)
{
}

Isotope::Isotope( char* name, double a, double z, double n)
: Material( std::string(name) ),
  m_A(a),
  m_Z(z),
  m_N(n)
{
}

Isotope::Isotope( std::string name, double a, double z, double n, double density )
: Material( name, density, 0.0, 0.0 ),
  m_A(a),
  m_Z(z),
  m_N(n)
{
}

Isotope::Isotope( char* name, double a, double z, double n, double density )
: Material( std::string(name), density, 0.0, 0.0 ),
  m_A(a),
  m_Z(z),
  m_N(n)
{
}

// Destructor
Isotope::~Isotope()
{
}

