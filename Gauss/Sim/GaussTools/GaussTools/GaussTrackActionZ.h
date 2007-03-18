// $Id: GaussTrackActionZ.h,v 1.3 2007-03-18 19:16:12 gcorti Exp $
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONZ_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONZ_H 1

// Include files
#include "GaussTrackActionBase.h"

// Forward declaration
class GaussTrajectory       ;
class GaussTrackInformation ;

/** @class GaussTrackActionZ GaussTrackActionZ.h 
 *  
 *  Track action, valid for certain region in Z 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2004-02-19
 */
class GaussTrackActionZ : public GaussTrackActionBase 
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
  GaussTrackActionZ
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~GaussTrackActionZ();
  
private:
  
  // default constructor   is disabled 
  GaussTrackActionZ() ; 
  // copy    constructor   is disabled 
  GaussTrackActionZ           ( const GaussTrackActionZ& ) ; 
  // assignement operator  is disabled 
  GaussTrackActionZ& operator=( const GaussTrackActionZ& ) ;
  
protected:
  
  double zMin           () const { return m_zMin ; }
  double zMax           () const { return m_zMax ; }
  
private:
  
  double  m_zMin            ;
  double  m_zMax            ;
  
};


#endif // GAUSSTOOLS_GAUSSTRACKACTIONZ_H

