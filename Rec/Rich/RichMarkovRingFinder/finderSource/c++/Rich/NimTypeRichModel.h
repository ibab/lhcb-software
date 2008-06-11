
#ifndef LESTER_NIMTYPERICHMODEL_H
#define LESTER_NIMTYPERICHMODEL_H

// fwd dec
#include "NimTypeRichModel.fwd"

// includes
#include <iostream>
#include <string>
#include <map>
#include "EventDescription.fwd"
#include "Data.fwd"
#include "CircleParams.fwd"
#include "Hit.fwd"
#include "Small2Vector.fwd"

namespace Lester {

  // declaration
  class NimTypeRichModel
  {

  public:

    /// Default constructor (reads cache file)
    NimTypeRichModel() 
    { 
      m_cacheLocation = getCacheLocation();
      readCacheFromFile(); 
    }

    // was in Rich.h -------------------------------------------------------------------

    double totalLogProbOfEventDescriptionGivenData(const EventDescription & rp,
                                                   const Data & d) const;
    double totalLogProbOfDataGivenEventDescription(const Data & d,
                                                   const EventDescription & rp) const;

    // were in RichPriors.h: ------------------------------------------------------------

    int sampleFromNumberOfCirclesDistribution() const;
    double priorProbabilityOfNumberOfCircles(const int n) const;

    int sampleFromNumberOfHitsDueToCircle(const CircleParams & cp) const;
    // probability counterpart not yet implemented

    int sampleFromNumberOfHitsDueToBackground() const;
    // probability counterpart not yet implemented

    Hit sampleHitDueToBackground() const;
    double priorProbabilityOfHitDueToBackground(const Small2Vector & p) const;

    Hit sampleHitDueToCircle(const CircleParams & c) const;
    double priorProbabilityOfHitDueToCircle(const Small2Vector & p, const CircleParams & cp) const;

    // sampling counterpart not yet implemented
    double singleHitProbabilityDistributionGivenEventDescription(const Hit & h,
                                                                 // given
                                                                 const EventDescription & ed) const;


    // were in CircleParams.h --------------------------------------------------------------

    /// assume number of hits appearing on ring is poisson.  Determine the mean of that poisson dist, given a CircleParams.
    double meanNumberOfHitsMuExpectedOn(const CircleParams & cp) const;
    double meanNumberOfHitsMuExpectedFromBg() const;
    double meanNumberOfHitsMuExpectedFromBgGiven(const EventDescription & ed) const;
    double meanNumberOfHitsMuExpectedGiven(const EventDescription & ed) const;
    CircleParams sampleCircle() const;
    double priorProbabilityOf(const CircleParams & cp) const;

    // were in CirclePriors ------------------------------------------
    class SampleIsImpossible { };
    double sampleFromCircleRadiusDistribution() const;
    double priorProbabilityOfRadius(const double r) const;
    double sampleFromCircleRadiusDistributionAbove(const double r) const;
    double priorProbabilityOfRadiusAbove(const double r) const;

    double sampleFromApproximateCoPointSeparationDistribution() const;

  private:

    typedef std::map<double,double> CacheMap;
    mutable CacheMap m_cache;
    std::string m_cacheLocation;

  private:

    /// Read the cache fill into the internal map
    void readCacheFromFile();

    /// Compute and fill cache
    double fillCache(const double deltaOnTwo) const;

    double approxCoPointSepFunctionPart2(const double deltaOnTwo, const double rSq) const;

    double approxCoPointSepFunctionPart1(const double deltaOnTwo) const;

    std::string getCacheLocation() const;

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

    // were in EventDescription.h -------------------------------------------------------

    EventDescription sampleAnEventDescriptionFromPrior() const;
    double priorProbabilityOf(const EventDescription & ed) const;

    // were in Constants.h --------------------------------------------------------------
  private:

    static const double backgroundRadius;
    static const int    meanNumberOfRings;
    static const double circleHitsPerUnitLengthParameter;
    static const double circleRadiusSigmaAboutMean;
    static const double circleCenXMean;
    static const double circleCenYMean;
    static const double circleCenXSig;
    static const double circleCenYSig;
    static const double circleProbabilityDistributionAlphaParameter;
    static const double circleProbabilityDistributionEpsilonParameter;
    static const double areaScaleForSignal;
    static const double areaScaleForEverything;
    static const double backgroundMeanParameter; /// should be the poisson mean number of hits from BG given nothing else, but the whole treatment of BG hits should change in the near future.
  public:
    static const double circleMeanRadiusParameter; /// should be the mean radius of rich-rings.  Is often used also to set a length scale for CPQuantization.
  };

}

#endif

