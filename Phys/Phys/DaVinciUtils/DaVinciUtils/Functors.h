// $Id: $
#ifndef DAVINCIUTILS_FUNCTORS_H 
#define DAVINCIUTILS_FUNCTORS_H 1

// Include files

/** @class Functors Functors.h DaVinciUtils/Functors.h
 *  
 *  General helper functions for use in DaVinci.
 *  These should only know about Gaudi classes and patterns.
 *  No knowledge of DaVinci specific code should be assumed.
 * 
 *
 *  @author Juan Palacios
 *  @date   2011-01-10
 */

namespace DaVinci 
{

namespace Utils
{

  typedef std::unary_function<const ContainedObject*, bool> ContainedObjectPredicate;
  


  //template <class T> 
  struct InTES : public std::unary_function<const ContainedObject*, bool> 
  {

    inline bool operator() (const ContainedObject* obj) const    
    {
      return ( 0!=obj && 0!=obj->parent()) ;
    }
    
  };

  ///  Return true for all objects.
  template <class T>
  struct TRUE : public std::unary_function<const T*, bool> {

    inline bool operator() (const T*) {
      return true;
    }
    
  };

  ///  Return false for all objects.
  template <class T>
  struct FALSE : public std::unary_function<const T*, bool> {

    inline bool operator() (const T*) {
      return false;
    }
    
  };


} // namespace Utils
  
} // namespace DaVinci

#endif // DAVINCIUTILS_FUNCTORS_H
