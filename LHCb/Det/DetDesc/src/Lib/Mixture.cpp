/// DetDesc 
#include "DetDesc/MaterialException.h"
#include "DetDesc/Mixture.h"
///

//////////////////////////////////////////////////////////////////////////////////////////
Mixture::Mixture( const std::string&  name    , 
		  const double        a       , 
		  const double        z       , 
		  const double        density ,
		  const double        rl      , 
		  const double        al      ,
		  const double        temp    ,
		  const double        press   ,
		  const eState s              )
  : Material( name ,         density , rl, al , temp , press , s )
  , m_elements(   )
  , m_atoms   (   )
  , m_own     ( 0 )
  , m_A       ( a )
  , m_Z       ( z )
  , m_N       ( 0 )
{};
//////////////////////////////////////////////////////////////////////////////////////////
Mixture::~Mixture() {}
//////////////////////////////////////////////////////////////////////////////////////////////////////
const SmartRef<Element>& Mixture::element( const unsigned int i ) const 
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].second ; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
      SmartRef<Element>& Mixture::element( const unsigned int i )        
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].second ; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
const double Mixture::elementFraction( const unsigned int i ) const        
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].first ; 
};
//////////////////////////////////////////////////////////////////////////////////////////////////////
void Mixture::addElement( const SmartRef<Element>& e, const int nOfAtoms , const bool comp )  
{
  ///
  if( m_own )
    { throw MaterialException("Mixture::addElement, could not add element to 'simple'-mixture",this);}
  if( !e    ) 
    { throw MaterialException("Mixture::addElement, could not add (Element*)=0! ",this) ; }
  if( m_elements.size() != m_atoms.size() )
    { throw MaterialException("Mixture::addElement:: mismatch in container sizes! ",this);}
  ///
  m_elements.push_back( Entry( 0 , e ) );
  m_atoms.push_back   ( nOfAtoms       ) ; 
  ///
  if( true == comp )  { computeByAtoms(); }
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
void Mixture::addElement( const SmartRef<Element>& e, const double fraction, const bool comp )  
{ 
  if( m_own            )
    { throw MaterialException("Mixture::addElement, could not add element to 'simple'-mixture ",this);}
  if( !e               ) 
    { throw MaterialException("Mixture::addElement: non valid pointer!"+name());}
  if( !m_atoms.empty() )
    { throw MaterialException("Mixture::addElement: could not add Factions to Atoms! ",this); }
  ///
  Elements::iterator it  = m_elements.begin() ; 
  while( m_elements.end() != it ) 
    { if( e->name() == it->second->name() ) { it->first += fraction ; break ; } ; ++it ; } 
  ///
  if( m_elements.end() == it )
    { m_elements.push_back( Entry( fraction , e ) ); }  
  ///  
  if( true == comp ) { compute() ; }
};
//////////////////////////////////////////////////////////////////////////////////////////
void Mixture::addMixture( const SmartRef<Mixture>& mx, const double fraction, const bool comp)  
{
  if( !mx ) { throw MaterialException("Mixture::addMixture:: non valid pointer!");}
  ///
  for( Elements::const_iterator it = mx->elements().begin() ; mx->elements().end() != it ; ++it )
    { 
      addElement( it->second , fraction*(it->first) ); 
    }   
  ///  
  if( true == comp ) { compute() ; }
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode Mixture::compute()
{
  if      ( m_atoms.empty()                      ) { return computeByFraction() ; }
  else if ( m_atoms.size () == m_elements.size() ) { return computeByAtoms   () ; } 
  else 
    { throw MaterialException("Mixture::compute: could not compute "+name() );}
  return StatusCode::FAILURE ; 
};
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode Mixture::computeByAtoms()  
{
  ///
  if( m_elements.empty() ) { addMyself() ;}
  ///
  if( m_elements.size() != m_atoms.size() )
    { throw MaterialException("Mixture::computeByAtoms:: mismatch in container sizes!",this);}
  ///
  /// Compute molecular weight
  double Amol     = 0.0;
  for( unsigned int i1 = 0 ; i1 < m_elements.size() ; ++i1 )
    { Amol += m_atoms[i1] * m_elements[i1].second->A(); }
  ///
  /// Compute proprotion by weight for each element
  for( unsigned int i2 = 0; i2 < m_elements.size() ;  ++i2  )      
    { m_elements[i2].first = ( m_atoms[i2] * m_elements[i2].second->A() ) / Amol; }
  ///
  m_atoms.clear();
  ///
  return computeByFraction();
  ///
}
//////////////////////////////////////////////////////////////////////////////////////////
StatusCode Mixture::computeByFraction()  
{
  ///
  if( !m_atoms.empty() )
    { throw MaterialException("Mixture::computeByFraciton! Atoms nust be empty!",this);}
  //
  if( m_elements.empty() ) { addMyself() ; m_atoms.clear() ; }
  ///
  /// recompute fractions
  double frsum = 0.0  ;
  for( Elements::iterator it = m_elements.begin() ; m_elements.end() != it ; ++it )
    { frsum += it->first ; } 
  if( frsum < 0 ) 
    { throw MaterialException("Mixture::computeByFractions:: not positive fraction sum!",this);}
  /// rescale  fractions
  for( Elements::iterator it = m_elements.begin() ; m_elements.end() != it ; ++it )
    {  it->first /= frsum ; } 
  ///
  m_A = 0 ;
  m_Z = 0 ;
  m_N = 0 ;
  ///
  double radlen = 0 ; 
  double lambda = 0 ;
  ///
  for( Elements::iterator it = m_elements.begin() ; m_elements.end() != it ; ++it )
    { 
      double   frac = it->first  ;
      Element* elem = it->second ;
      /// 
      m_A += frac * elem->A() ;
      m_Z += frac * elem->Z() ;
      m_N += frac * elem->N() ;
      ///
      double  Atomsi = Avogadro * density() * frac / elem->A();
      radlen  += Atomsi * elem->tsaiFactor() ; 
      ///
      if( elem->absorptionLength() > 0.0 ) 
	{ lambda += frac/elem->absorptionLength() ; }
    } 
  ///
  if( radlen > 0 ) { setRadiationLength  ( 1.0 / radlen ) ; } 
  if( lambda > 0 ) { setAbsorptionLength ( 1.0 / lambda ) ; } 
  ///
  return 
    A()       <= 0 ? StatusCode::FAILURE :
    Z()       <= 0 ? StatusCode::FAILURE :
    N()       <= 0 ? StatusCode::FAILURE :
    radlen    <= 0 ? StatusCode::FAILURE :
    density() <= 0 ? StatusCode::FAILURE : 
    lambda    <= 0 ? StatusCode::FAILURE : StatusCode::SUCCESS ;
  ///
};
//////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer&    Mixture::serialize ( StreamBuffer& s ) const 
{
  Material::serialize( s );
  s << A() << Z() << N() ;
  s << ( ( 0 == m_own ) ? m_elements.size() : m_elements.size() - 1 ) ; 
  for( Elements::const_iterator i1 = m_elements.begin() ; m_elements.end() != i1 ; ++i1 )
    { if( m_own != i1->second ) { s << i1->first << i1->second(this) ; } }
  s << m_atoms.size() ;  
  for( Atoms::const_iterator i2 = m_atoms.begin() ; m_atoms.end() != i2 ; ++i2 )
    { s << *i2 ; }
  return s ;
};
/////////////////////////////////////////////////////////////////////////////////////////////////
StreamBuffer&     Mixture::serialize ( StreamBuffer& s )       
{
  Material::serialize( s );
  s >> m_A >> m_Z >> m_N ; 
  Elements::size_type size1;
  s >> size1 ; 
  m_elements.clear();
  m_elements = Elements( size1 );
  for( Elements::iterator i1 = m_elements.begin() ; m_elements.end() != i1 ; ++i1 )
    { s >> i1->first >> i1->second(this) ; }
  m_own = 0 ;
  ///
 Atoms::size_type size2;
  s >> size2;
  m_atoms.clear();
  m_atoms = Atoms( size2 );
  for( Atoms::iterator i2 = m_atoms.begin() ; m_atoms.end() != i2 ; ++i2 )
    { s >> *i2 ; }
  //
  compute();
  ///
  return s ;
};
/////////////////////////////////////////////////////////////////////////////////////////////////
MsgStream&        Mixture::fillStream ( MsgStream&   s ) const 
{
  Material::fillStream( s ) ;
  if( !m_elements.empty() )
    {
      s << "\t #MassComponents=" << std::setw(2)  << m_elements.size() 
	<< endreq ;
      for( Elements::const_iterator it  = m_elements.begin() ; m_elements.end() != it ; ++it )
	{ 
	  s << "\t\tcomponent#"     << std::setw(2) << it - m_elements.begin() 
	    << " massfraction="     << std::setw(9) << it->first 
	    << endreq 
	    << "\t"                 << it->second; 
	}
    }
  ///
  if( !m_atoms.empty() )
    {
      s << "\t #AtomComponents=" << std::setw(2)  << m_atoms.size() 
	<< endreq ;
      for( Atoms::const_iterator it  = m_atoms.begin() ; m_atoms.end() != it ; ++it )
	{ 
	  s << "\t\tcomponent#"  << std::setw(2) << it - m_atoms.begin() 
	    << " atom="          << std::setw(2) << it 
	    << endreq ; 
	}
    }
  return s;
};    
/////////////////////////////////////////////////////////////////////////////////////////////////
std::ostream&     Mixture::fillStream ( std::ostream& s ) const 
{
  Material::fillStream( s ) ;
  if( !m_elements.empty() ) 
    {
      s << "\t #MassComponents=" << std::setw(2)  << m_elements.size()
	<< std::endl ;
      for( Elements::const_iterator it  = m_elements.begin() ; m_elements.end() != it ; ++it )
	{ 
	  s << " \t\tcomponent#"    << std::setw(2) << it - m_elements.begin() 
	    << " massfraction="     << std::setw(9) << it->first 
	    << std::endl
	    << "\t"                 << it->second; 
	}
    }
  ///
  if( !m_atoms.empty() )
    {
      s << "\t #AtomComponents=" << std::setw(2)  << m_atoms.size() 
	<< std::endl;
      for( Atoms::const_iterator it  = m_atoms.begin() ; m_atoms.end() != it ; ++it )
	{ 
	  s << " \t\tcomponent#"    << std::setw(2) << it - m_atoms.begin() 
	    << " atom="             << std::setw(2) << it 
	    << std::endl ; 
	}
    }
  return s;
};    
/////////////////////////////////////////////////////////////////////////////////////////////////
StatusCode Mixture::addMyself() 
{
  if( 0 != m_own         ) 
    { throw MaterialException("Mixture::addMyself: could not add myself twice! ",this); }
  if( !m_elements.empty() )
    { throw MaterialException("Mixture::addMyself: could not add myself to existing elements! ",this); }
  if( !m_atoms.empty() )
    { throw MaterialException("Mixture::addMyself: could not add myself to existing atoms! ",this); }
  ///
  m_own = new Element( name             () ,
                       "XX"                , 
		       A                () , 
		       Z                () , 
		       density          () , 
                       radiationLength  () , 
                       absorptionLength () , 
                       temperature      () , 
                       pressure         () , 
                       state            () ) ;
  ///
  m_own->compute();
  m_own->setName( m_own->name() + "[ownElementForMixture]" );
  ///
  m_elements.push_back( Entry( 1 , m_own ) );
  m_atoms.push_back( 1 );
  ///
  return StatusCode::SUCCESS;
};
/////////////////////////////////////////////////////////////////////////////////////////////////
















