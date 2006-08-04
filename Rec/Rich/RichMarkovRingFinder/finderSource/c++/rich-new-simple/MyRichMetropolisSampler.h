
#ifndef LESTER_MY_RICH_METROPOLIS_SAMPLER_H
#define LESTER_MY_RICH_METROPOLIS_SAMPLER_H

#include "MarkovChainSampler/MetropolisSampler.h"
#include "RichSpace.h"
#include "Data.fwd"
#include <map>

namespace Lester {

  class MyRichMetropolisSampler :
    public MetropolisSampler<RichSpace::PointType> {
  private:
    Data & data;
  public:
    typedef RichSpace SpaceType;
    typedef RichSpace::PointType PointType;
    MyRichMetropolisSampler(const PointType & initialPoint, Data & d);
    double logTargetDistribution(const PointType & point) const;
    double logProbOfSuggestingFirstGivenLast(const PointType & /* suggestedPoint */,
                                             const PointType & /* lastPoint */ ) const;
  private:
    double jitterFactor(const double fractionalWidth=0.1) const;
  public:
    double pDrop(const int nRings) const;
  private:
  public:
  private:
    mutable std::map<int, CircleParams> lastCircs;
    PointType randomlyDeleteCircle() const;
    PointType randomlyAddCircle(const double s3) const;
  private:
    PointType doNewThreePointCircleAdd() const;
    PointType doThreePointCircleAdd() const;
    PointType doReverseCircleAdd() const;
    PointType addOrRemoveCircle() const;
  public:
    PointType suggestNewPoint() const;
  };

}

#endif
