// $Id: Element.h,v 1.14 2006-05-17 16:02:38 cattanem Exp $
/// $Id: Element.h,v 1.14 2006-05-17 16:02:38 cattanem Exp $
#ifndef DETDESC_ELEMENT_H
#define DETDESC_ELEMENT_H 1 
/// STL
#include <vector>
/// DetDesc 
#include "DetDesc/Material.h"
#include "DetDesc/CLIDElement.h"

/// Forward declarations
class Isotope;

/** @class Element Element.h DetDesc/Element.h

    Element class defines the Element material component.
    An Element material component can defined by composition of Isotopes.
    The composition is done only by fraction of the mass.

    @author Radovan Chytracek
    @author Vanya Belyaev 
*/
class Element : virtual public Material
{
public:
  ///
  typedef  std::pair<double,SmartRef<Isotope> >  Entry    ;
  typedef  std::vector<Entry>                    Isotopes ;  
  ///
public:
  ///
  Element( const std::string&  name     = ""   , 
           const std::string&  symb     = "??" ,
           const double        a        =  0   , 
           const double        z        =  0   , 
           const double        density  =  0   ,
           const double        rl       =  0   , 
           const double        al       =  0   ,  
           const double        temp     =  Gaudi::Units::STP_Temperature,
           const double        press    =  Gaudi::Units::STP_Pressure,
           const eState        s        =  stateUndefined );
  // Destructor
  virtual ~Element();
  ///
  virtual inline const CLID& clID    () const { return Element::classID() ; };
  static         const CLID& classID ()       { return CLID_Element       ; }; 
  ///
  /// return symbol of element 
  inline const std::string &      symbol  () const { return m_symb ; }
  inline void                  setSymbol (const std::string& S ) { m_symb = S; } 
  /// Number of isotopes in the material
  const int                nOfIsotopes() const;
  /// Return an isotope by index
  const SmartRef<Isotope>& isotope        ( unsigned int i ) const;
        SmartRef<Isotope>& isotope        ( unsigned int i )      ;
  /// return vector of isotopes
  const Isotopes&          isotopes       () const ;
        Isotopes&          isotopes       ()       ;
  /// Return a fraction of an isotope by index
  const double             isotopeFraction( unsigned int i ) const;
  
  /** Add an isotope into this material
      After addition of the last isotope user can call this method
      with "comp" argument set to"true" and compute() method will
      do its job automatically, otherwise the user must call compute()
      method explicitly
  */
  void addIsotope ( const SmartRef<Isotope>& iPtr  , 
                    const double             Fract , const bool comp = false );
  void addIsotope ( const Entry&             iPtr  , const bool comp = false );
  
  /** Remove an isotope from the vector of isotopes
      compute() method invocation is done as described above
  */
  void removeIsotope ( const SmartRef<Isotope>& iPtr , const bool comp = false );

  ///        Atomic mass [g/mole]
  virtual inline const double    A() const                ;
  virtual inline void         setA( const double value ) ;
  ///        Atomic number
  virtual inline const double    Z() const;
  virtual inline void         setZ( const double value );
  ///        Number of nucleons
  virtual inline const double    N() const;
  ///  Coulomb factor 
  inline const double  coulombFactor() const ;
  ///  Tsai    factor 
  inline const double  tsaiFactor   () const ;
  ///

  /// Compute quantities after addition of all isotopes
  void    compute();
  /// Compute Coulomb factor
  void    ComputeCoulombFactor  ();
  /// Comput Tsai factor
  void    ComputeLradTsaiFactor ();

  /// compute interaction length
  void ComputeInteractionLength();

  /// compute radiation length
  void ComputeRadiationLength();

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const ;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const ;
  ///
private:  
  /// Effective atomic mass
  double                  m_Aeff;
  /// Effective atomic number
  double                  m_Zeff;
  /// Vector of isotopes this material is composed of
  Isotopes                m_isotopes;  
  /// Coulomb factor
  double                  m_coulomb;
  /// Tsai's factor
  double                  m_tsai;
  /// symbol
  std::string             m_symb;
};
///
#include "DetDesc/Element.icpp"
///

#endif // DETDESC_MATERIAL_ELEMENT_H










