
//-----------------------------------------------------------------------------
/** @file RichSimplePhotonPredictor.h
 *
 *  Header file for tool : Rich::Rec::SimplePhotonPredictor
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHPHOTONPREDICTOR_H
#define RICHRECTOOLS_RICHPHOTONPREDICTOR_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event
#include "Event/RichRecPixel.h"
#include "Event/RichRecSegment.h"

// Interfaces
#include "RichRecBase/IRichPhotonPredictor.h"
#include "RichRecBase/IRichRecGeomTool.h"

// RichKernel
#include "RichKernel/RichPoissonEffFunctor.h"

// boost
#include "boost/format.hpp"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class SimplePhotonPredictor RichSimplePhotonPredictor.h
     *
     *  Tool which performs the association between RichRecTracks and
     *  RichRecPixels to form RichRecPhotons.
     *
     *  This particular implementation uses a simple fixed cut range per radiator,
     *  on the seperation between the pixel and ray-traced track impact point.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class SimplePhotonPredictor : public Rich::Rec::ToolBase,
                                  virtual public IPhotonPredictor
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      SimplePhotonPredictor( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

      /// Destructor
      virtual ~SimplePhotonPredictor(){}

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Is it possible to make a photon candidate using this segment and pixel.
      bool photonPossible( LHCb::RichRecSegment * segment,
                           LHCb::RichRecPixel * pixel ) const;

    private: // private data

      /// Geometry tool
      const IGeomTool * m_geomTool = nullptr;

      std::vector<double> m_minROI;  ///< Min hit radius of interest around track centres
      std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
      std::vector<double> m_maxROI2; ///< Square of m_maxROI
      std::vector<double> m_minROI2; ///< Square of m_minROI

      mutable std::vector<unsigned int> m_Nselected; ///< Number of selected combinations for each radiator
      mutable std::vector<unsigned int> m_Nreject;   ///< Number of rejected combinations for each radiator

    };

  }
}

#endif // RICHRECTOOLS_RICHPHOTONPREDICTOR_H
