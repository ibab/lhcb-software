#ifndef    GIGA_GIGAMagFieldUniform_H
#define    GIGA_GIGAMagFieldUniform_H 1 

#include "GiGa/GiGaMagField.h"

template <class MagField> class GiGaMagFieldFactory;


/** @class GiGaMagFieldUniform     GiGaMagFieldUniform.h  GiGa/GiGaMagFieldUniform.h

    Implemenation of "Uniform Magnetic Field" object.
     

    @author Vanya Belyaev
*/

class GiGaMagFieldUniform: public GiGaMagField 
{
  ///
  friend class GiGaMagFieldFactory<GiGaMagFieldUniform>;
  ///
 protected:
  /// constructor 
  GiGaMagFieldUniform( const std::string& , ISvcLocator*); 
  /// virtual destructor 
  virtual ~GiGaMagFieldUniform();
  ///
 public:
  ///
  virtual void GetFieldValue ( const double [3], double *B  ) const ;
  ///
 private:
  ///
  GiGaMagFieldUniform           ()                             ; /// no default constructor! 
  GiGaMagFieldUniform           ( const GiGaMagFieldUniform& ) ; /// no copy constructor!
  GiGaMagFieldUniform& operator=( const GiGaMagFieldUniform& ) ; /// no assignment! 
  ///
 private:
  ///
  float    m_Bx ; 
  float    m_By ; 
  float    m_Bz ; 
  ///
};
///




#endif  // GIGA_GIGAMagFieldUniform_H
