// $Id: DimInfoHisto.h,v 1.2 2006-09-29 15:51:30 ukerzel Exp $
#ifndef DIMINFOHISTO_H 
#define DIMINFOHISTO_H 1

// Include files
#include "dic.hxx"
#include <string>

// forward declarations
class TH1;
class TH2;


/** @class DimInfoHisto DimInfoHisto.h
 *  
 *
 *  @author Ulrich Kerzel
 *  based on code by J. Helder Lopes
 *  @date   2006-08-08
 *
 * histogram format (from Helder) returned by getData()
 * 1Hd  dimension,nXBins,xMin,xMax,2*(UNDERFLOW,"in range" bins, OVERFLOW): entries and errors
 * 2Hd  dimension,nXBins,xMin,xMax,nYBins,yMin,yMax,
 *      2*((UNDERFLOW,"in range" bins, OVERFLOW)_x*(UNDERFLOW,"in range" bins,OVERFLOW)_y)
 */
class DimInfoHisto : public DimInfo{


public: 
  /// Standard constructor
  DimInfoHisto(std::string serviceName,
               int         refreshTime,
               int         verbosity = 0); 

  virtual ~DimInfoHisto( ); ///< Destructor

  TH1*  get1DHisto();
  TH2*  get2DHisto();
  bool  serviceOK();
  bool  serviceUpdated();
  void  ResetServiceUpdated();  // resetset m_serviceUpdated to false again
                                
  
protected:

private:
  void        infoHandler();   // overloaded from DimInfo
                               // called whenever server updates information
  void        set2DData();
  void        set1DData();
  

  bool        m_serviceOK;
  bool        m_serviceUpdated;  // change to true once call-back is received and values 
                                 // may have changed
  bool        m_bookedHistogram;
  int         m_verbosity;
  int         m_histoDimension; //1 for TH1, 2 for TH2  
  int         m_serviceSize;
  std::string m_serviceName;  
  float*      m_histoData;
  TH1*        m_histogram1D;
  TH2*        m_histogram2D;


  

}; // class DimInfoHisto
#endif // DIMINFOHISTO_H
