
#ifndef LESTER_MY_RICH_METROPOLIS_SAMPLER_H
#define LESTER_MY_RICH_METROPOLIS_SAMPLER_H

#include "MarkovChainSampler/MetropolisSampler.h"
#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "RichSpace.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <map>
#include "probabilityUtilities/ProbabilityUtils.h"
#include "Data.h"
#include "geometry/CircleTheorems.h"
#include "probabilityUtilities/FiniteRelativeProbabilityChooser.h"
#include "CirclePriors.h"
#include "Alterations.h"
#include "FinderExternalException.h"

namespace Lester {

  template <class Mode, const Mode & mode>
  class MyRichMetropolisSampler : public MetropolisSampler<RichSpace<Mode,mode> > {

  public:
    typedef RichSpace<Mode, mode> RichSpaceT;
    typedef Data<Mode, mode> DataT;
    typedef Alterations<Mode, mode> AlterationsT;
    typedef RichStuff<Mode, mode> RichStuffT;
    typedef CirclePriors<Mode, mode> CirclePriorsT;
    typedef CircleParams<Mode, mode> CircleParamsT;
    typedef RichParams<Mode, mode> RichParamsT;

  public:
    typedef MetropolisSampler<RichSpaceT> MetropolisSamplerT;

  private:
    MyRichMetropolisSampler(); // deliberately not implemented!
  public:
    const DataT & data;
  private:
    mutable AlterationsT alterations;
  public:
    class UnsupportedOption : public FinderExternalException {
    public:
      UnsupportedOption(const std::string & s) : FinderExternalException(s) {};
    };
    typedef RichSpaceT SpaceType;
    typedef typename RichSpaceT::PointType PointType;

    MyRichMetropolisSampler(const PointType & initialPoint, const DataT & d) :
      MetropolisSamplerT (initialPoint),
      data(d) {
      // when we start up .. there will be no point in attempting to speed up caching by using cache information!!!, so:
      alterations.alterationsNecessitateCompleteRecalculation();
    };
  public:
    double logTargetDistribution(const PointType & point) const {
      try {
        const double logProb = RichStuffT::totalLogProb(point, data, alterations);
        if (Lester::finite(logProb)) {
          return logProb;
        } else {
          std::cerr<< "Oh dear .. point is TOO LIKELY for the log of its probability to be represented as a double!  I will have to ignore it even though it is clearly good!" << std::endl;
          throw typename MetropolisSamplerT::LogOfZero();
        };
      } catch (Lester::LogOfZero &) {
        throw typename MetropolisSamplerT::LogOfZero();
      };
    };
  private:
    double logProbOfSuggestingFirstGivenLast(const PointType &,
                                             const PointType &) const {
      /*      return 1./(suggestedPoint.m0()*
              suggestedPoint.m1()*
              suggestedPoint.m2()*
              suggestedPoint.m3()*
              // deliberately no mention of A0
              suggestedPoint.tanBeta()
              // deliberately no mention of sgnMu
              ); */
      return 0;
    };
  private:
    inline double jitterFactor(const double fractionalWidth=0.1) const {
      const double ans = RandGauss::shoot(0.,fractionalWidth);
      return exp(ans);
    };
  public:
    double pDrop(const int nRings) const {
      double ans=0.9;
      const double one=1;
      for (int n=nRings+10; n>=nRings; --n) {
        const double dn=static_cast<double>(n)+one;
        ans=one-ans/dn;
      };
      return ans;
    };
  private:
    mutable std::map<int, CircleParamsT> lastCircs;
    inline const PointType randomlyDeleteCircle() const {
      const unsigned int siz=(**this).circs.size();
      if (siz==0) {
        // Can't delete ... must stay where we are.

        // Nothing to do to alterations;
        alterations.didNothing();
        return **this;
      } else {  // remove a circ
        const unsigned int nToRemove = RandFlat::shootInt(siz);
        //assert(nToRemove >= 0);
        assert(nToRemove < siz);

        RichParamsT newrp = **this;

        lastCircs[siz]=newrp.circs[nToRemove];

        {
          const unsigned int sizMinusOne = siz - 1;
          if (sizMinusOne != nToRemove) {
            newrp.circs[nToRemove] = newrp.circs.back();
            alterations.exchanged(sizMinusOne, nToRemove);
          };
        };

        newrp.circs.pop_back();
        alterations.deletedAtBack();
        //std::cerr << "X" << (**this).circs.size()<<"-"<<newrp.circs.size()<<" "<<std::flush;

        //alterations.changedLocationOf();

        return newrp;
      };
    };
    inline const PointType randomlyAddCircle(const double s3) const {
      const int nDataPoints = data.hits.size();

      const double prob = RandFlat::shoot();
      if ( prob < s3 ) {
        if (nDataPoints>=3) {
          // we have enough data points for a 3pt circle add.
          if (Constants::useNewThreePointMethod) {
            return doNewThreePointCircleAdd();
          } else {
            return doThreePointCircleAdd();
          };
        } else {
          // fall-back to plan b
          return doReverseCircleAdd();

        };
      } else {
        return doReverseCircleAdd();
      };
    };
  private:

    /// Put new circle in based on really complicated and mysterious choice of three
    /// points through which the circle goes (first one chosen by random)
    inline const PointType doNewThreePointCircleAdd() const {
      const int nDataPoints = data.hits.size();
      assert(nDataPoints>=3);

      //First choose a random hit as the initial seed.
      static bool first = true;
      if (first) {
        first = false;
        std::cerr << "doNewThreePointCircleAdd() could be improved by seeding on a non-random point" <<std::endl;
      };
      const int a = RandFlat::shootInt(nDataPoints);
      assert(a>=0 && a<nDataPoints);
      const Hit & ha =  data.hits[a];


      while (true) {

        try {

          FiniteRelativeProbabilityChooser<int> frpc;
          {
            // Initialise the frpc!
            for (int b=0; b<nDataPoints; ++b) {
              // We don't want to compute a distance to ourselves ...
              if (b!=a) {
                const Hit & hb =  data.hits[b];
                const double prob_that_a_and_b_are_in_same_circle
                  = CirclePriorsT::PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
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
          const Hit & hb =  data.hits[b];

          // Now need to choose the third point!
          // First reset the frpc:
          frpc.clear();
          {
            // Initialise the frpc again!
            for (int c=0; c<nDataPoints; ++c) {
              // We don't want to compute a distance to the ones we already have ...
              if (c!=a && c!=b) {
                const Hit & hc =  data.hits[c];

                try {
                  const double r = CircleTheorems::radiusOfCircleThrough(ha,hb,hc);

                  const double prob_that_a_b_and_c_are_in_same_circle
                    = CirclePriorsT::priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(ha,hb,hc,r);
                  //std::cerr << "LLOOM " << c<<" " <<prob_that_a_b_and_c_are_in_same_circle << " " << r << std::endl;
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
          const Hit & hc =  data.hits[c];


          //The scene is set to return the new point now!

          RichParamsT newrp = **this;

          try {
            const Hep2Vector flatCentre(CircleTheorems::centreOfCircleThrough(ha,hb,hc));
            const double rrr = ((flatCentre-ha).mag());


            const CircleParamsT cp(flatCentre,rrr);
            //if (false && Constants::showSomeIdeas && RandFlat::shootInt(Constants::numberOfFramesBetweenIdeas)==1) {
              //Colour::kMagenta().issue();
              //cp.draw(*globalCanvas2);
              //globalCanvas2->update();
            //};

            newrp.circs.push_back(cp);
            alterations.createdAtBack();
            return newrp;
          } catch (CircleTheorems::PointAtInfinity &) {
            // give up and just return same :
            alterations.didNothing();
            return newrp;
          } catch (CircleTheorems::RadiusIsInfinite &) {
            // give up and just return same :
            alterations.didNothing();
            return newrp;
          };

          //std::cerr <<"flag"<<(newrp).circs.size() << std::endl;
          // we should not be able to get here!
          assert(false);

        } catch (FiniteRelativeProbabilityChooser<int>::NoSamplesPossible &) {
          static bool first = true;
          if (first) {
            first = false;
            std::cerr << "The throw/catch situation w.r.t very unlikely start points is handled very badly at present ... please fix!" << std::endl;
            std::cerr << "Moreover, I have just discovered that the above warning essage no longer means anything to me and I am now really suspious that it may be a potential bug.  Definitely need to fix.  For the moment I will just pretend to do nothing!" << std::endl;
          };
          alterations.didNothing();
          return **this;
        };
      };

    };

    /// Put new circle in based on simple (random) choice of three
    /// points through which the circle goes
    inline const PointType doThreePointCircleAdd() const {
      //std::cerr << "Trying to do 3pt circle add from "<<(**this).circs.size() << " to ";
      const int nDataPoints = data.hits.size();
      assert(nDataPoints>=3);
      std::vector<int> chosenPoints=getSetOfMDifferentIntsFromN(nDataPoints, 3);
      const Hep2Vector & a = data.hits[chosenPoints[0]];
      const Hep2Vector & b = data.hits[chosenPoints[1]];
      const Hep2Vector & c = data.hits[chosenPoints[2]];
      RichParamsT newrp = **this;

      try {
        const Hep2Vector p = CircleTheorems::centreOfCircleThrough(a,b,c);
        const double rrr = ((a-p).mag());
        {
          const CircleParamsT c(p,rrr);
          newrp.circs.push_back(c);
        };

        alterations.createdAtBack();
        //std::cerr <<"flag"<<(newrp).circs.size() << std::endl;
        return newrp;
      } catch (CircleTheorems::PointAtInfinity &) {
        // Give up!
        alterations.didNothing();
        return newrp;
      };
    };

    /// Undo the effect of a previous circle deletion (un-ignore it rather than actually delete)
    inline const PointType doReverseCircleAdd() const {
      const int newsiz=(**this).circs.size()+1;

      RichParamsT newrp = **this;
      if (lastCircs.count(newsiz)==0) {
        CircleParamsT c;
        c.jokeSetRandom();
        newrp.circs.push_back(c);
      } else {
        newrp.circs.push_back(lastCircs[newsiz]);
      };
      // std::cerr << "+" << (**this).circs.size() << "-"<<newrp.circs.size()<<" "<<std::flush;
      alterations.createdAtBack();
      return newrp;
    };
    inline const PointType addOrRemoveCircle() const {
      //const int nCircs =  (**this).circs.size();
      //const int nDataPoints = data.hits.size();
      //double s3Temp;
      //std::cerr << "Moo " <<  nDataPoints << std::endl;
      const double delProb = Constants::circleDeletionProb;//+0.*pDropC(nCircs, nDataPoints, s3Temp);
      const double s3 = 0.9;//s3Temp;


      const double p=RandFlat::shoot();

      //std::cerr<< "AOR"<<p<<":"<<delProb;
      //std::cerr<<".";
      if (p<delProb) {
        return randomlyDeleteCircle();
      } else {
        //std::cerr << "Trying to add " <<  s3 << std::endl;
        return randomlyAddCircle(s3);
      };

    };
  private:

    const PointType suggestNewPoint() const {

      alterations.clear();

      const double switcher = RandFlat::shoot();

      typedef enum {
        remove,
        pretendToRemove,
        add
      } State;

      static std::map<int, CircleParamsT> lastCircsOld;

      /*if (switcher<0.01) { // was 0.45
        PointType p;
        p.jokeSetRandom(); // Random!
        alterations should be implementd within this section before it is ever re-used!;
        return p;
        } else */
      if (switcher<0.20) {
        //std::cerr<<"ME"<<switcher;
        return (**this).jitterSymm2(alterations); // jitter SOME circles
        //      } else if (switcher<0.60) { // was 0.9
        //  return (**this).jitterSymm1(); // jitter ALL circles
      } else {


        /*
        if (Constants::useOldNewPointMethod) {
          while(true) {
            throw UnsupportedOption("CHRIS HASN'T PUT alterations SUPPORT IN Constants::useOldNewPointMethod YET!!");
          };
          const int siz =  (**this).circs.size();
          const double delprob = pDrop(siz);

          const State state = (RandFlat::shoot() < delprob )?(siz==0?pretendToRemove:remove):add;
          //std::cerr << "siz " <<siz<<" delprob=" << delprob<<" chose to " << state << std::endl;

          if (state==add) {
            // add a circ

            const int newsiz=siz+1;

            RichParamsT newrp = **this;
            if (lastCircsOld.count(newsiz)==0) {
              CircleParamsT c;
              c.jokeSetRandom();
              newrp.circs.push_back(c);
            } else {
              newrp.circs.push_back(lastCircsOld[newsiz]);
            };
            // std::cerr << "+" << (**this).circs.size() << "-"<<newrp.circs.size()<<" "<<std::flush;
            return newrp;

          } else if (state==pretendToRemove) {
            return **this;
          } else {  // remove a circ
            assert(siz>=1);

            int nToRemove;
            while (
                   (
                    nToRemove=static_cast<int>(RandFlat::shoot()*( siz )),
                    nToRemove < 0 || nToRemove >= siz

                    )
                   ) {};

            RichParamsT newrp = **this;

            lastCircsOld[siz]=newrp.circs[nToRemove];

            newrp.circs[nToRemove] = newrp.circs.back();
            newrp.circs.pop_back();
            //std::cerr << "X" << (**this).circs.size()<<"-"<<newrp.circs.size()<<" "<<std::flush;

            return newrp;
          }
        } 
        */
        //else {

        // New method!!
        return addOrRemoveCircle();
        //}
      }
    }

  private:

    // Here we override a pair of base class virtual methods:
    virtual void notifyOfRejectionOfSuggestion() const 
    {
      data.popCacheBack();
    }
    virtual void notifyOfAcceptanceOfSuggestion() const 
    {
      data.popCacheFront();
    }

  };

};

#endif
