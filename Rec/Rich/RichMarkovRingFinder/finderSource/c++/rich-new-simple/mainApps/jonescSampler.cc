
#include "CrudeSampler.h"
#include "Utils/PressAnyKey.h"
#include "urandom/seedCLHEPStaticEngine.h"
#include "stringToNumber/stringToNumber.h"
#include <stdlib.h> // for system
#include <time.h>
#include <sstream>
#include <set>
#include "GenericRingFinder/GenericInferrer.h"
#include "Constants.h"
#include "GraphicsObjects.h"
#include "Data.h"
#include "EventDescription.h"
#include "NimTypeRichModel.h"

using namespace Lester;

int main(int nArgs, char * args[]) {

#ifdef LESTER_USE_GRAPHICS
  try {
    GraphicsObjects::globalCanvas = new Canvas(nArgs, args, "Hits",400,400,10,50);
    GraphicsObjects::globalCanvas2 = new Canvas(nArgs, args, "Recent",400,400,420,50);
    GraphicsObjects::globalCanvas3 = new Canvas(nArgs, args, "Final",400,400,830,50);

    GraphicsObjects::wc = new WarpableCanvas(*GraphicsObjects::globalCanvas, Constants::viewRangeParameter,false);
    GraphicsObjects::wc2 = new WarpableCanvas(*GraphicsObjects::globalCanvas2, Constants::viewRangeParameter,false);
    GraphicsObjects::wc3 = new WarpableCanvas(*GraphicsObjects::globalCanvas3, Constants::viewRangeParameter,false);
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

  try
  {

    boost::shared_ptr<NimTypeRichModel> ntrm(new NimTypeRichModel);

    while(true) {

      Data data;
      const bool randomData=false;
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
      std::cout << "Read in " <<  input.hits.size() << " hits " << std::endl;
      if ( input.hits.size() > 300 )
      {
        std::cout << "** EVENT TOO BIG -> SKIPPING" << std::endl;
        continue;
      }

      CrudeSampler c(ntrm);

      // options
      //c.configuration.clearAllparams();

      //c.configuration.setParam( "RunForFixedIterations", 1000 );
      
      /*
      c.configuration.setParam( "ScaleNumItsByHits", true );
      c.configuration.setParam( "TargetIterations", 2000 );
      c.configuration.setParam( "TargetHits", 250  );
      c.configuration.setParam( "AbsMaxIts", 20000 );
      c.configuration.setParam( "AbsMinIts", 400   );
      */

      boost::shared_ptr<GenRingF::GenericResults> outputP = c.fit(input);
      const GenRingF::GenericResults & output = *outputP;

      std::cout << "Markov fit took " << output.numIterations << " iterations in "
                << output.timeTaken << " ms" << std::endl;

      const EventDescription currentPoint(output);
      std::cout << "Final answer was " << currentPoint << std::endl;

      //nimPaperRevisionData(output, data);

      // test inferrer
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

  }
  catch ( const std::exception & expt )
  {
    std::cerr << "Exception : " << expt.what() << std::endl;
    return 1;
  }

  return 0;
}

