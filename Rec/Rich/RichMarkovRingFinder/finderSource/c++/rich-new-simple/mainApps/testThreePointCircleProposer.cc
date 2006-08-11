
#include "ThreePointCircleProposerB.h"
#include "Data.h"
#include "Utils/PressAnyKey.h"
#include "urandom/seedCLHEPStaticEngine.h"
#include "Constants.h"
#include <stdlib.h> // for system
#include "GraphicsObjects.h"
#include "RectilinearCPQuantizer.h"
#include "NimTypeRichModel.h"

using namespace Lester;

Canvas * globalCanvas;
Canvas * globalCanvas2;

int main(int nArgs, char * args[]) {
  std::cout << "Hello" << std::endl;

  globalCanvas = new Canvas(nArgs, args, "First 100 proposals",400,400,10,50);
  globalCanvas2 = new Canvas(nArgs, args, "Just the hits",400,400,420,50);
  const double sc=Constants::viewRangeParameter;
  const double x1=-sc, y1=-sc, x2=sc,y2=sc;

  std::ofstream file1("pic1.eps"); 
  globalCanvas->sendEpsTo(&file1);
  globalCanvas ->setOrthoProjection(x1,y1,x2,y2);
  globalCanvas2->setOrthoProjection(x1,y1,x2,y2);

  NimTypeRichModel ntrm;

  Data data;
  const bool randomData=true;
  if (randomData) {
    long seed = seedCLHEPStaticEngine();
    std::cout << "Random seed was " << seed << std::endl;
    data.jokeSetRandom(ntrm);
    {
      std::ofstream f("recent.seeds", std::ios_base::app);
      f << seed << " ";
    };
    system("date >> recent.seeds");
  } else {
    data.setFromFile("DATA/Events/rich2_3_OLD.txt");
  };


  RectilinearCPQuantizer rcpq(ntrm);
  ThreePointCircleProposerB p(data, rcpq,
			      ntrm.circleMeanRadiusParameter*0.1,
			      ntrm.circleMeanRadiusParameter*0.1,
			      ntrm);

  CircleParams c;
  globalCanvas2->colour(Colour::kBlack());
  data.draw(*globalCanvas2);
  globalCanvas2->update();

  sleep(1);
  for (unsigned int i=0; i<100; ++i) {
    //globalCanvas->clear();
    //globalCanvas->colour(Colour::kBlack());
    //data.draw(*globalCanvas);
    c = p.sample();
    globalCanvas->colour(Colour::kRed());
    c.draw(*globalCanvas);
    globalCanvas->update();
    std::cout << "Sample made and drawn" << std::endl;
    //pressAnyKey();
    //const double prob = p.probabilityOf(c);
    //std::cout << "Probability determined to be " << prob << std::endl;
    //pressAnyKeyQQuit();
  };
  globalCanvas->colour(Colour::kBlack());
  data.draw(*globalCanvas);
  globalCanvas->update();
  globalCanvas->sendEpsTo(0);
  file1.close();
  pressAnyKeyQQuit();
  
  std::cout << p << std::endl;

  std::cout << "\n";
  return 0;
};
