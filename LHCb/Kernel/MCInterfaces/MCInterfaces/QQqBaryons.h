// $Id: $
#ifndef MCINTERFACES_QQQBARYONS_H 
#define MCINTERFACES_QQQBARYONS_H 1

#include <string>
#include <map>
// Include files

/** @class QQqBaryons QQqBaryons.h LbGenXicc/QQqBaryons.h
 *  
 *
 *  @author Giacomo GRAZIANI
 *  @date   2012-02-16
 */
class QQqBaryons {
public:
  QQqBaryons() {initQQqBaryons();}
  virtual ~QQqBaryons() {};
protected:
  enum QQqState { Undef=0,Xiccpp, Xiccp, Xibcp, Xibc0, Xibb0, Xibbm, Omegaccp, Omegabc0, Omegabbm};
  std::map<std::string, QQqState> m_mapBaryon;
  std::map<std::string, int> m_mapBaryonPID;
  void initQQqBaryons() {
    m_mapBaryon["Xi_cc++"]=Xiccpp;           m_mapBaryonPID["Xi_cc++"]=4422;   
    m_mapBaryon["Xi_cc+"]=Xiccp;             m_mapBaryonPID["Xi_cc+"] =4412; 
    m_mapBaryon["Xi_bc+"]=Xibcp;             m_mapBaryonPID["Xi_bc+"] =5242;
    m_mapBaryon["Xi_bc0"]=Xibc0;             m_mapBaryonPID["Xi_bc0"] =5142;
    m_mapBaryon["Xi_bb0"]=Xibb0;             m_mapBaryonPID["Xi_bb0"] =5522;
    m_mapBaryon["Xi_bb-"]=Xibbm;             m_mapBaryonPID["Xi_bb-"] =5512;
    m_mapBaryon["Omega_cc+"]=Omegaccp;       m_mapBaryonPID["Omega_cc+"]=4432;
    m_mapBaryon["Omega_bc0"]=Omegabc0;       m_mapBaryonPID["Omega_bc0"]=5342;
    m_mapBaryon["Omega_bb-"]=Omegabbm;       m_mapBaryonPID["Omega_bb-"]=5532;
  }
};
#endif // MCINTERFACES_QQQBARYONS_H
