#ifndef GAUCHO_DIMH1D_H
#define GAUCHO_DIMH1D_H 1

#include <string>
#include <iostream>
#include "dis.hxx"


/** @class DimH1D Gaucho/DimH1D.h
    
This class publishes an 1d histogram as defined in the AIDA interface
as a Dim Rpc server. The server transmits parameters and bin contents + bin errors
on request to a Dim Rcp client.

@author Philippe Vannerem
*/

//forward declaration
namespace AIDA {class IHistogram1D;}
class DimCommand;
class DimRpc;

class DimH1D : public DimRpc {
public: 
  DimH1D(std::string h1dname, AIDA::IHistogram1D* InfoVar);
  virtual ~DimH1D();
private:
  void rpcHandler();
  std::string m_name;
  int m_nbins;
  float* m_data;
  float* m_pars;
  AIDA::IHistogram1D* m_h1d;
};

#endif // GAUCHO_DIMH1D_H
