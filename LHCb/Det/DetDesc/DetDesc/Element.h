/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/Element.h,v 1.3 2001-01-25 12:12:29 mato Exp $
#ifndef DETDESC_ELEMENT_H
#define DETDESC_ELEMENT_H

/// Include files
#include <vector>

#include "DetDesc/Material.h"
//#include "DetDesc/Material/Isotope.h"

/// Forward and extern declarations
class Isotope;

static const CLID CLID_Element = 1202;

/** @class Element Element.h DetDesc/Element.h

    Element class defines the Element material component.
    An Element material component can defined by composition of Isotopes.
    The composition is done only by fraction of the mass.

    @author Radovan Chytracek
*/
class Element : public Material
{
  
public:
  
  // Constructors
  Element( std::string name );
  Element( char* name );
  
  Element( std::string name, double a, double z );
  Element( char* name, double a, double z );
  
  Element( std::string name, double a, double z, double n );
  Element( char* name, double a, double z, double n );
  
  Element( std::string name, double a, double z, double n, double density );
  Element( char* name, double a, double z, double n, double density );
  
  Element( std::string name, int nOfIsotops );
  Element( char* name, int nOfIsotops );
  
  // Destructor
  ~Element();
  
  /// Number of isotopes in the material
  const int nOfIsotopes() const;
  
  /// Return an isotope by index
  const Isotope* isotope( unsigned int i ) const;
  Isotope*       isotope( unsigned int i );

  /// Return a fraction of an isotope by index
  double isotopeFraction( unsigned int i ) const;
    
  /** Add an isotope into this material
      After addition of the last isotope user can call this method
      with "comp" argument set to"true" and compute() method will
      do its job automatically, otherwise the user must call compute()
      method explicitly
  */
  void addIsotope( Isotope* iPtr, double fract, bool comp = false );
  
  /** Remove an isotope from the vector of isotopes
      compute() method invocation is done as described above
  */
  void removeIsotope( Isotope* iPtr, bool comp = false );
  
  /// Compute quantities after addition of all isotopes
  void compute();
  
  double coulombFactor();

  double tsaiFactor();

  const CLID& clID() const;
  static const CLID& classID();

  ///	Atomic mass [g/mole]
  const double A() const;
  void setA ( double value );
  
  ///	Atomic number
  const double Z() const;
  void setZ( double value );
  
  ///	Number of nucleons
  const double N() const;
  void setN( double value );

  /// Compute Coulomb factor
  void ComputeCoulombFactor();

  /// Comput Tsai factor
  void ComputeLradTsaiFactor();

private:

  /// Effective atomic mass
  double                m_Aeff;

  /// Effective atomic number
  double                m_Zeff;

  /// Effective number of nucleons
  double                m_Neff;
  
  /// Number of isotopes in the material
  int                   m_nOfIsotopes;
  
  /// Vector of isotopes this material is composed of
  std::vector<Isotope*> m_isotopes;
  
  /** Vector of fractions corresponding to IsotopeVector items 
      defining the abundance of the given isotope in the material
  */
  std::vector<double>   m_fractions;

  /// Coulomb factor
  double                m_coulomb;

  /// Tsai's factor
  double                m_tsai;
  
};


// Class Element 
// Get and Set Operations for Class Attributes
inline const CLID& Element::clID() const {
  return classID();
} 

inline const CLID& Element::classID() {
  return CLID_Element;
} 

inline const int Element::nOfIsotopes() const {
  return m_isotopes.size();
}

// Return an isotope by index
inline const Isotope* Element::isotope( unsigned int i ) const {
  return( ( 0 != m_isotopes.size() ) ? m_isotopes[i] : (Isotope*)0 );
}

inline Isotope* Element::isotope( unsigned int i ) {
  return( ( 0 != m_isotopes.size() ) ? m_isotopes[i] : (Isotope*)0 );
}

// Return a fraction mass of an isotope by index
inline double Element::isotopeFraction( unsigned int i ) const {
  return( ( 0 != m_fractions.size() ) ? m_fractions[i] : 0.0 );
}

inline double Element::coulombFactor()
{
  return m_coulomb;
}

inline double Element::tsaiFactor()
{
  return m_tsai;
}

inline const double Element::A() const {
  return m_Aeff;
}

inline void Element::setA( double value ) {
  m_Aeff = value;
}

inline const double Element::Z() const {
  return m_Zeff;
}

inline void Element::setZ( double value ) {
  m_Zeff = value;
}

inline const double Element::N() const {
  return m_Neff;
}

inline void Element::setN( double value ) {
  m_Neff = value;
}

#endif // DETDESC_MATERIAL_ELEMENT_H
