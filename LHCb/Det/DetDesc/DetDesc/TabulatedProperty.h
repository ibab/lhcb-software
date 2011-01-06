// $Id: TabulatedProperty.h,v 1.6 2005-12-07 13:19:07 cattanem Exp $
#ifndef     DETDESC_TABULATEDPROPERTY_H
#define     DETDESC_TABULATEDPROPERTY_H 1 
/// STL
#include <vector> 
/// GaudiKernel
#include "DetDesc/ValidDataObject.h"
/// DetDesc
#include "DetDesc/CLIDTabulatedProperty.h"
///
class MsgStream    ;
///

/** @class TabulatedProperty TabulatedProperty.h DetDesc/TabulatedProperty.h
    
    Class to define some (non-trivial) Property of materials 
    and Surfaces. Primary intended to describe optical properties 
    of materials and surfaces  (requests from RICH group). But since it 
    is quite general it could be used for other similar stuff.
    
    @author  Vanya Belyaev
    @date    26/02/2001
    @author  Marco Clemencic
    @date    16/08/2005
    
*/

class TabulatedProperty: public ValidDataObject
{
  ///
public:
  ///
  typedef std::pair<double,double>   Entry;
  typedef std::vector<Entry>         Table;
  ///
public:
  /// constructor
  TabulatedProperty( const std::string& Name = "");
  virtual ~TabulatedProperty();
  /// 
  virtual const CLID& clID    () const { return TabulatedProperty::classID() ; }
  static  const CLID& classID ()       { return CLID_TabulatedProperty       ; }

  /// Fill the output stream (ASCII)
  virtual std::ostream& fillStream ( std::ostream& s ) const ;
  /// Fill the output stream (ASCII)
  virtual MsgStream&    fillStream ( MsgStream&    s ) const ;
  ///
  /// acessors to property type
  inline const std::string&     type () const ;
  inline TabulatedProperty&  setType ( const std::string& T );   
  /// acessors to  name of x-axis 
  inline const std::string&     xAxis() const ;
  inline TabulatedProperty&  setXAxis( const std::string& T );   
  /// acessors to  name of y-axis 
  inline const std::string&     yAxis() const ;
  inline TabulatedProperty&  setYAxis( const std::string& T );   
  /// acessors to the table itself 
  inline       Table&        table   ()       ;
  inline const Table&        table   () const ;
  /// another form of accessors to table 
  inline operator       Table&       ()       ;
  inline operator const Table&       () const ;
  ///  
  /** Fill a table from the function 
      @param func  The function. It could the the type of function, pointer to function, 
      STL adaptor for member function, STL functional or any type of functor
      or function object   
      @param first "Iterator"(in STL sence) to a  first   element in a "sequence" 
      @param last  "Iterator"(in STL sence) to a (last+1) element in a "sequence" 
  */
  template< class Func , class Iter> 
  inline const StatusCode fill( Func func , Iter first , Iter last );
  ///
  /// Prepare an XML string representing the object.
  virtual std::string toXml(const std::string &name = std::string()) const;

  /// Perform a deep copy (needed for usage in CondDB). A GaudiException is
  /// thrown if the object is not a TabulatedProperty.
  using ValidDataObject::update;
  virtual void update ( ValidDataObject& obj );
  
private:
  ///
  std::string m_type  ;
  ///
  std::string m_xAxis ; 
  ///
  std::string m_yAxis ; 
  ///
  Table       m_table ;
  ///
};
///
#include "DetDesc/TabulatedProperty.icpp"
///

#endif  //  DETDESC_TABULATEDPROPERTY_H












