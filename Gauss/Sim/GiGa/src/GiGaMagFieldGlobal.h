#ifndef    GIGA_GIGAMagFieldGLOBAL_H
#define    GIGA_GIGAMagFieldGLOBAL_H 1 

#include "GiGa/GiGaMagField.h"

template <class MagField> class GiGaMagFieldFactory;


/** @class GiGaMagFieldGlobal     GiGaMagFieldGlobal.h GiGa/GiGaMagFieldGlobal.h

    Implemenation of "Global Magnetic Field" object.
    It is just the delegation to IMagneticSvc 

    @author Vanya Belyaev
*/

class GiGaMagFieldGlobal: public GiGaMagField 
{
  ///
  friend class GiGaMagFieldFactory<GiGaMagFieldGlobal>;
  ///
 protected:
  /// constructor 
  GiGaMagFieldGlobal( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaMagFieldGlobal();
  ///
 public:
  ///
  virtual void GetFieldValue ( const double [3], double *B  ) const ;
  ///
 private:
  ///
  GiGaMagFieldGlobal           ()                            ; /// no default constructor! 
  GiGaMagFieldGlobal           ( const GiGaMagFieldGlobal& ) ; /// no copy constructor!
  GiGaMagFieldGlobal& operator=( const GiGaMagFieldGlobal& ) ; /// no assignment! 
  ///
 private:
  ///
};
///




#endif  // GIGA_GIGAMagFieldGLOBAL_H
