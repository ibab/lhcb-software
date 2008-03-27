
//---------------------------------------------------------------------------
/** @file RichRecBackgroundEstiClustering.h
 *
 *  Header file for algorithm class : Rich::Rec::BackgroundEstiClustering
 *
 *  CVS Log :-
 *  $Id: RichRecBackgroundEstiClustering.h,v 1.1 2008-03-27 11:06:01 jonrob Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/01/2003
 */
//---------------------------------------------------------------------------

#ifndef RICHRECALGORITHMS_RichRecBackgroundEstiClustering_H
#define RICHRECALGORITHMS_RichRecBackgroundEstiClustering_H 1

// Base class
#include "RichRecBackgroundEstiAvHPD.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------
    /** @class BackgroundEstiClustering RichRecBackgroundEstiClustering.h
     *
     *  Background estimation algorithm for RICH HPDs
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   10/01/2003
     */
    //---------------------------------------------------------------------------

    class BackgroundEstiClustering : public BackgroundEstiAvHPD
    {

    public:

      /// Standard constructor
      BackgroundEstiClustering( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

      virtual ~BackgroundEstiClustering(); ///< Destructor

   protected: // methods

      /// Compute the background term for each pixel
      virtual void pixelBackgrounds() const;

    private: // methods

      /// Compute the 'background weight' for the given pixel
      double backWeight( LHCb::RichRecPixel * pixel ) const;

    };

  }
}

#endif // RICHRECALGORITHMS_RichRecBackgroundEstiClustering_H
