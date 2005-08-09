
#include "CrudeSampler.h"
#include "Utils/PressAnyKey.h"
#include "urandom/seedCLHEPStaticEngine.h"
#include "stringToNumber/stringToNumber.h"
#include <stdlib.h> // for system
#include <time.h>
#include "MyRichMetropolisSampler.h"

#include "GraphicsObjects.h"

using namespace Lester;

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

  Data data;
  const bool randomData=true;
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
    data.setFromFile("DATA/rich2_3.txt");
  };


  // Convert data to the generic input format:
  GenRingF::GenericInput input;
  for (Lester::Data::Hits::const_iterator it = data.hits.begin();
       it!= data.hits.end();
       ++it) {
    static unsigned int i=0;
    input.hits.push_back(GenRingF::GenericHit(i++, it->x(), it->y()));
  };
 

  /*
  std::cout << p << std::endl;

  std::cout << "\n";
  */


  CrudeSampler c;

  const GenRingF::GenericResults output = c.fit(input);
  

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
    

    const RichParams currentPoint(output);
    std::cout << "Final answer was " << currentPoint << std::endl;
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

  return 0;
};
