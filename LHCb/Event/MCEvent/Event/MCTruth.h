//	====================================================================
//	MCTruth.h
//	--------------------------------------------------------------------
//
//	Package   : Event/Event
//
//	Author    : Markus Frank
//
//	====================================================================
// $Id $
#ifndef EVENT_MCTRUTH_H
#define EVENT_MCTRUTH_H 1

#include "GaudiKernel/StatusCode.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/SmartRef.h"

class DataObject;
class ContainedObject;

/** Monte Carlo namespace.
*/
StatusCode makeMCTruthLink(const DataObject* from, const DataObject* to);
void* accessMCTruth( void* (*cast)(DataObject*), const DataObject* from);
void* accessMCTruth( void* (*cast)(ContainedObject*), const ContainedObject* from);

/** Monte Carlo truth accessor class: MCTruth MCTruth.h
 *
 *  Description:
 *  A small class to access the Monte carlo truth information
 *  according to the LHCb event model.
 *
 *
 *  Examples:
 *
 *  1) Setting a Monte-Carlo truth link:
 *    1.a) Add MC truth link (for entire container): mcDigits -> digits
 *          KeyedContainer<MCDigit>* mcdigits = ...;
 *          KeyedContainer<Digit>*     digits = ...;
 *          StatusCode sc = MCTruth(digits).set(mcdigits);
 *          if ( !sc.isSuccess() )    { ... handle error ... }
 *    1.b) Add MC truth individually (less performat; does same thing n-times)
 *          for ( KeyedContainer<Digit>::iterator i= .... )  {
 *            MCDigit* mcdigit = ...create it or retrieve it...
 *            MCTruth(*i).set(mcdigit);
 *          }
 *
 *  2) Follow the Monte-Carlo truth of an object:
 *    2.a) Follow individual object:
 *          for ( KeyedContainer<Digit>::iterator i= .... )  {
 *            MCDigit* mcdigit = MCTruth<MCDigit>(*i);
 *            ... work with MC truth information ...
 *          }
 *    2.b) Follow globally (more efficient):
 *          KeyedContainer<Digit>*     digits = ...;
 *          KeyedContainer<MCDigit>* c = 
 *            MCTruth<KeyedContainer<MCDigit>>::get(digits)
 *          for ( KeyedContainer<Digit>::iterator i= .... )  {
 *            MCDigit* mcdigit = c->object((*i)->key());
 *            ... work wiwth MC truth information ...
 *          }
 *
 *  3) Using the function templates
 *
 *    3.a) Set MC truth (Same usage on container level)
 *    MCDigit* mcdigit = ...;
 *    Digit*     digit = ...;
 *    if ( setMCTruth(digit, mcdigit).isSuccess() )  {
 *      // all ok.
 *    }
 *
 *    3.b) Retrieve MC truth
 *    for ( KeyedContainer<Digit>::iterator i= .... )  {
 *      MCDigit* mcdigit = mcTruth<MCDigit>(*i)
 *        ... work wiwth MC truth information ...
 *    }      
 *
 *  @author  M.Frank
 *  @version 1.0
 */
template <class TYPE>
class MCTruth   {
private:
  /// Reference to originating DataObject pointer
  const ContainedObject*  m_cnt;
  const DataObject*       m_dobj;
private:
  void setup(const ContainedObject* p)  {
    m_dobj = object(p);
    m_cnt = p;
  }
  void setup(const DataObject* p)  {
    m_dobj = p;
    m_cnt = 0;
  }
  /// Determine object type
  const DataObject* object(const ContainedObject* p)  const
  { return (p==0) ? 0 : p->parent();                                       }
  const DataObject* object(const DataObject* p) const { return p;          }
  /// Formalized cast of type DataObject
  static void* cast(DataObject* p)      { return dynamic_cast<TYPE*>(p);   }
  /// Formalized cast of type ContainedObject
  static void* cast(ContainedObject* p) { return dynamic_cast<TYPE*>(p);   }

public:
  /// Standard templated constructor taking any object pointer as source address
  template<class T> MCTruth(const T* t) { setup(t);                        }
  /// Standard templated constructor taking any object pointer as source address
  template<class T> MCTruth(const SmartRef<T>& t) { setup(t.target());     }
  /// Standard templated constructor taking any object pointer as source address
  template<class T> MCTruth(SmartDataPtr<T>& t) {   setup(t.ptr());        }
  /// Automatic conversion to the target MC truth address
  operator TYPE*()  const  {  return (0==m_cnt) ? get(m_dobj) : get(m_cnt);}
  /// Set Monte Carlo truth link to specified object
  template<class T> StatusCode 
  set(const T* t)              { return makeMCTruthLink(m_dobj,object(t)); } 
  /// Retrieve Monte Carlo truth information from passed object
  template<class T> static 
  TYPE* get(const T* t)        { return (TYPE*)accessMCTruth(cast,t);      }

  /// Adapter: Set Monte Carlo truth link to specified object
  template<class T> 
  StatusCode set(SmartDataPtr<T>& t)   { return set(t.ptr());              }
  /// Adapter: Set Monte Carlo truth link to specified object
  template<class T> 
  StatusCode set(const SmartRef<T>& t) { return set(t.target());           }
  /// Adapter: Retrieve Monte Carlo truth information from passed object
  template<class T> static 
  TYPE* get(SmartDataPtr<T>& t)        { return get(t.ptr());              }
  /// Adapter: Retrieve Monte Carlo truth information from passed object
  template<class T> static
  TYPE* get(const SmartRef<T>& t)      { return get(t.target());           }
};

/// Function template to set MC truth information using raw pointers
template <class F, class T> inline
StatusCode setMCTruth(const F* f, const T* t)
{ return MCTruth<F>(f).set(t);                                      }

/// Function template to set MC truth information using SmartDataPtr
template <class F, class T> inline
StatusCode 
setMCTruth(SmartDataPtr<F>& f, SmartDataPtr<T>& t) 
{ return setMCTruth(f.ptr(), t.ptr());                              }

/// Function template to set MC truth information using SmartDataPtr + Pointer
template <class F, class T> inline
StatusCode 
setMCTruth(SmartDataPtr<F>& f, const T* t) 
{ return setMCTruth(f.ptr(), t);                                    }

/// Function template to set MC truth information using Pointer + SmartDataPtr
template <class F, class T> inline
StatusCode 
setMCTruth(const F* f, SmartDataPtr<T>& t)
{ return setMCTruth(f,t.ptr());                                     }

/// Function template to set MC truth information using SmartRefs
template <class F, class T> inline 
StatusCode setMCTruth(const SmartRef<F>& f, const SmartRef<T>& t) 
{ return setMCTruth(f.target(), t.target());                        }

/// Function template to set MC truth information using SmartRef + Pointer
template <class F, class T> inline
StatusCode setMCTruth(const SmartRef<F>& f, const T* t) 
{ return setMCTruth(f.target(), t);                                 }

/// Function template to set MC truth information using Pointer + SmartRef
template <class F, class T> inline
StatusCode setMCTruth(const F* f, const SmartRef<T>& t)
{ return setMCTruth(f,t.target());                                  }

/// Function template to set MC truth information using SmartDataPtr + SmartRef
template <class F, class T> inline
StatusCode setMCTruth(SmartDataPtr<F>& f, const SmartRef<T>& t) 
{ return setMCTruth(f.ptr(), t.target());                           }

/// Function template to set MC truth information using SmartRef + SmartDataPtr
template <class F, class T> inline
StatusCode setMCTruth(const SmartRef<F>& f, SmartDataPtr<T>& t)
{ return setMCTruth(f.target(),t.ptr());                            }

/// Function template to retrieve MC truth information
template <class T> inline
T* mcTruth(const ContainedObject* f)  { return (T*)MCTruth<T>(f);   }

/// Function template to retrieve MC truth information
template <class T> inline
T* mcTruth(const DataObject* f)       { return (T*)MCTruth<T>(f);   }

#endif // EVENT_MCTRUTH_H
