
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : Rich::Rec::TabulatedSignalDetectionEff
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichTabulatedSignalDetectionEff_H
#define RICHRECTOOLS_RichTabulatedSignalDetectionEff_H 1

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"
#include "RichRecBase/IRichMassHypothesisRingCreator.h"

// Kernel
#include "Kernel/RichDetectorType.h"
#include "RichKernel/BoostArray.h"
#include "RichKernel/RichMap.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"
#include "RichDet/DeRichPDPanel.h"
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
                                        virtual public ISignalDetectionEff,
                                        virtual public IIncidentListener
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

      /** Get the correct DeRichPD object for the given RichSmartID
       *  @param[in] hpdID The RichSmartID for the PD
       *  @return Pointer to the associated DeRichPD object
       */
      inline const DeRichPD * dePD( const LHCb::RichSmartID pdID ) const
      {
        return (m_pdPanels[pdID.rich()][pdID.panel()])->dePD(pdID);
      }

    private: // definitions

      /// photodetector panels per rich
      typedef boost::array<const DeRichPDPanel*, Rich::NPDPanelsPerRICH> PDPanelsPerRich;

      /// photodetector for each rich
      typedef boost::array<PDPanelsPerRich, Rich::NRiches> RichPDPanels;

      /// Map for count by photon detector
      typedef Rich::Map<const LHCb::RichSmartID,unsigned int> PDCount;

      // Map for count by mirror segment
      typedef Rich::Map<const DeRichSphMirror *,unsigned int> MirrorCount;

    private: // Private data

      /// Pointer to ring creator
      const IMassHypothesisRingCreator * m_massHypoRings = nullptr;

      /// Pointers to RICHes
      std::vector<const DeRich*> m_riches;

      /// Cached value storing product of quartz window eff. and digitisation pedestal loss
      double m_qEffPedLoss{0};

      /// HPD panels
      RichPDPanels m_pdPanels;

      // PD count
      mutable unsigned int m_totalInPD{0};
      mutable PDCount m_pdCount;
      mutable MirrorCount m_primMirrCount, m_secMirrCount;

      // Cache pointer to last ring ring, which the cached PD count data refers to
      mutable const LHCb::RichRecRing * m_lastRing = nullptr;

    };

  }
}

#endif // RICHRECTOOLS_RichTabulatedSignalDetectionEff_H
