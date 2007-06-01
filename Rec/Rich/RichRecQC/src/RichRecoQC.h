
//---------------------------------------------------------------------------------
/** @file RichRecoQC.h
 *
 *  Header file for RICH reconstruction monitoring algorithm : Rich::Rec::MC::RecoQC
 *
 *  CVS Log :-
 *  $Id: RichRecoQC.h,v 1.21 2007-06-01 06:50:12 cattanem Exp $
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   2002-07-02
 */
//---------------------------------------------------------------------------------

#ifndef RICHRECQC_RICHRECOQC_H
#define RICHRECQC_RICHRECOQC_H 1

// base class
#include "RichRecBase/RichRecHistoAlgBase.h"

// from Gaudi
#include "AIDA/IHistogram1D.h"
#include "AIDA/IHistogram2D.h"

// Interfaces
#include "MCInterfaces/IRichRecMCTruthTool.h"

// temporary histogramming numbers
#include "RichRecBase/RichDetParams.h"

// interfaces
#include "RichRecBase/IRichTrackSelector.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"

// RichKernel
#include "RichKernel/RichStatDivFunctor.h"

// GaudiPI
//#include "AIDA_Proxy/AIDA_Proxy.h"

// boost
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {
    namespace MC
    {

      //---------------------------------------------------------------------------------
      /** @class RecoQC RichRecoQC.h
       *
       *  Quality control monitor for Rich Reconstruction
       *
       *  @author Chris Jones  Christopher.Rob.Jones@cern.ch
       *  @date   2002-07-02
       */
      //---------------------------------------------------------------------------------

      class RecoQC : public RichRecHistoAlgBase
      {

      public:

        /// Standard constructor
        RecoQC( const std::string& name, ISvcLocator* pSvcLocator );

        virtual ~RecoQC( ); ///< Destructor

        virtual StatusCode initialize();    // Algorithm initialization
        virtual StatusCode execute   ();    // Algorithm execution
        virtual StatusCode finalize  ();    // Algorithm finalization

      private: // methods

        /// Fit a Gaussian to the given distribution
        //AIDA::IFitResult * fitHisto( AIDA::IHistogram1D * histo,
        //                             const std::pair<double,double> & range,
        //                             const std::vector<double> & params );

        /// Access RichParticleProperties tool on demand
        const IParticleProperties * partPropTool() const
        {
          if ( !m_richPartProp ) { acquireTool( "RichParticleProperties", m_richPartProp ); }
          return m_richPartProp;
        }

        /// access RichCherenkovAngle tool on demand
        const ICherenkovAngle * ckAngleTool() const
        {
          if ( !m_ckAngle ) { acquireTool( "RichCherenkovAngle", m_ckAngle ); }
          return m_ckAngle;
        }

        /// access RichRecMCTruthTool tool on demand
        const Rich::Rec::MC::IMCTruthTool * richRecMCTool() const
        {
          if ( !m_richRecMCTruth ) { acquireTool( "RichRecMCTruthTool", m_richRecMCTruth ); }
          return m_richRecMCTruth;
        }

        /// access RichCherenkovResolution tool on demand
        const ICherenkovResolution * ckResTool() const
        {
          if ( !m_ckRes ) { acquireTool( "RichCherenkovResolution", m_ckRes ); }
          return m_ckRes;
        }

      private: // data

        // Pointers to tool instances
        mutable const IParticleProperties * m_richPartProp; ///< Rich Particle properties
        mutable const ICherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
        mutable const ICherenkovResolution * m_ckRes; ///< Cherenkov angle resolution tool
        mutable const Rich::Rec::MC::IMCTruthTool* m_richRecMCTruth;  ///< Pointer to RichRecMCTruthTool interface
        mutable const ITrackSelector * m_trSelector;  ///< Track selector

        // job options selection cuts
        std::vector<double> m_minBeta;           ///< minimum beta value for 'saturated' tracks
        std::vector<double> m_minP; ///< Min momentum per radiator
        std::vector<double> m_maxP; ///< Max momentum per radiator

        std::vector<unsigned int> m_truePhotCount; ///< Total number of true cherenkov photons per radiator
        std::vector<unsigned int> m_nSegs;         ///< Total number of track segments per radiator

        std::vector<double> m_chThetaRecHistoLimitMax; ///< Max theta limit for histos for each rad
        std::vector<double> m_chThetaRecHistoLimitMin; ///< Min theta limit for histos for each rad

      };

    }
  }
}

#endif // RICHRECQC_RICHRECOQC_H
