
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorBase.h
 *
 *  Header file for tool base class : RichPixelCreatorBase
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorBase.h,v 1.4 2005-06-23 15:13:05 jonrob Exp $
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
#include "RichKernel/IRichHPDInfoTool.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"
#include "RichKernel/RichHashMap.h"
#include "RichKernel/BoostArray.h"

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
 *  @todo Keep an eye on the restting of the iterators to default values. Need to define
 *        better where is the most appropriate place to do this.
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
  RichRecPixels * richPixels() const;

  // Access the begin iterator for the pixels in the given RICH detector
  RichRecPixels::iterator begin( const Rich::DetectorType rich ) const;

  // Access the end iterator for the pixels in the given RICH detector
  RichRecPixels::iterator end( const Rich::DetectorType rich ) const;

  // Access the begin iterator for the pixels in the given RICH detector
  RichRecPixels::iterator begin( const Rich::DetectorType rich,
                                 const Rich::Side         panel ) const;

  // Access the end iterator for the pixels in the given RICH detector
  RichRecPixels::iterator end( const Rich::DetectorType rich,
                               const Rich::Side         panel ) const;

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
  bool pixelIsOK( const RichSmartID id ) const;

  /** Save a given pixel to the TES container
   *
   *  @param pix Pointer to the RichRecPixel to save
   */
  void savePixel( RichRecPixel * pix ) const;

  /** Compute the average radiator distortion corrected positions
   *  in local HPD panel coordinate system, 
   *  for each valid radiator for the given pixel
   */
  void computeRadCorrLocalPositions( RichRecPixel * pixel ) const;

protected: // data

  /// Flag to signify all pixels have been formed
  mutable bool m_allDone;

  /// Map between RichSmartID and the associated RichRecPixel
  mutable RichHashMap< RichSmartID::KeyType, RichRecPixel* > m_pixelExists;

  /// Map indicating if a given RichSmartID has been considered already
  mutable RichHashMap< RichSmartID::KeyType, bool > m_pixelDone;

private: // data

  /// HPD information tool
  const IRichHPDInfoTool * m_hpdTool;

  /// Reconstruction geometry tool
  const IRichRecGeomTool * m_recGeom;

  /// Pointer to RichRecPixels
  mutable RichRecPixels * m_pixels;

  /// Flag to turn on or off the book keeping features to save cpu time.
  bool m_bookKeep;

  /// Flag to turn on or off the explicit checking of the HPD status
  bool m_hpdCheck;

  /// Flags for which RICH detectors to create pixels for
  std::vector<bool> m_usedDets;

  /// Location of RichRecPixels in TES
  std::string m_richRecPixelLocation;

  /// Begin iterators for various Rich and panel combinations
  mutable boost::multi_array<RichRecPixels::iterator,2> m_begins;

  /// End iterators for various Rich and panel combinations
  mutable boost::multi_array<RichRecPixels::iterator,2> m_ends;

  /// Begin iterators for each RICH
  mutable boost::array<RichRecPixels::iterator,Rich::NRiches> m_richBegin;

  /// End iterators for each RICH
  mutable boost::array<RichRecPixels::iterator,Rich::NRiches> m_richEnd;

  /// Hit count tally
  mutable boost::array<unsigned int, Rich::NRiches> m_hitCount;

  /// Event count
  unsigned int m_Nevts;

  /// Flag to indicate if the tool has been used in a given event
  mutable bool m_hasBeenCalled;

private: // methods

  /// Printout the pixel creation statistics
  void printStats() const;

private: // helper classes

  /// Class to sort the RichRecPixels according to detector regions
  class SortByRegion
    : std::binary_function<const RichRecPixel*,const RichRecPixel*,bool>
  {
  public:
    /// Method to return true if RichRecPixel p1 should be listed before p2
    inline bool operator() ( const RichRecPixel * p1,
                             const RichRecPixel * p2 ) const
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

inline bool RichPixelCreatorBase::pixelIsOK( const RichSmartID id ) const
{
  //const bool validID = id.pixelDataAreValid();
  //if ( !validID ) Warning("RichSmartID is invalid");
  return (
          //validID &&                 // RichSmartID is valid
          useDetector(id.rich()) &&  // This RICH is in use
          ( !m_hpdCheck || m_hpdTool->hpdIsActive(id) ) // If required, check HPD is alive
          );
}

inline void RichPixelCreatorBase::savePixel( RichRecPixel * pix ) const
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

}

inline void 
RichPixelCreatorBase::computeRadCorrLocalPositions( RichRecPixel * pixel ) const
{
  if ( Rich::Rich1 == pixel->detector() )
  {
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::Aerogel),Rich::Aerogel);
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::C4F10),Rich::C4F10);  
  } 
  else
  {
    pixel->setRadCorrLocalPosition(m_recGeom->correctAvRadiatorDistortion(pixel->localPosition(),Rich::CF4),Rich::CF4);
  } 
}

#endif // RICHRECBASE_RICHPIXELCREATORBASE_H
