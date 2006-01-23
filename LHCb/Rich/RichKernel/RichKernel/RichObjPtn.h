
//--------------------------------------------------------------------------------
/** @file RichObjPtn.h
 *
 *  Header file for RICH utility class : RichObjPtn
 *
 *  CVS Log :-
 *  $Id: RichObjPtn.h,v 1.3 2006-01-23 13:48:35 jonrob Exp $
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   13/11/2005
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHOBJPTN_H
#define RICHKERNEL_RICHOBJPTN_H 1

//--------------------------------------------------------------------------------
/** @class RichObjPtn RichObjPtn.h RichKernel/RichObjPtn.h
 *
 *  A utility class providing on demand object creation and some auto pointer
 *  behaviour
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   13/11/2005
 */
//--------------------------------------------------------------------------------

template < class TYPE >
class RichObjPtn
{

public:

  /// Default Constructor
  RichObjPtn()                      : m_obj(0)   {}

  /** Constructor from pointer to the underlying object
   *
   *  @param obj Pointer to object to use as underlying data object
   *
   *  @attention Using this method, RichObjPtn takes ownership of
   *  the object pointed at by obj. In particular, it is deleted
   *  when this RichObjPtn goes out of scope or is deleted itself.
   *  Consequently users should NOT manually delete objects they
   *  pass to a RichObjPtn
   */
  explicit RichObjPtn( TYPE * obj ) : m_obj(obj) {}

  /// Destructor
  ~RichObjPtn() { delete m_obj; }

private:

  /// Check if new object is needed
  inline TYPE* checkObj() const
  {
    if ( 0 == m_obj ) { m_obj = new TYPE(); }
    return m_obj;
  }

public:

  /// Check if an object is defined
  inline bool objectExists()      const { return 0 != m_obj; }

  /// Dereference operator to const object
  inline const TYPE* operator->() const { return checkObj(); }

  /// Dereference operator
  inline TYPE* operator->()             { return checkObj(); }

  /// Simple const access method
  inline const TYPE* object()     const { return checkObj(); }

  /// Simple access method
  inline TYPE* object()                 { return checkObj(); }
  
private:

  /// Pointer to the data object
  mutable TYPE * m_obj;

};

/// Implement ostream << method
template <class TYPE>
inline std::ostream& operator << ( std::ostream& s, 
                                   const RichObjPtn<TYPE>& ptn )
{
  s << *(ptn.object());
  return s;
}

#endif // RICHKERNEL_RICHOBJPTN_H
