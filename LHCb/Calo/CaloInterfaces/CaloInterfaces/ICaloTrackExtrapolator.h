// $Id: ICaloTrackExtrapolator.h,v 1.3 2002-04-27 19:22:31 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.2  2001/11/08 19:50:01  ibelyaev
//  update in interfaces
//
// Revision 1.1.1.1  2001/11/02 16:53:13  ibelyaev
// New Package: the first release
//
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
 *  Auxillary interface to isolate the track extrapolation 
 *  business from calorimeter applications  
 *
 *  @author Dima Rusinov  Dmitri.Roussinov@cern.ch
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   01/11/2001
 */

class ICaloTrackExtrapolator: 
  virtual public IAlgTool , 
  public std::binary_function<double,TrState*,StatusCode>
{
 public:
  
  /** the uniqie interface identifier 
   *  @return the uniqie interface identifier 
   */
  static const InterfaceID& interfaceID()
    { return IID_ICaloTrackExtrapolator; }
  
  /** The main method 
   *  @param newZ position of  reference plane 
   *  @param state track state  ( ATTENTION! state IS MODIFIED!) 
   *  @return status code 
   */
  virtual StatusCode extrapolate 
    ( const double newZ  , 
      TrState*     state ) const = 0 ;
  
  /** The main method (functor interface)
   *  @param newZ position of  reference plane 
   *  @param state track state  ( ATTENTION! state IS MODIFIED!) 
   *  @return status code 
   */
  virtual StatusCode operator()
    ( const double newZ  , 
      TrState*     state ) const = 0 ;  
  
 protected:
  
  /// destructor (virtual and protected) 
  virtual ~ICaloTrackExtrapolator(){};
  
};

// ============================================================================
// The End 
// ============================================================================
#endif // CALOINTERFACES_ICALOTrackEXTRAPOLATOR_H
// ============================================================================
