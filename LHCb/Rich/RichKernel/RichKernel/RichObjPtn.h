
//--------------------------------------------------------------------------------
/** @file RichObjPtn.h
 *
 *  Header file for RICH utility class : Rich::ObjPtn
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   13/11/2005
 */
//--------------------------------------------------------------------------------

#ifndef RICHKERNEL_RICHOBJPTN_H
#define RICHKERNEL_RICHOBJPTN_H 1

namespace Rich
{

  //--------------------------------------------------------------------------------
  /** @class ObjPtn RichObjPtn.h RichKernel/RichObjPtn.h
   *
   *  A utility class providing on demand object creation and some auto pointer
   *  like behaviour
   *
   *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
   *  @date   13/11/2005
   */
  //--------------------------------------------------------------------------------

  template < class TYPE >
  class ObjPtn
  {

  public:

    /// Default Constructor
    ObjPtn() = default;

    /** Constructor from pointer to the underlying object
     *
     *  @param obj Pointer to object to use as underlying data object
     *
     *  @attention Using this method, RichObjPtn takes ownership of
     *  the object pointed at by obj. In particular, it is deleted
     *  when this RichObjPtn goes out of scope or is deleted itself.
     *  Consequently users should NOT manually delete objects they
     *  pass to a RichObjPtn.
     */
    explicit ObjPtn( TYPE * obj ) : m_obj( obj ) { }

    /** Copy Constructor from another object of type TYPE
     *
     *  @param obj Reference to object to use as underlying data object
     *
     *  This method requires the underlying object to have a valid copy constructor
     *  with syntax TYPE( TYPE& obj )
     */
    explicit ObjPtn( const TYPE & obj ) : m_obj( new TYPE(obj) ) { }

    /** Move Constructor from another object of type TYPE
     *
     *  @param obj Reference to object to use as underlying data object
     *
     *  This method requires the underlying object to have a valid move constructor
     *  with syntax TYPE( TYPE& obj )
     */
    explicit ObjPtn( TYPE && obj ) : m_obj( new TYPE(std::move(obj)) ) { }

    /// Destructor
    ~ObjPtn() { delete m_obj; }

  private:

    /// Check if new object is needed
    inline TYPE* checkObj() const noexcept
    {
      return ( m_obj ? m_obj : ( m_obj = new TYPE() ) );
    }

  public:

    /// Check if an object is defined
    inline bool objectExists()      const noexcept { return nullptr != m_obj; }

    /// Dereference operator to const object
    inline const TYPE* operator->() const noexcept { return checkObj(); }

    /// Dereference operator
    inline TYPE* operator->()             noexcept { return checkObj(); }

    /// Simple const access method
    inline const TYPE* object()     const noexcept { return checkObj(); }

    /// Simple access method
    inline TYPE* object()                 noexcept { return checkObj(); }

    /// Inherit a new object
    inline void inherit( TYPE * obj )     noexcept { delete m_obj; m_obj = obj; }

    /// Overload output to ostream
    friend inline std::ostream& operator << ( std::ostream& os,
                                              const Rich::ObjPtn<TYPE>& ptn )
    {
      return os << *(ptn.object());
    }

  private:

    /// Pointer to the data object
    mutable TYPE * m_obj = nullptr;

  };

}

#endif // RICHKERNEL_RICHOBJPTN_H
