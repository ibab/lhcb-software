// $Id: ICaloTrackExtrapolator.h,v 1.1.1.1 2001-11-02 16:53:13 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef CALOINTERFACES_ICALOTrackEXTRAPOLATOR_H 
#define CALOINTERFACES_ICALOTrackEXTRAPOLATOR_H 1
// Include files
// GaudiKernel
#include "GaudiKernel/IAlgTool.h" 
// CaloInterfaces
#include "CaloInterfaces/IIDICaloTrackExtrapolator.h"

// forward declaration
class TrState;  ///< from Tr/TrKernel package

/** @class ICaloTrackExtrapolator ICaloTrackExtrapolator.h 
 *            CaloInterfaces/ICaloTrackExtrapolator.h
 *  
 *  Auxilary interface to isolate the track extrapolation 
 *  business from calorimeter applications  
 *
 *  @author Dima Rusinov  Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class ICaloTrackExtrapolator: virtual public IAlgTool 
{
public:
  
  static const InterfaceID& interfaceID() 
  { return IID_ICaloTrackExtrapolator; }
  
  /** The main method 
   *  @param newZ position of  reference plane 
   *  @param state track state  ( ATTENTION! state IS MODIFIED!) 
   *  @return status code 
   */
  virtual StatusCode extrapolate ( const double newZ  , 
                                   TrState*     state ) = 0 ;

  /** destructor 
   */  
  virtual ~ICaloTrackExtrapolator(){}; 
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOTrackEXTRAPOLATOR_H
// ============================================================================
