#ifndef RICHMARKOV_RICH_STUFF_TCC
#define RICHMARKOV_RICH_STUFF_TCC

#include "RichStuff.h"
#include "RichParams.h"
//#include "Data.h"
#include "Alterations.h"


template<class Mode, const Mode & mode>
void RichMarkov::RichStuff<Mode, mode>::draw(Canvas & canvas,
                                         const RichParamsT & rp,
                                         const DataT & d,
                                         const bool showTruth) {
  d.draw(canvas, showTruth);
  rp.draw(canvas);
};

template<class Mode, const Mode & mode>
double RichMarkov::RichStuff<Mode, mode>::totalLogProb(const RichParamsT & rp, const DataT & d, const AlterationsT & alterations) {
  
  double logAns=0;
  {
    // expected number of total hits:
    const double mu = rp.sigmaMuGeometricallyCorrected();

    // actual number of hits:
    const int n = d.hits.size();

    // should actually have
    //      ans *= RichMarkov::poissonProb(n,mu);
    // followed by the
    //      ans *= factorial(n)
    // which occurs in the next block to account for a combinatorial factor for the hit probabilities that are about to arrive, so to save time I combine the two factors together into
    const bool muIsTooSmall=!(RichMarkov::finite(log(mu)));
    if (mu==0 && n>0) {
      //std::cerr << "erm1 " <<mu<<" " <<n<<" " << muIsTooSmall<<std::endl;
      throw LogOfZero(); //return 0;  // infintely unlikely!
    } else if (mu==0 && n==0) {
      //std::cerr << "erm2 " <<mu<<" " <<n<<" " << muIsTooSmall<<std::endl;
      logAns+=0; //ans *= 1;
    } else if (muIsTooSmall) {
      // what about numerical issues related to taking log of mu when mu is small.
      if (n==0) {
        //std::cerr << "erm3 " <<mu<<" " <<n<<" " << muIsTooSmall<<std::endl;
        logAns -= mu; //ans *= exp(-mu);
      } else {
        //std::cerr << "erm4 " <<mu<<" " <<n<<" " << muIsTooSmall<<std::endl;
        throw LogOfZero(); //return 0;
      };
    } else {
      //std::cerr << "erm5 " <<mu<<" " <<n<<" " << muIsTooSmall
      //<<" " << n*log(mu)-mu << " " <<exp(n*log(mu)-mu)
      //  << std::endl;
      logAns+= (n*log(mu)-mu); //ans *= exp(n*log(mu)-mu);
    };
  };

  assert("step0");
  assert(RichMarkov::finite(logAns) && "step1" );

  // decide what to do vis-a-vis speeded up recalculation:
  assert(!(alterations.empty())); // we expect at the very least a "didNothing!");
  const typename AlterationsT::Alteration & firstAlteration = alterations.front();

  const bool alterationsNecessitateCompleteRecalculation =
    ( typeid(firstAlteration) ==
      typeid(typename
             AlterationsT::AlterationsNecessitateCompleteRecalculation) );

  //if (alterationsNecessitateCompleteRecalculation) {
  //  std::cout << "SUGGESTING COMPLETE RECALC IN RichStuff::totalLogProb" << std::endl;
  //};

  {
 
    for (typename DataT::Hits::const_iterator hit = d.hits.begin();
         hit!=d.hits.end();
         ++hit) {
      // In principle you should do the following:
      // const double sigmaMuGeometricallyCorrected = rp.sigmaMuGeometricallyCorrected();
      // const double p = rp.probTimesSigmaMuGeometricallyCorrected(*hit)/sigmaMuGeometricallyCorrected;
      // but I can factor the division out and do it outside the loop saving time ... so instead
      const double p = rp.probTimesSigmaMuGeometricallyCorrected(*hit, alterationsNecessitateCompleteRecalculation, alterations);
      hit->pushCacheBack(p);
      const double lp = log(p);
      if ( p==0 ||!RichMarkov::finite(lp)) {
        // ok .. we are going to throw this point, but before we do so. we must resolve the state of the Hit caches.  Now we can either do this by by throwing away the cache data already collected -- but then we will have to convince routines that come later that there is in this particular special case no cache information available (rejected points will expect to have their cache deleted .. and we won't want rouge things deleting non-existen caches) so although this solution will be the better in the long run, it will be hard to program initially.  For the moment we can do a slower "fill cache up cache with junk" solutin:
        static bool first = true;
        if (first) {
          std::cerr << "See inefficiency comment in RichStuff.cpp" << std::endl;
          first=false;
        };
        {
          // fill remainder of cache with junk.
          for (typename DataT::Hits::const_iterator remainingHits = (++hit);
               remainingHits != d.hits.end();
               ++remainingHits) {
            const double garbage=0; // exact value is unimportant -- should never be used!
            remainingHits->pushCacheBack(garbage);
          };
        };
        // now we can throw as we were originally intending ..
        throw LogOfZero();
      };
      logAns+=lp;//ans *= rp.prob(*hit);
    };
    const double sigmaMuGeometricallyCorrected = rp.sigmaMuGeometricallyCorrected();
    const double logSigmaMuGeometricallyCorrected = log(sigmaMuGeometricallyCorrected);
    // now correct for the divisions not done inside the loop:
    const double numberOfDivisionsNotDoneMadeIntoADouble =  d.hits.size();
    logAns -= numberOfDivisionsNotDoneMadeIntoADouble*logSigmaMuGeometricallyCorrected;
  };

  assert(RichMarkov::finite(logAns) && "step2");

  // prior:
  if (Constants::usePrior) {
    /*
    // in principle I should have
    //        ans *= RichMarkov::poissonProb(rp.circs.size(),meanNumberOfRings);
    // followed by
    //        ans *= factorial(rp.circs.size())
    // to accound for a combinatorial factor in what is about to follow.
    // however this involves a needless double (cancelling) evaluation of n!,
    // where n=rp.circs.size(), so instead I write the following faster combination
    // of the two:
    {
    // actually I have just realised that contrary to the comments above, since in my case the order DOES count, as I don't always sort my circles into some order, I should NOT multiply by the factorial factor mentioned above, so I should just stick  to using the poisson factor.  As a result, I comment out the three lines below and replace them by the one four below!
    //const double n = static_cast<double>(rp.circs.size());
    //const double mu = meanNumberOfRings;
    //ans *= exp(n*log(mu)-mu);  // times n! divided by n!
    ans *= poissonProb(rp.circs.size(),meanNumberOfRings);
    };


    for (typename RichParamsT::Circs::const_iterator it = rp.circs.begin();
    it!=rp.circs.end();
    it++) {
    const CircleParamsT & c = *it;
    ans *= c.priorProbability();
    };
    */

    const double pep=rp.priorProbability();
    const double lpep = log(pep);
    if (pep==0 || !RichMarkov::finite(lpep)) {
      throw LogOfZero();
    };
    logAns+=lpep; //ans *=  rp.priorProbability();
    assert(RichMarkov::finite(logAns) && "step3");

  };

  return logAns;

};

#endif
