
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.h
 *
 *  Header file for tool base class : RichPixelCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorBase.h,v 1.10 2006-06-14 22:04:02 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECBASE_RICHPIXELCREATORBASE_H
#define RICHRECBASE_RICHPIXELCREATORBASE_H 1

// STL
#include <functional>

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPixelCreator.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichKernel/IRichPixelSuppressionTool.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/IRichRawBufferToSmartIDsTool.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichMap.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/BoostArray.h"

// RichDet
#include "RichDet/DeRichSystem.h"

// Boost
#include "boost/multi_array.hpp"

//---------------------------------------------------------------------------------------
/** @class RichPixelCreatorBase RichPixelCreatorBase.h RichRecBase/RichPixelCreatorBase.h
 *
 *  Base class for all RichRecPixel creator tools.
 *
 *  Implements common functionaility needed by all concrete implementations.
 *  Derived classes must implement the methods newPixels and buildPixel using
 *  whatever means appropriate for that implementation.
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   20/04/2005
 *
 *  @todo Find a better way to handle the filling of the pixel iterators, that avoids a
 *        seperate loop and if possible maps
 */
//---------------------------------------------------------------------------------------

class RichPixelCreatorBase : public RichRecToolBase,
                             virtual public IRichPixelCreator,
                             virtual public IIncidentListener
{

public:

  /// Standard constructor
  RichPixelCreatorBase( const std::string& type,
                        const std::string& name,
                        const IInterface* parent );

  /// Destructor
  virtual ~RichPixelCreatorBase( ) {};

  // Initialize method
  virtual StatusCode initialize();

  // Finalize method
  virtual StatusCode finalize();

  // Implement the handle method for the Incident service.
  // This is used to inform the tool of software incidents.
  virtual void handle( const Incident& incident );

public: // methods from interface

  // Returns a pointer to the RichRecPixels
  LHCb::RichRecPixels * richPixels() const;

  // Access the begin iterator for the pixels in the given RICH detector
  LHCb::RichRecPixels::iterator begin( const Rich::DetectorType rich ) const;

  // Access the end iterator for the pixels in the given RICH detector
  LHCb::RichRecPixels::iterator end( const Rich::DetectorType rich ) const;

  // Access the begin iterator for the pixels in the given RICH detector
  LHCb::RichRecPixels::iterator begin( const Rich::DetectorType rich,
                                       const Rich::Side         panel ) const;

  // Access the end iterator for the pixels in the given RICH detector
  LHCb::RichRecPixels::iterator end( const Rich::DetectorType rich,
                                     const Rich::Side         panel ) const;

  // Access the begin iterator for the pixels in the given RICH HPD
  LHCb::RichRecPixels::iterator begin( const LHCb::RichSmartID hpdID ) const;

  // Access end begin iterator for the pixels in the given RICH HPD
  LHCb::RichRecPixels::iterator end( const LHCb::RichSmartID hpdID ) const;

  // Form all possible RichRecPixels from RawBuffer
  // The most efficient way to make all RichRecPixel objects in the event.
  StatusCode newPixels() const;

protected: // methods

  /// Initialise for a new event
  virtual void InitNewEvent();

  /// Finalise current event
  virtual void FinishEvent();

  /// Read the pixels and fill the RICH and panel begin and end iterators
  void fillIterators() const;

  /// Reset all iterators to default values
  void resetIterators() const;

  /// Sort the RichRecPixel container into detector regions
  void sortPixels() const;

  /// Access the final RichRecPixel location in the TES
  const std::string & pixelLocation() const;

  /// Is book keeping to be performed
  bool bookKeep() const;

  /** Check if a given RICH detector is to be used
   *
   *  @param rich The RICH detector type
   *
   *  @return boolean indicating if the given RICH detector is active
   *  @retval true  RICH detector is active
   *  @retval false RICH detector is not in use
   */
  bool useDetector( const Rich::DetectorType rich ) const;

  /** Check the status of the given RICH channel (RichSmartID)
   *
   *  @param id The RichSmartID to check
   *
   *  @return boolean indicating if the given channel is active
   *  @retval true  channel is active
   *  @retval false channel is not in use
   */
  bool pixelIsOK( const LHCb::RichSmartID id ) const;

  /** Save a given pixel to the TES container
   *
   *  @param pix Pointer to the RichRecPixel to save
   */
  void savePixel( LHCb::RichRecPixel * pix ) const;

  /** Compute the average radiator distortion corrected positions
   *  in local HPD panel coordinate system,
   *  for each valid radiator for the given pixel
   */
  void computeRadCorrLocalPositions( LHCb::RichRecPixel * pixel ) const;

  /** Apply HPD pixel suppression, if configured to do so
   *
   *  @param hpdID    RichSmartID for HPD
   *  @param smartIDs Vector of pixel smartIDs for this HPD
   *
   *  @return boolean indicating if any suppression occured
   *  @retval true Some (or all) pixels have been suppressed. In the case the vector of
   *               pixel RichSmartIDs is changed
   *  @retval false No pixels are suppressed
   */
  bool applyPixelSuppression( const LHCb::RichSmartID hpdID,
                              LHCb::RichSmartID::Vector & smartIDs ) const;

  /// Build a new RichRecPixel
  LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

  /// Access the RichSmartIDTool
  inline const IRichSmartIDTool * smartIDTool() const
  {
    if (!m_idTool) { acquireTool( "RichSmartIDTool",    m_idTool,  0, true ); }
    return m_idTool;
  }

  /// Access the RichSmartIDDecoder
  inline const IRichRawBufferToSmartIDsTool * smartIDdecoder() const
  {
    if (!m_decoder) { acquireTool( "RichSmartIDDecoder", m_decoder, 0, true ); }
    return m_decoder;
  }

protected: // data

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Map between RichSmartID and the associated RichRecPixel
  mutable Rich::HashMap< LHCb::RichSmartID::KeyType, LHCb::RichRecPixel* > m_pixelExists;

  /// Map indicating if a given RichSmartID has been considered already
  mutable Rich::HashMap< LHCb::RichSmartID::KeyType, bool > m_pixelDone;

private: // data

  /// Pointer to RICH system detector element
  const DeRichSystem * m_richSys;

  /// Reconstruction geometry tool
  const IRichRecGeomTool * m_recGeom;

  /// HPD occupancy tool
  const IRichPixelSuppressionTool * m_hpdOcc;

  /// Pointer to RichSmartID tool
  mutable const IRichSmartIDTool * m_idTool;

  /// Raw Buffer Decoding tool
  mutable const IRichRawBufferToSmartIDsTool * m_decoder;

  /// Pointer to RichRecPixels
  mutable LHCb::RichRecPixels * m_pixels;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Flag to turn on or off the explicit checking of the HPD status
  bool m_hpdCheck;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Begin iterators for various Rich and panel combinations
  mutable boost::multi_array<LHCb::RichRecPixels::iterator,2> m_begins;

  /// End iterators for various Rich and panel combinations
  mutable boost::multi_array<LHCb::RichRecPixels::iterator,2> m_ends;

  /// Begin iterators for each RICH
  mutable boost::array<LHCb::RichRecPixels::iterator,Rich::NRiches> m_richBegin;

  /// End iterators for each RICH
  mutable boost::array<LHCb::RichRecPixels::iterator,Rich::NRiches> m_richEnd;

  typedef Rich::Map<const RichSmartID,LHCb::RichRecPixels::iterator> HPDItMap;

  /// Begin iterators for each HPD
  mutable HPDItMap m_hpdBegin;

  /// End iterators for each HPD
  mutable HPDItMap m_hpdEnd;

  /// Hit count tally
  mutable boost::array<unsigned int, Rich::NRiches> m_hitCount;

  /// Suppressed hit count tally
  mutable boost::array<unsigned int, Rich::NRiches> m_suppressedHitCount;

  /// Event count
  unsigned int m_Nevts;

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

  /// Should HPD occupancy be monitored
  bool m_moniHPDOcc;

private: // methods

  /// Printout the pixel creation statistics
  void printStats() const;

private: // helper classes

  /// Class to sort the RichRecPixels according to detector regions
  class SortByRegion
    : std::binary_function<const LHCb::RichRecPixel*,const LHCb::RichRecPixel*,bool>
  {
  public:
    /// Method to return true if RichRecPixel p1 should be listed before p2
    inline bool operator() ( const LHCb::RichRecPixel * p1,
                             const LHCb::RichRecPixel * p2 ) const
    {
      return ( p1->smartID().dataBitsOnly().key() < p2->smartID().dataBitsOnly().key() );
    }
  };

};

inline void RichPixelCreatorBase::sortPixels() const
{
  std::sort( richPixels()->begin(), richPixels()->end(), SortByRegion() );
}

inline const std::string & RichPixelCreatorBase::pixelLocation() const
{
  return m_richRecPixelLocation;
}

inline bool RichPixelCreatorBase::bookKeep() const
{
  return m_bookKeep;
}

inline bool RichPixelCreatorBase::useDetector( const Rich::DetectorType rich ) const
{
  return m_usedDets[rich];
}

inline bool RichPixelCreatorBase::pixelIsOK( const LHCb::RichSmartID id ) const
{
  return (
          //validID &&                 // RichSmartID is valid
          useDetector(id.rich()) &&  // This RICH is in use
          ( !m_hpdCheck || m_richSys->hpdIsActive(id) ) // If required, check HPD is alive
          );
}

inline void RichPixelCreatorBase::savePixel( LHCb::RichRecPixel * pix ) const
{
  richPixels()->insert( pix );
  ++m_hitCount[pix->smartID().rich()];
  m_hasBeenCalled = true;
}

inline void RichPixelCreatorBase::resetIterators() const
{

  // RICH and panels
  m_begins[Rich::Rich1][Rich::top]    = richPixels()->begin();
  m_begins[Rich::Rich1][Rich::bottom] = richPixels()->begin();
  m_begins[Rich::Rich2][Rich::left]   = richPixels()->begin();
  m_begins[Rich::Rich2][Rich::right]  = richPixels()->begin();
  m_ends[Rich::Rich1][Rich::top]      = richPixels()->begin();
  m_ends[Rich::Rich1][Rich::bottom]   = richPixels()->begin();
  m_ends[Rich::Rich2][Rich::left]     = richPixels()->begin();
  m_ends[Rich::Rich2][Rich::right]    = richPixels()->begin();

  // RICH only
  m_richBegin[Rich::Rich1] = richPixels()->begin();
  m_richBegin[Rich::Rich2] = richPixels()->begin();
  m_richEnd[Rich::Rich1]   = richPixels()->begin();
  m_richEnd[Rich::Rich2]   = richPixels()->begin();

  // HPD
  m_hpdBegin.clear();
  m_hpdEnd.clear();

}

inline void
RichPixelCreatorBase::computeRadCorrLocalPositions( LHCb::RichRecPixel * pixel ) const
{
  if ( Rich::Rich1 == pixel->detector() )
  {
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::Aerogel),Rich::Aerogel);
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::Rich1Gas),Rich::Rich1Gas);
  }
  else
  {
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::Rich2Gas),Rich::Rich2Gas);
  }
}

inline bool
RichPixelCreatorBase::applyPixelSuppression( const LHCb::RichSmartID hpdID,
                                             LHCb::RichSmartID::Vector & smartIDs ) const
{
  const unsigned int startSize = smartIDs.size();
  const bool suppressed =
    ( !m_moniHPDOcc ? false : m_hpdOcc->applyPixelSuppression(hpdID,smartIDs) );
  m_suppressedHitCount[hpdID.rich()] += (startSize-smartIDs.size());
  return suppressed;
}

#endif // RICHRECBASE_RICHPIXELCREATORBASE_H
