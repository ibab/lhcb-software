
#ifndef RICHMARKOV_RICH_PARAMS_H
#define RICHMARKOV_RICH_PARAMS_H

#include "RichParams.fwd"

//#include "MarkovChainSampler/Ntuple.h"

#include "CircleParams.h"
#include <vector>
#include <iostream>
#include "RichStuff.h"
#include "MathsConstants/MathsConstants.h"
#include "Constants.h"
#include "probabilityUtilities/ProbabilityUtils.h"
#include "RichPriors.h"
#include "Alterations.h"

namespace RichMarkov {

  // fwd dec
  class Canvas;

  template <class Mode, const Mode & mode>
  class RichParams /*: public Ntuple<7,double>*/ {

  public:

    typedef RichPriors<Mode,mode>   RichPriorsT;
    typedef CircleParams<Mode,mode> CircleParamsT;
    typedef Alterations<Mode,mode>  AlterationsT;

  public:


    typedef std::vector<CircleParamsT> Circs;

    inline const Circs & getCircles() const { return circs; }
    inline void addCircle(const CircleParamsT & c) {
      circs.push_back(c);
    };
    Circs circs;

    //static const bool backgroundIsVariable = false;
    double meanBackground;

  public:

    inline double priorProbability() const {
      double ans = 1;
      
      // in principle I should have
      //        ans *= RichMarkov::poissonProb(rp.circs.size(),meanNumberOfRings);
      // followed by
      //        ans *= factorial(rp.circs.size())
      // to accound for a combinatorial factor in what is about to follow.
      // however this involves a needless double (cancelling) evaluation of n!,
      // where n=rp.circs.size(), so instead I write the following faster combination
      // of the two:
      
      // actually I have just realised that contrary to the comments above, since in my case the order DOES count, as I don't always sort my circles into some order, I should NOT multiply by the factorial factor mentioned above, so I should just stick  to using the poisson factor.  As a result, I comment out the three lines below and replace them by the one four below!
      //const double n = static_cast<double>(rp.circs.size());
      //const double mu = meanNumberOfRings;
      //ans *= exp(n*log(mu)-mu);  /* times n! divided by n! */
      // SUPERSEDED BY NEXT LINE      ans *= poissonProb(circs.size(),meanNumberOfRings);
      ans *= RichPriorsT::priorProbabilityOfNumberOfCircles(circs.size());
      
      for (typename Circs::const_iterator it = circs.begin();
           it!=circs.end();
           it++) {
        const CircleParamsT & c = *it;
        const double contribution = c.priorProbability();
        ans *= contribution;
      }
      
      // and now the part I left out for ages (by accident!) which ensures that we compare hypotheses with different numbers of circles on an equivalent footing.  This insertion is somewhat guessed, so would be a good idea to get a statistician to confirm what I am doing.

      // Basically, for every circle I multiply the prior by one over the expectation (under the circles prior) of the circles prior itself!

      const double two = 2;
      const double strangeFactor = (two * MathsConstants::sqrtPi);
      const double oneOnCharacteristicAreaForCentresWandering( strangeFactor * strangeFactor * mode.circleCenXSig() * mode.circleCenYSig() );
      const double oneOnCharacteristicLengthForRadiiWandering( strangeFactor * Constants::circleRadiusSigmaAboutMean );
      const double characteristicThingForOneCircle( oneOnCharacteristicAreaForCentresWandering * oneOnCharacteristicLengthForRadiiWandering );
      const double characteristicThingForAllCircles( pow(characteristicThingForOneCircle, static_cast<double>(circs.size())) );

      ans *= characteristicThingForAllCircles;

      return ans;
    }

    bool operator==(const RichParams & other) const {
      return ( circs==other.circs && meanBackground == other.meanBackground );
    }

    RichParams() : meanBackground(Constants::backgroundMeanParameter) {
      // Beware! Variables are randomized!
      //jokeSetRandom();
    }

    std::ostream & printMeTo(std::ostream & os) const;

    double sigmaMu() const {
      double ans=0;
      for (typename Circs::const_iterator it=circs.begin();
           it!=circs.end();
           ++it) {
        ans += it->meanNumberOfHits();
      };
      ans += meanBackground;
      return ans;
    };
    double sigmaMuGeometricallyCorrected() const {
      double ans=0;
      for (typename Circs::const_iterator it=circs.begin();
           it!=circs.end();
           ++it) {
        ans += it->meanNumberOfHitsGeometricallyCorrected();
      };
      ans += meanBackground;
      return ans;
    };

    inline double probTimesSigmaMuGeometricallyCorrected(const Hit & hit,
                                                         const bool alterationsNecessitateCompleteRecalculation,
                                                         const AlterationsT & alterations) const {
      // Why is this multiplied by sigmaMuGeometricallyCorrected?
      // Because the function that calls this can factor the division
      // outside all of its calls ... so we save some time.
      //WERE IT NOT TO ME MULTIPLIED WOULD NEED double sigmaMuGeometricallyCorrected = 0;
      if (alterationsNecessitateCompleteRecalculation) {

        double ans=0;
        hit.proposedCircleProbabilityCache().resize(circs.size());
        {
          unsigned int i=0;
          for (typename Circs::const_iterator it=circs.begin();
               it != circs.end();
               ++it) {
            //WERE IT NOT TO ME MULTIPLIED WOULD NEED const double muGeometricallyCorrected = it->meanNumberOfHitsGeometricallyCorrected();
            //WERE IT NOT TO ME MULTIPLIED WOULD NEED sigmaMuGeometricallyCorrected += muGeometricallyCorrected;
            //**** const double mu = it->meanNumberOfHits();
            //**** const double bitToBeAdded = mu*RichPriorsT::priorProbabilityOfHitDueToCircle(hit,*it);
            //**** two lines above now replaced by
            const double bitToBeAdded = AlterationsT::Alteration::bitToBeCached(hit, *it);
            hit.proposedCircleProbabilityCache()[i++]=bitToBeAdded;
            ans += bitToBeAdded;
          };
        };
        //WERE IT NOT TO ME MULTIPLIED WOULD NEED sigmaMuGeometricallyCorrected += meanBackground;
        ans += meanBackground*RichPriorsT::priorProbabilityOfHitDueToBackground(hit);
        return ans
          //WERE IT NOT TO ME MULTIPLIED WOULD NEED /sigmaMuGeometricallyCorrected
          ;
      } else {
        // use cache information!

        double ans = hit.getCacheFront();

        hit.proposedCircleProbabilityCache()=hit.currentCircleProbabilityCache();

        const typename AlterationsT::const_iterator end = alterations.end();
        for (typename AlterationsT::const_iterator it = alterations.begin();
             it != end;
             ++it) {
          const typename AlterationsT::Alteration * const alterationPtr = *it;
          alterationPtr->modifyCacheVectorEtc(ans, hit.proposedCircleProbabilityCache(), hit, circs);
        };
        return ans;
      };
    };
    void jokeSetRandom /* deprecated! */ () {
      circs.clear();
      const int n=RichPriorsT::sampleFromNumberOfCirclesDistribution();
      for (int i=0; i<n; i++) {
        CircleParamsT circ;
        circ.jokeSetRandom();
        circs.push_back(circ);
      };
    };
    RichParams jitterSymm1() const {
      // Jitter ALL circles
      RichParams rp;
      for (typename Circs::const_iterator it=circs.begin();
           it!=circs.end();
           ++it) {
        rp.circs.push_back(it->jitterSymm1());
      };
      return rp;
    };
    //
    //inline double suggestJitteredBg() const {
    //assert(backgroundIsVariable); // it makes no sense to have called this routine otherwise!
    //const double trialnewbg = (meanBackground+0.3*RandGauss::shoot());
    //const double ans = ((trialnewbg>0)?trialnewbg:meanBackground);
    //return ans;
    //};
    //
    RichParams jitterSymm2(AlterationsT & alterations) const {
      // Jitter SOME circles and or the bakckground
      const unsigned int siz = circs.size();
      const unsigned int sizPlusOptionalOne = siz; // the 1 is for the background!
      if (sizPlusOptionalOne>0) {
        const unsigned int numToJitter = lestersRandomNumberToJitter1(sizPlusOptionalOne);
        RichParams rp=*this;
        const std::vector<unsigned int> toJitter
          = getSetOfMDifferentUnsignedIntsFromN(sizPlusOptionalOne, numToJitter);
        //std::cerr<<"j[" << numToJitter<<"]";
        for (unsigned int i=0; i<numToJitter; ++i) {
          const unsigned int toJit = toJitter[i];
          /*
            if (backgroundIsVariable) {
            if (toJit<siz) {
            // toJit represents a circle
            rp.circs[toJit]=(rp.circs[toJit].jitterSymm1());
            } else {
            // toJit represents "the background"
            assert(toJit==siz);
            rp.meanBackground = rp.suggestJitteredBg();
            };
            } else {
          */
          // toJit ALWAYS represents a circle

          // not needed now toJit is unsigned: assert(toJit>=0);
          assert(toJit<siz);
          rp.circs[toJit]=(rp.circs[toJit].jitterSymm1());
          alterations.altered(toJit);
          /* }; */
        };
        return rp;
      } else {
        alterations.didNothing();
        return *this; // RICHMARKOV STRANGE
      };
    };
    void draw(Canvas & canvas) const;
  };


}

template<class Mode, const Mode & mode>
std::ostream & operator<<(std::ostream & os, const RichMarkov::RichParams<Mode,mode> & rp);

#include "RichParams.tcc"

#endif
