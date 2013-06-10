/******************************************************************************
**      Small class to implement a Finite State Machine
**
**  AUTHORS:
**
**      M.Frank
**
**  CREATION DATE:  22-MAR-2013
**
******************************************************************************/
#ifndef ONLINEKERNEL_CPP_CALLBACK_H
#define ONLINEKERNEL_CPP_CALLBACK_H
#include <algorithm>

/**@class Callback  Callback.h CPP/Callback.h
*
*  Definition of the generic callback structure for member functions
*
* @author  M.Frank
* @date    01/03/2013
* @version 0.1
*/
struct Callback  {
  typedef unsigned long (*func_t)(void*,const void*,const void*);
  typedef std::pair<void*,void*> mfunc_t;
  typedef unsigned long ulong;

  void*   par;
  func_t  call;
  mfunc_t func;
  Callback() : par(0), call(0), func(0,0) {  }
  Callback(void* p) : par(p), call(0), func(0,0) {  }
  Callback(void* p, void* mf, func_t c) : par(p), call(c) { func = *(mfunc_t*)mf; }

  operator bool() const { return (call && par && func.first); }
  unsigned long execute(const void* user_param) const { return (*this) ? call(par,&func,user_param) : 0;}

  template <typename T> static T* cast(void* p)  { return (T*)p;  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)(const void*)) {
    struct Wrapper { 
      typedef R (T::*pmf_t)(const void*);
      union Functor {
	void*   ptr[2];
	pmf_t   pmf;
	Functor(const void* f)  { *(mfunc_t*)ptr = *(mfunc_t*)f; }
	Functor(pmf_t f)        { pmf = f;                       }
      };
      static mfunc_t pmf (pmf_t f)  { return Functor(f).ptr; }
      static ulong   call(void* o, const void* f, const void* u) {
	pmf_t pmf = Functor(f).pmf;
	R res = (cast<T>(o)->*pmf)(u); 
	return (ulong)res;
      }
    };
    typename Wrapper::Functor f(pmf);
    func = *(mfunc_t*)f.ptr;
    call = Wrapper::call;
    return *this;
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)(const void*) const) {
    struct Wrapper { 
      typedef R (T::*pmf_t)(const void*) const;
      union Functor {
	void*   ptr[2];
	pmf_t   pmf;
	Functor(const void* f)  { *(mfunc_t*)ptr = *(mfunc_t*)f; }
	Functor(pmf_t f)        { pmf = f;                       }
      };
      static mfunc_t pmf (pmf_t f)  { return Functor(f).ptr; }
      static ulong   call(void* o, const void* f, const void* u) {
	pmf_t pmf = Functor(f).pmf;
	R res = (cast<T>(o)->*pmf)(u); 
	return (ulong)res;
      }
    };
    typename Wrapper::Functor f(pmf);
    func = *(mfunc_t*)f.ptr;
    call = Wrapper::call;
    return *this;
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)()) {
    struct Wrapper { 
      typedef R (T::*pmf_t)();
      typedef  unsigned long ulong;
      union Functor {
	void*   ptr[2];
	pmf_t   pmf;
	Functor(const void* f)  { *(mfunc_t*)ptr = *(mfunc_t*)f; }
	Functor(pmf_t f)        { pmf = f;                       }
      };
      static mfunc_t pmf (pmf_t f)  { return Functor(f).ptr;   }
      static ulong   call(void* o, const void* f, const void*) {
	pmf_t pmf = Functor(f).pmf;
	R res = (cast<T>(o)->*pmf)(); 
	return (ulong)res;
      }
    };
    typename Wrapper::Functor f(pmf);
    func = *(mfunc_t*)f.ptr;
    call = Wrapper::call;
    return *this;
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)() const) {
    struct Wrapper { 
      typedef R (T::*pmf_t)() const;
      typedef  unsigned long ulong;
      union Functor {
	void*   ptr[2];
	pmf_t   pmf;
	Functor(const void* f)  { *(mfunc_t*)ptr = *(mfunc_t*)f; }
	Functor(pmf_t f)        { pmf = f;                       }
      };
      static mfunc_t pmf(pmf_t f)  {
	const Functor func(f); 
	return mfunc_t(func.ptr[0],func.ptr[1]);
      }
      static ulong   call(void* o, const void* f, const void*) {
	pmf_t pmf = Functor(f).pmf;
	R res = (cast<T>(o)->*pmf)(); 
	return (ulong)res;
      }
    };
    typename Wrapper::Functor f(pmf);
    func = *(mfunc_t*)f.ptr;
    call = Wrapper::call;
    return *this;
  }
  template <typename T> static Callback make(void* p, T pmf) { return Callback(p).make(pmf); }
};

#endif  // ONLINEKERNEL_CPP_CALLBACK_H
