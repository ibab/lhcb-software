// ============================================================================
#ifndef DETDESC_MIXTURE_H
#define DETDESC_MIXTURE_H
// detDesc
#include "DetDesc/Material.h"
#include "DetDesc/Element.h"
#include "DetDesc/CLIDMixture.h"
/// Include files
#include <vector>


/** @class Mixture Mixture.h DetDesc/Mixture.h
 *
 *  The Mixture class can be composed of Elements and other Mixtures.
 *  Mixture of elements can be compoased either by fraction of the mass
 *  or by number of atoms of the each component.
 *  Mixture of mixtures and elements is composed only by fraction of the
 *  mass.
 *
 *  @author Radovan Chytracek
 */
class Mixture : public  Material
{

public:

  typedef std::pair<double,SmartRef<Element> >  Entry    ;
  typedef std::vector<Entry>                    Elements ;
  typedef std::vector<int>                      Atoms    ;

  /// Constructors for simple materials
  Mixture( const std::string&  name    = ""               ,
           const double        a       = 0                ,
           const double        z       = 0                ,
           const double        i       = 0                ,
           const double        density = 0                ,
           const double        rl      = 0                ,
           const double        al      = 0                ,
           const double        temp    = Gaudi::Units::STP_Temperature,
           const double        press   = Gaudi::Units::STP_Pressure,
           const eState        s       = stateUndefined   );

  /// Destructor
  ~Mixture() override;

  /// Number of components in the material mixture
  inline int nOfItems() const;

  /** Add an element into the mixture by specifying
   *  the number of the atoms  ( material compuund )
   */
  void addElement( const SmartRef<Element>& e  ,
                   const int    nOfAtoms ,
                   const bool comp = false );

  /** Add an element into the mixture by specifying
   * the fraction of the mass ( material mixture )
   */
  void addElement( const SmartRef<Element>& e  ,
                   const double fraction ,
                   const bool comp = false );

  /** Add another mixture into this mixture by specifying its fraction
      of the mass
  */
  void addMixture( const SmartRef<Mixture>& mx ,
                   const double fraction ,
                   const bool comp = false);

  /// Return i-th element's pointer
  const SmartRef<Element>&  element ( const unsigned int i ) const ;
  SmartRef<Element>&  element ( const unsigned int i )       ;

  /// return all elements
  inline const Elements&           elements() const ;
  inline       Elements&           elements()       ;

  /// compute all quantities
  StatusCode compute();

  /// Return fraction of mass of the i-th element
  double   elementFraction( const unsigned int i ) const ;

  // Obligatory implementation of Material interface

  //       Atomic mass [g/mole]
  virtual inline double    A() const override;
  virtual inline void   setA( const double value ) override;
  //       Atomic number
  virtual inline double    Z() const override;
  virtual inline void   setZ( const double value ) override;
  //       Mean excitation energy
  virtual inline double    I() const override;
  virtual inline void   setI( const double value ) override;
  //       Parameters for Density Effect Correction
  virtual inline double    C() const override;
  virtual inline void   setC( const double value ) override;
  virtual inline double    a() const override;
  virtual inline void   seta( const double value ) override;
  virtual inline double    m() const override;
  virtual inline void   setm( const double value ) override;
  virtual inline double    X0() const override;
  virtual inline void   setX0( const double value ) override;
  virtual inline double    X1() const override;
  virtual inline void   setX1( const double value ) override;



  ///        Number of nucleons
  virtual inline double    N() const override;
  
  inline const CLID& clID    () const override { return Mixture::classID(); }
  static         const CLID& classID ()       { return CLID_Mixture      ; }

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const  override;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const  override;

public:

  /// Reset the properties back to defaults
  virtual void reset() override;

protected:

  /// Add Myself ...
  StatusCode addMyself();
  /// Compute effective A,Z,I and fractions for compounds
  StatusCode computeByAtoms();
  /// Compute effective A,Z,I and number of atoms per volume for each element
  StatusCode computeByFraction () ;

private:

  /// Vector of elements
  Elements       m_elements ;

  /// Corresponding number of atoms for each element
  Atoms          m_atoms    ;

  /// own element (if needed)
  std::unique_ptr<Element>       m_own      ;

  
  double         m_A ;
  double         m_Z ;
  double         m_I ;
  double         m_a ;
  double         m_m ;
  double         m_C ;
  double         m_X0;
  double         m_X1;
  
};

#include "GaudiKernel/SystemOfUnits.h"

/////////////////////////////////////////////////////////////////////////////////
inline int    Mixture::nOfItems() const { return m_elements.size(); }
/// Effective A is returned /////////////////////////////////////////////////////
inline double Mixture::A()        const { return m_A    ; }
/// Effective A is computed, can't be set explicitly ////////////////////////////
inline void   Mixture::setA( const double /*value*/ ){}
/// Effective Z is returned /////////////////////////////////////////////////////
inline double Mixture::Z()        const { return m_Z    ; }
/// Effective Z is computed, can't be set explicitly ////////////////////////////
inline void   Mixture::setZ( const double /*value*/ ){} 
/// Effective I is returned                          ////////////////////////////
inline double Mixture::I()        const { return m_I    ; }  
// Effective I is computed, can't be set explicitly ////////////////////////////
inline void   Mixture::setI( const double /*value*/ ){} 

/// Parameters for density effect correction are set and returned ////////////////
inline double Mixture::C()        const { return m_C    ; }
inline void   Mixture::setC( const double /*value*/ ){}
inline double Mixture::a()        const { return m_a    ; }
inline void   Mixture::seta( const double /*value*/ ){}
inline double Mixture::m()        const { return m_m    ; }
inline void   Mixture::setm( const double /*value*/ ){}
inline double Mixture::X0()        const { return m_X0    ; }
inline void   Mixture::setX0( const double /*value*/ ){}
inline double Mixture::X1()        const { return m_X1    ; }
inline void   Mixture::setX1( const double /*value*/ ){}


/// Effective N is returned
inline double Mixture::N()        const { return m_A*Gaudi::Units::mole/Gaudi::Units::g; }  
/////////////////////////////////////////////////////////////////////////////////
inline const Mixture::Elements& Mixture::elements() const { return m_elements ; }
inline       Mixture::Elements& Mixture::elements()       { return m_elements ; }
/////////////////////////////////////////////////////////////////////////////////

#endif // DETDESC_MIXTURE_H
