// $Id: GiGaMiscGetEventAlg.h,v 1.5 2002-05-24 15:57:59 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log:
// ============================================================================
#ifndef    GIGA_GIGAMISCGETEVENTALG_H 
#define    GIGA_GIGAMISCGETEVENTALG_H 1 
// ============================================================================
// Include files
#include "GaudiKernel/Algorithm.h"
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
class GiGaMiscGetEventAlg : 
  public Algorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<GiGaMiscGetEventAlg>;
  
public:

  /** the standard Algorithm initialization method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode initialize ();

  /** the standard Algorithm execution method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode execute    ();

  /** the standard Algorithm finalization method 
   *  @see Algorithm
   *  @return status code
   */
  virtual StatusCode finalize   ();
  
protected:
  
  /** standard constructor
   *  @param name name of teh algorithm 
   *  @param SvcLoc pointer to service locator 
   */
  GiGaMiscGetEventAlg( const std::string& name   , 
                       ISvcLocator*       SvcLoc ); 

  /** destructor 
   */
  virtual ~GiGaMiscGetEventAlg();
  
private:
  
  /** default constructor is private! 
   */
  GiGaMiscGetEventAlg();

  /** copy  constructor is private! 
   *  @param copy object to be copied  
   */
  GiGaMiscGetEventAlg 
  ( const GiGaMiscGetEventAlg& copy );

  /** assignement operator is private! 
   *  @param copy object to be copied  
   */
  GiGaMiscGetEventAlg& operator= 
  ( const GiGaMiscGetEventAlg& copy );
  
private:
  ///
  std::string    m_particles   ;
  std::string    m_vertices    ;
  std::string    m_hits    ;

  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif  // GIGA_GIGAMISCGETEVENTALG_H 
// ============================================================================
