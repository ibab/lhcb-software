//-----------------------------------------------------------------------------
/** @file RichStereoFitter.h
 *
 *  Header file for tool : Rich::Rec::StereoFitter
 *
 *  CVS Log :-
 *  $Id: RichStereoFitter.h,v 1.12 2009-01-20 15:49:30 cattanem Exp $
 *
 *  @author Luigi Delbuono  delbuono@in2p3.fr
 *  @date   27/06/2007
 */
//-----------------------------------------------------------------------------

// RichStereoFitter
#ifndef RICHRECSTEREOTOOLS_RICHSTEREOFITTER_H
#define RICHRECSTEREOTOOLS_RICHSTEREOFITTER_H 1

// from Gaudi
#include "GaudiKernel/PhysicalConstants.h"

// base classes
#include "RichRecBase/RichRecToolBase.h"

// Event model
#include "Event/RichRecSegment.h"
#include "Event/RichRecRing.h"

// interfaces
#include "RichRecBase/IRichStereoFitter.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichCherenkovResolution.h"
#include "RichRecBase/IRichStereoProjection.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichKernel/IRichParticleProperties.h"
#include "RichRecBase/IRichTrackEffectiveRefractiveIndex.h"
#include "RichRecBase/IRichPhotonSignal.h"
#include "RichKernel/IRichDetParameters.h"

// Kernel
#include "RichKernel/RichMap.h"
#include "Kernel/FPEGuard.h"

//Direct Root Math include
#include "Math/GenVector/VectorUtil.h" // For various basic vector routines  (angle, etc.)

//GSL math library
#include "gsl/gsl_sf_gamma.h"
#include "gsl/gsl_math.h"
#include "gsl/gsl_eigen.h"
#include "gsl/gsl_sf_erf.h"

// boost
#include "boost/numeric/conversion/bounds.hpp"
#include "boost/limits.hpp"
#include "boost/assign/list_of.hpp"

namespace Rich
{
  namespace Rec
  {

    // various useful constants
    namespace
    {
      static const int    s_NmassHyp(5);  ///< number of mass hypothesis
      static const int    s_nIterMax(10);
      static const double s_diffChi2Lim(0.5);  ///< delta chi2 min between iterations
      static const double s_MinThetaCerenkov(1e-8);
      static const double s_MinThetaCerenkov2(1e-6);
      static const double s_MinthetaExpRes(1e-4);    ///< Min theta experimental resolution
      static const double s_AsymptoticResDefault(0.55e-03);  ///< default (radians) asymptotic thetac resolution from TDR (with chromatics ???)
      static const int s_NmaxFitPhot(250);    ///< max number of photons used by the fit (i.e, in fit window)
    }

    //--------------------------------------------------------------------------------------
    /** @class StereoFitter IRichStereoFitter.h RichStereoFitter.h
     *
     *  Tool to perform a fit to a group of photons, as used by the Stereographic
     *  PID algorithm
     *
     *  @author Luigi Delbuono  delbuono@in2p3.fr
     *  @date   20/06/2007
     */
    //--------------------------------------------------------------------------------------

    class StereoFitter : public Rich::Rec::ToolBase,
                         virtual public IStereoFitter
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      StereoFitter( const std::string& type,
                    const std::string& name,
                    const IInterface* parent );

      /// Destructor
      virtual ~StereoFitter() {};

      // Initialize method
      StatusCode initialize();

    public: // methods (and doxygen comments) inherited from public interface

      // Project (RichRec) photons pertaining to one segment in the stereo
      // plane perpendicular to the segemnt direction
      virtual IStereoFitter::Result Fit( LHCb::RichRecSegment *richSegment,
                                         const IStereoFitter::Configuration & config ) const;

    private: // Private data

      //---------- parameters normally read in options files
      ///Geometric data: Z of Rich(s) entrance and exit window
      std::vector<double> m_zinWindowRich;
      std::vector<double> m_zoutWindowRich;
      ///Geometric data: Rich(s) interface and radiator thickness
      std::vector<double> m_thickInterface;
      std::vector<double> m_thickRadiator;
      ///Geometric data: indices, curvatures
      std::vector<double> m_curvX;   // curvature errors in xz and yz * momentum
      std::vector<double> m_curvY;   //

      std::vector<double> m_globGammmaNorm;   //global photon dispersion renormalization factors

      ///Asymptotic resolutions
      std::vector<double> m_asymptoticResFwrd ;
      std::vector<double> m_asymptoticResMatch;
      std::vector<double> m_asymptoticResPreDet;   //Seed or veloTT
      std::vector<double> m_asymptoticResKsTrk;
      double m_nSigmaCut;

      ///Scattering constant
      double m_scatt;

      //----------

      //Tools interfaces
      const IStereoProjection *m_stereoProj;      //Pointer to RichStereoProjection tool
      const ICherenkovAngle *m_ckAngleTool;
      const ICherenkovResolution *m_ckResTool;
      const IParticleProperties *m_richPartProp;
      const IGeomEff *m_geomEffic;                //Pointer to RichGeomEff tool
      const IPhotonSignal *m_photonSig;
      const ITrackEffectiveRefractiveIndex * m_refIndex;
      const IDetParameters * m_detParams;

      //fitter internal variables ('mutable' because of 'const' functions and object....)
      mutable double m_err_xp;
      mutable double m_err_yp;
      mutable double m_var_beta;
      mutable double m_var_index;
      mutable double m_var_nocor;
      mutable double m_RadiusGuess;
      mutable double m_radiusSquaredPid;

      mutable double m_err_x2;
      mutable double m_err_y2;
      mutable double m_err_xy;
      mutable double m_err_R;

      mutable double m_photonThcSigma;
      mutable double m_newErrorPerPhotonSelect;

      //Fit related variables
      mutable double m_invCov[s_NmaxFitPhot][s_NmaxFitPhot];
      mutable double m_mat[3][3];
      mutable double m_right[3];
      mutable double m_errPar[3][3];
      mutable double m_sol[3];
      mutable double m_chi2;
      mutable double m_chi2Exp;
      mutable double m_prob;
      mutable double m_probExp;
      mutable int    m_dof;
      mutable double m_Cerenkov;
      mutable double m_CerenkovErr;

      //keep previous iteration solution info (used in case of Chi2 increase)
      mutable double m_solPrev[3];
      mutable double m_chi2Prev;
      mutable double m_chi2ExpPrev;
      mutable double m_probPrev;
      mutable double m_probExpPrev;
      mutable int    m_dofPrev;
      mutable double m_CerenkovPrev;
      mutable double m_CerenkovErrPrev;
      mutable double m_photonThcSigmaPrev;


      //segment variables
      mutable Gaudi::XYZVector   m_segEntryDir;  ///< Stored segment entry direction
      mutable Gaudi::XYZVector   m_segMidDir;    ///< Stored segment mid point direction
      mutable Gaudi::XYZVector   m_segExitDir;   ///< Stored segment exit direction
      mutable Gaudi::XYZPoint    m_segEntryPtn;  ///< Stored segment entry point
      mutable Gaudi::XYZPoint    m_segMidPtn;    ///< Stored segment mid point point
      mutable Gaudi::XYZPoint    m_segExitPtn;   ///< Stored segment exit point

      //class global variables
      mutable double g_XCenterGuess, g_YCenterGuess;

      typedef std::pair<double,double> ThetaPhi;
      typedef Rich::Map< LHCb::RichRecPhoton*, ThetaPhi > ThetaPhiMap;
      /// map containing original photon information before updating
      mutable ThetaPhiMap m_origPhotInfo;

    private: // methods

      bool trkErrStereo(const LHCb::State *state,
                        LHCb::RichRecSegment *segment,
                        double &lengthEffect,
                        double &errMom,
                        double &err_tx2,
                        double &err_ty2,
                        double &trkCharge) const;


      double improvedErrorPerPhoton_index( const LHCb::RichRecSegment * segment,
                                           const double PTrk,
                                           const double lengthEffect,
                                           const double errMom,
                                           const double err_tx2,
                                           const double err_ty2,
                                           const double ckExp,
                                           const Rich::RadiatorType rad,
                                           const double asymptoticResSub,
                                           const Rich::ParticleIDType pidType ) const;


      bool goodPhotonTheta( LHCb::RichRecPhoton *photon, const Rich::ParticleIDType pidHypo,
                            double thcNominal, double thcPhotSig, double nthcPhotSigMax,
                            unsigned int ncandsPerPixMax, double maxBkgProb ) const;


      bool newCovarianceMatrix(LHCb::RichRecRing &recRing) const;
      bool solveChi2Equations(LHCb::RichRecRing &recRing) const;
      int transferFitSolution(LHCb::RichRecRing &recRing) const;

      //small utility functions
      void updateSegmentDirection(LHCb::RichRecSegment *richSegment, LHCb::RichRecPhoton::ConstVector &photons,
                                  double Xcenter, double Ycenter) const;
      void updateAllPhotonAngles(LHCb::RichRecSegment *richSegment, LHCb::RichRecPhoton::ConstVector &photons) const;
      void resetSegmentAndPhotons(LHCb::RichRecSegment *richSegment,
                                  const IStereoFitter::Configuration & config) const;
      double chiSquare(LHCb::RichRecRing &recRing, double radiusSquare) const;
      double Proba(double chi2,double ndl) const;

      double radiusFitted() const;
      double radiusFittedSquare() const;
      double XcenterFitted() const;
      double YcenterFitted() const;
      bool invert3x3Matrix(double mat[3][3], double invMat[3][3]) const;

    };

    inline double StereoFitter::XcenterFitted() const { return(m_sol[0]); }

    inline double StereoFitter::YcenterFitted() const { return(m_sol[1]); }

    inline double StereoFitter::radiusFittedSquare() const
    {
      const double result = m_sol[2]+m_sol[0]*m_sol[0]+m_sol[1]*m_sol[1];
      return ( result>0 ? result : 0 );
    }

    inline double StereoFitter::radiusFitted() const
    {
      return std::sqrt(radiusFittedSquare());
    }

  }
}

#endif // RICHRECSTEREOTOOLS_RICHSTEREOFITTER_H
