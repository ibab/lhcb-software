// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
/*
 * counted_ptr - simple reference counted pointer.
 *
 * The is a non-intrusive implementation that allocates an additional
 * int and pointer for every counted object.
 */

/* Got this originally from:
   http://ootips.org/yonat/4dev/counted_ptr.h

   applied a few changes ('signed' with //jr)
   o removed friend that's specialisation of the own class (with Y)
   o added conversion to bool (well, to void*, apparently that's safer)
   o completely changed the way the pointer is stored - now
     as a pointer to void; this facilitates conversions of
     counted_ptr<someclass> in the same way as real pointers to
     a pointer of, say, counted_ptr<someclassesBase>
   Jonas.
*/

#ifndef COUNTED_PTR_H
#define COUNTED_PTR_H

/* For ANSI-challenged compilers, you may want to #define
 * NO_MEMBER_TEMPLATES or explicit */

//#define NO_MEMBER_TEMPLATES

//#include <iostream>

namespace MINT{

class counted_ptr_counter{
 public:
  counted_ptr_counter(unsigned c = 1) : count(c) {}
  unsigned    count;
};


template <typename X> class counted_ptr
{

 public: // many of these should be protected, but doesn't work.
  X* ptr;
  counted_ptr_counter* itsCounter;
 
  bool ok() const{
    return (0 != itsCounter) && (0 != ptr);
  }

  void acquire(const counted_ptr<X>& other) throw()
  { // increment the count
    // std::cout << "acquiring" << std::endl;
    itsCounter = other.itsCounter;
    ptr = other.ptr;
    if (ok()) ++(itsCounter->count);
  }
  
  template<typename Y>
  void acquire(const counted_ptr<Y>& other) throw()
  { // increment the count
    // std::cout << "acquiring different type" << std::endl;
    itsCounter = other.itsCounter;
    ptr = (X*) (other.ptr);
    if (ok()) ++(itsCounter->count);
  }
  
  void release()
  { // decrement the count, delete if it is 0
    // std::cout << "releasing" << std::endl;
    if (ok()) {
      if (--(itsCounter->count) == 0) {
	delete ptr;
	delete itsCounter;
      }
      ptr=0;
      itsCounter = 0;
    }
  }

 counted_ptr(X* p = 0) // allocate a new counter
   : ptr(p)
    , itsCounter(0){
    if (p){
      itsCounter = new counted_ptr_counter(1);
      ptr = p;
    }
  }
  ~counted_ptr()
    {release();}
  counted_ptr(const counted_ptr& r) throw()
    {
      // std::cout << "copy-constructing" << std::endl;
      acquire(r);
    }
  counted_ptr& operator=(const counted_ptr& r)
    {
      // std::cout << "copy- = " << std::endl;
      if (this != &r) {
	release();
	acquire(r);
      }
      return *this;
    }
  
#ifndef NO_MEMBER_TEMPLATES
    //template <class Y> friend class counted_ptr<Y>; // jr
    template <typename Y> counted_ptr(const counted_ptr<Y>& r) throw()
        {
	  // std::cout << "copy-constructing other" << std::endl;
	  acquire(r);
	}
    template <typename Y> counted_ptr& operator=(const counted_ptr<Y>& r)
    {
      if ((void*)this != (void*) (&r)) {
	  release();
	  // std::cout << "copy = other" << std::endl;
	  acquire(r);
        }
        return *this;
    }
#endif // NO_MEMBER_TEMPLATES

    X* get()const throw(){return (this->ok() ? static_cast<X*>(ptr) : 0);}
    X& operator*()  const throw(){return *(static_cast<X*>(get()));}
    X* operator->() const throw(){return static_cast<X*>(get());}
    bool unique()   const throw()
        {return (itsCounter ? itsCounter->count == 1 : true);}

    
    operator void*() const{ // jr
      return (void*) get();
    }
    

    
    //    operator bool() const{ // jr
    //      return 0 != get();
    //    }
    
    
    /* wise? not sure. 
    operator X*() const{ // jr
      return (X*) get();
    }
    */
    

};

template <typename X> class const_counted_ptr : public counted_ptr<X>{
 public:    
  bool ok() const{
    return counted_ptr<X>::ok();
  }
  void acquire(const counted_ptr<X>& c) throw(){
    counted_ptr<X>::acquire(c);}
  
  template<typename Y>
    void acquire(const counted_ptr<Y>& other) throw(){
    counted_ptr<X>::acquire(other);}

  void release(){
    counted_ptr<X>::release();}

  explicit const_counted_ptr(X* p = 0) // allocate a new counter
    : counted_ptr<X>(p){};
  ~const_counted_ptr()
    {release();}
  const_counted_ptr(const const_counted_ptr& other) throw()
    :counted_ptr<X>(other)
    {}
  const_counted_ptr(const counted_ptr<X>& other) throw()
    :counted_ptr<X>(other)
    {}
  const_counted_ptr& operator=(const const_counted_ptr& r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }
      return *this;
    }
  const_counted_ptr& operator=(const counted_ptr<X>& r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }
      return *this;
    }
  
  template <typename Y> const_counted_ptr(const const_counted_ptr<Y>& r) throw()
    {
      acquire(r);
    }
  template <typename Y> const_counted_ptr(const counted_ptr<Y>& r) throw()
    {
      acquire(r);
    }
  template <typename Y> const_counted_ptr& operator=(const const_counted_ptr<Y>& r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }
      return *this;
    }
  template <class Y> const_counted_ptr& operator=(const counted_ptr<Y>& r)
    {
      if (this != &r) {
	release();
	acquire(r);
      }
      return *this;
    }

  const X* get() const throw(){return (this->ok() ? static_cast<const X*>(this->ptr) : 0);}
  
  const X& operator*()  const throw(){return *(static_cast<const X*>(get()));}
  const X* operator->() const throw(){return static_cast<const X*>(get());}
  
};

}// namespace MINT

#endif // COUNTED_PTR_H
