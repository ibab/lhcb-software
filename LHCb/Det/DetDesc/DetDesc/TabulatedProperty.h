// $Id: TabulatedProperty.h,v 1.2 2003-04-25 08:52:24 sponce Exp $
#ifndef     DETDESC_TABULATEDPROPERTY_H
#define     DETDESC_TABULATEDPROPERTY_H 1 
/// STL
#include <vector> 
/// GaudiKernel
#include "GaudiKernel/DataObject.h"
/// DetDesc
#include "DetDesc/CLIDTabulatedProperty.h"
///
class MsgStream    ;
class StreamBuffer ;
///

/** @class TabulatedProperty TabulatedProperty.h DetDesc/TabulatedProperty.h
    
    Class to define some (non-trivial) Property of materials 
    and Surfaces. Primary intended to describe optical properties 
    of materials and surfaces  (requests from RICH group). But since it 
    is quite general it could be used for other similar stuff.
    
    @author  Vanya Belyaev
    @date    26/02/2001
*/

class TabulatedProperty: public DataObject
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
  ///
  /// Serialize the object for writing
  virtual StreamBuffer& serialize  ( StreamBuffer& s ) const ;
  /// Serialize the object for reading
  virtual StreamBuffer& serialize  ( StreamBuffer& s )       ;
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












