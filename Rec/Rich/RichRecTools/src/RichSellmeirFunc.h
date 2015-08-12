
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.h
 *
 *  Header file for tool : Rich::Rec::SellmeirFunc
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHSELLMEIRFUNC_H
#define RICHRECTOOLS_RICHSELLMEIRFUNC_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichParticleIDType.h"
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/RichRecSegment.h"

// Detector Description
#include "RichDet/DeRich1.h"
#include "RichDet/DeRichAerogelRadiator.h"

// interfaces
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichKernel/IRichParticleProperties.h"

// VDT
#include "vdt/exp.h"
#include "vdt/log.h"

namespace Rich
{
  namespace Rec
  {

    //-----------------------------------------------------------------------------
    /** @class SellmeirFunc RichSellmeirFunc.h
     *
     *  Tool to calculate quantities using the Sellmeir function and related
     *  parameters.
     *
     *  Uses formulae 37-39 in CERN-EP/89-150  (Ypsilantis)
     *
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/03/2002
     */
    //-----------------------------------------------------------------------------

    class SellmeirFunc : public Rich::Rec::ToolBase,
                         virtual public ISellmeirFunc
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      SellmeirFunc( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

      /// Destructor
      virtual ~SellmeirFunc() {};

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Computes the number of photons emitted in the given energy range for a
      // given RichRecSegment under a certain mass hypothesis
      double photonsInEnergyRange( const LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id,
                                   const double botEn,
                                   const double topEn ) const;

    private: // methods

      /// Update the cached parameters
      StatusCode umsUpdate();

      /// internal calculation for photon yield
      double paraW( const Rich::RadiatorType rad, const double energy ) const;

    private: // data

      // Sellmeir parameters
      double m_selF1[Rich::NRadiatorTypes];
      double m_selF2[Rich::NRadiatorTypes];
      double m_selE1[Rich::NRadiatorTypes];
      double m_selE2[Rich::NRadiatorTypes];
      double m_molW[Rich::NRadiatorTypes];
      double m_rho[Rich::NRadiatorTypes];

      // cached parameters for speed
      double m_RXSPscale[Rich::NRadiatorTypes];
      double m_RXSMscale[Rich::NRadiatorTypes];
      double m_REP[Rich::NRadiatorTypes];
      double m_REM[Rich::NRadiatorTypes];
      double m_X[Rich::NRadiatorTypes];

      // particle hypothesis masses squared
      boost::array<double,Rich::NParticleTypes> m_particleMassSq;

      // Rich1 Detector element
      DeRich1 * m_Rich1DE;

      // particle properties
      const IParticleProperties * m_partProp;

    };

    inline double SellmeirFunc::paraW ( const Rich::RadiatorType rad,
                                        const double energy ) const
    {
      const double X = m_RXSPscale[rad] * (double)vdt::fast_logf( (float)((m_REP[rad]+energy)/(m_REP[rad]-energy)) );
      const double Y = m_RXSMscale[rad] * (double)vdt::fast_logf( (float)((m_REM[rad]+energy)/(m_REM[rad]-energy)) );
      return m_X[rad] * (X-Y);
    }

  }
}

#endif // RICHRECTOOLS_RICHSELLMEIRFUNC_H
