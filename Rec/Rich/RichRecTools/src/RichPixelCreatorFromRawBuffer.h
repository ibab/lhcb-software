
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorFromRawBuffer.h
 *
 *  Header file for tool : RichPixelCreatorFromRawBuffer
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorFromRawBuffer.h,v 1.4 2005-03-02 14:53:30 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H
#define RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H 1

// from Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/ToolFactory.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichKernel
#include "RichKernel/RichHashMap.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

/** @class RichPixelCreatorFromRawBuffer RichPixelCreatorFromRawBuffer.h
 *
 *  Tool for the creation and book-keeping of RichRecPixel objects.
 *  Works directly from RichSmartIDs decoded from the RawEvent
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   30/10/2004
 */

class RichPixelCreatorFromRawBuffer : public RichRecToolBase,
                                      virtual public IRichPixelCreator,
                                      virtual public IIncidentListener {

public: // Methods for Gaudi Framework

  /// Standard constructor
  RichPixelCreatorFromRawBuffer( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorFromRawBuffer(){}

  // Initialize method
  StatusCode initialize();

  // Finalize method
  StatusCode finalize();

  // Implement the handle method for the Incident service.
  // This is used to inform the tool of software incidents.
  void handle( const Incident& incident );

public: // methods (and doxygen comments) inherited from public interface

  // Returns a RichRecPixel object pointer for given ContainedObject.
  // If if it not possible NULL is return.
  RichRecPixel * newPixel( const ContainedObject * obj ) const;

  // Form all possible RichRecPixels from RawBuffer
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

  // Returns a pointer to the RichRecPixels
  RichRecPixels * richPixels() const;

private: // methods

  /// Initialise for a new event
  void InitNewEvent();

  /// Build a new RichRecPixel
  RichRecPixel * buildPixel ( const RichSmartID id ) const;

private: // data

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// Pointer to RichSmartID tool
  IRichSmartIDTool * m_idTool;

  /// Raw Buffer Decoding tool 
  IRichRawBufferToSmartIDsTool * m_decoder;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Pointer to pixel map
  mutable RichHashMap< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;
  mutable RichHashMap< RichSmartID::KeyType, bool > m_pixelDone;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

};

inline void RichPixelCreatorFromRawBuffer::InitNewEvent()
{
  // Initialise navigation data
  m_allDone = false;
  m_pixels  = 0;
  if ( m_bookKeep ) 
  {
    m_pixelExists.clear();
    m_pixelDone.clear();
  }
}

#endif // RICHRECTOOLS_RICHPIXELCREATORFROMRAWBUFFER_H
