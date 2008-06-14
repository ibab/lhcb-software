
#ifndef LESTER_RichDataModel_H
#define LESTER_RichDataModel_H

// includes
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include "Rich/EventDescription.fwd"
#include "Rich/Data.fwd"
#include "Rich/CircleParams.h"
#include "Rich/Hit.fwd"
#include "Rich/Small2Vector.fwd"
#include "Rich/FiniteRelativeProbabilityChooser.h"
#include "Rich/RegularProbabilityDistribution1D.h"
#include "Rich/ProbabilityAbove.h"
#include "Rich/ProbabilityUtils.h"
#include "Rich/CircleTheorems.h"

namespace Lester
{

  // declaration
  class RichDataModel
  {

  public:

    /// Default constructor (reads cache file)
    RichDataModel()
      : m_enableFileCache(true),
        m_richThetaSampler(NULL),
        m_richThetaDistribution(NULL),
        m_richProbabilityThetaAbove(NULL)
    { m_cache.clear(); }

    /// Destructor
    ~RichDataModel() { cleanUp(); }

    double totalLogProbOfEventDescriptionGivenData(const EventDescription & rp,
                                                   const Data & d) const;
    double totalLogProbOfDataGivenEventDescription(const Data & d,
                                                   const EventDescription & rp) const;

    int sampleFromNumberOfCirclesDistribution() const;
    double priorProbabilityOfNumberOfCircles(const int n) const;

    int sampleFromNumberOfHitsDueToCircle(const CircleParams & cp) const;
    // probability counterpart not yet implemented

    int sampleFromNumberOfHitsDueToBackground() const;
    // probability counterpart not yet implemented

    Hit sampleHitDueToBackground() const;
    double priorProbabilityOfHitDueToBackground(const Small2Vector & p) const;

    Hit sampleHitDueToCircle(const CircleParams & c) const;
    double priorProbabilityOfHitDueToCircle(const Small2Vector & p,
                                            const CircleParams & cp) const;

    // sampling counterpart not yet implemented
    double
    singleHitProbabilityDistributionGivenEventDescription(const Hit & h,
                                                          const EventDescription & ed) const;

    /// assume number of hits appearing on ring is poisson.  Determine the mean of that poisson dist, given a CircleParams.
    double meanNumberOfHitsMuExpectedOn(const CircleParams & cp) const;
    double meanNumberOfHitsMuExpectedFromBg() const;
    double meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed) const;
    double meanNumberOfHitsMuExpectedGiven(const EventDescription & ed) const;
    CircleParams sampleCircle() const;
    double priorProbabilityOf(const CircleParams & cp) const;

    class SampleIsImpossible { };
    double sampleFromCircleRadiusDistribution() const;
    double priorProbabilityOfRadius(const double r) const;
    double sampleFromCircleRadiusDistributionAbove(const double r) const;
    double priorProbabilityOfRadiusAbove(const double r) const;

    double sampleFromApproximateCoPointSeparationDistribution() const;

  protected:

    /// Setup the parameters for each concrete implementation
    void initialise();

  private:

    typedef std::map<double,double> CacheMap;
    mutable CacheMap m_cache;
    std::string m_cacheLocation;
    mutable bool m_enableFileCache;

  public:

    /// Set the option to turn on/off the file cache
    inline void enableFileCache( const bool enable = true )
    {
      m_enableFileCache = enable;
    }

  private:

    /// Read the cache fill into the internal map
    void readCacheFromFile();

    /// Compute and fill cache
    double fillCache(const double deltaOnTwo) const;

    double approxCoPointSepFunctionPart2(const double deltaOnTwo, const double rSq) const;

    double approxCoPointSepFunctionPart1(const double deltaOnTwo) const;

    std::string getCacheLocation() const;

    void cleanUp();

    std::string dataFilesDir();

  public:

    double priorProbabilityOfThreePointsBeingOnACircle
    (const Small2Vector & a,
     const Small2Vector & b,
     const Small2Vector & c) const;

    double priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius
    (const Small2Vector & a,
     const Small2Vector & b,
     const Small2Vector & c,
     const double r) const;

  public:

    double PROPTO_priorProbabilityOfTwoPointsBeingOnACircle
    (const Small2Vector & a,
     const Small2Vector & b) const;

    Small2Vector sampleFromCircleCentreDistribution() const;
    double priorProbabilityOfCentre(const Small2Vector & cent) const;

    EventDescription sampleAnEventDescriptionFromPrior() const;
    double priorProbabilityOf(const EventDescription & ed) const;

  protected:

    /// Name of theta distribution data file
    std::string m_thetaDataFile;

    /// Cache file name
    std::string m_cacheDataFile;

  private:

    typedef FiniteRelativeProbabilityChooser<double> RichThetaSampler;
    RichThetaSampler * m_richThetaSampler;
    inline const RichThetaSampler & richThetaSampler() const
    { return *m_richThetaSampler; }

    typedef RegularProbabilityDistribution1D<double> RichThetaDistribution;
    RichThetaDistribution * m_richThetaDistribution;
    inline const RichThetaDistribution & richThetaDistribution() const
    { return *m_richThetaDistribution; }

    typedef ProbabilityAbove<double>                 RichProbabilityThetaAbove;
    RichProbabilityThetaAbove * m_richProbabilityThetaAbove;
    inline const RichProbabilityThetaAbove & richProbabilityThetaAbove() const
    { return *m_richProbabilityThetaAbove; }

  protected: // constants

    double backgroundRadius;
    int    meanNumberOfRings;
    double circleHitsPerUnitLengthParameter;
    double circleRadiusSigmaAboutMean;
    double circleCenXMean;
    double circleCenYMean;
    double circleCenXSig;
    double circleCenYSig;
    double circleProbabilityDistributionAlphaParameter;
    double circleProbabilityDistributionEpsilonParameter;
    double areaScaleForSignal;
    double areaScaleForEverything;
    double backgroundMeanParameter; ///< should be the poisson mean number of hits from BG given nothing else, but the whole treatment of BG hits should change in the near future.

  protected:

    static const double s_max_exponent;
    static const double s_min_exponent;

  public:

    double circleMeanRadiusParameter; /// should be the mean radius of rich-rings.  Is often used also to set a length scale for CPQuantization.

  };

  inline double RichDataModel::sampleFromCircleRadiusDistribution() const
  {
    return richThetaSampler().sampleAnIndex();
  }

  inline double RichDataModel::priorProbabilityOfRadius(const double r) const
  {
    return richThetaDistribution()(r);
  }

  inline double RichDataModel::sampleFromCircleRadiusDistributionAbove(const double r) const
  {
    try
    {
      return richThetaSampler().sampleAnIndexAbove(r);
    }
    catch ( RichThetaSampler::NoSamplesPossible & )
    {
      throw SampleIsImpossible();
    }
  }

  inline double RichDataModel::priorProbabilityOfRadiusAbove(const double r) const
  {
    return richProbabilityThetaAbove()(r);
  }

  inline double RichDataModel::approxCoPointSepFunctionPart2(const double deltaOnTwo,
                                                             const double rSq) const
  {
    const double deltaOnTwoSq = deltaOnTwo*deltaOnTwo;
    const double inner = rSq - deltaOnTwoSq;
    return ( inner<=0 ? 0 : rSq/(deltaOnTwo*std::sqrt(inner)) );
  }

  inline CircleParams RichDataModel::sampleCircle() const
  {
    return CircleParams( sampleFromCircleCentreDistribution(),
                         sampleFromCircleRadiusDistribution() );
  }

  inline double RichDataModel::priorProbabilityOf(const CircleParams & cp) const
  {
    return ( priorProbabilityOfCentre(cp.centre()) *
             priorProbabilityOfRadius(cp.radius()) );
  }

  inline double RichDataModel::priorProbabilityOfCentre(const Small2Vector & cent) const
  {
    return ( gaussianProb(cent.x(), circleCenXMean, circleCenXSig)*
             gaussianProb(cent.y(), circleCenYMean, circleCenYSig) );
  }

  inline double RichDataModel::priorProbabilityOfHitDueToBackground(const Small2Vector & p) const
  {
    const double sigSq = backgroundRadius*backgroundRadius;
    const double rSqOnSigSq = p.mag2()/sigSq;
    const double half = 0.5;
    const double one = 1;
    return (one/(MathsConstants::twoPi*sigSq))*exp(-half*rSqOnSigSq);
  }

  inline double RichDataModel::priorProbabilityOfNumberOfCircles(const int n) const
  {
    return poissonProb(n, meanNumberOfRings);
  }

  /// assume number of hits appearing on ring is poisson.  Determine the mean of that poisson dist, given a CircleParams.
  inline double RichDataModel::meanNumberOfHitsMuExpectedOn(const CircleParams & cp) const
  {
    return MathsConstants::pi * circleHitsPerUnitLengthParameter * cp.radius();
  }

  /// don't confuse "meanNumberOfHitsMuExpectedFromBg()" with "meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed)" !
  inline double RichDataModel::meanNumberOfHitsMuExpectedFromBg() const
  {
    return backgroundMeanParameter;
  }

  /// don't confuse "meanNumberOfHitsMuExpectedFromBg()" with "meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed)" !
  inline double RichDataModel::meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription &) const
  {
    //#warning "Should perhaps the bg mean be expected to be higher for events with lots of rings?  Or are these independent?  We assume independence here!"
    return meanNumberOfHitsMuExpectedFromBg();
  }

  inline double
  RichDataModel::priorProbabilityOfThreePointsBeingOnACircle ( const Small2Vector & a,
                                                               const Small2Vector & b,
                                                               const Small2Vector & c ) const
  {
    try
    {
      const double r = CircleTheorems::radiusOfCircleThrough(a,b,c);
      return priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(a,b,c,r);
    }
    catch (CircleTheorems::RadiusIsInfinite&)
    {
      return 0;
    }
  }

}

#endif

