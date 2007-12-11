
//---------------------------------------------------------------------------------
/** @file RichBaseTrSegMakerFromRecoTracks.h
 *
 *  Header file for tool : Rich::Rec::BaseTrSegMakerFromRecoTracks
 *
 *  CVS Log :-
 *  $Id: RichBaseTrSegMakerFromRecoTracks.h,v 1.1 2007-12-11 14:17:42 jonrob Exp $
 *
 *  @author Chris Jones    Christopher.Rob.Jones@cern.ch
 *  @date   14/01/2002
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECTRACKTOOLS_RichBaseTrSegMakerFromRecoTracks_H
#define RICHRECTRACKTOOLS_RichBaseTrSegMakerFromRecoTracks_H 1

// from Gaudi
#include "GaudiKernel/IToolSvc.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SystemOfUnits.h"

// base class and interface
#include "RichRecBase/RichRecToolBase.h"
#include "RichRecBase/IRichTrSegMaker.h"

// Event model
#include "Event/Track.h"
#include "Event/State.h"

// LHCbKernel
#include "Kernel/RichSmartID.h"

// RichDet
#include "RichDet/DeRich.h"
#include "RichDet/DeRichRadiator.h"
#include "RichDet/DeRichBeamPipe.h"

// GSL
#include "gsl/gsl_math.h"

namespace Rich
{
  namespace Rec
  {

    //---------------------------------------------------------------------------------
    /** @class BaseTrSegMakerFromRecoTracks RichBaseTrSegMakerFromRecoTracks.h
     *
     *  Base class to tools that create RichTrackSegments from Tracks.

     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   14/01/2002
     */
    //---------------------------------------------------------------------------------

    class BaseTrSegMakerFromRecoTracks : public Rich::Rec::ToolBase,
                                         virtual public ITrSegMaker
    {

    public: // Methods for Gaudi Framework

      /// Standard Constructor
      BaseTrSegMakerFromRecoTracks( const std::string& type,
                                    const std::string& name,
                                    const IInterface* parent );

      /// Standard Destructor
      virtual ~BaseTrSegMakerFromRecoTracks( );

      // Initialization of the tool after creation
      virtual StatusCode initialize();

    protected: // methods

      /// Access the DeRichBeamPipe objects, creating as needed on demand
      inline const DeRichBeamPipe* deBeam( const Rich::RadiatorType rad ) const
      {
        const Rich::DetectorType rich = ( Rich::Rich2Gas == rad ? Rich::Rich2 : Rich::Rich1 );
        if ( !m_deBeam[rich] )
        {
          m_deBeam[rich] = getDet<DeRichBeamPipe>( Rich::Rich1 == rich ?
                                                   DeRichBeamPipeLocation::Rich1BeamPipe :
                                                   DeRichBeamPipeLocation::Rich2BeamPipe );
        }
        return m_deBeam[rich];
      }

      /// Test flag to see if beam pipe intersections should be checked
      inline bool checkBeamPipe( const Rich::RadiatorType rad ) const
      {
        return m_checkBeamP[rad];
      }

      /// Test flag to see if a given radiator is in use
      inline bool usedRads( const Rich::RadiatorType rad ) const
      {
        return m_usedRads[rad];
      }

    private: // data

      /// RICH beampipe object for each radiator
      mutable std::vector<const DeRichBeamPipe*> m_deBeam;

      /// Check for beam pipe intersections in each radiator ?
      std::vector<bool> m_checkBeamP;

      /// Flags to turn on/off individual radiators
      std::vector<bool> m_usedRads;

    };

  }
}

#endif // RICHRECTRACKTOOLS_RichBaseTrSegMakerFromRecoTracks_H
