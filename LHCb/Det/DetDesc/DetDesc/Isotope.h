// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/Isotope.h,v 1.6 2001-03-05 14:57:10 ibelyaev Exp $
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
  
  /// Constructors
  Isotope( const std::string name    = ""              , 
           const double      a       = 0               , 
           const double      z       = 0               , 
           const double      n       = 0               , 
           const double      density = 0               ,
	   const double      rl      = 0               ,  
	   const double      al      = 0               ,
	   const double      temp    = STP_Temperature , 
	   const double      press   = STP_Pressure    , 
	   const eState      s       = stateUndefined  );
  // Destructor
  virtual ~Isotope();
  
  //	Atomic mass [g/mole]
  virtual inline const double   A() const;
  virtual inline void        setA(  const double value );
  
  //	Atomic number
  virtual inline const double   Z() const;
  virtual inline void        setZ(  const double value );
  
  //	Number of nucleons
  virtual inline const double   N() const;
  virtual inline void        setN(  const double value );
  
  inline const CLID& clID    () const { return Isotope::classID(); } 
  static const CLID& classID ()       { return CLID_Isotope; } 
  
  /// serialization for read and write 
  virtual StreamBuffer& serialize( StreamBuffer& s )       ; 
  virtual StreamBuffer& serialize( StreamBuffer& s ) const ; 

protected:
  
  //	Atomic mass [g/mole]
  double m_A;
  
  //	Atomic number
  double m_Z;
  
  //	Number of nucleons
  double m_N;
  
};
///
#include "DetDesc/Isotope.icpp"
///

#endif // DETDESC_ISOTOPE_H
