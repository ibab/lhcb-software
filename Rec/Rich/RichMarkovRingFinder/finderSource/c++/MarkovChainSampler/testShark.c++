
/*
#include "MyMetropolisSampler.h"
#include <iostream>
#include "opengl/Canvas.h"
#include "SugraSpace.h"
#include "TH1D.h"
#include "RN.h"

int main() {

  using namespace Lester;
  using namespace std;
  //int i=9;

  typedef MyMetropolisSampler<RN<1>::PointType> MySampler;
  const MySampler::PointType initialPoint(1.);

  MySampler m(initialPoint);

  Lester::Canvas c(800,800);
  //c.Range(-5,-5,5,5);
  //c.SetPhi(210);
  cout << *m << endl;
  //c.MoveTo((*m)[0], (*m)[1]);
  //c.Update();
  TH1D h2("dkf","kjhkjH",30,-2,10);
  //cout << "he" << endl;
  h2.Draw();
  //cout << "mousdfn" << endl;
  while (true) {
    for (int i=0; i<100000; i++) {
      ++m;
      //cout << *m << endl;
      //    c.MoveTo((*m)[0], (*m)[1]);
      // c.DrawTo((*m)[0], (*m)[1]);
      h2.Fill((*m)[0]);
      //std::cout << "FROG " << (*m)[0] << std::endl;
    };
    //cout << *m << endl;
    h2.Draw();
    std::cout << "Eff: " << m.efficiency() << std::endl;
    c.Update();
  };

  c.Freeze();
  return 0;
  };*/

#include <iostream>
#include "MarkovChainSampler/SharkfinLeapfrogSampler.h"
#include "Utils/ExponentialDistribution.h"
#include "Utils/NormalDistribution.h"
#include "Utils/CauchyDistribution.h"
#include "Utils/VectorSpaceObject.h"
#include "Utils/SampleAndProbabilityModuleSum.h"

struct Thingy /********: VectorSpaceObject<Thingy>*/ {
  explicit Thingy(const double d) :
    m_d(d) {
  };
  double d() const {
    return m_d;
  };
  Thingy operator*(const double d) const {
    return Thingy(d*m_d);
  };
  Thingy operator+(const Thingy & other) const {
    return Thingy(m_d+other.m_d);
  };
  Thingy operator-(const Thingy & other) const {
    return Thingy(m_d-other.m_d);
  };
private:
  double m_d;
};

std::ostream & operator<<(std::ostream & os, const Thingy & t) {
  return os << t.d();  
};


struct MySLS : public Lester::SharkfinLeapfrogSampler<Thingy> {
  typedef Lester::SharkfinLeapfrogSampler<Thingy>::ExternalPointsType ExternalPointsType; 
  MySLS(const ExternalPointsType & initialPoints, const Lester::SharkfinLeapfrogSampler<Thingy>::Sharkfin & sharkfin) :
    Lester::SharkfinLeapfrogSampler<Thingy>(initialPoints, sharkfin) {
  };
  double logTargetDistribution(const Thingy & x) const {
    //usleep(10);
    //if (x.d()<-1.3 || x.d()>1.3) {
    //  throw Lester::LogarithmicTools::LogOfZero();
    //};
    if (!(x.d()>0 && x.d()<1 || x.d()>25 && x.d()<26)) {
      throw Lester::LogarithmicTools::LogOfZero();
    };
    return 0;
    const double ans=log(fabs(x.d())); if(finite(ans)) {return ans;} else {throw Lester::LogarithmicTools::LogOfZero();};
    //return log(x.d()*x.d());
    //return -0.5*(x.d()-0.3)*(x.d()-0.3)/(0.2*0.2);
    //return 12.*log(fabs(sin(10*(x.d()*x.d()))));
    //return 12.*log(fabs(sin(10*(x.d()))));
    //return -log(x.d()*x.d()+1.);
  };
};

int main() {

  
  MySLS::ExternalPointsType initialPoints;
  
  for (unsigned int i=0; i<4; ++i) {
    typedef double DD;
    initialPoints.push_back(boost::shared_ptr<const Thingy>( new Thingy(RandFlat::shoot(0.50,0.58))));
  }

  boost::shared_ptr<const NormalDistribution> 
    jiggle(new NormalDistribution(-1.0, 0.1));

  boost::shared_ptr<const NormalDistribution> 
    returnHome(new NormalDistribution(0.0, 0.1));

  boost::shared_ptr<const SampleAndProbabilityModuleSum<double> >
    jiggleOrReturnHome(new SampleAndProbabilityModuleSum<double>(jiggle, returnHome,
							 3,1));

  boost::shared_ptr<const CauchyDistribution>
    explore(new CauchyDistribution(0.0, 1.0));

  boost::shared_ptr<const SampleAndProbabilityModuleSum<double> >
    sharkfin(new SampleAndProbabilityModuleSum<double>(explore, jiggleOrReturnHome,
						       1,1));

  boost::shared_ptr<const ExponentialDistribution> expo(new ExponentialDistribution());
  boost::shared_ptr<const NormalDistribution> leapf(new NormalDistribution(1.0,0.1));


  MySLS sampler(initialPoints, sharkfin);

  
  unsigned int i=0;
  while(true) {

    ++i;
    ++sampler;

    std::cout << i << " " << *(sampler.last()) << " " << sampler.efficiency() << std::endl;
    /*    
    const MySLS::ExternalPointsType & cp = (*sampler);
    for (MySLS::ExternalPointsType::const_iterator it = cp.begin();
	 it != cp.end();
	 ++it) {
      
      std::cout << i << " " << **it << " " << sampler.efficiency() << "\n";
    };
    std::cout << std::flush;
    */

  };


};
