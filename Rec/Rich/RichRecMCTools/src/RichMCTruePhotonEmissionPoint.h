
//-----------------------------------------------------------------------------
/** @file RichMCTruePhotonEmissionPoint.h
 *
 *  Header file for tool : Rich::Rec::MC::TruePhotonEmissionPoint
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichMCTruePhotonEmissionPoint_H
#define RICHRECTOOLS_RichMCTruePhotonEmissionPoint_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichPhotonEmissionPoint.h"
#include "MCInterfaces/IRichRecMCTruthTool.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/MCRichOpticalPhoton.h"
#include "Event/MCRichSegment.h"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //-----------------------------------------------------------------------------
      /** @class TruePhotonEmissionPoint RichMCTruePhotonEmissionPoint.h
       *
       *  Tool providing the best emission point for a given track segment.
       *
       *  This implementations uses MC information to cheat to the true emission point. 
       *
       *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
       *  @date   15/03/2002
       */
      //-----------------------------------------------------------------------------

      class TruePhotonEmissionPoint : public Rich::Rec::ToolBase,
                                      virtual public IPhotonEmissionPoint
      {

      public: // Methods for Gaudi Framework

        /// Standard constructor
        TruePhotonEmissionPoint( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

        /// Destructor
        virtual ~TruePhotonEmissionPoint();

        // Initialize method
        StatusCode initialize();

      public: // methods (and doxygen comments) inherited from public interface

        // Get the best emission point for the given segment and pixel
        bool emissionPoint( const LHCb::RichRecSegment * segment,
                            const LHCb::RichRecPixel * pixel,
                            Gaudi::XYZPoint & emissPnt ) const;

        // Get the best emission point for the given segment and pixel, at the given fractional
        // distance along the radiator trajectory (0 for start point, 1 for end point)
        bool emissionPoint( const LHCb::RichRecSegment * segment,
                            const LHCb::RichRecPixel * pixel,
                            const double fracDist,
                            Gaudi::XYZPoint & emissPnt ) const;

      private: // methods

        /// Compute the best average MC emission point for a given segment
        void bestAvMCSegmentPoint( const LHCb::RichRecSegment * segment,
                                   Gaudi::XYZPoint & emissPnt ) const;

        /// Compute the best average MC emission point for a given segment
        void bestAvMCSegmentPoint( const LHCb::RichRecSegment * segment,
                                   const double fracDist,
                                   Gaudi::XYZPoint & emissPnt ) const;

      private: // Private data
        
        /// Rich Reconstruction MC Truth tool
        const Rich::Rec::MC::IMCTruthTool * m_mcRecTool = nullptr; 

        /** Option to enable to attempts to compute average MC emission
            for a given segment, when it is not possible to isolate a
            specific MC photon object */
        bool m_useAvMCSeg;

      };

    }
  }
}

#endif // RICHRECTOOLS_RichTruePhotonEmissionPoint_H
