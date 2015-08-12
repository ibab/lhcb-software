
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithPanelFlip.h
 *
 *  Header file for tool : Rich::Rec::PixelCreatorWithPanelFlip
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   18/02/2011
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPixelCreatorWithPanelFlip_H
#define RICHRECTOOLS_RichPixelCreatorWithPanelFlip_H 1

// from Gaudi
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// base class
#include "RichPixelCreatorFromRawBuffer.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class PixelCreatorWithPanelFlip RichPixelCreatorWithPanelFlip.h
     *
     *  Tool for the creation and book-keeping of RichRecPixel objects.
     *
     *  Inherits from RichPixelCreatorFromRawBuffer, but flips the HPD panel
     *  information ( So left <-> right in RICH2, up <-> down in RICH1 )
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   18/02/2011
     */
    //---------------------------------------------------------------------------------

    class PixelCreatorWithPanelFlip : public PixelCreatorFromRawBuffer
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PixelCreatorWithPanelFlip( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      /// Destructor
      virtual ~PixelCreatorWithPanelFlip() {}

      // Initialize method
      virtual StatusCode initialize();

    protected: // methods

      /// Build a new RichRecPixel from a Rich::HPDPixelCluster
      virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

      /// Build a new RichRecPixel a single LHCb::RichSmartID
      virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID & id ) const;

    private: // methods

      /// Flip the given RichSmartID
      LHCb::RichSmartID flip( const LHCb::RichSmartID & id ) const;

    private: // data

      /// Flags to turn on the flipping for RICH1 and RICH2
      std::vector<bool> m_flipRICH;

    };

  }
}

#endif // RICHRECTOOLS_RichPixelCreatorWithPanelFlip_H
