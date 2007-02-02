
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.h
 *
 *  Header file for tool : Rich::Rec::SellmeirFunc
 *
 *  CVS Log :-
 *  $Id: RichSellmeirFunc.h,v 1.12 2007-02-02 10:10:41 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RICHSELLMEIRFUNC_H
#define RICHRECTOOLS_RICHSELLMEIRFUNC_H 1

// base class
#include "RichRecBase/RichRecToolBase.h"

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichParticleIDType.h"
#include "RichKernel/BoostArray.h"

// Event model
#include "Event/RichRecSegment.h"

// Detector Description
#include "RichDet/DeRich1.h"

// interfaces
#include "RichRecBase/IRichSellmeirFunc.h"
#include "RichKernel/IRichParticleProperties.h"

//-----------------------------------------------------------------------------
/** @namespace Rich
 *
 *  General namespace for RICH software
 *
 *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
 *  @date   08/07/2004
 */
//-----------------------------------------------------------------------------
namespace Rich
{

  /** @namespace Rec
   *
   *  General namespace for RICH reconstruction software
   *
   *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
   *  @date   08/07/2004
   */
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
      double photonsInEnergyRange( LHCb::RichRecSegment * segment,
                                   const Rich::ParticleIDType id,
                                   const double botEn,
                                   const double topEn ) const;

    private: // methods

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

      // Aerogel specific parameters
      double m_waveIndepTrans;

      // cached parameters for speed
      double m_RXSPscale[Rich::NRadiatorTypes];
      double m_RXSMscale[Rich::NRadiatorTypes];
      double m_REP[Rich::NRadiatorTypes];
      double m_REM[Rich::NRadiatorTypes];
      double m_X[Rich::NRadiatorTypes];

      // particle hypothesis masses squared
      boost::array<double,Rich::NParticleTypes> m_particleMassSq;

    };

  }
}

#endif // RICHRECTOOLS_RICHSELLMEIRFUNC_H
