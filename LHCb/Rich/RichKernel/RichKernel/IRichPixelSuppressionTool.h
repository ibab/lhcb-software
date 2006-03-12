
//-----------------------------------------------------------------------------
/** @file IRichPixelSuppressionTool.h
 *
 *  Header file for tool interface : IRichPixelSuppressionTool
 *
 *  CVS Log :-
 *  $Id: IRichPixelSuppressionTool.h,v 1.2 2006-03-12 16:12:37 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2003-07-31
 */
//-----------------------------------------------------------------------------

#ifndef RICHKERNEL_IRichPixelSuppressionTool_H
#define RICHKERNEL_IRichPixelSuppressionTool_H 1

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "Kernel/RichSmartID.h"

/// Static Interface Identification
static const InterfaceID IID_IRichPixelSuppressionTool( "IRichPixelSuppressionTool", 1, 0 );

//-----------------------------------------------------------------------------
/** @class IRichPixelSuppressionTool IRichPixelSuppressionTool.h RichKernel/IRichPixelSuppressionTool.h
 *
 *  Interface for tool for apply suppression to HPD pixels
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

class IRichPixelSuppressionTool : public virtual IAlgTool
{

public:

  /** static interface identification
   *  @return unique interface identifier
   */
  static const InterfaceID& interfaceID() { return IID_IRichPixelSuppressionTool; }

  /** Applies pixel suppression to the given HPD
   *
   *  @param hpd HPD identifier
   *  @param smartIDs reference to vector of pixel RichSmartIDs for the given HPD
   *
   *  @return Boolean indicating if pixels have been suppressed or not
   *  @retval true Some (or all) pixels have been suppressed. In the case the vector of
   *               pixel RichSmartIDs is changed
   *  @retval false No pixels are suppressed
   */
  virtual bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                      LHCb::RichSmartID::Vector & smartIDs ) const = 0;

};

#endif // RICHKERNEL_IRichHighOccHPDSuppressionTool_H
