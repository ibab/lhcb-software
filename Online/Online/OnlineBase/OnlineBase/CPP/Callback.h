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
  template <typename T> struct Wrapper { 
  public:
    typedef T pmf_t;
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
  };
  template <typename T> const Callback& _make(ulong (*fptr)(void* o, const void* f, const void* u), T pmf) {
    typename Wrapper<T>::Functor f(pmf);
    func = mfunc_t(f.ptr[0],f.ptr[1]);
    call = fptr;
    return *this;
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)(const void*)) {
    typedef R (T::*pfunc_t)(const void*);
    struct _Wrapper : public Wrapper<pfunc_t>  {
      static ulong call(void* o, const void* f, const void* u) 
      {	return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))(u); }
    };
    return _make(_Wrapper::call, pmf);
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)(const void*) const) {
    typedef R (T::*pfunc_t)(const void*) const;
    struct _Wrapper : public Wrapper<pfunc_t>  { 
      static ulong  call(void* o, const void* f, const void* u) 
      {	return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))(u); }
    };
    return _make(_Wrapper::call, pmf);
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)()) {
    typedef R (T::*pfunc_t)();
    struct _Wrapper : public Wrapper<pfunc_t>  { 
      static ulong  call(void* o, const void* f, const void*) 
      {	return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();  }
    };
    return _make(_Wrapper::call, pmf);
  }
  template <typename R, typename T> const Callback& make(R (T::*pmf)() const) {
    typedef R (T::*pfunc_t)() const;
    struct _Wrapper : public Wrapper<pfunc_t>  { 
      static ulong  call(void* o, const void* f, const void*)
      {	return (ulong) (cast<T>(o)->*(typename Wrapper<pfunc_t>::Functor(f).pmf))();  }
    };
    return _make(_Wrapper::call, pmf);
  }

  template <typename T> static Callback make(void* p, T pmf) { return Callback(p).make(pmf); }
};
#endif  // ONLINEKERNEL_CPP_CALLBACK_H
