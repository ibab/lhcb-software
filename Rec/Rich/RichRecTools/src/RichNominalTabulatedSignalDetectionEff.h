
//-----------------------------------------------------------------------------
/** @file RichNominalTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : Rich::Rec::NominalTabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichNominalTabulatedSignalDetectionEff.h,v 1.1 2008-02-15 10:21:16 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichNominalTabulatedSignalDetectionEff_H
#define RICHRECTOOLS_RichNominalTabulatedSignalDetectionEff_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// interfaces
#include "RichRecBase/IRichSignalDetectionEff.h"

// Kernel
#include "Kernel/RichDetectorType.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class NominalTabulatedSignalDetectionEff RichNominalTabulatedSignalDetectionEff.h
     *
     *  Tool providing calculations for the photon detection efficiencies.
     *
     *  Uses the nominal HPD Q.E. curve and mirror reflectivities.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class NominalTabulatedSignalDetectionEff : public Rich::Rec::ToolBase,
                                               virtual public ISignalDetectionEff
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      NominalTabulatedSignalDetectionEff( const std::string& type,
                                          const std::string& name,
                                          const IInterface* parent );

      /// Destructor
      virtual ~NominalTabulatedSignalDetectionEff() {}

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Returns the detection efficiency for a cherenkov photon of a given energy
      double photonDetEfficiency( LHCb::RichRecSegment * segment,
                                  const Rich::ParticleIDType hypo,
                                  const double energy ) const;

    private:  // Private data

      /// Pointers to RICHes
      std::vector<const DeRich*> m_riches;

      /// Cached value storing product of quartz window eff. and digitisation pedestal loss
      double m_qEffPedLoss;

    };

  }
}

#endif // RICHRECTOOLS_RichNominalTabulatedSignalDetectionEff_H
