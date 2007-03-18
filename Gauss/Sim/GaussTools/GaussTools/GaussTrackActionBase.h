// $Id: GaussTrackActionBase.h,v 1.5 2007-03-18 19:16:12 gcorti Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONBASE_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONBASE_H 1
// Include files

// from Gaudi
#include "GaudiKernel/Kernel.h"

// from GiGa 
#include "GiGa/GiGaTrackActionBase.h"

// Forward declaration
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


#endif // GAUSSTOOLS_GAUSSTRACKACTIONBASE_H
