// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/Isotope.h,v 1.4 2001-01-25 15:36:43 ibelyaev Exp $
#ifndef DETDESC_ISOTOPE_H
#define DETDESC_ISOTOPE_H

// Include files
#include "DetDesc/Material.h"

#include "DetDesc/CLIDIsotope.h"

/** @class Isotope Isotope.h DetDesc/Isotope.h

    Isotope class defines the Isotope material component.
    An Element material component can defined by composition of Isotopes.
    See the Element class for details.

   @author Radovan Chytracek
*/
class Isotope : public Material
{
  
public:
  
  // Constructors
  Isotope( std::string name );
  Isotope( char* name );
  Isotope( std::string name, double a, double z, double n );
  Isotope( char* name, double a, double z, double n );
  Isotope( std::string name, double a, double z, double n, double density );
  Isotope( char* name, double a, double z, double n, double density );
  
  // Destructor
  ~Isotope();
  
  //	Atomic mass [g/mole]
  const double A() const;
  void setA ( double value );
  
  //	Atomic number
  const double Z() const;
  void setZ( double value );
  
  //	Number of nucleons
  const double N() const;
  void setN( double value );
  
  const CLID& clID() const;
  static const CLID& classID();
  
protected:
  
  //	Atomic mass [g/mole]
  double m_A;
  
  //	Atomic number
  double m_Z;
  
  //	Number of nucleons
  double m_N;
  
};

// Get and Set Operations for Class Attributes
inline const CLID& Isotope::clID() const {
  return classID();
} 

inline const CLID& Isotope::classID() {
  return CLID_Isotope;
} 

inline const double Isotope::A() const {
  return m_A;
}

inline void Isotope::setA( double value ) {
  m_A = value;
}

inline const double Isotope::Z() const {
  return m_Z;
}

inline void Isotope::setZ( double value ) {
  m_Z = value;
}

inline const double Isotope::N() const {
  return m_N;
}

inline void Isotope::setN( double value ) {
  m_N = value;
}

#endif // DETDESC_ISOTOPE_H
