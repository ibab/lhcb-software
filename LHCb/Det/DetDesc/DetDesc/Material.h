// $Id: Material.h,v 1.13 2008-10-28 12:04:37 cattanem Exp $
#ifndef DETDESC_MATERIAL_H
#define DETDESC_MATERIAL_H
/// STL
#include <string>
#include <vector>
/// GaudiKernel
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartRefVector.h"
#include "GaudiKernel/PhysicalConstants.h"
///
class TabulatedProperty;
class MsgStream;

// Material states, by default is stateUndefined
typedef enum {
  stateUndefined = 0,
  stateSolid,
  stateLiquid,
  stateGas
} eState;

/** @class Material Material.h DetDesc/Material.h

    Material class is a base class for all brands of material
    component classes: Isotope, Element, Mixture.
    Defines a common interface to access the info about a material.
    The end user should always deal with materials via a pointer to
    the Material class.

    @author Radovan Chytracek
*/
class Material : public DataObject
{
public:
  ///
  typedef SmartRefVector<TabulatedProperty>   Tables;
  ///
public:
  
  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const ;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const ;
  ///

  /// Equality operators, NOTE! These operator work with pointers
  bool operator==( const Material* right ) const;
  bool operator!=( const Material* right ) const;
  
  /// Material name accessors
  virtual const std::string& name() const;
  virtual void            setName(  const std::string& value );
  
  /// Material density accessors [g/cm3]
  virtual double  density() const;
  virtual void setDensity(  const double value );
  
  /// Material temperature, by default is STP [K]
  virtual double  temperature() const;
  virtual void setTemperature(  const double value );
  
  /// Material pressure, by default is STP [atm]
  virtual double  pressure() const;
  virtual void setPressure(  const double value );
  
  /// Material state, by default is stateUndefined
  virtual eState  state() const;
  virtual void setState(  const eState value );
  
  /// Material radiation length [cm]
  virtual double  radiationLength() const;
  virtual void setRadiationLength(  const double value );
  
  /// Material absorption length [cm]
  virtual double  absorptionLength() const;
  virtual void setAbsorptionLength(  const double value );
  
  /// Atomic mass [g/mole]
  virtual double  A() const = 0;
  virtual void setA(  const double value ) = 0;
  
  /// Atomic number
  virtual double  Z() const = 0;
  virtual void setZ(  const double value ) = 0; 

  /// Mean excitiation energy
  virtual double  I() const = 0;
  virtual void setI(  const double value ) = 0;

  /// Parameters for density effect correction
  virtual double  C()  const = 0;
  virtual void setC(   const double value ) = 0;
  virtual double  a()  const = 0;
  virtual void seta(   const double value ) = 0;
  virtual double  m()  const = 0;
  virtual void setm(   const double value ) = 0;
  virtual double  X1() const = 0;
  virtual void setX1(  const double value ) = 0;
  virtual double  X0() const = 0;
  virtual void setX0(  const double value ) = 0;

  /// Number of nucleons
  virtual double N() const = 0;
  
  /// some tabulated properties
  inline       Tables&  tabulatedProperties()       ;
  inline const Tables&  tabulatedProperties() const ;
  ///
  
protected:
  
  /// Constructors of a simple material
  Material( const std::string& name  = "", 
            const double       dens  = 0 , 
            const double       rl    = 0 , 
            const double       al    = 0 ,
            const double       temp  = Gaudi::Units::STP_Temperature,
            const double       press = Gaudi::Units::STP_Pressure,
            const eState       s     = stateUndefined );
  
  /// Destructor
  virtual ~Material();
  
private:
  
  // Material name 
  std::string m_name;

  // Material density
  double      m_density;
  
  // Material radiation length 
  double      m_radiationLength;

  // Material absorption length 
  double      m_absorptionLength;
  
  // Material temperature, by default is STP [K]
  double      m_temperature;
  
  // Material pressure, by default is STP [atm]
  double      m_pressure;
  
  // Material state, by default is stateUndefined
  eState      m_state;
  
  // Tabulated properties of material
  Tables      m_props;
  
};
///
#include "DetDesc/Material.icpp"
///

#endif // DETDESC_MATERIAL_MATERIAL_H
