
//-----------------------------------------------------------------------------
/** @file RichTrackEffectiveRefractiveIndex.h
 *
 *  Header file for tool : Rich::Rec::TrackEffectiveRefractiveIndex
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   27/11/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichTrackEffectiveRefractiveIndex_H
#define RICHRECTRACKTOOLS_RichTrackEffectiveRefractiveIndex_H 1

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Base class and interfaces
#include "RichRecBase/RichRecToolBase.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
#include "RichKernel/IRichRefractiveIndex.h"

// RichDet
#include "RichDet/DeRichRadiator.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TrackEffectiveRefractiveIndex RichTrackEffectiveRefractiveIndex.h
     *
     *  Computes the effective refractive index for a given track, taking into
     *  account the precise details of the radaitor media each track traverses.
     *
     *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
     *  @date   27/11/2008
     *
     *  @todo Review if this tool is really needed or if the underlying refractive
     *        index tool can be used directly instead.
     */
    //-----------------------------------------------------------------------------

    class TrackEffectiveRefractiveIndex : public Rich::Rec::ToolBase,
                                          virtual public ITrackEffectiveRefractiveIndex
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TrackEffectiveRefractiveIndex( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent );

      /// Destructor
      virtual ~TrackEffectiveRefractiveIndex() { }

      // Initialization of the tool after creation
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from interface

      // Calculates the refractive index for the given track segment at a
      // given photon energy.
      virtual double refractiveIndex ( const LHCb::RichRecSegment * segment,
                                       const double energy ) const;

      // Calculates the average refractive index for track segment
      // for all visable photon energies.
      virtual double refractiveIndex ( const LHCb::RichRecSegment * segment ) const;

      // Calculates the refractive index R.M.S. for the radiator associated 
      // to the given track segment
      virtual double refractiveIndexRMS ( const LHCb::RichRecSegment * segment ) const;

      // Calculates the refractive index standard deviation for the radiator associated 
      // to the given track segment
      virtual double refractiveIndexSD  ( const LHCb::RichRecSegment * segment ) const;

    private: // Private data

      /// Pointer to general refractive index tool
      const IRefractiveIndex * m_refIndex;

    };

  }
}

#endif // RICHRECTRACKTOOLS_RichTrackEffectiveRefractiveIndex_H
