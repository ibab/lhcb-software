
#include "CrudeSampler.h"
#include "Utils/PressAnyKey.h"
#include "urandom/seedCLHEPStaticEngine.h"
#include "stringToNumber/stringToNumber.h"
#include <stdlib.h> // for system
#include <time.h>
#include <sstream>
#include <set>
#include "MyRichMetropolisSampler.h"

#include "GraphicsObjects.h"

using namespace Lester;

void nimPaperRevisionData(const GenRingF::GenericResults & output, const Data & data);

int main(int nArgs, char * args[]) {

  const std::string tag=(nArgs<=1  ?   "default"  :  args[1]);
  const std::string epsFileName1=std::string("snap_")+tag+"_1.eps";
  const std::string epsFileName2=std::string("snap_")+tag+"_2.eps";
  std::cout << "Hello " << epsFileName1 << std::endl;
  std::cout << "Hello " << epsFileName2 << std::endl;

#ifdef LESTER_USE_GRAPHICS
  GraphicsObjects::globalCanvas = new Canvas(nArgs, args, "First 100 proposals",400,400,10,50);
  GraphicsObjects::globalCanvas2 = new Canvas(nArgs, args, "Just the hits",400,400,420,50);

  GraphicsObjects::wc = new WarpableCanvas(*GraphicsObjects::globalCanvas, Constants::viewRangeParameter);
  GraphicsObjects::wc2 = new WarpableCanvas(*GraphicsObjects::globalCanvas2, Constants::viewRangeParameter);

  const double sc=Constants::viewRangeParameter;
  const double x1=-sc, y1=-sc, x2=sc,y2=sc;
  GraphicsObjects::globalCanvas ->setOrthoProjection(x1,y1,x2,y2);
  GraphicsObjects::globalCanvas2->setOrthoProjection(x1,y1,x2,y2);
#endif

  while(true) {

    Data data;
    const bool randomData=false;
    if (randomData) {
      long seed = seedCLHEPStaticEngine();
      //long seed = seedCLHEPStaticEngineWith(190040282);
      //long seed = seedCLHEPStaticEngineWith(218775376); // slow but right answer
      //long seed = seedCLHEPStaticEngineWith(428767025); // even slower but right answer
      std::cout << "Random seed was " << seed << std::endl;
      data.jokeSetRandom();
      {
        std::ofstream f("recent.seeds", std::ios_base::app);
        f << seed << " ";
      };
      system("date >> recent.seeds");
    } else {
      std::ostringstream file;
      static int iF(0);
      ++iF;
      file << "DATA/Events/Rich2-data" << iF << ".txt";
      try {
        data.setFromFile(file.str().c_str());
      }
      catch ( const std::exception & ex )
      {
        std::cout << "Error reading data : " << file.str() << std::endl;
        return 1;
      }
    };


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


    CrudeSampler c;

    boost::shared_ptr<GenRingF::GenericResults> outputP = c.fit(input);
    const GenRingF::GenericResults & output = *outputP;


    const RichParams currentPoint(output);
    std::cout << "Final answer was " << currentPoint << std::endl;

    //nimPaperRevisionData(output, data);

#ifdef LESTER_USE_GRAPHICS
    {
      std::ofstream eps1(epsFileName1.c_str());
      std::ofstream eps2(epsFileName2.c_str());
      GraphicsObjects::globalCanvas->sendEpsTo(&eps1, 1000.);
      GraphicsObjects::globalCanvas2->sendEpsTo(&eps2, 1000.);

      GraphicsObjects::globalCanvas ->setOrthoProjection(x1,y1,x2,y2);
      GraphicsObjects::globalCanvas2->setOrthoProjection(x1,y1,x2,y2);


      GraphicsObjects::globalCanvas->clear();
      Colour::kBlack().issue();
      data.draw(*GraphicsObjects::wc);


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

    // test inferrer
    try
    {
      std::cout << "Testing inferrer" << std::endl;
      for ( GenRingF::GenericResults::GenericRings::const_iterator iRing = output.rings.begin();
            iRing != output.rings.end();
            ++iRing )
      {
        std::cout << " -> Ring " << *iRing << std::endl;
        for ( GenRingF::GenericInput::GenericHits::const_iterator iHit = input.hits.begin();
              iHit != input.hits.end(); ++iHit )
        {
          std::cout << "  -> Hit " << *iHit << std::endl;
          const double prob = output.inferrer->probabilityHitWasMadeByGivenCircle(iHit,iRing);
          std::cout << "     probabilityHitWasMadeByGivenCircle = " << prob << std::endl;
        }
      }
    }
    catch ( const std::exception & ex )
    {
      std::cerr << "Caught exception : " << ex.what() << std::endl;
      return 1;
    }

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
      const double distSq = (jt->centre() - Hep2Vector(it->x(), it->y())).mag2();
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
