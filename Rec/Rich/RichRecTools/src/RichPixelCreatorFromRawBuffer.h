
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.h
 *
 *  Header file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.h,v 1.8 2006-01-23 14:20:44 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichPixelCreatorBase.h"

// interfaces
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

//---------------------------------------------------------------------------------
/** @class RichPixelCreatorFromRawBuffer RichPixelCreatorFromRawBuffer.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Works directly from RichSmartIDs decoded from the RawEvent
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

class RichPixelCreatorFromRawBuffer : public RichPixelCreatorBase
{

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPixelCreatorFromRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRawBuffer() {}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  LHCb::RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from RawBuffer
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

private: // methods

  /// Build a new RichRecPixel
  LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

private: // data

  /// Pointer to RichSmartID tool
  const IRichSmartIDTool * m_idTool;

  /// Raw Buffer Decoding tool
  const IRichRawBufferToSmartIDsTool * m_decoder;

};

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H
