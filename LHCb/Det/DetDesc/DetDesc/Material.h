/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/Material.h,v 1.2 2001-01-25 12:12:29 mato Exp $
#ifndef DETDESC_MATERIAL_H
#define DETDESC_MATERIAL_H

/// Include files
#include <string>
#include <vector>

#include "CLHEP/Units/PhysicalConstants.h"
#include "GaudiKernel/DataObject.h"

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
  
  /// Equality operators, NOTE! These operator work with pointers
  bool operator==( const Material* right ) const;
  bool operator!=( const Material* right ) const;
  
  /// Material name accessors
  virtual const std::string& name() const;
  virtual void setName( std::string& value );
  virtual void setName( const std::string& value );
  virtual void setName( char* value );
  virtual void setName( const char* value );
  
  /// Material density accessors [g/cm3]
  virtual const double density() const;
  virtual void setDensity( double value );
  
  /// Material temperature, by default is STP [K]
  virtual const double temperature() const;
  virtual void setTemperature( double value );
  
  /// Material pressure, by default is STP [atm]
  virtual const double pressure() const;
  virtual void setPressure( double value );
  
  /// Material state, by default is stateUndefined
  virtual const eState state() const;
  virtual void setState( eState value );
  
  /// Material radiation length [cm]
  virtual const double radiationLength() const;
  virtual void setRadiationLength( double value );
  
  /// Material absorption length [cm]
  virtual const double absorptionLength() const;
  virtual void setAbsorptionLength( double value );
  
  /// Atomic mass [g/mole]
  virtual const double A() const = 0;
  virtual void setA ( double value ) = 0;
  
  /// Atomic number
  virtual const double Z() const = 0;
  virtual void setZ( double value ) = 0;
  
  /// Number of nucleons
  virtual const double N() const = 0;
  virtual void setN( double value ) = 0;

protected:
  
  /// Default Constructor
  Material( std::string name );
  
  /// Constructors of a simple material
  Material( std::string name, double dens, double rl, double al,
    double temp  = STP_Temperature,
    double press = STP_Pressure,
    eState s     = stateUndefined
    );
  
  Material( char* name, double dens, double rl, double al,
    double temp  = STP_Temperature,
    double press = STP_Pressure,
    eState s     = stateUndefined
    );
  
  /// Destructor
  virtual ~Material();
  
private:
  
  // Material name
  std::string m_name;
  
  // Material density [g/cm-3]
  double      m_density;
  
  // Material radiation length [cm]
  double      m_radiationLength;

  // Material absorption length [cm]
  double      m_absorptionLength;
  
  // Material temperature, by default is STP [K]
  double      m_temperature;
  
  // Material pressure, by default is STP [atm]
  double      m_pressure;
  
  // Material state, by default is stateUndefined
  eState      m_state;
  
};


// Class Material 
// Get and Set Operations for Class Attributes
  
inline const std::string& Material::name() const {
  return m_name;
}

inline void Material::setName( std::string& value ) {
  m_name = value;
}

inline void Material::setName( const std::string& value ) {
  m_name = value;
}

inline void Material::setName( char* value ) {
  m_name = std::string( value );
}

inline void Material::setName( const char* value ) {
  m_name = std::string( value );
}

inline const double Material::density() const {
  return m_density;
}

inline void Material::setDensity( double value ) {
  m_density = value;
}

inline const double Material::temperature() const {
  return m_temperature;
}

inline void Material::setTemperature( double value ) {
  m_temperature = value;
}

inline const double Material::pressure() const {
  return m_pressure;
}

inline void Material::setPressure( double value ) {
  m_pressure = value;
}

inline const eState Material::state() const {
  return m_state;
}

inline void Material::setState( eState value ) {
  m_state = value;
}

inline const double Material::radiationLength() const {
  return m_radiationLength;
}

inline void Material::setRadiationLength( double value ) {
  m_radiationLength = value;
}

inline const double Material::absorptionLength() const {
  return m_absorptionLength;
}

inline void Material::setAbsorptionLength( double value ) {
  m_absorptionLength = value;
}

#include "DetDesc/Material.icpp"

#endif // DETDESC_MATERIAL_MATERIAL_H
