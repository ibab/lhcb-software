
#ifndef LESTER_THREEPOINTCIRCLEPROPOSERB_H
#define LESTER_THREEPOINTCIRCLEPROPOSERB_H

// fwd dec
#include "ThreePointCircleProposerB.fwd"

// includes
#include <iostream>
#include <map>
#include "NewCircleProposer.h"
#include "Data.h"
#include "Utils/NormalDistribution.h"
#include "HitIndexTriple.h"
#include "RegularCPQuantizer.h"

// declaration
namespace Lester
{

  template < class DATAMODEL >
  class ThreePointCircleProposerB : public NewCircleProposer
  {

  private:

    typedef std::map<HitIndexTriple, double> Cache1;
    mutable Cache1 m_cache1;
    typedef std::map<QuantizedCircleParams, long long> VisitCache;
    mutable VisitCache m_visitCache;
    const Data & m_data;
    const RegularCPQuantizer & m_cpQuantizer;
    CircleParams tryToSample() const;
    CircleParams samplePrivate() const;
    double CACHED_probabilityOfBasingACircleOn(const int i1,
                                               const int i2,
                                               const int i3) const;
    double probabilityOfBasingACircleOn(const int i1,
                                        const int i2,
                                        const int i3) const;
    double probabilityOfBasingACircleOn(const int i1,
                                        const int i2,
                                        const int i3,
                                        const CircleParams & whatItSmearedTo) const;
    const double m_circleCentreSmearingWidth;
    const double m_radiusSmearingWidth;
    const double m_wanderWidthSq;
    const DATAMODEL & m_ntrm;
    const NormalDistribution m_centreCptSmearer;
  public:
    /// The circleCentreSmearingWidth is the width in x (and also in y) by which the circle centre will be smeared out before being returned by the sample method. "radiusSmearingWidth" has the same role. The "...Sq" terminator indicates the square of the quantity has been taken.
    ThreePointCircleProposerB(const Data & d,
                              const RegularCPQuantizer & cpq,
                              const double circleCentreSmearingWidthSq,
                              const double radiusSmearingWidthSq,
                              const DATAMODEL & ntrm);
    std::ostream & printMeTo(std::ostream & os) const
    {
      return os << "ThreePointCircleProposerB[]";
    }
    friend inline std::ostream & operator<<(std::ostream & os,
                                            const Lester::ThreePointCircleProposerB<DATAMODEL> & obj)
    {
      return obj.printMeTo(os);
    }
    CircleParams sample() const;
    double probabilityOf(const CircleParams & circleParams) const;
  };
}

#endif

