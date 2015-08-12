
//-----------------------------------------------------------------------------
/** @file RichTabulatedGasQuartzWindowAbs.h
 *
 *  Header file for tool : Rich::Rec::TabulatedGasQuartzWindowAbs
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H
#define RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// Detector Description
#include "DetDesc/DetectorElement.h"

// Event model
#include "Event/RichRecSegment.h"

// RichDet
#include "RichDet/DeRich1.h"
#include "RichDet/DeRich2.h"

// interfaces
#include "RichRecBase/IRichGasQuartzWindowAbs.h"

// VDT
#include "vdt/exp.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class TabulatedGasQuartzWindowAbs RichTabulatedGasQuartzWindowAbs.h
     *
     *  Tool to calculate transmission properties for the gas quartz window. Uses
     *  the tabulated information stored in the XML database.
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class TabulatedGasQuartzWindowAbs : public Rich::Rec::ToolBase,
                                        virtual public IGasQuartzWindowAbs
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      TabulatedGasQuartzWindowAbs( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent );

      /// Destructor
      virtual ~TabulatedGasQuartzWindowAbs();

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Rayleigh scattering probability for given pathlength (segment) and photon energy
      double photonTransProb( const LHCb::RichRecSegment * segment,
                              const double energy ) const;

    private: // Private data

      /// Pointers to RICHes
      std::vector<DeRich*> m_riches;

      /// Thickness of windows in each RICH
      std::vector<double> m_qWinZSize;

    };

  }
}

#endif // RICHRECTOOLS_RICHGASQUARTZWINDOWABS_H
