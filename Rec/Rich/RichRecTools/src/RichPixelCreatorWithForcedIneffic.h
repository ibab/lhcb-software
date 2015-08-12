
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithForcedIneffic.h
 *
 *  Header file for tool : Rich::Rec::PixelCreatorWithForcedIneffic
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2006
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H
#define RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H 1

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
    /** @class PixelCreatorWithForcedIneffic RichPixelCreatorWithForcedIneffic.h
     *
     *  Tool for the creation and book-keeping of RichRecPixel objects.
     *
     *  Inherits from RichPixelCreatorFromRawBuffer, but in additional simulates
     *  additional inefficiencies by randomly removing a certain percentage of hits.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   23/11/2006
     */
    //---------------------------------------------------------------------------------

    class PixelCreatorWithForcedIneffic : public PixelCreatorFromRawBuffer
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      PixelCreatorWithForcedIneffic( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent );

      /// Destructor
      virtual ~PixelCreatorWithForcedIneffic() {}

      // Initialize method
      virtual StatusCode initialize();

      // Finalize method
      virtual StatusCode finalize();

    protected: // methods

      /// Build a new RichRecPixel from a Rich::HPDPixelCluster
      virtual LHCb::RichRecPixel * buildPixel ( const Rich::HPDPixelCluster& cluster ) const;

      /// Build a new RichRecPixel a single LHCb::RichSmartID
      virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID & id ) const;

    private:

      /// random number generator
      mutable Rndm::Numbers m_rndm;

      /// The fraction of pixels to reject
      double m_rejFrac[Rich::NRiches][Rich::NPDPanelsPerRICH];

    };

  }
}

#endif // RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H
