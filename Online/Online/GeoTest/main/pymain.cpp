//
//  pymain.cpp
//  
//
//  Created by Pere Mato on 20/1/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#include "DetDesc/lcdd/LCDD.h"
#include <iostream>
#include <vector>
#include <string>
#include "TPython.h"


using namespace std;
using namespace DetDesc;
using namespace Geometry;

Geometry::LCDD* compact2lcdd(int argc, char **argv)  {
  string input;
  
  input = "compact.xml";
  if ( argc>1 ) {
    input = argv[1];
  }
  
  TPython::Exec("import lcdd");
  TPython::Exec("lcdd.loadDrivers()");
  
  LCDD *lcdd;
  lcdd = (LCDD*)(void*)TPython::Eval("lcdd.fromCompact('/Users/mato/Development/AIDA/GeoTest/cmt/ild_ex_compact.xml')");
  
  if(lcdd) lcdd->dump();
  
  return lcdd;
}

#include "TRint.h"

//______________________________________________________________________________
int run_interpreter(int argc, char **argv)   {
  vector<char*> args;
  
  for(int i=0; i<argc;++i) args.push_back((char*)argv[i]);
  
  // Create an interactive ROOT application
  TRint *theApp = new TRint("Rint", &argc, argv);
  
  /* Geometry::LCDDImp *lcdd = */ compact2lcdd((int)args.size(),&args[0]);
  //delete lcdd->document().operator->();
  
  // and enter the event loop...
  theApp->Run();
  delete theApp;
  return 0;
}

#include "DetDesc/detector/ILDExTPC.h"

int main(int argc,char** argv)  {
  
  //LCDD* lcdd = compact2lcdd(argc,argv);
  //Subdetector* det = lcdd->detector('TPC');
  //cout << "detector type = " << det.type() << endl;
  //return 0;
  return run_interpreter(argc,argv);
}
