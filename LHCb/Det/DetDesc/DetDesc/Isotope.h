// $Id: Isotope.h,v 1.12 2008-05-20 08:15:06 smenzeme Exp $
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
	   const double      i       = 0               ,
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

 //        Mean excitation energy
  virtual inline const double   I() const;
  virtual inline void        setI(  const double value );

  //       Parameters for density effect correction
  virtual inline const double   C() const;
  virtual inline void        setC(  const double value );
  virtual inline const double   a() const;
  virtual inline void        seta(  const double value );
  virtual inline const double   m() const;
  virtual inline void        setm(  const double value );
  virtual inline const double   X0() const;
  virtual inline void        setX0(  const double value );
  virtual inline const double   X1() const;
  virtual inline void        setX1(  const double value );


  
  //        Number of nucleons
  virtual inline const double   N() const;
  
  inline const CLID& clID    () const { return Isotope::classID(); } 
  static const CLID& classID ()       { return CLID_Isotope; } 
  
protected:
  
  //        Atomic mass [g/mole]
  double m_A;
  
  //        Atomic number
  double m_Z;

  //        Excitation energy
  double m_I;

  //        Parameters for density effect correction
  double m_C;
  double m_a;
  double m_m;
  double m_X0;
  double m_X1;
  
};
///
#include "DetDesc/Isotope.icpp"
///

#endif // DETDESC_ISOTOPE_H
