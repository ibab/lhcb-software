// $Id: ICaloTrackMatch.h,v 1.2 2001-11-09 14:30:04 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1.1.2  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKMATCH_H 
#define CALOINTERFACES_ICALOTRACKMATCH_H 1
// Include files
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloTrackMatch.h"

class CaloPosition ; ///< from CaloEvent package 
class TrTrack      ; ///< from TrKernel  package

/** @class ICaloTrackMatch ICaloTrackMatch.h CaloInterfaces/ICaloTrackMatch.h
 *  
 *  The generic interface for matching of 
 *  calorimeter object with tracking object 
 *
 *  The potential implementation candidates are:
 *
 *    - full matching of CaloParticle/CaloHypo object with Track 
 *      for electron/positron PID. The energy is taken into account 
 *      for matching procedure
 *    - matching of position of cluster with track for photon 
 *      selection/identification. The energy is not taken 
 *      into account in the matching procedure 
 *    - matching of track DIRECTION before magnet with 
 *      CaloCluster/CaloHypo/CaloParticle for searching for 
 *      bremmstrahlung photons. Energy is not taken 
 *      into account.
 * 
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author Dima  Rusinov Dmitri.Roussinov@cern.ch
 *  @date   30/10/2001
 */

class ICaloTrackMatch: virtual public IAlgTool
{
public:
  
  /** interface identification
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_ICaloTrackMatch ; }
  
  /** standard initialization
   *  @return status code
   */
  virtual StatusCode initialize () = 0 ;
  
  /** standard finalization 
   *  @return status code
   */
  virtual StatusCode finalize   () = 0 ;
  
  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  virtual StatusCode match ( const CaloPosition* caloObj  , 
                             const TrTrack*      trObj    ,
                             double&             chi2     ) = 0 ;

  /** destructor
   */
  virtual ~ICaloTrackMatch(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOTRMATCH_H
// ============================================================================
