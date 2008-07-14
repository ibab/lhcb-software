// $Id: Mixture.cpp,v 1.13 2008-07-14 08:35:31 smenzeme Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// DetDesc 
#include "DetDesc/MaterialException.h"
#include "DetDesc/Mixture.h"
#include <cmath>

//
Mixture::Mixture( const std::string&  name    , 
                  const double        a       , 
                  const double        z       , 
		  const double        i       ,
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
    , m_I       ( i )
   {
   };
//
Mixture::~Mixture() {}
//
const SmartRef<Element>& Mixture::element( const unsigned int i ) const 
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].second ; 
};
//
      SmartRef<Element>& Mixture::element( const unsigned int i )        
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].second ; 
};
//
const double Mixture::elementFraction( const unsigned int i ) const        
{
  if( i >= m_elements.size() ) 
    { throw MaterialException("Mixture::element:: wrong index! ", this ); } 
  return m_elements[i].first ; 
};
//
void Mixture::addElement( const SmartRef<Element>& e, 
                          const int nOfAtoms , 
                          const bool comp )  
{
  ///
  if( m_own )
    { throw MaterialException( "Mixture::could not add element!",this);}
  if( !e    ) 
    { throw MaterialException("Mixture::could not add element",this) ; }
  if( m_elements.size() != m_atoms.size() )
    { throw MaterialException("Mixture::mismatch container sizes! ",this);}
  ///
  m_elements.push_back( Entry( 0 , e ) );
  m_atoms.push_back   ( nOfAtoms       ) ; 
  ///
  if( true == comp )  { computeByAtoms(); }
  ///
};
///
void Mixture::addElement( const SmartRef<Element>& e, 
                          const double fraction     , 
                          const bool comp )  
{ 
  if( m_own            )
    { throw MaterialException("Mixture::could not add element!",this);}
  if( !e               ) 
    { throw MaterialException("Mixture::non valid pointer!"+name());}
  if( !m_atoms.empty() )
    { throw MaterialException("Mixture:could not add element!",this); }
  ///
  Elements::iterator it  = m_elements.begin() ; 
  while( m_elements.end() != it ) 
    { if( e->name() == it->second->name() ) 
      { it->first += fraction ; break ; } ; ++it ; } 
  ///
  if( m_elements.end() == it )
    { m_elements.push_back( Entry( fraction , e ) ); }  
  ///  
  if( true == comp ) { compute() ; }
};
///
void Mixture::addMixture( const SmartRef<Mixture>& mx, 
                          const double fraction, const bool comp)  
{
  if( !mx ) { throw MaterialException("Mixture::non valid pointer!");}
  ///
  for( Elements::const_iterator it = mx->elements().begin() ; 
       mx->elements().end() != it ; ++it )
    { 
      addElement( it->second , fraction*(it->first) ); 
    }   
  ///  
  if( true == comp ) { compute() ; }
  ///
};
///
StatusCode Mixture::compute()
{
  if      ( m_atoms.empty()                      ) 
    { return computeByFraction() ; }
  else if ( m_atoms.size () == m_elements.size() ) 
    { return computeByAtoms   () ; } 
  else 
    { throw MaterialException("Mixture::compute: could not compute "+name() );}
  return StatusCode::FAILURE ; 
};
///
StatusCode Mixture::computeByAtoms()  
{
  ///
  if( m_elements.empty() ) { addMyself() ;}
  ///
  if( m_elements.size() != m_atoms.size() )
    { throw MaterialException(std::string("Mixture::computeByAtoms::") 
                              + "mismatch in container sizes!",this);}
  ///
  /// Compute molecular weight
  double sum     = 0.0;
  for( unsigned int i1 = 0 ; i1 < m_elements.size() ; ++i1 )
    sum += m_atoms[i1]; 
  ///
  /// Compute proprotion by weight for each element
  for( unsigned int i2 = 0; i2 < m_elements.size() ;  ++i2  )      
    m_elements[i2].first = m_atoms[i2]/ sum; 
  ///
  m_atoms.clear();
  ///
  return computeByFraction();
  ///
}
///
StatusCode Mixture::computeByFraction()  
{
  ///
  if( !m_atoms.empty() )
    { throw MaterialException(std::string("Mixture::computeByFraciton!") 
                              + "Atoms nust be empty!",this);}
  //
  if( m_elements.empty() ) { addMyself() ; m_atoms.clear() ; }
  ///
  /// recompute fractions
  double frsum = 0.0  ;
  for( Elements::iterator it1 = m_elements.begin() ; 
       m_elements.end() != it1 ; ++it1 )
    { frsum += it1->first ; } 
  if( frsum < 0 ) 
    { throw MaterialException(std::string("Mixture::computeByFractions::") 
                              + "not positive fraction sum!",this);}
  /// rescale  fractions
  for( Elements::iterator it2 = m_elements.begin() ; 
       m_elements.end() != it2 ; ++it2 )
    {  it2->first /= frsum ; } 
  ///
  m_A = 0 ;
  m_Z = 0 ;
  m_I = 0;
  ///
  double radleninv = 0 ; 
  double lambdainv = 0 ;
  ///
  for( Elements::iterator it3 = m_elements.begin() ; 
       m_elements.end() != it3 ; ++it3 ) { 
    double   frac = it3->first  ;
    Element* elem = it3->second ;
    /// 
    m_A += frac * elem->A() ;
    m_Z += frac * elem->Z() ;

   

    m_I += frac*elem->Z()*log(elem->I()); 
    // 
    // Use the aproximate formula for radiation lengh of mixtures 1/x0 = sum(wi/Xi)
    if( elem->radiationLength() > 0.0 && elem->density() > 0.0) {
      radleninv += frac/(elem->radiationLength() * elem->density());
    } else {
      radleninv += frac * Gaudi::Units::Avogadro * elem->tsaiFactor() / elem->A();
    }
    //
    if( elem->absorptionLength() > 0.0 ) { 
      lambdainv += frac/(elem->absorptionLength() * elem->density()); 
    }
  }

  m_I = std::exp(m_I/m_Z);

  double A1 = 1*Gaudi::Units::g/Gaudi::Units::mole;
  double D1 = 1*Gaudi::Units::g/Gaudi::Units::cm3;

  double plasma = 28.816*sqrt(density()/D1*Z()*A1/A())*Gaudi::Units::eV;

  m_C = 1 + 2*log(m_I/plasma);
  double x_a = m_C/4.606;

  if ( m_I < 100 * Gaudi::Units::eV )
      if ( m_C > 3.681){
	  m_X0 = 0.326*m_C-1.0;
	  m_X1 = 2;
      } else {
	  m_X0 = 0.2;
	  m_X1 = 2;
      }
  else 
      if ( m_C > 5.215){
	  m_X0 = 0.326*m_C-1.5;
	  m_X1 = 3;
      } else {
	  m_X0 = 0.2;
	  m_X1 = 3;
      }

  m_a = 4.606*(x_a-m_X0)/pow(m_X1-m_X0,3.0);
  m_m = 3.0;

  // only update the radiation and interaction length if the attribute is
  // is not provided in the input file

  if( radiationLength() == 0.0 ) {
    if( radleninv > 0 ) { setRadiationLength( 1.0 / radleninv / density() ); }
  }
  if( absorptionLength() == 0.0 ) {
    if( lambdainv > 0 ) { setAbsorptionLength( 1.0 / lambdainv /density() ); } 
  }
  //
  return 
    A()       <= 0 ? StatusCode::FAILURE :
    Z()       <= 0 ? StatusCode::FAILURE :
    I()       <= 0 ? StatusCode::FAILURE :
    radleninv <= 0 ? StatusCode::FAILURE :
    density() <= 0 ? StatusCode::FAILURE : 
    lambdainv <= 0 ? StatusCode::FAILURE : StatusCode::SUCCESS ;
  ///
};
///
MsgStream&        Mixture::fillStream ( MsgStream&   s ) const 
{
  Material::fillStream( s ) ;
  if( !m_elements.empty() )
    {
      s << "\t #MassComponents=" << std::setw(2)  << m_elements.size() 
        << endreq ;
      for( Elements::const_iterator it  = m_elements.begin() ; 
           m_elements.end() != it ; ++it )
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
      for( Atoms::const_iterator it  = m_atoms.begin() ; 
           m_atoms.end() != it ; ++it )
        { 
          s << "\t\tcomponent#"  << std::setw(2) << it - m_atoms.begin() 
            << " atom="          << std::setw(2) << *it  
            << endreq ; 
        }
    }
  return s;
};    
///
std::ostream&     Mixture::fillStream ( std::ostream& s ) const 
{
  Material::fillStream( s ) ;
  if( !m_elements.empty() ) 
    {
      s << "\t #MassComponents=" << std::setw(2)  << m_elements.size()
        << std::endl ;
      for( Elements::const_iterator it  = m_elements.begin() ; 
           m_elements.end() != it ; ++it )
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
      for( Atoms::const_iterator it  = m_atoms.begin() ; 
           m_atoms.end() != it ; ++it )
        { 
          s << " \t\tcomponent#"    << std::setw(2) << it - m_atoms.begin() 
            << " atom="             << std::setw(2) << *it  
            << std::endl ; 
        }
    }
  return s;
};    
///
StatusCode Mixture::addMyself() 
{
  if( 0 != m_own         ) 
    { throw MaterialException(std::string("Mixture::addMyself: ") 
                              + "could not add myself twice! ",this); }
  if( !m_elements.empty() )
    { throw MaterialException(std::string("Mixture::addMyself: ") 
                              + "could not add myself to " 
                              + "existing elements! ",this); }
  if( !m_atoms.empty() )
    { throw MaterialException(std::string("Mixture::addMyself: could ") 
                              +  "not add myself to existing atoms! ",this); }
  ///
  m_own = new Element( name             () ,
                       "XX"                , 
                       A                () , 
                       Z                () ,
		       I                () ,
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



