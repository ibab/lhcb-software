
//-----------------------------------------------------------------------------
/** @file RichTabulatedSignalDetectionEff.h
 *
 *  Header file for tool : Rich::Rec::TabulatedSignalDetectionEff
 *
 *  CVS Log :-
 *  $Id: RichTabulatedSignalDetectionEff.h,v 1.13 2007-03-10 13:19:20 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
#define RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H 1

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
    /** @class TabulatedSignalDetectionEff RichTabulatedSignalDetectionEff.h
     *
     *  Tool providing calculations for the photon detection efficiencies
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     *
     *  @todo Add support for different QE curves for each HPD
     *  @todo Use RICH2 nominal HPD when available
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

      // Finalize method
      StatusCode finalize();

    public: // methods (and doxygen comments) inherited from public interface

      // Returns the detection efficiency for a cherenkov photon of a given energy
      double photonDetEfficiency( LHCb::RichRecSegment * segment,
                                  const double energy ) const;

    private:  // Private data

      /// Pointers to RICHes
      std::vector<const DeRich*> m_riches;

      /// quartz window losses efficiency
      double m_quartzWinEff;

      /// Digitisation pedestal loss
      double m_pedLoss;

    };

  }
}

#endif // RICHRECTOOLS_RICHTABULATEDSIGNALDETECTIONEFF_H
