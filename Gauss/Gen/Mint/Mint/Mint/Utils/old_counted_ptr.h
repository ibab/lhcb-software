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

namespace MINT{

class counted_ptr_counter{
 public:
  counted_ptr_counter(void* p = 0, unsigned c = 1) : ptr(p), count(c) {}
  void*       ptr;
  unsigned    count;
};


template <typename X> class counted_ptr
{
 public:
    // itsCounter, aquire, release SHOULD be private, but then things like
    // counted_ptr<IDalitzEvent> = sth that is counted_ptr<DalitzEvent>
    // don't work - I think it's a compiler limitation.
  counted_ptr_counter* itsCounter;

    void acquire(counted_ptr_counter* c) throw()
    { // increment the count
        itsCounter = c;
        if (c) ++(c->count);
    }

    void release()
    { // decrement the count, delete if it is 0
        if (itsCounter) {
            if (--itsCounter->count == 0) {
                delete (X*)(itsCounter->ptr);
                delete itsCounter;
            }
            itsCounter = 0;
        }
    }

 public:
  typedef X element_type;
  
  explicit counted_ptr(X* p = 0) // allocate a new counter
    : itsCounter(0) {if (p) itsCounter = new counted_ptr_counter(p);}
  ~counted_ptr()
    {release();}
  counted_ptr(const counted_ptr& r) throw()
    {acquire(r.itsCounter);}
  counted_ptr& operator=(const counted_ptr& r)
    {
      if (this != &r) {
	release();
	acquire(r.itsCounter);
      }
      return *this;
    }
  
#ifndef NO_MEMBER_TEMPLATES
    //template <class Y> friend class counted_ptr<Y>; // jr
    template <class Y> counted_ptr(const counted_ptr<Y>& r) throw()
        {
	  // with this trick we check (I hope) if the conversion
	  // is legal (jr):
	  static_cast<X*>(r.get());
	  acquire(r.itsCounter);
	}
    template <class Y> counted_ptr& operator=(const counted_ptr<Y>& r)
    {
        if (this != &r) {
	  // with this trick we check (I hope) if the conversion
	  // is legal (jr):
	  static_cast<X*>(r.get());
	  release();
	  acquire(r.itsCounter);
        }
        return *this;
    }
#endif // NO_MEMBER_TEMPLATES

    X& operator*()  const throw(){return *(static_cast<X*>(itsCounter->ptr));}
    X* operator->() const throw(){return static_cast<X*>(itsCounter->ptr);}
    X* get()        const throw(){return ((itsCounter) ? static_cast<X*>(itsCounter->ptr) : 0);}
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
  void acquire(counted_ptr_counter* c) throw(){
    counted_ptr<X>::acquire(c);}
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
	acquire(r.itsCounter);
      }
      return *this;
    }
  const_counted_ptr& operator=(const counted_ptr<X>& r)
    {
      if (this != &r) {
	release();
	acquire(r.itsCounter);
      }
      return *this;
    }
  
#ifndef NO_MEMBER_TEMPLATES
  template <class Y> const_counted_ptr(const const_counted_ptr<Y>& r) throw()
    {
      // with this trick we check (I hope) if the conversion
      // is legal (jr):
      static_cast<X*>(r.get());
      acquire(r.itsCounter);
    }
  template <class Y> const_counted_ptr(const counted_ptr<Y>& r) throw()
    {
      // with this trick we check (I hope) if the conversion
      // is legal (jr):
      static_cast<X*>(r.get());
      acquire(r.itsCounter);
    }
  template <class Y> const_counted_ptr& operator=(const const_counted_ptr<Y>& r)
    {
      if (this != &r) {
	// with this trick we check (I hope) if the conversion
	// is legal (jr):
	static_cast<X*>(r.get());
	release();
	acquire(r.itsCounter);
      }
      return *this;
    }
  template <class Y> const_counted_ptr& operator=(const counted_ptr<Y>& r)
    {
      if (this != &r) {
	// with this trick we check (I hope) if the conversion
	// is legal (jr):
	static_cast<X*>(r.get());
	release();
	acquire(r.itsCounter);
      }
      return *this;
    }
#endif // NO_MEMBER_TEMPLATES
    const X& operator*()  const throw(){return *(static_cast<X*>(this->itsCounter->ptr));}
    const X* operator->() const throw(){return static_cast<X*>(this->itsCounter->ptr);}
    const X* get()        const throw(){return ((this->itsCounter) ? static_cast<X*>(this->itsCounter->ptr) : 0);}

};

}// namespace MINT

#endif // COUNTED_PTR_H
