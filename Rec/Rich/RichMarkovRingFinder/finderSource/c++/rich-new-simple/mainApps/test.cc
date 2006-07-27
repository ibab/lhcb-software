
#include "Constants.h"

const bool graphics = true;

#ifdef LESTER_USE_GRAPHICS
#include "opengl/Canvas.h"
Lester::Canvas * globalCanvas3;
Lester::Canvas * globalCanvas2;
Lester::Canvas * globalCanvas;
#endif

#include <iostream>
#include "urandom/seedCLHEPStaticEngine.h"
#include "CLHEP/Random/RandFlat.h"
#include "RichSpace.h"
#include "MyRichMetropolisSampler.h"
#include "Rich.h"
#include "Data.h"
#include "ProbabilityUtils.h"
#include "CirclePriors.h"
#include "RegularProbabilityDistribution1D.h"
#include "ProbabilityAbove.h"

int main(int nArgs, char * args[]) {

  using namespace Lester;
  using namespace std;

  globalCanvas=new Canvas(nArgs, args, "best so far",400,400,0,50);
  globalCanvas2=new Canvas(nArgs, args, "current sample",400,400,410,50);
  globalCanvas3=new Canvas(nArgs, args, "just the data",400,400,820,50);

  //int i=9;

  //{
    // const ProbabilityAbove<double> pa("erm");
    //for (double d=-0.02; d<0.04; d+=0.002) {
    // std::cout << "Pa[" << d << "] = "<< pa(d) << std::endl;
    //};
    //abort();
  //};
  // RegularProbabilityDistribution1D<double> thetaDist("DATA/rich2_thetaDist.txt");
  //for (double d=0; d<0.0418; d+=0.0318/2000.) {
  //  std::cout << "Test of RPD1D:  p[" << d << "] = " << thetaDist(d) << std::endl;
  //};
  //for (int i=0;i<1000000;i++) {
  //  Hep2Vector a(0,0);
  //  Hep2Vector b(RandFlat::shoot(.01,0.3));
  //  CirclePriors::PROPTO_priorProbabilityOfTwoPointsBeingOnACircle(a,b);
  //};
  /*
  {
    FiniteRelativeProbabilityChooser<int> fpc;
    for (int i=0; i<3; ++i) {
      fpc.addIndexAndProbability(i,RandFlat::shoot());
    };
    fpc.printMapTo(std::cout);
    cout << endl<< "SAMPLING" << endl<< endl;
    for (int i=0; i<100; ++i) {
      const int index = fpc.sampleAnIndex();
      cout << "sampled index " << index<< endl;
    };
    
  };
  */
  //exit(2);

#ifdef LESTER_USE_GRAPHICS
  if (graphics) {
    const double sc=Constants::viewRangeParameter;
    const double x1=-sc, y1=-sc, x2=sc,y2=sc;
    globalCanvas->setOrthoProjection(x1,y1,x2,y2);
    globalCanvas2->setOrthoProjection(x1,y1,x2,y2);
    globalCanvas3->setOrthoProjection(x1,y1,x2,y2);
  };
#endif

  if (nArgs<=1) {
    seedCLHEPStaticEngine();
    cerr << "Seed was " << HepRandom::getTheEngine()->getSeed() << endl;
  } else {
    long seed = atoi(args[1]);
    HepRandom::getTheEngine()->setSeed(seed,1);
    cerr << "Seed was " << seed << endl;
  };

  {
    const double x=1234;
    const double y=4321;
    const double r=23;
    const double th1=0.1;
    const double th2=1.6;
    const double th3=4.5;
    Hep2Vector a(Hep2Vector(x+r*cos(th1), y+r*sin(th1)));
    Hep2Vector b(Hep2Vector(x+r*cos(th2), y+r*sin(th2)));
    Hep2Vector c(Hep2Vector(x+r*cos(th3), y+r*sin(th3)));
    cout << "Centre of circle through a="<<a<<" b="<<b<<" c="<<c<<" is p="<<CircleTheorems::centreOfCircleThrough(a,b,c)<<" which should be "<<Hep2Vector(x,y)<<endl;
};

  Data data;
  data.setFromFile("DATA/rich2_3.txt");
  if (graphics) {
#ifdef LESTER_USE_GRAPHICS
    globalCanvas2->clear();
    RichParams dummy;
    Rich::draw(*globalCanvas2,dummy,data,true);
    globalCanvas2->update();
#endif
  }

  typedef MyRichMetropolisSampler MySampler;


  // Comment out next line to force single run
  //while(true) 
    {
  
  MySampler::PointType initialPoint; 
  initialPoint.jokeSetRandom(); // Random!
  //cout << "Initial point is " << initialPoint << endl;
  MySampler m(initialPoint,data);

  {
    /*
    for (int jj=0; jj<10; ++jj) {
      const mpq_class eap = m.eApprox(jj);
      cout << "e at "<<jj<<" approx = " << eap<<" which is about " << eap.get_d() << endl;
      };*/
    /*
    for (int h=3; h<10; h++) {
      std::cout << "chosing 3 from " <<h<<std::endl;
      for (int y=0; y<40; y++) {
	std::vector<int> chosenPoints=getSetOfMDifferentIntsFromN(h,3);
	for (unsigned int k=0; k<chosenPoints.size(); k++) {
	  std::cout << chosenPoints[k]<<",";
	};
	std::cout << endl;
      };
    };
    for(int jj=0; jj<0; jj++) {
      const int j = jj;//(jj<20?jj:jj+50);
      double s3;
      cout << j << " -- " << m.pDrop(j); 
      //	   << " x2B " << m.pDropB(j,2) 
      cout << " x2=" << m.pDropC(j,2,s3);
      cout << " s3=" << s3;  
      //  << " x3B " << m.pDropB(j,3) 
      cout << " x3=" << m.pDropC(j,3,s3);
      cout << " s3=" << s3; 
      //  << " x5B " << m.pDropB(j,5) 
      cout << " x5=" << m.pDropC(j,5,s3);
      cout << " s3=" << s3;
      //  << " x10B " << m.pDropB(j,10) 
      cout   << " x10=" << m.pDropC(j,10,s3);
      cout << " s3=" << s3; 
      cout << endl; 
    };
    */
  };


  try {
    m.test();

    //  {
    //for (int i=30; i<50; i++) {
    // cout << i<<" " << Lester::poissonProb(i,40)<<endl;
    //};
    //};
  //Lester::Canvas c(800,800);
  //c.SetPhi(210);
  cout << *m << endl;
  //c.MoveTo((*m)[0], (*m)[1]);
  //c.Update();
  //  TH2D h2("dkf","kjhkjH",30,-3,20,30,-3,20);
  //TH2D h2("dkf","kjhkjH",50,0,3000,50, 0,3000);
  //cout << "he" << endl;
  MySampler::PointType last = *m;
  bool first=true;

  double bestPSoFar=m.logProbabilityOfLastAcceptedPoint();
  MySampler::PointType bestRPSoFar = last;

  for (int frames=0; frames<30; ++frames) {
    //while(true) {
    std::cout << "FRAME NUMBER " << frames << std::endl;

    if ((!(*m == last)) || first) {
      first=false;
      if (graphics) {
#ifdef LESTER_USE_GRAPHICS
	globalCanvas3->clear();
	data.draw(*globalCanvas3,false);
	globalCanvas3->update();

	globalCanvas->clear();
	Rich::draw(*globalCanvas,*m,data);
	globalCanvas->update();
	
	globalCanvas2->clear();
	Rich::draw(*globalCanvas2,bestRPSoFar,data,true);
	globalCanvas2->update();

#endif
      };
    };
    
    last=*m;

    for (int i=0; i<Constants::numberOfFramesBetweenUpdates /*500*/ /*500000*/; ++i) {
      //cout << "LESTER kkk" << *m << endl;

      {
	const double p=(m.logProbabilityOfLastAcceptedPoint());
	if (p>bestPSoFar) {
	  bestPSoFar=p;
	  bestRPSoFar=(*m);
	  if (graphics && true) {
#ifdef LESTER_USE_GRAPHICS
	    globalCanvas2->clear();
	    Rich::draw(*globalCanvas2, bestRPSoFar,data,true);
	    globalCanvas2->update();
#endif
	  };
	};
      };
      //cout << "prob was " << Rich::prob(*m,data) << endl;
      //h2.Fill(((*m).m1()),((*m).m2()));
      ++m;
      //cout << *m << endl;
      //    c.MoveTo((*m)[0], (*m)[1]);
      // c.DrawTo((*m)[0], (*m)[1]);
      //double d;
      //cin >> d;
    };
    //h2.Draw("cont4z");
    std::cout << "Eff: " << m.efficiency() << " present n=" << (*m).circs.size() << std::endl;
    //c.Update();

    std::cout << "That was FRAME number " << frames << std::endl;
  };
  }
  catch (MetropolisSampler<RichSpace>::FirstPointBad a) {
    // go round again!
  };
    };
  CircleParams::statusReport();
#ifdef LESTER_USE_GRAPHICS
  //globalCanvas.Freeze();
#endif
  return 0;
};
