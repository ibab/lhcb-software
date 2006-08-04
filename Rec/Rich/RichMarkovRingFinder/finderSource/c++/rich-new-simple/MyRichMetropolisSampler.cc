
#include "MyRichMetropolisSampler.h"


#include "CLHEP/Random/RandGauss.h"
#include "CLHEP/Random/RandFlat.h"
#include "Data.h"
#include "Constants.h"
#include "FiniteRelativeProbabilityChooser.h"
#include "CirclePriors.h"
#include "Rich.h"
                                 // very old! //#include "gmpxx.h"

namespace Lester {

  MyRichMetropolisSampler::MyRichMetropolisSampler(const PointType & initialPoint, Data & d) :
    MetropolisSampler<PointType> (initialPoint),
    data(d) {
  }

  double MyRichMetropolisSampler::logTargetDistribution(const PointType & point) const {
    //std::cout << "Evaluating prob for " << point << std::endl;
    try {
      const double logProb = Rich::totalLogProb(point, data);
      //std::cout<< "I found logOrob for " << /*point << */" = " << logProb << std::endl;
      //std::cerr<<"prob = "<<ans<<std::endl;
      //const double prob = exp(logProb);
      if (finite(logProb)) {
	return logProb;
      } else {
	std::cerr<< "Oh dear .. point is TOO LIKELY for the log of its probability to be represented as a double!  I will have to ignore it even though it is clearly good!" << std::endl;
	throw Lester::LogarithmicTools::LogOfZero();
      };
    } catch (Lester::LogarithmicTools::LogOfZero l) {
      // rethrow it!
      throw;
    };
  }

  double MyRichMetropolisSampler::logProbOfSuggestingFirstGivenLast(const PointType & /* suggestedPoint */,
								    const PointType & /* lastPoint */ ) const {
    /*      return 1./(suggestedPoint.m0()*
	    suggestedPoint.m1()*
	    suggestedPoint.m2()*
	    suggestedPoint.m3()*
	    // deliberately no mention of A0
	    suggestedPoint.tanBeta()
	    // deliberately no mention of sgnMu
	    ); */
    return 0;
  }

  double MyRichMetropolisSampler::jitterFactor(const double fractionalWidth) const {
    const double ans = RandGauss::shoot(0.,fractionalWidth);
    //while ( (ans=RandGauss::shoot(0.,fractionalWidth)+1)<=0 ) {};
    //std::cout << "ans="<<ans<<std::endl;
    return exp(ans);
  }

  double MyRichMetropolisSampler::pDrop(const int nRings) const {
    double ans=0.9;
    const double one=1;
    for (int n=nRings+10; n>=nRings; --n) {
      const double dn=static_cast<double>(n)+one;
      ans=one-ans/dn;
    };
    return ans;
  }
  /*
    double pDropB(const int nRings, const int nDataPoints) const {
    double ans=0.9;
    const double one=1;

    static int lastN;//no need to initialise
    static double lastS3;//no need to initialise
    static bool first=true;

    double s3Temp; // no need to initialise
    if (first || nDataPoints!=lastN) {
    s3Temp = (
    nDataPoints<3
    ?
    0
    :
    one/(one+oneOnNChooseSmall(nDataPoints,3))
    );
    first=false;
    lastN=nDataPoints;
    lastS3=s3Temp;
    } else {
    s3Temp=lastS3;
    };
    const double s3=s3Temp;

    for (int n=nRings+10+nDataPoints*nDataPoints*nDataPoints*nDataPoints; n>=nRings; --n) {
    const double dn=static_cast<double>(n)+one;
    ans=one-ans/(dn*(one-s3));
    };

    return ans;
    };
  */
  /*
    mpq_class eApproxThing(const int n, const int m) const {
    if (n>=m) {
    return mpq_class(1);
    } else {
    const mpq_class thing = eApproxThing(n+1,m);
    const mpq_class one(1);
    const mpq_class twoN(2*n);
    return one+one/(one+one/(twoN+one/thing));
    };
    };*/
  /*
    mpq_class eApprox(const int iterations) const {
    // lowest approx is "iterations=0" giving 2.
    typedef std::map<int,mpq_class> Map;
    static Map cache;

    Map::const_iterator it = cache.find(iterations);
    if (it!=cache.end()) {
    return it->second;
    } else {
    const mpq_class one(1);
    const mpq_class ans = one+eApproxThing(1,1+iterations);
    cache[iterations]=ans;
    return ans;
    };
    };*/


  /*
    mpq_class pow(const mpq_class & x,
    const mpz_class & n) const {
    typedef std::pair<mpq_class, mpz_class> Pair;
    typedef std::map<Pair, mpq_class> Map;
    static Map cache;
    Pair p(x,n);
    Map::const_iterator it = cache.find(p);

    if (it!=cache.end()) {
    return (*it).second;
    } else {
    unsigned long int uin = n.get_ui();
    mpz_t toppow;
    mpz_t botpow;
    mpq_t res;
    mpz_init(toppow);
    mpz_init(botpow);
    mpq_init(res);
    mpz_pow_ui(toppow, x.get_num_mpz_t(), uin);
    mpz_pow_ui(botpow, x.get_den_mpz_t(), uin);
    mpq_set_num(res,toppow);
    mpq_set_den(res,botpow);
    const mpq_class result(res);
    mpz_clear(toppow);
    mpz_clear(botpow);
    mpq_clear(res);
    cache[p]=result;
    return result;
    };
    };

  */

  /*
    double pDropC(const int nRings, const int nDataPoints, double & s3_output_variable) const {
    static bool first=true;

    // std::cerr << "Baa " << nDataPoints << std::endl;

    typedef std::pair<int, int> Pair;
    Pair p(nRings,nDataPoints);
    typedef std::pair<double, double> OutputPair;

    typedef std::map<Pair,OutputPair> Map;
    static Map cache;

    if (first) {
    // Try to read cache in from file!
    first=false;

    std::ifstream f("pDropC.cache");
    if (true) { //FIX
    int m,n;
    double a,b;
    while (f>>m) {
    f>>n>>a>>b;
    Pair q(m,n);
    OutputPair op(a,b);
    cache[q]=op;
    std::cerr << "Read pDropC("
    <<m
    <<","
    <<n
    <<")="
    <<a<<" with s3="<<b<<" from pDropC.cache" << std::endl;
    };
    } else {
    std::cerr << "Could not file pDropC cache file!" << std::endl;
    };
    };

    Map::const_iterator it = cache.find(p);

    if (it!=cache.end()) {
    s3_output_variable=((*it).second.second);
    //std::cerr<<"Serving s3 from cache " << s3_output_variable<<" at nR=" << nRings << " and nD=" << nDataPoints << std::endl;
    return ((*it).second.first);
    } else {
    std::cerr <<"Calculating pDropC("<<nRings<<","<<nDataPoints<<") ... " << std::flush;
    mpq_class a(0);
    mpq_class b(1);
    const mpq_class one(1);
    const mpz_class onez(1);
    const mpq_class minusOne(-1);
    unsigned long int three=3;
    const mpz_class D3=nChooseSmall(mpz_class(nDataPoints), three);
    const mpz_class onePlusD3=onez+D3;
    const mpq_class qonePlusD3(onePlusD3);
    const mpq_class lambda=one/qonePlusD3;

    s3_output_variable = static_cast<double>(1)-lambda.get_d();
    //std::cerr<<"Serving s3 from calc " << s3_output_variable<<" at nR=" << nRings << " and nD=" << nDataPoints <<std::endl;


    if (nRings%2==1) {
    b = (-b);
    };
    for (int n=1; n<=nRings; ++n) {
    b *= (lambda*n);
    a = lambda*n*(one-a);
    };
    //std::cout << a << " " << b << std::endl;
    mpq_class lastAns(0); // but no need to initialise!
    int lastIterations=1;
    bool first=true;
    bool keepGoing=true;
    while (keepGoing) {
    lastIterations*=2;
    const mpq_class eApp=eApprox(lastIterations);
    const mpq_class p0 = one/pow(eApp,onePlusD3);
    const mpq_class newAns = a+b*p0;
    // see if it is worth stopping

    if ((!first)) {
    const mpq_class diff=newAns-lastAns;
    const double delta = fabs(diff.get_d());
    const double tol = 1.E-10;
    if (delta<tol) {
    keepGoing=false;
    };
    };

    lastAns=newAns;
    first=false;
    };

    const double actualAns=lastAns.get_d();
    cache[p]=OutputPair(actualAns,s3_output_variable);
    {
    std::ofstream cf("pDropC.cache",std::ios::app);
    cf << std::setprecision(25) << p.first <<" "<< p.second << " " <<actualAns << " " << s3_output_variable << std::endl;
    }
    std::cerr << "done." << std::endl;
    return actualAns;
    };
    };
  */

  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::randomlyDeleteCircle() const {
    const int siz=(**this).circs.size();
    if (siz==0) {
      // Can't delete ... must stay where we are.
      return **this;
    } else {  // remove a circ
      const int nToRemove = RandFlat::shootInt(siz);
      assert(nToRemove >= 0 && nToRemove < siz);

      RichParams newrp = **this;

      lastCircs[siz]=newrp.circs[nToRemove];

      newrp.circs[nToRemove] = newrp.circs.back();
      newrp.circs.pop_back();
      //std::cerr << "X" << (**this).circs.size()<<"-"<<newrp.circs.size()<<" "<<std::flush;

      return newrp;
    };
  }

  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::randomlyAddCircle(const double s3) const {
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
  
  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::doNewThreePointCircleAdd() const {
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
		= CirclePriors::PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(ha,hb);
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
		  = CirclePriors::priorProbabilityOfThreePointsBeingOnACircleWithKnownCircumradius(ha,hb,hc,r);
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
	const Hit & hc =  data.hits[c];


	//The scene is set to return the new point now!

	RichParams newrp = **this;

	try {
	  const Hep2Vector flatCentre(CircleTheorems::centreOfCircleThrough(ha,hb,hc));
	  const double rrr = ((flatCentre-ha).mag());

	  {
	    const CircleParams cp(flatCentre,rrr);
#ifdef LESTER_USE_GRAPHICS
	    if (Constants::showSomeIdeas && RandFlat::shootInt(Constants::numberOfFramesBetweenIdeas)==1) {
	      GraphicsObjects::globalCanvas->colour(
						    Lester::Colour::kCyclicRainbow(RandFlat::shoot()));
	      cp.draw(*GraphicsObjects::globalCanvas);
	      GraphicsObjects::globalCanvas->update();
	    };
#endif
	    newrp.circs.push_back(cp);
	  };
	} catch (CircleTheorems::PointAtInfinity pai) {
	  // give up and just return same :
	  return newrp;
	} catch (CircleTheorems::RadiusIsInfinite pai) {
	  // give up and just return same :
	  return newrp;
	};

	//std::cerr <<"flag"<<(newrp).circs.size() << std::endl;
	return newrp;

      } catch (FiniteRelativeProbabilityChooser<int>::NoSamplesPossible s) {
	static bool first = true;
	if (first) {
	  first = false;
	  std::cerr << "The throw/catch situation w.r.t very unlikely start points is handled very badly at present ... please fix!" << std::endl;
	};
      };

    };

  }

  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::doThreePointCircleAdd() const {
    //std::cerr << "Trying to do 3pt circle add from "<<(**this).circs.size() << " to ";
    const int nDataPoints = data.hits.size();
    assert(nDataPoints>=3);
    std::vector<int> chosenPoints=getSetOfMDifferentIntsFromN(nDataPoints, 3);
    const Hep2Vector & a = data.hits[chosenPoints[0]];
    const Hep2Vector & b = data.hits[chosenPoints[1]];
    const Hep2Vector & c = data.hits[chosenPoints[2]];
    RichParams newrp = **this;

    try {
      const Hep2Vector p = CircleTheorems::centreOfCircleThrough(a,b,c);
      const double rrr = ((a-p).mag());
      {
	const CircleParams c(p,rrr);
	newrp.circs.push_back(c);
      };

      //std::cerr <<"flag"<<(newrp).circs.size() << std::endl;
      return newrp;
    } catch (CircleTheorems::PointAtInfinity pa) {
      // Give up!
      return newrp;
    };
  }

  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::doReverseCircleAdd() const {
    const int newsiz=(**this).circs.size()+1;

    RichParams newrp = **this;
    if (lastCircs.count(newsiz)==0) {
      CircleParams c;
      c.jokeSetRandom();
      newrp.circs.push_back(c);
    } else {
      newrp.circs.push_back(lastCircs[newsiz]);
    };
    // std::cerr << "+" << (**this).circs.size() << "-"<<newrp.circs.size()<<" "<<std::flush;
    return newrp;
  }
    
  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::addOrRemoveCircle() const {
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

  }

  MyRichMetropolisSampler::PointType MyRichMetropolisSampler::suggestNewPoint() const {

    const double switcher = RandFlat::shoot();

    typedef enum {
      remove,
      pretendToRemove,
      add
    } State;

    static std::map<int, CircleParams> lastCircsOld;

    /*if (switcher<0.01) { // was 0.45
      PointType p;
      p.jokeSetRandom(); // Random!
      return p;
      } else */
    if (switcher<0.20) {
      //std::cerr<<"ME"<<switcher;
      return (**this).jitterSymm2(); // jitter SOME circles
      //      } else if (switcher<0.60) { // was 0.9
      //  return (**this).jitterSymm1(); // jitter ALL circles
    } else {


      if (Constants::useOldNewPointMethod) {
	const int siz =  (**this).circs.size();
	const double delprob = pDrop(siz);

	const State state = (RandFlat::shoot() < delprob )?(siz==0?pretendToRemove:remove):add;
	//std::cerr << "siz " <<siz<<" delprob=" << delprob<<" chose to " << state << std::endl;

	if (state==add) {
	  // add a circ

	  const int newsiz=siz+1;

	  RichParams newrp = **this;
	  if (lastCircsOld.count(newsiz)==0) {
	    CircleParams c;
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

	  RichParams newrp = **this;

	  lastCircsOld[siz]=newrp.circs[nToRemove];

	  newrp.circs[nToRemove] = newrp.circs.back();
	  newrp.circs.pop_back();
	  //std::cerr << "X" << (**this).circs.size()<<"-"<<newrp.circs.size()<<" "<<std::flush;

	  return newrp;
	};
      } else {
	// New method!!
	return addOrRemoveCircle();
      };
    };
  }
  

}

