// $Id: GiGaGetEventAlg.h,v 1.1.1.1 2004-02-20 19:43:29 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log:
// ============================================================================
#ifndef    GIGA_GIGAMISCGETEVENTALG_H 
#define    GIGA_GIGAMISCGETEVENTALG_H 1 
// ============================================================================
// Include files
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// forward declaration 
template <class ALGORITHM>
class AlgFactory            ; ///< GaudiKernel 

/** @class GiGaMiscGetEventAlg GiGaMiscGetEventAlg.h 
 *
 *  Almost empty/primitive algorithm to trigger GiGa 
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    21/02/2001
 */
class GiGaGetEventAlg : public GaudiAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<GiGaGetEventAlg>;
  
public:
  
  /** the standard Algorithm execution method 
   *  @see IAlgorithm
   *  @return status code
   */
  virtual StatusCode execute    ();
  
protected:
  
  /** standard constructor
   *  @param name name of teh algorithm 
   *  @param SvcLoc pointer to service locator 
   */
  GiGaGetEventAlg ( const std::string& name   , 
                    ISvcLocator*       SvcLoc ) ; 
  
  /// destructor 
  virtual ~GiGaGetEventAlg();
  
private:
  
  // default constructor is private! 
  GiGaGetEventAlg();
  // copy  constructor is private! 
  GiGaGetEventAlg            ( const GiGaGetEventAlg& copy );
  // assignement operator is private! 
  GiGaGetEventAlg& operator= ( const GiGaGetEventAlg& copy );
 
private:
  ///
  std::string    m_particles   ;
  std::string    m_vertices    ;
};

// ============================================================================
// The End 
// ============================================================================
#endif  // GIGA_GIGAMISCGETEVENTALG_H 
// ============================================================================
