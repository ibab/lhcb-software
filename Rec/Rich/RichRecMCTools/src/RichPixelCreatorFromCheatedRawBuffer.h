
//--------------------------------------------------------------------------------------
/** @file RichPixelCreatorFromCheatedRawBuffer.h
 *
 *  Header file for RICH reconstruction tool : RichPixelCreatorFromCheatedRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromCheatedRawBuffer.h,v 1.2 2006-12-01 16:18:24 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//--------------------------------------------------------------------------------------

#ifndef RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H
#define RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H 1

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichMCTruthTool.h"

// Event
#include "Event/MCRichOpticalPhoton.h"

//--------------------------------------------------------------------------------------
/** @class RichPixelCreatorFromCheatedRawBuffer RichPixelCreatorFromCheatedRawBuffer.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *
 *  Uses RichSmartIDs from RawBuffer but then refers to the
 *  MCRichOpticalPhoton objects to use the true MC hit positions.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/09/2003
 *
 *  @todo Find a way to deal properly with more than one MCRichHit per RichSmartID
 */
//--------------------------------------------------------------------------------------

class RichPixelCreatorFromCheatedRawBuffer : public RichPixelCreatorBase
{

public: // methods for Gaudi framework

  /// Standard constructor
  RichPixelCreatorFromCheatedRawBuffer( const std::string& type,
                                        const std::string& name,
                                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromCheatedRawBuffer(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

protected: // methods

  /// Build a new RichRecPixel
  virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

private: // data

  /// MC Truth tool
  const IRichMCTruthTool * m_mcTool;

};


#endif // RICHRECMCTOOLS_RichPixelCreatorFromCheatedRawBuffer_H
