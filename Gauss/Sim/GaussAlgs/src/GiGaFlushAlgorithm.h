// $Id: GiGaFlushAlgorithm.h,v 1.2 2004-02-22 16:52:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.1  2004/02/20 19:43:29  ibelyaev
// New package 
//
// Revision 1.1  2002/09/26 18:10:56  ibelyaev
//  repackageing: add all concrete implementations from GiGa
//
// Revision 1.1  2002/01/22 18:20:53  ibelyaev
//  Vanya: update for newer versions of Gaudi and Geant4
//
// ============================================================================
#ifndef GIGA_GIGAFLUSHALGORITHM_H 
#define GIGA_GIGAFLUSHALGORITHM_H 1
// ============================================================================
// Include files
// ============================================================================
// from GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// forward declarations
// ============================================================================
template <class ALGORITHM>
class AlgFactory             ; ///< GaudiKernel
class IGiGaSvc               ; ///< GiGa
// ============================================================================

/** @class GiGaFlushAlgorithm GiGaFlushAlgorithm.h 
 *  
 *  Very simple algorithm to tigger the event processing 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   22/01/2002
 */
class GiGaFlushAlgorithm : public GaudiAlgorithm 
{
  /// friend factory for instantiation 
  friend class AlgFactory<GiGaFlushAlgorithm>;

public:
  
  /** standard algorithm initialization 
   *  @return status code
   */
  virtual StatusCode initialize();   

  /** standard algorithm execution 
   *  @return status code
   */
  virtual StatusCode execute   ();   
  
protected:

  /** acessor to GiGa Service
   *  @return pointer to GiGa Service
   */
  inline IGiGaSvc* gigaSvc() const { return m_gigaSvc ; }
  
protected:
  
  /** Standard constructor
   *  @param Name name of the algorithm 
   *  @param SvcLoc pointer to service locator 
   */
  GiGaFlushAlgorithm( const std::string& Name   , 
                      ISvcLocator*       SvcLoc );
  
  /** destructor 
   */
  virtual ~GiGaFlushAlgorithm();
  
private:
  
  /** default constructor is private! 
   */
  GiGaFlushAlgorithm();
  
  /** copy constructor is private! 
   *  @param copy object to be copied 
   */
  GiGaFlushAlgorithm
  ( const GiGaFlushAlgorithm& copy ) ;

  /** assignement operator is private! 
   *  @param copy object to be copied 
   */
  GiGaFlushAlgorithm& operator=
  ( const GiGaFlushAlgorithm& copy ) ;

private:
  
  std::string     m_gigaSvcName ;
  IGiGaSvc*       m_gigaSvc     ;
  
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGA_GIGAFLUSHALGORITHM_H
// ============================================================================
