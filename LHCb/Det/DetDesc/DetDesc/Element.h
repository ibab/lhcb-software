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
class Element : public Material
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
	   const double        i        =  0   ,
           const double        density  =  0   ,
           const double        rl       =  0   ,
           const double        al       =  0   ,
           const double        temp     =  Gaudi::Units::STP_Temperature,
           const double        press    =  Gaudi::Units::STP_Pressure,
           const eState        s        =  stateUndefined );
  // Destructor
  ~Element() override = default;
  ///
  const CLID& clID() const override { return Element::classID() ; };
  static const CLID& classID ()     { return CLID_Element       ; };
  ///
  /// return symbol of element
  inline const std::string &      symbol  () const { return m_symb ; }
  inline void                  setSymbol (const std::string& S ) { m_symb = S; }
  /// Number of isotopes in the material
  int                      nOfIsotopes() const;
  /// Return an isotope by index
  const SmartRef<Isotope>& isotope        ( unsigned int i ) const;
        SmartRef<Isotope>& isotope        ( unsigned int i )      ;
  /// return vector of isotopes
  const Isotopes&          isotopes       () const ;
        Isotopes&          isotopes       ()       ;
  /// Return a fraction of an isotope by index
  double                   isotopeFraction( unsigned int i ) const;

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
  double    A() const override;
  void   setA( const double value ) override;
  ///        Atomic number
  double    Z() const override;
  void   setZ( const double value ) override;
  ///        Mean excitation energy
  double    I() const override;
  void   setI( const double value ) override;
  ///        Parameters for density effect correction
  double    C() const override;
  void   setC( const double value) override;
  double    a() const override;
  void   seta( const double value) override;
  double    m() const override;
  void   setm( const double value) override;
  double    X0() const override;
  void   setX0( const double value) override;
  double    X1() const override;
  void   setX1( const double value) override;
  ///        Number of nucleons
  double    N() const override;
  ///  Coulomb factor
  inline double  coulombFactor() const ;
  ///  Tsai    factor
  inline double  tsaiFactor   () const ;
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

  /// compute mean excitation energy
  void ComputeMeanExcitationEnergy();

  /// compute parameters for density correction
  void ComputeDensityEffect();


  /// Fill the output stream (ASCII)
  std::ostream& fillStream ( std::ostream& s ) const override;
  /// Fill the output stream (ASCII)
  MsgStream&    fillStream ( MsgStream&    s ) const override;
  ///
private:
  /// Effective atomic mass
  double                  m_Aeff;
  /// Effective atomic number
  double                  m_Zeff;
  // Effective mean escitation energy
  double                  m_Ieff;
  // Parameters for density effect correction
  double                  m_C;
  double                  m_a;
  double                  m_m;
  double                  m_X0;
  double                  m_X1;
  /// Vector of isotopes this material is composed of
  Isotopes                m_isotopes;
  /// Coulomb factor
  double                  m_coulomb;
  /// Tsai's factor
  double                  m_tsai;
  /// symbol
  std::string             m_symb;
};

#include "DetDesc/Isotope.h"
#include "GaudiKernel/SystemOfUnits.h"

/////////////////////////////////////////////////////////////////////////////////
inline int    Element::nOfIsotopes() const { return isotopes().size(); }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::coulombFactor() const { return m_coulomb; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::tsaiFactor   () const { return m_tsai; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::A() const { return m_Aeff; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setA( const double value ) { m_Aeff = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::Z() const { return m_Zeff; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setZ( const double value ) { m_Zeff = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::I() const { return m_Ieff; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setI( const double value ) { m_Ieff = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::C() const { return m_C; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setC( const double value ) { m_C = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::a() const { return m_a; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::seta( const double value ) { m_a = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::m() const { return m_m; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setm( const double value ) { m_m = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::X0() const { return m_X0; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setX0( const double value ) { m_X0 = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::X1() const { return m_X1; }
/////////////////////////////////////////////////////////////////////////////////
inline void   Element::setX1( const double value ) { m_X1 = value; }
/////////////////////////////////////////////////////////////////////////////////
inline double Element::N() const { return m_Aeff*Gaudi::Units::mole/Gaudi::Units::g; }
/////////////////////////////////////////////////////////////////////////////////
inline const Element::Isotopes& Element::isotopes () const { return m_isotopes; }
/////////////////////////////////////////////////////////////////////////////////
inline       Element::Isotopes& Element::isotopes ()       { return m_isotopes; }
/////////////////////////////////////////////////////////////////////////////////

#endif // DETDESC_MATERIAL_ELEMENT_H
