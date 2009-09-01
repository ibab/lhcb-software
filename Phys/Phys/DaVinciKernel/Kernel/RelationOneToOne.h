// $Id: RelationOneToOne.h,v 1.1 2009-09-01 16:10:14 jpalac Exp $
#ifndef KERNEL_RELATIONONETOONE_H 
#define KERNEL_RELATIONONETOONE_H 1

// Include files
#include "GaudiKernel/StatusCode.h"

namespace DaVinci 
{
/** @class RelationOneToOne Kernel/RelationOneToOne.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2009-09-01
 */
template <class T>
class RelationOneToOne {

public:
  typedef T::From_ From_;
  typedef T::To_   To_;
  typedef T::Range Range;
  

public: 
  /// Standard constructor
  RelationOneToOne(T& table)
    :
    m_table(table)
  {
  }
  
public:

  inline StatusCode i_relate(From_ from, To_ to) const
  {
    return m_table.relations(from).empty() ? m_table.i_relate(from, to) : StatusCode::FAILURE;
  }

  inline StatusCode i_removeFrom(From_ from) 
  {
    return m_table.i_reloveFrom(from);
  }
  

  inline StatusCode i_relateWithOverwrite(From_ from, To_ to) const
  {
    if ( m_table.relations(from).empty() ) return m_table.i_relate(from, to);

    if( m_table.i_removeFrom(from).isFailure() ) return StatusCode::FAILURE;

    return m_table.i_relate(from, to);
    
  }

  inline StatusCode i_clear() 
  {
    return m_table.i_clear();
  }
  
  inline StatusCode i_relations(From_ from) 
  {
    return m_table.i_relations(from);
  }

  inline StatusCode i_relations() 
  {
    return m_table.i_relations();
  }

  virtual ~RelationOneToOne( ); ///< Destructor

public:

  inline const T& table() const { return m_table; }
  

protected:

private:
  /// Standard constructor
  RelationOneToOne( ); 

private:
  T& m_table;
  
};
 
} // DaVinci namespace
#endif // KERNEL_RELATIONONETOONE_H
