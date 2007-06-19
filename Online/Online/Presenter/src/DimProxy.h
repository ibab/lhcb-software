// $Id: DimProxy.h,v 1.2 2007-06-19 13:35:14 psomogyi Exp $
#ifndef DIMINFOHISTO_H 
#define DIMINFOHISTO_H 1

//#include <boost/tokenizer.hpp>  // Boost

//#include <string>
#include "dim/dic.hxx"
//#include "OnlineHistDB/OnlineHistDB.h"
#include "presenter.h"

class TH1;
class TH2;
class TProfile;
class TProfile2D;
class OnlineHistDB;

//class OnlineHistogram;
class DimInfo;

using namespace pres;

/** @class DimProxy DimProxy.h
 *  @author based on code by J. Helder Lopes & Ulrich Kerzel
 *  @date   2006-08-08
 *
 * histogram format (from Helder) returned by getD*()
 * H1D  dimension, nXBins, xMin, xMax,
 *   2 * (UNDERFLOW, "in range" bins, OVERFLOW): entries and errors
 * H2D  dimension, nXBins, xMin, xMax, nYBins, yMin, yMax,
 *   2 * ((UNDERFLOW, "in range" bins, OVERFLOW)_x *
 *      (UNDERFLOW, "in range" bins, OVERFLOW)_y)
 */

 
class DimProxy : public DimInfo
{  
public:
  DimProxy(std::string serviceName,
                   int refreshTime,
                   MsgLevel verbosity,
                   OnlineHistDB* histoDB);
  
  virtual ~DimProxy();
  
//  bool    checkIfServiceSupported(); 
  bool    serviceOK();
  bool    serviceUpdated();
  void    resetServiceUpdated();  // resetset m_serviceUpdated to false again
//  TH1*    rootH1D();
//  TH1*    rootOffsetH1D();  
//  TH2*    rootH2D();  
//  TH2*    rootOffsetH2D();  
//  TH1*    rootHPD();
//  TH1*    rootOffsetHPD();  
//  float   floatCF4();
//  double  doubleCD8();  
//  int     intCI4();
  
  HistoServiceType getType();  

private:
//  void  fillHistogramH2D();
//  void  fillHistogramH1D();
//  void  fillHistogramHPD();
//  void  infoHandler();  // overloaded from DimInfo called whenever server
//                        // updates information
                                          
  HistoServiceType m_serviceType;
  bool  m_serviceOK;
  int   m_histoDimension;  // 1 for TH1, 2 for TH2
  int   m_serviceSize;  
  std::string m_serviceName;
  bool    m_serviceUpdated; // change to true once call-back is received and
                            // values may have changed        
  MsgLevel m_verbosity;  
//  bool    m_bookedHistogram;
//  int     m_CI4;
//  float   m_CF4;
//  double  m_CD8;
//  float   *m_histoData;
//  TH1     *m_H1D;    
//  TH2     *m_H2D;
//  TH1     *m_HPD;  
};
#endif // DIMINFOHISTO_H
