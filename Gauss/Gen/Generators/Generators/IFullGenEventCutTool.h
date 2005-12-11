// $Id: IFullGenEventCutTool.h,v 1.3 2005-12-11 23:21:47 robbep Exp $
#ifndef GENERATORS_IFULLGENEVENTCUTTOOL_H 
#define GENERATORS_IFULLGENEVENTCUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

class HepMCEvent ;
class HardInfo ;

/** @class IFullGenEventCutTool IFullGenEventCutTool.h "Generators/IFullGenEventCutTool.h"
 *  
 *  Abstract interface to generator level cut on full event. This type of
 *  cut is applied on a fully generated event: it includes pile-up interactions
 *  and all particles have already been decayed.
 * 
 *  @author Patrick Robbe
 *  @date   2005-11-17
 */

static const InterfaceID IID_IFullGenEventCutTool( "IFullGenEventCutTool" , 
                                                   1 , 0 ) ;

class IFullGenEventCutTool : virtual public IAlgTool {
public:
  /// Vector of HepMCEvent (LHCb event model class around HepMC::GenEvent)
  typedef std::vector< HepMCEvent * > EventVector ;

  /// Vector of hard process informations HardInfo
  typedef std::vector< HardInfo * >   HardVector  ;

  static const InterfaceID& interfaceID() { return IID_IFullGenEventCutTool ; }

  /** Apply the cut on a event.
   *  @param[in] theEventVector  Vector of all interactions in the event.
   *  @param[in] theHardVector   Vector of hard process informations of each 
   *                             pile-up interactions of the event.
   *  @return    true  if the full event passes the cut.
   */  
  virtual bool studyFullEvent( EventVector & theEventVector ,
                               HardVector & theHardVector ) const = 0 ;
};
#endif // GENERATORS_ICUTTOOL_H
