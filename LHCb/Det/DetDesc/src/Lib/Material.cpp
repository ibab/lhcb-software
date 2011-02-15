// $Id: Material.cpp,v 1.8 2009-04-17 08:54:24 cattanem Exp $ 
// ============================================================================
/// GaudiKernel
#include "GaudiKernel/System.h"
/// DetDesc
#include "DetDesc/Material.h"
#include "DetDesc/TabulatedProperty.h"

using namespace Gaudi::Units;

//////////////////////
Material::Material( const std::string& name        ,
                    const double       dens        ,
                    const double       rl          ,
                    const double       al          ,
                    const double       temp        ,
                    const double       press       ,
                    const eState       s           )
  : DataObject          (            )
  , m_name              ( name       )
  , m_density           ( dens       )
  , m_radiationLength   ( rl         )
  , m_absorptionLength  ( al         )
  , m_temperature       ( temp       )
  , m_pressure          ( press      )
  , m_state             ( s          )
  , m_props             (            )
{ }

//////////////////////
void Material::reset()
{
  setName             ( "" );
  setDensity          ( 0  );
  setRadiationLength  ( 0  );
  setAbsorptionLength ( 0  );
  setTemperature      ( Gaudi::Units::STP_Temperature );
  setPressure         ( Gaudi::Units::STP_Pressure    );
  setState            ( stateUndefined );
}

//////////////////////
Material::~Material() { m_props.clear() ;}

//////////////////////
MsgStream&        Material::fillStream ( MsgStream&   s ) const
{
  ///
  s << " Material/"        << System::typeinfoName( typeid( *this ) )
    << " \tname='"         << name() << "'"
    << endmsg              << "\t"
    << " Atomic[g/mole] ="
    << std::setw(12)   << A()                /(g/mole)
    << " Z            ="   << std::setw(12)   << Z()
    << " #Nucleons  ="     << std::setw(12)   << (int) N()
    << endmsg              << "\t"
    << " Density[g/cm3] ="
    << std::setw(12)   << m_density          /(g/cm3)
    << " X0 [cm]      ="   << std::setw(12)   << m_radiationLength  /cm
    << " Lambda[cm] ="     << std::setw(12)   << m_absorptionLength /cm
    << endmsg              << "\t"
    << " Temperature [K]=" << std::setw(12)   << m_temperature      /kelvin
    << " Pressure[atm]="   << std::setw(12)   << m_pressure         /atmosphere
    << " State="    ;
  ///
  if      ( stateSolid  == m_state ) { s << " Solid   " ; }
  else if ( stateLiquid == m_state ) { s << " Liquid  " ; }
  else if ( stateGas    == m_state ) { s << " Gas     " ; }
  else                               { s << "undefined" ; }
  ///
  s <<  " #properties="       << std::setw(2)   << m_props.size()
    << endmsg ;
  for( SmartRefVector<TabulatedProperty>::const_iterator it =
         m_props.begin() ; m_props.end() != it ; ++it )
  { s << "\t property#" << std::setw(2)
      << it-m_props.begin() << "   " << *it; }
  return s;
}
//////////////////////
std::ostream&     Material::fillStream ( std::ostream& s ) const
{
  ///
  s << " Material/"        << System::typeinfoName( typeid( *this ) )
    << " \tname='"         << name() << "'"
    << std::endl           << "\t"
    << " Atomic[g/mole] ="
    << std::setw(12)   << A()                /(g/mole)
    << " Z            ="   << std::setw(12)   << Z()
    << " #Nucleons  ="     << std::setw(12)   << (int) N()
    << std::endl           << "\t"
    << " Density[g/cm3] ="
    << std::setw(12)   << m_density          /(g/cm3)
    << " X0 [cm]      ="   << std::setw(12)   << m_radiationLength  /cm
    << " Lambda[cm] ="     << std::setw(12)   << m_absorptionLength /cm
    << std::endl           << "\t"
    << " Temperature [K]=" << std::setw(12)   << m_temperature      /kelvin
    << " Pressure[atm]="   << std::setw(12)   << m_pressure         /atmosphere
    << " State="    ;
  ///
  if      ( stateSolid  == m_state ) { s << " Solid   " ; }
  else if ( stateLiquid == m_state ) { s << " Liquid  " ; }
  else if ( stateGas    == m_state ) { s << " Gas     " ; }
  else                               { s << "undefined" ; }
  ///
  s << " #properties="    << std::setw(2)   << m_props.size()
    << std::endl;
  for( SmartRefVector<TabulatedProperty>::const_iterator it =
         m_props.begin() ; m_props.end() != it ; ++it )
  { s << "\t property#" << std::setw(2)
      << it-m_props.begin() << "   " << *it; }
  return s;
}


// ============================================================================
// End
// ============================================================================
