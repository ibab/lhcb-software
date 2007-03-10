
//-----------------------------------------------------------------------------
/** @file RichGeomEffFixedValue.h
 *
 *  Header file for tool : Rich::Rec::GeomEffFixedValue
 *
 *  CVS Log :-
 *  $Id: RichGeomEffFixedValue.h,v 1.11 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
#define RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"

// interfaces
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/IRichRecGeomTool.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class GeomEffFixedValue RichGeomEffFixedValue.h
     *
     *  Tool to perform a fast determination of the geometrical efficiency for
     *  a given RichRecSegment and mass hypothesis.
     *
     *  Uses a fixed value of the signal and scattered efficiencies on the HPD panel,
     *  and also takes into account the HPD panel boundaries.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class GeomEffFixedValue : public Rich::Rec::ToolBase,
                              virtual public IGeomEff
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      GeomEffFixedValue( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~GeomEffFixedValue() {};

      // Initialize method
      StatusCode initialize();

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Obtain geometrical efficiency for this track and hypothesis
      double geomEfficiency ( LHCb::RichRecSegment * segment,
                              const Rich::ParticleIDType id ) const;

      // Obtain scattered geometrical efficiency for this track and hypothesis
      double geomEfficiencyScat ( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType id ) const;

    private: // Private data

      // Pointers to tool instances
      const ICherenkovAngle * m_ckAngle;   ///< Cherenkov angle tool
      const IGeomTool * m_geomTool;     ///< Pointer to the Geometry tool

      std::vector<double> m_fixedValue;  ///< vector of fixed radiator geometrical efficiencies
      double m_fixedScatValue;           ///< vector of fixed radiator geometrical scatter efficiency

      /// Flag to turn on checking of HPD panel boundaries
      bool m_checkBoundaries;

    };

  }
}

#endif // RICHRECTOOLS_RICHGEOMEFFFIXEDVALUE_H
