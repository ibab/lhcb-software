
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.h
 *
 *  Header file for tool : Rich::Rec::TabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.h,v 1.15 2008-08-18 19:40:59 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRichAerogelRadiator.h"

// interfaces
#include "RichRecBase/IRichRayleighScatter.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TabulatedRayleighScatter RichTabulatedRayleighScatter.h
     *
     *  Tool to calculate quantities related to Rayleigh scattering using
     *  tabulated quantities in the XML database.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo Figure out how to properly deal with multiple aerogel volumes
     */
    //-----------------------------------------------------------------------------

    class TabulatedRayleighScatter : public Rich::Rec::ToolBase,
                                     virtual public IRayleighScatter
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TabulatedRayleighScatter( const std::string& type,
                                const std::string& name,
                                const IInterface* parent );

      /// Destructor
      virtual ~TabulatedRayleighScatter() {};

    public: // methods (and doxygen comments) inherited from public interface

      // Rayleigh scattering probability for given pathlength (segment) and photon energy
      double photonScatteredProb( const LHCb::RichRecSegment * segment,
                                  const double energy ) const;

    };

  }
}

#endif // RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
