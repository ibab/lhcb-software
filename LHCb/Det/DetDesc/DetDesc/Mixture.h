// $Id: Mixture.h,v 1.13 2007-10-25 07:31:27 cattanem Exp $ 
// ============================================================================ 
// CVS tag $Name: not supported by cvs2svn $
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
  ///  
public:
  ///
  typedef std::pair<double,SmartRef<Element> >  Entry    ;
  typedef std::vector<Entry>                    Elements ;
  typedef std::vector<int>                      Atoms    ;
  ///
  /// Constructors for simple materials
  Mixture( const std::string&  name    = ""               , 
           const double        a       = 0                , 
           const double        z       = 0                , 
           const double        density = 0                ,
           const double        rl      = 0                , 
           const double        al      = 0                ,
           const double        temp    = Gaudi::Units::STP_Temperature,
           const double        press   = Gaudi::Units::STP_Pressure,
           const eState        s       = stateUndefined   );
  
  /// Destructor
  virtual ~Mixture();
  
  /// Number of components in the material mixture
  inline const int nOfItems() const;
  
  /** Add an element into the mixture by specifying:
   *   a) the fraction of the mass ( material mixture )
   *  b) the number of the atoms  ( material compund )
   */
  void addElement( const SmartRef<Element>& e  , 
                   const int    nOfAtoms , 
                   const bool comp = false );
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
  const double   elementFraction( const unsigned int i ) const ;
  
  /// Obligatory implementation of Material interface
  
  ///        Atomic mass [g/mole]
  virtual inline const double    A() const;
  virtual inline void         setA( const double value );
  ///        Atomic number
  virtual inline const double    Z() const;
  virtual inline void         setZ( const double value );  
  ///        Number of nucleons
  virtual inline const double    N() const;
  ///
  virtual inline const CLID& clID    () const { return Mixture::classID(); }
  static         const CLID& classID ()       { return CLID_Mixture      ; } 

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const ;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const ;
  ///
protected:
  ///
  StatusCode addMyself();
  /// Compute effective A,Z and fractions for compounds
  StatusCode computeByAtoms();  
  /// Compute effective A,Z and number of atoms per volume for each element
  StatusCode computeByFraction () ;
  ///  
private:
  
  /// Vector of elements
  Elements       m_elements ;
  
  /// Corresponding number of atoms for each element
  Atoms          m_atoms    ;

  /// own element (if needed)
  Element*       m_own      ;  

  ///
  double         m_A ;
  double         m_Z ;
  ///
};

///
#include "DetDesc/Mixture.icpp"
///
   
#endif // DETDESC_MIXTURE_H
