// $Id: ICaloTrackMatch.h,v 1.4 2002-04-27 19:22:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $  
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2001/11/29 17:11:17  ibelyaev
//  improvement in CaloMatch interface
//
// Revision 1.2  2001/11/09 14:30:04  ibelyaev
//  update in ICaloTrackMatch interface
//
// Revision 1.1.1.2  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
// ============================================================================
#ifndef CALOINTERFACES_ICALOTRACKMATCH_H 
#define CALOINTERFACES_ICALOTRACKMATCH_H 1
// Include files
// STD & STL 
#include <functional>
#include <utility>
// GaudiKernel 
#include "GaudiKernel/IAlgTool.h"
// local 
#include "CaloInterfaces/IIDICaloTrackMatch.h"

class CaloPosition  ; ///< from CaloEvent package 
class TrStoredTrack ; ///< from TrEvent  package

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

class ICaloTrackMatch: 
  public  virtual IAlgTool ,
  public  std::binary_function<const CaloPosition*,const TrStoredTrack*,
  std::pair<StatusCode,double> >
{
 public:
  
  /** useful typedef for result
   *  - it is just a pair of status  and chi2 of the matching  
   */
  typedef std::pair<StatusCode,double>    MatchingPair;
  
  /** interface identification
   *  @return unique interface identifier 
   */
  static const InterfaceID& interfaceID() { return IID_ICaloTrackMatch ; }
  
  /** the main matching method  
   *
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @param chi2     returned value of chi2 of the matching
   *  @return status code for matching procedure 
   */
  virtual StatusCode match 
    ( const CaloPosition*   caloObj  , 
      const TrStoredTrack*  trObj    ,
      double&               chi2     ) = 0 ;
  
  /** The main matching method (Stl interface) 
   *  @param caloObj  pointer to "calorimeter" object (position)
   *  @param trObj    pointer to tracking object (track)
   *  @return pair of status code/chi2  for matching procedure 
   */
  virtual MatchingPair    operator() 
    ( const CaloPosition*   caloObj  , 
      const TrStoredTrack*  trObj    ) = 0 ;
  
 protected:
  
  /// destructor
  virtual ~ICaloTrackMatch(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOTRMATCH_H
// ============================================================================
