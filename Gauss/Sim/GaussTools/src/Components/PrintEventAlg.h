// $Id: PrintEventAlg.h,v 1.1 2002-12-12 15:19:34 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log:
// ============================================================================
#ifndef    GaussTools_PrintEventAlg_H 
#define    GaussTools_PrintEventAlg_H 1 
// ============================================================================
// Include files
#include "GaudiKernel/Algorithm.h"
// forward declaration 
template <class ALGORITHM>
class AlgFactory            ; ///< GaudiKernel 

/** @class PrintEventAlg.h 
 *
 *  Almost empty/primitive algorithm to trigger GiGa 
 *
 *  @author  Witold Pokorski
 *  @date    09/12/2002
 */
class PrintEventAlg: 
  public Algorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<PrintEventAlg>;
  
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
  PrintEventAlg( const std::string& name   , 
                       ISvcLocator*       SvcLoc ); 

  /** destructor 
   */
  virtual ~PrintEventAlg();
  
private:
  
  /** default constructor is private! 
   */
  PrintEventAlg();

  /** copy  constructor is private! 
   *  @param copy object to be copied  
   */
  PrintEventAlg
  ( const PrintEventAlg& copy );

  /** assignement operator is private! 
   *  @param copy object to be copied  
   */
  PrintEventAlg& operator= 
  ( const PrintEventAlg& copy );
  
private:
  ///
  std::string    m_particles   ;
  std::string    m_vertices    ;
  std::string    m_hits    ;
  long m_depth;

  void printDecayTree(long, const std::string&, const MCParticle*);

  /// Reference to the particle property service
  IParticlePropertySvc* m_ppSvc;

  ///
};

// ============================================================================
// The End 
// ============================================================================
#endif  //  
// ============================================================================
