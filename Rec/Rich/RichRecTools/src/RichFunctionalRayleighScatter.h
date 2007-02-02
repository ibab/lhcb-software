
//-----------------------------------------------------------------------------
/** @file RichFunctionalRayleighScatter.h
 *
 *  Header file for tool : Rich::Rec::FunctionalRayleighScatter
 *
 *  CVS Log :-
 *  $Id: RichFunctionalRayleighScatter.h,v 1.9 2007-02-02 10:10:40 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
#define RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// RichDet
#include "RichDet/DeRich1.h"

// Event model
#include "Event/RichRecSegment.h"

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

    //-----------------------------------------------------------------------------
    /** @class FunctionalRayleighScatter RichFunctionalRayleighScatter.h
     *
     *  Tool to calculate quantities related to Rayleigh scattering using an
     *  analytic functional form.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class FunctionalRayleighScatter : public Rich::Rec::ToolBase,
                                      virtual public IRayleighScatter
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      FunctionalRayleighScatter( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

      /// Destructor
      virtual ~FunctionalRayleighScatter() {};

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Rayleigh scattering probability for given pathlength (segment) and photyon energy
      double photonScatteredProb( const LHCb::RichRecSegment * segment,
                                  const double energy ) const;

    private:  // Private data

      // Rayleigh sattering parameters
      double m_eVToMicron;  ///< To converter between energy in eV to wavelenght in micrometers
      double m_AeroClarity; ///< The areogel clarity

    };

  }
}

#endif // RICHRECTOOLS_RICHFUNCTIONALRAYLEIGHSCATTER_H
