
#include "ThreePointCircleProposer.h"
#include "Utils/CannotConstructException.h"
#include "Utils/PressAnyKey.h"
#include "Utils/UpperGaussianDist.h"
#include "GraphicsObjects.h"
#include "ProbabilityUtils.h"
#include "FiniteRelativeProbabilityChooser.h"
#include "CircleTheorems.h"
#include "NimTypeRichModel.h"
#include <set>

namespace Lester {

  CircleParams ThreePointCircleProposer::sample() const {
    unsigned long failures=0;
    CircleParams ans;
    while(true) {
      try {
        ans = tryToSample();
        return ans;
      } catch (...) {
        // try again!
        ++failures;
        if (failures==1) {
          std::cout << "WARNING: The situation with respect to very unlikely start points has caused an ineficiency in " << __PRETTY_FUNCTION__ << " at line " << __LINE__ << " in " << __FILE__ << std::endl;
        } else if (failures==5) {
          std::cout << "SERIOUS WARNING: The situation with respect to very unlikely start points has causing a much larger problem in " << __PRETTY_FUNCTION__ << " at line " << __LINE__ << " in " << __FILE__ << " than previously thought possible.  Fix Immediately!" <<  std::endl;
        } else if (failures==100) {
          std::cout << "FATAL ERROR: Program probably stuck in an infinite loop in " << __PRETTY_FUNCTION__ << " at line " << __LINE__ << " in " << __FILE__ << ".  Am going to keep running in the hope that we escape this loop ... but I don't hold out much hope.  This is your fault for ignoring the previous two warnings." <<  std::endl;
        };
      };
    };
  };

  CircleParams ThreePointCircleProposer::tryToSample() const {
    const int nDataPoints = m_data.hits.size();
    assert(nDataPoints>=3); // assured by our constructor!

    //First choose a random hit as the initial seed.
    static bool first = true;
    if (first) {
      first = false;
      std::cerr << __PRETTY_FUNCTION__ << " could be improved by seeding on a non-random point (" << __FILE__ << " " << __LINE__ <<")"<<std::endl;
    };
    const int a = RandFlat::shootInt(nDataPoints);
    assert(a>=0 && a<nDataPoints);
    const Hit & ha = m_data.hits[a];

    try {

      FiniteRelativeProbabilityChooser<int> frpc;
      {
        // Initialise the frpc!
        for (int b=0; b<nDataPoints; ++b) {
          // We don't want to compute a distance to ourselves ...
          if (b!=a) {
            const Hit & hb =  m_data.hits[b];
            const double prob_that_a_and_b_are_in_same_circle
              = m_ntrm.PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
            //std::cerr << "KOOM " << b<<" " <<prob_that_a_and_b_are_in_same_circle << std::endl;
            frpc.addIndexAndProbability(b,prob_that_a_and_b_are_in_same_circle);
          };
        };
      };
      // Now ready to select the second point!
      //std::cerr<<"also about to ... " << std::flush;
      const int b = frpc.sampleAnIndex();
      //std::cerr<<"... also done" << std::flush;
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
              //std::cerr << "LLOOM " << c<<" " <<prob_that_a_b_and_c_are_in_same_circle << " " << r << std::endl;
              frpc.addIndexAndProbability(c,prob_that_a_b_and_c_are_in_same_circle  /r /* CONTROVERSIAL FACTOR */);
            } catch (CircleTheorems::RadiusIsInfinite rii) {
              // don't add the point to the frpc!
            };
          };
        };
      };
      // Now ready to select the third and last point!
      const int c = frpc.sampleAnIndex();
      //std::cout << "sdfsdfsdf " << nDataPoints << " a " << a << " b " << b << " c " <<c<< std::endl;
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

      } catch (CircleTheorems::PointAtInfinity pai) {
        std::cerr << "CircleTheorems::PointAtInfinity problem at line " << __LINE__ << " of " << __FILE__ << std::endl;
        throw;
      } catch (CircleTheorems::RadiusIsInfinite pai) {
        std::cerr << "CircleTheorems::RadiusIsInfinite problem at line " << __LINE__ << " of " << __FILE__ << std::endl;
        throw;
      };

    } catch (FiniteRelativeProbabilityChooser<int>::NoSamplesPossible s) {
      static bool first = true;
      if (first) {
        first = false;
        std::cerr << "The throw/catch situation w.r.t very unlikely start points is handled very badly at present ... please fix!" << std::endl;
      };
      throw;
    };

  }

  double ThreePointCircleProposer::probabilityOf(const CircleParams & circleParams) const {
    const int nDataPoints = m_data.hits.size();
    assert(nDataPoints>=3); // assured by our constructor!

    typedef std::set<int> IndexSet;
    IndexSet nearbyHitIndices;

    // first need to find at least three hits near circleParams ...
    // but will find all hits within numberOfWanderSigmaToSearch even if more than three exist.
    {
      double numberOfWanderSigmaToSearch = 3.0;
      while(nearbyHitIndices.size()<3) {
        std::cout << "Pass with sigmas " << numberOfWanderSigmaToSearch << std::endl;
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

    // draw the hits we have selected to check things are working:
#ifdef LESTER_USE_GRAPHICS
    if (GraphicsObjects::globalCanvas && true) { // can remove this region if undesired
      Data miniDataSet;
      for (IndexSet::const_iterator it = nearbyHitIndices.begin();
           it != nearbyHitIndices.end();
           ++it) {
        miniDataSet.hits.push_back(m_data.hits[*it]);
      };
      GraphicsObjects::globalCanvas->colour(Colour::kGreen());
      miniDataSet.draw(*GraphicsObjects::globalCanvas,false,3.0);
      GraphicsObjects::globalCanvas->update();
    };
#endif

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

  double ThreePointCircleProposer::CACHED_probabilityOfBasingACircleOn(const int i1,
                                                                       const int i2,
                                                                       const int i3) const {
    const HitIndexTriple h(i1,i2,i3);
    const Cache1::const_iterator it = m_cache1.find(h);
    if (it != m_cache1.end()) {
      const double p = it->second;
      //const double check = probabilityOfBasingACircleOn(i1,i2,i3);
      //if (p!=check) {
      //std::cout << "Mooon " << p << " " << check << " " << (p-check)/check << " " << h << std::endl;
      //};
      return p;
    } else {
      const double p = probabilityOfBasingACircleOn(i1,i2,i3);
      m_cache1[h]=p;
      //m_cache1.insert(std::pair<HitIndexTriple, double>(h,p));
      return p;
    };
  };

  double ThreePointCircleProposer::probabilityOfBasingACircleOn(const int a,
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
        for (int i=0; i<nDataPoints; ++i) {
          // We don't want to compute a distance to ourselves ...
          if (i!=a) {
            const Hit & hb =  m_data.hits[i];
            const double prob_that_a_and_b_are_in_same_circle
              = m_ntrm.PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
            total += prob_that_a_and_b_are_in_same_circle;
            if (i==b) {
              special += prob_that_a_and_b_are_in_same_circle;
            };
          };
        };
        //std::cout << "For part b frac is " << special << " / " << total << std::endl;
        const double frac = special/total;
        if (!finite(frac)) {
          // total is zero or close to zero
          return 0;
        };
        ans *= frac;
      };

      const Hit & hb = m_data.hits[b];

      // now what is the probability we would have chosen hit[c] to go with them?
      {
        double total=0;
        double special=0;
        for (int i=0; i<nDataPoints; ++i) {
          // We don't want to compute a distance to the ones we already have ...
          if (i!=a && i!=b) {
            double prob_that_a_b_and_c_are_in_same_circle=0;
            const Hit & hc =  m_data.hits[i];
            try {
              const double r = CircleTheorems::radiusOfCircleThrough(ha,hb,hc);
              prob_that_a_b_and_c_are_in_same_circle
                = m_ntrm.priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(ha,hb,hc,r);
            } catch (CircleTheorems::RadiusIsInfinite rii) {
              prob_that_a_b_and_c_are_in_same_circle=0;
            };

            total += prob_that_a_b_and_c_are_in_same_circle;
            if (i==c) {
              special += prob_that_a_b_and_c_are_in_same_circle;
            };
          };
        };
        //std::cout << "For part c frac is " << special << " / " << total << std::endl;
        const double frac = special/total;
        if (!finite(frac)) {
          // total is zero or close to zero
          return 0;
        };
        ans *= frac;
      };

      //const Hit & hc = m_data.hits[c];

      return ans;
    } catch (...) {
      return 0;
    };
  };

  double ThreePointCircleProposer::probabilityOfBasingACircleOn(const int a,
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
      };

      return ans;
    } catch (...) {
      return 0;
    };
  };

  ThreePointCircleProposer::ThreePointCircleProposer(const Data & d,
                                                     const double circleCentreSmearingWidth,
                                                     const double radiusSmearingWidth,
                                                     const NimTypeRichModel & ntrm):
    m_data(d),
    m_circleCentreSmearingWidth(circleCentreSmearingWidth),
    m_radiusSmearingWidth(radiusSmearingWidth),
    m_wanderWidthSq(circleCentreSmearingWidth*circleCentreSmearingWidth*2.0 + radiusSmearingWidth*radiusSmearingWidth),
    m_ntrm(ntrm),
    m_centreCptSmearer(0,circleCentreSmearingWidth) {
    if (m_data.hits.size() < 3) {
      throw CannotConstructException("You need at least 3 hits to construct a ThreePointCircleProposer!");
    };
  }

};
