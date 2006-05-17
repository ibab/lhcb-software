// $Id: Isotope.h,v 1.11 2006-05-17 16:02:38 cattanem Exp $
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
           const double      density = 0               ,
           const double      rl      = 0               ,  
           const double      al      = 0               ,
           const double      temp    = Gaudi::Units::STP_Temperature , 
           const double      press   = Gaudi::Units::STP_Pressure    , 
           const eState      s       = stateUndefined  );
  // Destructor
  virtual ~Isotope();
  
  //        Atomic mass [g/mole]
  virtual inline const double   A() const;
  virtual inline void        setA(  const double value );
  
  //        Atomic number
  virtual inline const double   Z() const;
  virtual inline void        setZ(  const double value );
  
  //        Number of nucleons
  virtual inline const double   N() const;
  
  inline const CLID& clID    () const { return Isotope::classID(); } 
  static const CLID& classID ()       { return CLID_Isotope; } 
  
protected:
  
  //        Atomic mass [g/mole]
  double m_A;
  
  //        Atomic number
  double m_Z;
 
  
};
///
#include "DetDesc/Isotope.icpp"
///

#endif // DETDESC_ISOTOPE_H
