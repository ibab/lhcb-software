#ifndef __SmartPointer_H__
#define __SmartPointer_H__

/// Definition of a class holding a pointer to an object with automatic destruction and type casting
/** 

    {\Large{\bf template <class Type> class SmartPointer}}      
    \\
    SmartPointer class:						    \\
    - Hold pointer to parent object				    \\
    - Automatic conversion to requested parent class		    \\
    - Automatic conversion to requested parent's pointer type	    \\
    - Automatic release on Destruction				    \\
    \\
    M.Frank				    \\
*/
template <class Type> class SmartPointer {
protected:
  //@Man: Protected member variables
  /// Pointer to the object to be held
  Type* __pType;
public:
  //@Man: Public member functions
  //  SmartPointer::SmartPointer
  //  ========================
  ///  Standard Constructor: Returns reference to SmartPointer instance
  //  Arguments: None
  explicit SmartPointer( Type* obj = 0 ) {
    __pType = obj;
  }

  //  SmartPointer::~SmartPointer
  //  ========================
  ///  Standard Destructor
  //  Arguments: None
  virtual ~SmartPointer() {
    delete __pType;
  }

  //  SmartPointer::operator Type*
  //  ============================
  ///  Automatic conversion to pointer of the Object if required.
  //
  //  Arguments: None
  //  Return Value:     Type* 
  virtual operator Type* () { 
    return __pType; 
  }

  //  SmartPointer::operator Type&
  //  ============================
  ///  Automatic conversion to reference of the Object
  //
  //  Arguments: None
  //  Return Value:     Type&
  virtual operator Type& () { 
    return *__pType;
  }

  //  SmartPointer::operator =
  //  =========================
  /// Load entry to the object
  //
  //  Arguments: Pointer to entry to be loaded
  //  Return Value:     bool
  virtual void operator = ( Type* ptr )  {
    __pType = ptr;
  }
};
#endif
