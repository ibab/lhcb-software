#ifndef GAUCHO_DIMH2D_H
#define GAUCHO_DIMH2D_H 1

#include <string>
#include <iostream>
#include "dis.hxx"


/** @class DimH2D Gaucho/DimH2D.h
    
This class publishes an 2d histogram as defined in the AIDA interface
as a Dim Rpc server. The server transmits parameters and bin contents + bin errors
on request to a Dim Rcp client.

@author Philippe Vannerem
*/

//forward declaration
namespace AIDA {class IHistogram2D;}
class DimCommand;
class DimRpc;
class IMessageSvc;

class DimH2D : public DimRpc {
public: 
  DimH2D(std::string h2dname, const AIDA::IHistogram2D* InfoVar);
  virtual ~DimH2D();
private:
  void rpcHandler();
  std::string m_name;
  int m_nbinsX;
  int m_nbinsY;
  float* m_entries;
  float* m_errors;
  float* m_pars;
  const AIDA::IHistogram2D* m_h2d;
};

#endif // GAUCHO_DIMH2D_H
