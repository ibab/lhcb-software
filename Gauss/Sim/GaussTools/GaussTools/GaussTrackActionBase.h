// $Id: GaussTrackActionBase.h,v 1.3 2004-04-20 04:27:14 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2004/02/22 16:51:54  ibelyaev
//  few minor fixes
//
// Revision 1.1  2004/02/20 19:35:25  ibelyaev
//  major update
// 
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBASE_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBASE_H 1
// ============================================================================
// Include files
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/GiGaTrackActionBase.h"
// ============================================================================

class GaussTrajectory       ;
class GaussTrackInformation ;

/** @class GaussTrackActionBase GaussTrackActionBase.h 
 *  
 *  Track action, valid for certain region in Z 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionBase : public GiGaTrackActionBase 
{
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackActionBase> ;
public:
  
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalise the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode finalize   () ;
  
protected:
  
  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GaussTrackActionBase
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionBase();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionBase() ; 
  // copy    constructor   is disabled 
  GaussTrackActionBase           ( const GaussTrackActionBase& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionBase& operator=( const GaussTrackActionBase& ) ;
  
protected:
  
  // get the valid Gauss Trajectory
  GaussTrajectory*       trajectory    () const ;
  // get the valid Gauss tarck information 
  GaussTrackInformation* trackInfo     () const ;
  // get the valid Gauss tarck information 
  GaussTrackInformation* trackInfo     ( G4Track*  track ) const ;
  // store children tracks 
  StatusCode             storeChildren () const ;
  
protected :

  // mark information "to be stored" and increment 
  // the flag counter 
  StatusCode mark ( GaussTrackInformation* info ) const ;
  
private:
  
  // 'long' counter 
  typedef ulonglong UCounter        ;
  // counter for all created trajectories 
  mutable UCounter  m_nTrajectories ;
  // counter for all created track information objects 
  mutable UCounter  m_nTrackInfos   ;
  // counter of "decisions"
  mutable size_t    m_marked        ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONBASE_H
// ============================================================================
