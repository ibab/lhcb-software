
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : Rich::Rec::TabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.h,v 1.16 2009-07-30 11:23:56 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTabulatedSignalDetectionEff_H
#define RICHRECTOOLS_RichTabulatedSignalDetectionEff_H 1

// STL
//#include <memory>

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichRecBase/IRichRayTraceCherenkovCone.h"
#include "RichRecBase/IRichCherenkovAngle.h"

// Kernel
#include "Kernel/RichDetectorType.h"
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichMap.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichHPDPanel.h"
#include "RichDet/DeRichSphMirror.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TabulatedSignalDetectionEff RichTabulatedSignalDetectionEff.h
     *
     *  Tool providing calculations for the photon detection efficiencies.
     *
     *  Use the correct HPD Q.E. curves plus mirror segments for each track and
     *  mass hypothesis, using a weighted average technique.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class TabulatedSignalDetectionEff : public Rich::Rec::ToolBase,
                                        virtual public ISignalDetectionEff
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TabulatedSignalDetectionEff( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

      /// Destructor
      virtual ~TabulatedSignalDetectionEff() {};

      // Initialize method
      StatusCode initialize();

      // Implement the handle method for the Incident service.
      // This is used to inform the tool of software incidents.
      void handle( const Incident& incident );

    public: // methods (and doxygen comments) inherited from public interface

      // Returns the detection efficiency for a cherenkov photon of a given energy
      double photonDetEfficiency( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType hypo,
                                  const double energy ) const;

    private: // methods

      /** Returns the appropriate Rich HPD panel detector element name for 
       *  the given RICH and panel.
       *  @param[in] rich  The RICH detector type
       *  @param[in] panel The RICH panel
       *  @return The RICH panel name in the XML description
       */
      const std::string & pdPanelName( const Rich::DetectorType rich,
                                       const Rich::Side         panel ) const;

      /** Get the correct DeRichHPD object for the given RichSmartID
       *  @param[in] hpdID The RichSmartID for the HPD
       *  @return Pointer to the associated DeRichHPD object
       */
      inline const DeRichHPD * deHPD( const LHCb::RichSmartID hpdID ) const
      {
        //info() << "Getting HPD in " << hpdID.rich() << " " << hpdID.panel() << endmsg;
        return (m_hpdPanels[hpdID.rich()][hpdID.panel()])->deHPD(hpdID);
      }

      /// Get the local ring for the given segment and hypo
      const LHCb::RichRecRing * ckRing( LHCb::RichRecSegment * segment,
                                        const Rich::ParticleIDType hypo ) const;

    private: // definitions

      /// photodetector panels per rich
      typedef boost::array<const DeRichHPDPanel*, Rich::NHPDPanelsPerRICH> HPDPanelsPerRich;

      /// photodetector for each rich
      typedef boost::array<HPDPanelsPerRich, Rich::NRiches> RichHPDPanels;

    private: // Private data

      /// Cherenkov cone ray tracing tool
      const IRayTraceCherenkovCone * m_coneTrace;

      // Pointers to tool instances
      const ICherenkovAngle * m_ckAngle;

      /// Pointers to RICHes
      std::vector<const DeRich*> m_riches;

      /// Cached value storing product of quartz window eff. and digitisation pedestal loss
      double m_qEffPedLoss;

      /// HPD panels
      RichHPDPanels m_hpdPanels;

      /// JO flag to switch between simple or detail HPD description in ray tracing
      bool m_useDetailedHPDsForRayT;

      /// Cached trace modes for each radiator
      std::vector<LHCb::RichTraceMode> m_traceModeRad;

      /// Number of ring points per radiator
      std::vector<unsigned int> m_nPoints;

      mutable const LHCb::RichRecSegment * m_last_segment;
      mutable const LHCb::RichRecRing    * m_last_ring;
      mutable Rich::ParticleIDType m_last_hypo;

    };

  }
}

#endif // RICHRECTOOLS_RichTabulatedSignalDetectionEff_H
