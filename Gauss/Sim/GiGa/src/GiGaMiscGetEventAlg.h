#ifndef    GIGA_GIGAMISCGETEVENTALG_H 
#define    GIGA_GIGAMISCGETEVENTALG_H 1 


// Include files
#include "GaudiKernel/Algorithm.h"

class IGiGaSvc;

/** @class GiGaMiscGetEventAlg GiGaMiscGetEventAlg.h GiGa/GiGaMiscGetEventalg.h

    Almost empty algorithm to trigger GiGa 

    @author  Vanya Belyaev
    @date    21/02/2001
*/

///
class GiGaMiscGetEventAlg : public Algorithm 
{
  ///
 public:
  /// Constructor of this form must be provided
  GiGaMiscGetEventAlg(const std::string& name, ISvcLocator* pSvcLocator); 
  ///
  virtual ~GiGaMiscGetEventAlg(){};
  ///  
  /// Three mandatory member functions of any algorithm
  StatusCode initialize ();
  StatusCode execute    ();
  StatusCode finalize   ();
  ///
 protected:
  ///
  inline IGiGaSvc* gigaSvc() const { return m_gigaSvc; } 
  ///
 private:
  ///
  std::string    m_GiGaSvcName; 
  IGiGaSvc*      m_gigaSvc; 
  ///
};
///

#endif  // GIGA_GIGAMISCGETEVENTALG_H 
