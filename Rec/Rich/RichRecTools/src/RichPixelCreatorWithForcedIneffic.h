
//---------------------------------------------------------------------------------
/** @file RichPixelCreatorWithForcedIneffic.h
 *
 *  Header file for tool : Rich::Rec::PixelCreatorWithForcedIneffic
 *
 *  CVS Log :-
 *  $Id: RichPixelCreatorWithForcedIneffic.h,v 1.3 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   23/11/2006
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H
#define RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H 1

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/RndmGenerators.h"
#include "GaudiKernel/IRndmGenSvc.h"

// base class
#include "RichPixelCreatorFromRawBuffer.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  //-----------------------------------------------------------------------------
  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
  //-----------------------------------------------------------------------------
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

      /// Build a new RichRecPixel
      virtual LHCb::RichRecPixel * buildPixel ( const LHCb::RichSmartID id ) const;

    private:

      /// random number generator
      mutable Rndm::Numbers m_rndm;

      /// The fraction of pixels to reject
      double m_rejFrac[Rich::NRiches][Rich::NHPDPanelsPerRICH];

    };

  }
}

#endif // RICHRECTOOLS_RichPixelCreatorWithForcedIneffic_H
