// $Id: CaloPi0Monitor.h,v 1.1.1.1 2004-10-25 09:00:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/17 12:00:59  ibelyaev
//  add new algorithm and update for new base
//
// Revision 1.2  2002/06/26 15:56:46  ibelyaev
//  minor update of histograms
//
// Revision 1.1.1.1  2002/06/16 13:51:56  ibelyaev
// new package
//
// Revision 1.1  2002/06/15 15:40:53  ibelyaev
//  new Pi0 monitoring algorithm
//
// Revision 1.3  2002/06/15 12:53:13  ibelyaev
//  version update
//
// ============================================================================
#ifndef CALOEX_CALOPi0Monitor_H 
#define CALOEX_CALOPi0Monitor_H 1
// Include files
// from STL
#include <string>
// from CaloKernel
#include "CaloKernel/CaloAlgorithm.h"
// forward declarations 
class IHistogram1D;   // from AIDA

/** @class CaloPi0Monitor CaloPi0Monitor.h
 *  
 *  Simple pi0 monitoring algorithm
 *  It produces 7 histograms: 
 *  <ol>
 *  <li> "Raw" mass distribution of 2 photons </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for each photon </li>
 *  <li> Mass distribution of 2 photons after     Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 0.5*Pt cut for combination </li>
 *  <li> Mass distribution of 2 photons after 2.0*Pt cut for combination </li>
 *  </ol>
 *
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   02/11/2001
 */

class CaloPi0Monitor : public CaloAlgorithm 
{
  /// friend factory for instantiation
  friend class AlgFactory<CaloPi0Monitor>;

public:
  
  /** standard algorithm initialization 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode initialize();   
  
  /** standard algorithm execution 
   *  @see CaloAlgorithm
   *  @see     Algorithm
   *  @see    IAlgorithm
   *  @return status code 
   */
  virtual StatusCode execute   ();   
  
protected:
  
  /** Standard constructor
   *  @param   name   algorithm name 
   *  @param   svcloc pointer to service locator 
   */
  CaloPi0Monitor( const std::string& name   , 
              ISvcLocator*       svcloc );
  
  /// destructor (virtual and protected)
  virtual ~CaloPi0Monitor();
  
private:

  /// default  construstor  is  private 
  CaloPi0Monitor(); 
  /// copy     construstor  is  private 
  CaloPi0Monitor
  ( const CaloPi0Monitor& );
  /// assignement operator  is  private 
  CaloPi0Monitor& operator=
  ( const CaloPi0Monitor& );
  
private:
  
  // cut on Photon Pt 
  double         m_ptPhoton        ;
  
  // cut on Pi0    Pt 
  double         m_ptPi0           ;
  
  // 2 photon  mass distribution  (no cuts)
  IHistogram1D*  m_2gamma          ;
  
  // 2 photon  mass distribution  (both photons have some minimal pt)
  IHistogram1D*  m_2gammaPtPhoton  ;

  // 2 photon  mass distribution  (both photons have some minimal pt)
  IHistogram1D*  m_2gammaPtPhoton1 ;

  // 2 photon  mass distribution  (both photons have some minimal pt)
  IHistogram1D*  m_2gammaPtPhoton2 ;
  
  // 2 photon  mass distribution  (Pi0  has some minimal pt )
  IHistogram1D*  m_2gammaPtPi0     ;

  // 2 photon  mass distribution  (Pi0  has some minimal pt )
  IHistogram1D*  m_2gammaPtPi01    ;

  // 2 photon  mass distribution  (Pi0  has some minimal pt )
  IHistogram1D*  m_2gammaPtPi02    ;
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOPi0Monitor_H
// ============================================================================
