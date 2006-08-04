
#include "Rich.h"
#include "RichParams.h"
#include "Data.h"
#include "Utils/LogarithmicTools.h"
#include "Constants.h"

#ifdef LESTER_USE_GRAPHICS
void Lester::Rich::draw(BasicCanvasInterface & canvas,
			const RichParams & rp,
			const Data & d,
			const bool showTruth) {
  d.draw(canvas, showTruth);
  rp.draw(canvas);
};
#endif

double Lester::Rich::totalLogProb(const RichParams & rp, const Data & d) {
  
  //try {

    double logAns=0;
    {
      // expected number of total hits:
      const double mu = rp.sigmaMu();
      
      // actual number of hits:
      const int n = d.hits.size();
      
      // should actually have     
      //      ans *= Lester::poissonProb(n,mu);
      // followed by the
      //      ans *= factorial(n)
      // which occurs in the next block to account for a combinatorial factor for the hit probabilities that are about to arrive, so to save time I combine the two factors together into
      const bool muIsTooSmall=!(finite(log(mu))); 
      if (mu==0 && n>0) {
	//std::cerr << "erm1 " <<mu<<" " <<n<<" " << muIsTooSmall<<std::endl;
	throw Lester::LogarithmicTools::LogOfZero(); //return 0;  // infintely unlikely!
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
	  throw Lester::LogarithmicTools::LogOfZero(); //return 0;
	};
      } else {
	//std::cerr << "erm5 " <<mu<<" " <<n<<" " << muIsTooSmall
	  //<<" " << n*log(mu)-mu << " " <<exp(n*log(mu)-mu) 
	//  << std::endl;
	logAns+= (n*log(mu)-mu); //ans *= exp(n*log(mu)-mu);
      };
    };
    
    assert("step0");
    assert(finite(logAns) && "step1" );
    
    {
      //      ans *= factorial(n) // not needed as incorporated above
      for (Data::Hits::const_iterator hit = d.hits.begin();
	   hit!=d.hits.end();
	   ++hit) {
	const double p = rp.prob(*hit);
	const double lp = log(p);
	if (p==0 || !finite(lp)) {
	  throw Lester::LogarithmicTools::LogOfZero();
	};
	logAns+=lp;//ans *= rp.prob(*hit);
      };
    };
    
    assert(finite(logAns) && "step2");
    
    // prior:
    if (Constants::usePrior) {
      /*
      // in principle I should have
      //        ans *= Lester::poissonProb(rp.circs.size(),meanNumberOfRings);
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
      
      
      for (RichParams::Circs::const_iterator it = rp.circs.begin();
      it!=rp.circs.end();
      it++) {
      const CircleParams & c = *it;
      ans *= c.priorProbability();
      };
      */
      
      const double pep=rp.priorProbability();
      const double lpep = log(pep);
      if (pep==0 || !finite(lpep)) {
	throw Lester::LogarithmicTools::LogOfZero();
      };
      logAns+=lpep; //ans *=  rp.priorProbability();
      //std::cout << "kjhkhkjhjH"<<logAns << " " << pep << " " << lpep << std::endl;
      assert(finite(logAns) && "step3");
      
    };
    
    //{
    //  const double ans = exp(logAns);
    //  std::cout << ans << " " << logAns << std::endl;
    //  assert(finite(ans) && "step4");
    //  return ans;
    //};
    return logAns;

    //} catch (Lester::LogarithmicTools::LogOfZero l) {
    //  return 0;
    //};

};
