/// GaudiKernel
#include "GaudiKernel/System.h"
/// DetDecs 
#include "DetDesc/Material.h"
#include "DetDesc/TabulatedProperty.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
Material::Material( const std::string& name   , 
		    const double       dens   , 
		    const double       rl     , 
		    const double       al     ,
		    const double       temp   , 
		    const double       press  , 
		    const eState       s      )
  : DataObject          ( name       )
  , m_name              ( ""         )
  , m_density           ( dens       )
  , m_radiationLength   ( rl         )
  , m_absorptionLength  ( al         )
  , m_temperature       ( temp       )
  , m_pressure          ( press      )
  , m_state             ( s          ) 
  , m_props             (            )
{};
/////////////////////////////////////////////////////////////////////////////////////////////////
Material::~Material() { m_props.clear() ;}
/////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer&     Material::serialize( StreamBuffer& s ) const 
{
  DataObject::serialize( s ) ;
  return s << m_name
	   << m_density          
	   << m_radiationLength 
	   << m_absorptionLength
	   << m_temperature      
	   << m_pressure         
	   <<( unsigned int ) m_state            
           << m_props (this );
};
/////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer&     Material::serialize( StreamBuffer& s )       
{
  DataObject::serialize( s ) ;
  s >> m_name
    >> m_density          
    >> m_radiationLength 
    >> m_absorptionLength
    >> m_temperature      
    >> m_pressure     ;
  unsigned int i ;
  s >> i ;
  m_state = (eState) i ;
  return s >>  m_props (this );
};
/////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&        Material::fillStream ( MsgStream&   s ) const 
{
  ///
  s << " Material/"        << System::typeinfoName( typeid( *this ) )  
    << " \tname='"         << name() << "'"
    << endreq              << "\t" 
    << " Atomic[g/mole] =" << std::setw(12)   << A()                /(g/mole)               
    << " Z            ="   << std::setw(12)   << Z() 
    << " #Nucleons  ="     << std::setw(12)   << (int) N()  
    << endreq              << "\t" 
    << " Density[g/cm3] =" << std::setw(12)   << m_density          /(g/cm3)            
    << " X0 [cm]      ="   << std::setw(12)   << m_radiationLength  /cm      
    << " Lambda[cm] ="     << std::setw(12)   << m_absorptionLength /cm        
    << endreq              << "\t" 
    << " Temperature=[K]=" << std::setw(12)   << m_temperature      /kelvin     
    << " Pressure[atm]="   << std::setw(12)   << m_pressure         /atmosphere
    << " State="    ;   
  ///
  if      ( stateSolid  == m_state ) { s << " Solid   " ; } 
  else if ( stateLiquid == m_state ) { s << " Liquid  " ; }
  else if ( stateGas    == m_state ) { s << " Gas     " ; }
  else                               { s << "undefined" ; } 
  ///
  s <<  " #properties="       << std::setw(2)   << m_props.size() 
    << endreq ;
  for( SmartRefVector<TabulatedProperty>::const_iterator it = m_props.begin() ; m_props.end() != it ; ++it )
    { s << "\t property#" << std::setw(2) << it-m_props.begin() << "   " << *it; }
  return s;
};    
/////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream&     Material::fillStream ( std::ostream& s ) const 
{
  ///
  s << " Material/"        << System::typeinfoName( typeid( *this ) )  
    << " \tname='"         << name() << "'"
    << std::endl           << "\t" 
    << " Atomic[g/mole] =" << std::setw(12)   << A()                /(g/mole)               
    << " Z            ="   << std::setw(12)   << Z() 
    << " #Nucleons  ="     << std::setw(12)   << (int) N()  
    << std::endl           << "\t" 
    << " Density[g/cm3] =" << std::setw(12)   << m_density          /(g/cm3)            
    << " X0 [cm]      ="   << std::setw(12)   << m_radiationLength  /cm      
    << " Lambda[cm] ="     << std::setw(12)   << m_absorptionLength /cm        
    << std::endl           << "\t" 
    << " Temperature=[K]=" << std::setw(12)   << m_temperature      /kelvin     
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
  for( SmartRefVector<TabulatedProperty>::const_iterator it = m_props.begin() ; m_props.end() != it ; ++it )
    { s << "\t property#" << std::setw(2) << it-m_props.begin() << "   " << *it; }
  return s;
};    
/////////////////////////////////////////////////////////////////////////////////////////////////












