/*
 * Monitortst.h
 *
 *  Created on: Feb 10, 2011
 *      Author: beat
 */

#ifndef MONITORTST_H_
#define MONITORTST_H_


// Include files
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Property.h"
#include <map>
#include <sstream>
#include <string>


class IRegistry;
//class DimService;

class Monitortst : public GaudiAlgorithm
{
public:
   /// Constructor of this form must be provided
  Monitortst(const std::string& name, ISvcLocator* pSvcLocator);

  StatusCode initialize();
  StatusCode execute();
  StatusCode finalize();
  StatusCode testeExecute();
  StatusCode start();
  StatusCode stop();

  int cntr[100];
  std::string cntrName[100];
  std::string cntrComm[100];
};


#endif /* MONITORTST_H_ */
