// $Id: GaussTrackActionZ.h,v 1.1 2004-02-20 19:35:25 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef GAUSSTOOLS_GAUSSTRACKACTIONZ_H 
#define GAUSSTOOLS_GAUSSTRACKACTIONZ_H 1
// ============================================================================
// Include files
// ============================================================================
#include "GaussTrackActionBase.h"
// ============================================================================

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
  /// friend factory for instantiation 
  friend class GiGaFactory<GaussTrackActionZ> ;
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

// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSTOOLS_GAUSSTRACKACTIONZ_H
// ============================================================================
