/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/Mixture.h,v 1.4 2001-01-25 15:36:44 ibelyaev Exp $
#ifndef DETDESC_MIXTURE_H
#define DETDESC_MIXTURE_H

/// Include files
#include <vector>

#include "DetDesc/Material.h"
#include "DetDesc/Element.h"
#include "DetDesc/CLIDMixture.h"


/** @class Mixture Mixture.h DetDesc/Mixture.h

    The Mixture class can be composed of Elements and other Mixtures.
    Mixture of elements can be compoased either by fraction of the mass
    or by number of atoms of the each component.
    Mixture of mixtures and elements is composed only by fraction of the
    mass.

    @author Radovan Chytracek
*/
class Mixture : public Material
{
  
public:
  
  /// Constructor fort a simple material
  Mixture( std::string name, double dens, double rl, double al,
    double temp, double press, eState s
    );
  
  /// Constructors for simple materials
  Mixture( std::string name, double a, double z, double density,
    double rl, double al,
    double temp  = STP_Temperature,
    double press = STP_Pressure,
    eState s     = stateUndefined
    );
  Mixture( char* name, double a, double z, double density,
    double rl, double al,
    double temp  = STP_Temperature,
    double press = STP_Pressure,
    eState s     = stateUndefined
    );
  
  /** Constructors for materials filled later by addition of elements
      and/or other mixtures
  */
  Mixture( std::string name, double density, int nOfItms );
  Mixture( char*        name, double density, int nOfItms );
  
  /// Destructor
  ~Mixture();
  
  /// Number of components in the material mixture
  const int nOfItems() const;
  
  /** Add an element into the mixture by specifying:
      a) the fraction of the mass ( material mixture )
      b) the number of the atoms  ( material compund )
  */
  void Mixture::addElement( Element* e, int nOfAtoms, bool comp = false);
  void Mixture::addElement( Element* e, double fraction, bool comp = false);
  
  /** Add another mixture into this mixture by specifying its fraction
      of the mass
  */
  void Mixture::addMixture( Mixture* mx, double fraction, bool comp = false);
  
  /// Return i-th element's pointer
  const Element* element( int i ) const;
  Element*       element( int i );
  
  /// Return fraction of mass of the i-th element
  double   elementFraction( int i );
  
  /// Return i-th element's number of atoms per element's volume
  double   elementnOfAtoms( int i );
  
  /// Compute effective A,Z and fractions for compounds
  void computeByAtoms();
  
  /// Compute effective A,Z and number of atoms per volume for each element
  void computeByFraction();
  
  /// Obligatory implementation of IMaterial interface
  
  ///	Atomic mass [g/mole]
  const double A() const;
  void setA ( double value );
  
  ///	Atomic number
  const double Z() const;
  void setZ( double value );
  
  ///	Number of nucleons
  const double N() const;
  void setN( double value );
  
  const CLID& clID() const;
  static const CLID& classID();
  
private:
  
  /// Number of components in the material mixture
  int                    m_nOfItems;
  
  /// Vector of elements
  std::vector<Element*>  m_elements;
  
  /// Corresponding fractions of mass for each element
  std::vector<double>    m_fractions;
  
  /// Corresponding number of atoms for each element
  std::vector<double>    m_atoms;
  
  /// Effective atomic mass
  double                 m_aEff;
  
  /// Effective atomic number
  double                 m_zEff;
  
};

// Class Mixture 
// Get and Set Operations for Class Attributes (inline)
inline const CLID& Mixture::clID() const {
  return classID();
} 

inline const CLID& Mixture::classID() {
  return CLID_Mixture;
} 
  
inline const int Mixture::nOfItems() const {
  return m_elements.size();
}

// Obligatory implementation of Material interface

/// Effective A is returned
inline const double Mixture::A() const {
  return m_aEff;
}

/// Effective A is computed, can't be set explicitly
inline void Mixture::setA( double /*value*/ ){ }

/// Effective Z is returned
inline const double Mixture::Z() const { 
  return m_zEff; 
}

/// Effective Z is computed, can't be set explicitly
inline void Mixture::setZ( double /*value*/ ){ }

/// Does it make sense to return the number of nucleons for a mixture ???
inline const double Mixture::N() const { 
  return 0.0; 
}

/// Does it make sense to set the number of nucleons for a mixture ???
inline void Mixture::setN( double /*value*/ ){ }

/// Return i-th element's pointer
inline const Element* Mixture::element( int i ) const {
  return( (0 != m_elements.size()) ? m_elements[i] : (Element *)0);
}

inline Element*       Mixture::element( int i ) {
  return( (0 != m_elements.size()) ? m_elements[i] : (Element *)0);
}

/// Return fraction of mass of the i-th element
inline double Mixture::elementFraction( int i ) {
  return( (0 != m_fractions.size()) ? m_fractions[i] : 0.0 );
}

/// Return i-th element's number of atoms per element's volume
inline double Mixture::elementnOfAtoms( int i ) {
  return( (0 != m_atoms.size()) ? m_atoms[i] : 0 );
}
  
#endif // DETDESC_MIXTURE_H
  
