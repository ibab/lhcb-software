
//-----------------------------------------------------------------------------
/** @file RichTabulatedRayleighScatter.h
 *
 *  Header file for tool : Rich::Rec::TabulatedRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichTabulatedRayleighScatter.h,v 1.12 2007-02-02 10:10:42 jonrob Exp $
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

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
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

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Rayleigh scattering probability for given pathlength (segment) and photon energy
      double photonScatteredProb( const LHCb::RichRecSegment * segment,
                                  const double energy ) const;

    private:  // Private data

      /// Pointer to aerogel detector element
      const DeRichAerogelRadiator * m_aero;

    };

  }
}

#endif // RICHRECTOOLS_RICHTABULATEDRAYLEIGHSCATTER_H
