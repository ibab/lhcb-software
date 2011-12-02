
#include "ThreePointCircleProposer.h"
#include "Utils/CannotConstructException.h"
#include "Utils/UpperGaussianDist.h"
#include "Utils/CheckForNan.h"
#include "Utils/MessageHandler.h"
#include "ProbabilityUtils.h"
#include "FiniteRelativeProbabilityChooser.h"
#include "CircleTheorems.h"
#include "GenericRingFinder/GenericRingFinder.h"
#include <set>

namespace Lester
{

  template < class DATAMODEL >
  CircleParams ThreePointCircleProposer<DATAMODEL>::sample() const {
    unsigned long long failures=0;
    CircleParams ans;
    while(true)
    {
      try
      {
        ans = tryToSample();
        return ans;
      }
      catch (...)
      {
        // try again!
        ++failures;
        if (failures==1)
        {
          Lester::messHandle().warning() << "The situation with respect to very unlikely start points has caused an ineficiency in "
                                         << __FUNCTION__ << " at line " << __LINE__ << " in " << __FILE__ << Lester::endmsg;
        }
        else if (failures==5)
        {
          Lester::messHandle().warning() << "The situation with respect to very unlikely start points has causing a much larger problem in "
                                         << __FUNCTION__ << " at line " << __LINE__ << " in " << __FILE__
                                         << " than previously thought possible.  Fix Immediately!" << Lester::endmsg;
        }
        else if (failures==100)
        {
          Lester::messHandle().warning() << "Program probably stuck in an infinite loop in " << __FUNCTION__ << " at line " << __LINE__
                                         << " in " << __FILE__ << " -> aborting" << Lester::endmsg;
          throw GenRingF::GenericRingFinder::CouldNotFit("Stuck in potential infinite loop -> abort");
        }
      }
    }
  }

  template < class DATAMODEL >
  CircleParams ThreePointCircleProposer<DATAMODEL>::tryToSample() const
  {
    const int nDataPoints = m_data.hits.size();
    assert(nDataPoints>=3); // assured by our constructor!

    //First choose a random hit as the initial seed.
    static bool first = true;
    if (first) {
      first = false;
      Lester::messHandle().debug() << __FUNCTION__ << " could be improved by seeding on a non-random point (" << __FILE__ << " " << __LINE__ <<")"
                                   <<Lester::endmsg;
    };
    const int a = RandFlat::shootInt(nDataPoints);
    assert(a>=0 && a<nDataPoints);
    const Hit & ha = m_data.hits[a];

    try {

      FiniteRelativeProbabilityChooser<int> frpc;
      {
        // Initialise the frpc!
        for (int b=0; b<nDataPoints; ++b)
        {
          // We don't want to compute a distance to ourselves ...
          if (b!=a)
          {
            const Hit & hb =  m_data.hits[b];
            const double prob_that_a_and_b_are_in_same_circle
              = m_ntrm.PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
            frpc.addIndexAndProbability(b,prob_that_a_and_b_are_in_same_circle);
          }
        }
      }
      // Now ready to select the second point!
      const int b = frpc.sampleAnIndex();
      assert(b!=a && b>=0 && b<nDataPoints);
      const Hit & hb =  m_data.hits[b];

      // Now need to choose the third point!
      // First reset the frpc:
      frpc.clear();
      {
        // Initialise the frpc again!
        for (int c=0; c<nDataPoints; ++c) {
          // We don't want to compute a distance to the ones we already have ...
          if (c!=a && c!=b) {
            const Hit & hc =  m_data.hits[c];

            try {
              const double r = CircleTheorems::radiusOfCircleThrough(ha,hb,hc);

              const double prob_that_a_b_and_c_are_in_same_circle
                = m_ntrm.priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(ha,hb,hc,r);
              frpc.addIndexAndProbability(c,prob_that_a_b_and_c_are_in_same_circle  /r /* CONTROVERSIAL FACTOR */);
            } catch (CircleTheorems::RadiusIsInfinite&) {
              // don't add the point to the frpc!
            };
          };
        };
      };
      // Now ready to select the third and last point!
      const int c = frpc.sampleAnIndex();
      assert(c!=a && c!=b && c>=0 && c<nDataPoints);
      const Hit & hc =  m_data.hits[c];


      //The scene is set to return the sampled circleParams now!

      try {
        const Small2Vector & flatCentre = CircleTheorems::centreOfCircleThrough(ha,hb,hc);
        const double rrr = ((flatCentre-ha).mag());

        // The only thing left to do now is to smear the above!
        const UpperGaussianDist ugdRadiusSmearer(rrr, m_radiusSmearingWidth, 0);

        const double smearedX = flatCentre.x() + m_centreCptSmearer.sample();
        const double smearedY = flatCentre.y() + m_centreCptSmearer.sample();
        const double smearedR = ugdRadiusSmearer.sample();

        return CircleParams(Small2Vector(smearedX,smearedY), smearedR);

      } catch (CircleTheorems::PointAtInfinity&) {
        Lester::messHandle().error() << "CircleTheorems::PointAtInfinity problem at line " << __LINE__ << " of " << __FILE__ << Lester::endmsg;
        throw;
      } catch (CircleTheorems::RadiusIsInfinite&) {
        Lester::messHandle().error() << "CircleTheorems::RadiusIsInfinite problem at line " << __LINE__ << " of " << __FILE__ << Lester::endmsg;
        throw;
      };

    } catch (FiniteRelativeProbabilityChooser<int>::NoSamplesPossible&) {
      static bool first = true;
      if (first) {
        first = false;
        Lester::messHandle().warning() << "The throw/catch situation w.r.t very unlikely start points is handled very badly at present ... please fix!"
                                       << Lester::endmsg;
      };
      throw;
    };

  }

  template < class DATAMODEL >
  double ThreePointCircleProposer<DATAMODEL>::probabilityOf(const CircleParams & circleParams) const {
    const int nDataPoints = m_data.hits.size();
    assert(nDataPoints>=3); // assured by our constructor!

    typedef std::set<int> IndexSet;
    IndexSet nearbyHitIndices;

    // first need to find at least three hits near circleParams ...
    // but will find all hits within numberOfWanderSigmaToSearch even if more than three exist.
    {
      double numberOfWanderSigmaToSearch = 3.0;
      while(nearbyHitIndices.size()<3) {
        const double numberOfWanderSigmaToSearchSq = numberOfWanderSigmaToSearch*numberOfWanderSigmaToSearch;
        const double maxPermittedDistanceSq = numberOfWanderSigmaToSearchSq*m_wanderWidthSq;
        for (int i=0; i<nDataPoints; ++i) {
          const double wanderDistSq = circleParams.distanceSqTo(m_data.hits[i]);
          if (wanderDistSq<maxPermittedDistanceSq) {
            nearbyHitIndices.insert(i);
          };
        };
        numberOfWanderSigmaToSearch*=1.2;
      };
    };

    double ans = 0;

    // Go through all ordered triples:
    for (IndexSet::const_iterator it1 = nearbyHitIndices.begin();
         it1 != nearbyHitIndices.end();
         ++it1) {
      //const Hit & h1=m_data.hits[*it1];
      for (IndexSet::const_iterator it2 = nearbyHitIndices.begin();
           it2 != nearbyHitIndices.end();
           ++it2) {
        if (it2!=it1) {
          //const Hit & h2=m_data.hits[*it2];
          for (IndexSet::const_iterator it3 = nearbyHitIndices.begin();
               it3 != nearbyHitIndices.end();
               ++it3) {
            if (it3!=it1 && it3!=it2) {
              //const Hit & h3=m_data.hits[*it3];

              const double partialAnswer = probabilityOfBasingACircleOn(*it1,
                                                                        *it2,
                                                                        *it3,
                                                                        circleParams);
              ans += partialAnswer;
              /*
              // Draw the triples just to make sure:
              if (false) {
              globalCanvas->colour(Colour::kRed);
              h1.draw(*globalCanvas,4);
              h2.draw(*globalCanvas,4);
              h3.draw(*globalCanvas,4);
              globalCanvas->update();
              pressAnyKey();
              };
              */
            };
          };
        };
      };
    };

    return ans;
  }

  template < class DATAMODEL >
  double ThreePointCircleProposer<DATAMODEL>::CACHED_probabilityOfBasingACircleOn(const int i1,
                                                                                  const int i2,
                                                                                  const int i3) const {
    const HitIndexTriple h(i1,i2,i3);
    const Cache1::const_iterator it = m_cache1.find(h);
    if (it != m_cache1.end()) {
      const double p = it->second;
      return p;
    } else {
      const double p = probabilityOfBasingACircleOn(i1,i2,i3);
      m_cache1[h]=p;
      //m_cache1.insert(std::pair<HitIndexTriple, double>(h,p));
      return p;
    };
  };

  template < class DATAMODEL >
  double ThreePointCircleProposer<DATAMODEL>::probabilityOfBasingACircleOn(const int a,
                                                                           const int b,
                                                                           const int c) const {
    const int nDataPoints = m_data.hits.size();

    try {
      double ans=1;

      const Hit & ha = m_data.hits[a];

      // first, there would be a hit from the unlikeliness of choosing hit[a]:
      ans /= m_data.hits.size();

      // now, what is the probability we would have chosen hit[b] to go with it?
      {
        double total=0;
        double special=0;
        for (int i=0; i<nDataPoints; ++i)
        {
          // We don't want to compute a distance to ourselves ...
          if (i!=a)
          {
            const Hit & hb =  m_data.hits[i];
            const double prob_that_a_and_b_are_in_same_circle
              = m_ntrm.PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
            total += prob_that_a_and_b_are_in_same_circle;
            if (i==b)
            {
              special += prob_that_a_and_b_are_in_same_circle;
            }
          }
        }
        //Lester::messHandle().debug() << "For part b frac is " << special << " / " << total << Lester::endmsg;
        const double frac = special/total;
        if (!Lester::lfin(frac))
        {
          // total is zero or close to zero
          return 0;
        }
        ans *= frac;
      }

      const Hit & hb = m_data.hits[b];

      // now what is the probability we would have chosen hit[c] to go with them?
      {
        double total=0;
        double special=0;
        for (int i=0; i<nDataPoints; ++i)
        {
          // We don't want to compute a distance to the ones we already have ...
          if (i!=a && i!=b)
          {
            double prob_that_a_b_and_c_are_in_same_circle=0;
            const Hit & hc =  m_data.hits[i];
            try
            {
              const double r = CircleTheorems::radiusOfCircleThrough(ha,hb,hc);
              prob_that_a_b_and_c_are_in_same_circle
                = m_ntrm.priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(ha,hb,hc,r);
            }
            catch (CircleTheorems::RadiusIsInfinite&)
            {
              prob_that_a_b_and_c_are_in_same_circle=0;
            }

            total += prob_that_a_b_and_c_are_in_same_circle;
            if (i==c)
            {
              special += prob_that_a_b_and_c_are_in_same_circle;
            }
          }
        }
        //Lester::messHandle().debug() << "For part c frac is " << special << " / " << total << Lester::endmsg;
        const double frac = special/total;
        if (!Lester::lfin(frac))
        {
          // total is zero or close to zero
          return 0;
        }
        ans *= frac;
      };

      //const Hit & hc = m_data.hits[c];

      return ans;
    } catch (...) {
      return 0;
    }
  }

  template < class DATAMODEL >
  double ThreePointCircleProposer<DATAMODEL>::probabilityOfBasingACircleOn(const int a,
                                                                           const int b,
                                                                           const int c,
                                                                           const CircleParams & whatItSmearedTo) const {

    try {

      double ans = CACHED_probabilityOfBasingACircleOn(a,b,c);

      const Hit & ha = m_data.hits[a];
      const Hit & hb = m_data.hits[b];
      const Hit & hc = m_data.hits[c];

      // Finally, need to find out how likely the circle would be to have smeared to the current position!
      {
        const Small2Vector & flatCentre = CircleTheorems::centreOfCircleThrough(ha,hb,hc);
        const Small2Vector & deltaCentre = whatItSmearedTo.centre() - flatCentre;
        const double rrr = ((flatCentre-ha).mag());

        const UpperGaussianDist ugdRadiusSmearer(rrr, m_radiusSmearingWidth, 0);

        ans *= m_centreCptSmearer.probabilityOf(deltaCentre.x());
        ans *= m_centreCptSmearer.probabilityOf(deltaCentre.y());
        ans *=   ugdRadiusSmearer.probabilityOf(rrr            );
      }

      return ans;
    }
    catch (...)
    {
      return 0;
    }
  }

  template < class DATAMODEL >
  ThreePointCircleProposer<DATAMODEL>::ThreePointCircleProposer(const Data & d,
                                                                const double circleCentreSmearingWidth,
                                                                const double radiusSmearingWidth,
                                                                const DATAMODEL & ntrm):
    m_data(d),
    m_circleCentreSmearingWidth(circleCentreSmearingWidth),
    m_radiusSmearingWidth(radiusSmearingWidth),
    m_wanderWidthSq(circleCentreSmearingWidth*circleCentreSmearingWidth*2.0 + radiusSmearingWidth*radiusSmearingWidth),
    m_ntrm(ntrm),
    m_centreCptSmearer(0,circleCentreSmearingWidth)
  {
    if (m_data.hits.size() < 3)
    {
      throw CannotConstructException("You need at least three hits to construct a circle");
    }
  }

}

// Instanciate specific templates

#include "Rich/Rich1DataModel.h"
template class Lester::ThreePointCircleProposer<Lester::Rich1DataModel>;

#include "Rich/Rich2DataModel.h"
template class Lester::ThreePointCircleProposer<Lester::Rich2DataModel>;
