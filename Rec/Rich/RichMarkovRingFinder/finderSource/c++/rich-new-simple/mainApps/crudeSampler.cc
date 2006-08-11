
#include "CrudeSampler.h"
#include "Utils/PressAnyKey.h"
#include "urandom/seedCLHEPStaticEngine.h"
#include "stringToNumber/stringToNumber.h"
#include <stdlib.h> // for system
#include <time.h>
#include <set>
#include "Constants.h"
#include "GraphicsObjects.h"
#include "Data.h"
#include "EventDescription.h"
#include "NimTypeRichModel.h"

using namespace Lester;

void nimPaperRevisionData(const GenRingF::GenericResults & output, const Data & data);

int main(int nArgs, char * args[]) {

  const std::string tag=(nArgs<=1  ?   "default"  :  args[1]);
  const std::string epsFileName1=std::string("snap_")+tag+"_1.eps";
  const std::string epsFileName2=std::string("snap_")+tag+"_2.eps";
  std::cout << "Hello " << epsFileName1 << std::endl;
  std::cout << "Hello " << epsFileName2 << std::endl;

#ifdef LESTER_USE_GRAPHICS
  try {
    GraphicsObjects::globalCanvas = new Canvas(nArgs, args, "Hits",400,400,10,50);
    GraphicsObjects::globalCanvas2 = new Canvas(nArgs, args, "Recent",400,400,420,50);
    GraphicsObjects::globalCanvas3 = new Canvas(nArgs, args, "Final",400,400,830,50);
    
    GraphicsObjects::wc = new WarpableCanvas(*GraphicsObjects::globalCanvas, Constants::viewRangeParameter);
    GraphicsObjects::wc2 = new WarpableCanvas(*GraphicsObjects::globalCanvas2, Constants::viewRangeParameter);
    GraphicsObjects::wc3 = new WarpableCanvas(*GraphicsObjects::globalCanvas3, Constants::viewRangeParameter);
  } catch (...) {
    std::cerr << "There was some problem creating the canvasses -- aborting at " << __FILE__ << " " << __LINE__ << std::endl;
    return 2;
  }
  const double sc=Constants::viewRangeParameter;
  const double x1=-sc, y1=-sc, x2=sc,y2=sc;
  GraphicsObjects::globalCanvas ->setOrthoProjection(x1,y1,x2,y2);
  GraphicsObjects::globalCanvas2->setOrthoProjection(x1,y1,x2,y2);
  GraphicsObjects::globalCanvas3->setOrthoProjection(x1,y1,x2,y2);
#endif


  boost::shared_ptr<NimTypeRichModel> ntrm(new NimTypeRichModel);

  while(true) {

    Data data;
    const bool randomData=true;
    if (randomData) {
      long seed = seedCLHEPStaticEngine();
      //long seed = seedCLHEPStaticEngineWith(190040282);
      //long seed = seedCLHEPStaticEngineWith(218775376); // slow but right answer
      //long seed = seedCLHEPStaticEngineWith(428767025); // even slower but right answer
      std::cout << "Random seed was " << seed << std::endl;
      data.jokeSetRandom(*ntrm);
      {
        std::ofstream f("recent.seeds", std::ios_base::app);
        f << seed << " ";
      };
      system("date >> recent.seeds");
    } else {
      data.setFromFile("DATA/Events/rich2_3_OLD.txt");
    };

#ifdef LESTER_USE_GRAPHICSSS
    GraphicsObjects::wc->clear();
    Colour::kBlack().issue();
    data.draw(*GraphicsObjects::wc);
    GraphicsObjects::wc->update();
#endif

    // Convert data to the generic input format:
    GenRingF::GenericInput input;
    for (Lester::Data::Hits::const_iterator it = data.hits.begin();
         it!= data.hits.end();
         ++it) {
      static unsigned int i=0;
      input.hits.push_back(GenRingF::GenericHit(GenRingF::GenericHitIndex(i++), it->x(), it->y()));
    };


    /*
      std::cout << p << std::endl;

      std::cout << "\n";
    */


    CrudeSampler c(ntrm);

    boost::shared_ptr<GenRingF::GenericResults> outputP = c.fit(input);
    const GenRingF::GenericResults & output = *outputP;

    const EventDescription currentPoint(output);
    std::cout << "Final answer was " << currentPoint << std::endl;


    nimPaperRevisionData(output, data);

#ifdef LESTER_USE_GRAPHICSSS
    {
      std::ofstream eps1(epsFileName1.c_str());
      std::ofstream eps2(epsFileName2.c_str());
      GraphicsObjects::globalCanvas->sendEpsTo(&eps1, 1000.);
      GraphicsObjects::globalCanvas3->sendEpsTo(&eps2, 1000.);

      GraphicsObjects::globalCanvas ->setOrthoProjection(x1,y1,x2,y2);
      GraphicsObjects::globalCanvas3->setOrthoProjection(x1,y1,x2,y2);


      GraphicsObjects::globalCanvas->clear();
      GraphicsObjects::globalCanvas3->clear();
      Colour::kBlack().issue();
      data.draw(*GraphicsObjects::wc3);


      //    Colour::kRed().issue();
      currentPoint.draw(*GraphicsObjects::wc);
      GraphicsObjects::wc->update();
      //std::cout << "Sample made and drawn" << std::endl;
      //pressAnyKey();
      //const double prob = p.probabilityOf(c);
      //std::cout << "Probability determined to be " << prob << std::endl;
      //pressAnyKeyQQuit();

      GraphicsObjects::wc2->clear();
      data.draw(*GraphicsObjects::wc2,true);
      GraphicsObjects::wc2->update();

      GraphicsObjects::globalCanvas->sendEpsTo(0);
      GraphicsObjects::globalCanvas2->sendEpsTo(0);
      eps1.close();
      eps2.close();
    };
#endif

  };

  return 0;
};



void nimPaperRevisionData(const GenRingF::GenericResults & output, const Data & data) {

  const double n_true = data.secretCircs.size();
  const double n_reco = output.rings.size();
  std::cout << "JJJA n_true " << n_true << " n_reco " << n_reco << std::endl;

  //find distances of each ring to nearest reco ring:

  std::multiset<double> distances;
  for (GenRingF::GenericResults::GenericRings::const_iterator it = output.rings.begin();
       it != output.rings.end();
       ++it) {

    double distSqSoFar=0;
    double radiusSoFar=0;
    bool first=true;
    // find nearest reco ring
    for (Data::Circs::const_iterator jt = data.secretCircs.begin();
         jt != data.secretCircs.end();
         ++jt) {
      const double distSq = (jt->centre() - Small2Vector(it->x(), it->y())).mag2();
      const double rad = jt->radius();
      if (first) {
        distSqSoFar = distSq;
        radiusSoFar = rad;
        first=false;
      } else {
        if (distSqSoFar > distSq) {
          distSqSoFar = distSq;
          radiusSoFar = rad;
        };
      };
    }


    // so now we know how far away the circle nearest to "it" was.
    const double dist = first ? -1 : sqrt(distSqSoFar);

    const double deltaRad = radiusSoFar - (it->radius());

    distances.insert(dist);
    std::cout << "JJJB " << dist << std::endl;
    std::cout << "JJJC " << first << " " << deltaRad << std::endl;



  };
};
