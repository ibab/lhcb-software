#ifndef CALOMONIDST_CALOMONIALG_H 
#define CALOMONIDST_CALOMONIALG_H 1
// ============================================================================
// Include files
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiKernel/AlgFactory.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "CaloUtils/Calo2Dview.h"
// ============================================================================
// AIDA 
// ============================================================================
#include "AIDA/IHistogram1D.h"

// @class CaloMoniAlg CaloMoniAlg.h
//
//   @see GaudiHistoAlg
//   @see GaudiAlgorithm
//   @see      Algorithm
//   @see     IAlgorithm

class CaloMoniAlg : public Calo2Dview
{
public:
// Standard constructor
//   @param   name        algorithm name
//   @param   pSvcLocator pointer to service locator
  CaloMoniAlg( const std::string &name, ISvcLocator *pSvcLocator );  
  virtual ~CaloMoniAlg();
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize();    ///< Algorithm initialization
  
// address/location/name in Transient Store of input data container
  const std::string              &inputData() const { return m_inputData; }
// vector of input data containers
  const std::vector<std::string> &inputs()    const { return m_inputs; }
// address/location/name in Transient Store of detector data
  const std::string              &detData()   const { return m_detData; }
// set address/location/name in Transient Store of input data container
  void setInputData( const std::string &addr ) { m_inputData = addr; }
// add address/location/name in Transient Store of input data container
  void addToInputs(  const std::string &addr ) { m_inputs.push_back( addr ); }
// set address/location/name in Transient Store of detector data
  void setDetData(   const std::string &addr ) { m_detData = addr; }
// booking histogram
  inline AIDA::IHistogram1D *hBook1( const std::string hid,
                                    const std::string titl,
                                   const double low=0,
                                 const double high=100,
                                const unsigned long bins=100 ){ 
    if(!doHisto(hid))return NULL;
    h1[hid] = book1D( hid, titl, low, high, bins );
    return h1[hid];
  }
  inline AIDA::IHistogram2D *hBook2( const std::string hid,
                                    const std::string titl,
                                    const double lowx=0,
                                    const double highx=100,
                                    const unsigned long binsx=100,
                                    const double lowy=0,
                                    const double highy=100,
                                    const unsigned long binsy=100 ){ 
    if(!doHisto(hid))return NULL;
    h2[hid] = book2D( hid, titl, lowx, highx, binsx, lowy, highy, binsy );
    return h2[hid];
  }
  
// fill histogram
  inline AIDA::IHistogram1D* hFill1( std::string hid, double value, double w=1. ){ 
    if(!doHisto(hid))return NULL;
    return  fill(h1[hid],value,w);
  }
  
  inline AIDA::IHistogram2D* hFill2( std::string hid, double x, double y, double w=1. ){ 
    if(!doHisto(hid))return NULL;
    return fill(h2[hid],x,y,w); 
  }
protected:
//
// Histogram Map
  std::map< std::string, AIDA::IHistogram1D * > h1;
  std::map< std::string, AIDA::IHistogram2D * > h2;
//
private:
// address/location/name in Transient Store of input data container
  std::string              m_inputData;
// vector of addresses in the case of 'few' inputs
  std::vector<std::string> m_inputs;
// address/location/name in Transient Store of detector data
  std::string              m_detData;

protected:
  //
  double m_energyMin;
  double m_etMin;
  double m_massMin;
  double m_yMin;
  double m_xMin;
  double m_multMin;
  double m_sizeMin;
  double m_energyMax;
  double m_etMax;
  double m_massMax;
  double m_yMax;
  double m_xMax;
  double m_multMax;
  double m_sizeMax;
  int m_energyBin;
  int m_etBin;
  int m_massBin;
  int m_yBin;
  int m_xBin;
  int m_multBin;
  int m_sizeBin;

  double m_eFilter;
  double m_etFilter;
  double m_massFilterMin; 
  double m_massFilterMax;
  std::vector<std::string> m_histoList;
  std::vector<std::string> m_removeHisto;
  bool doHisto(std::string histo){
    bool ok = false;
    for( std::vector<std::string>::iterator ih = m_histoList.begin() ; m_histoList.end() != ih ; ih++){
      if( histo == *ih || "All" == *ih ){
        ok= true;
        break;
      }
    }
    for( std::vector<std::string>::iterator ih = m_removeHisto.begin() ; m_removeHisto.end() != ih ; ih++){
      if( histo == *ih){
        ok=false;
        break;
      }
    }
    return ok;
  }
  

};
// ============================================================================
// The END 
// ============================================================================
#endif // CALOMONIDST_CALOMONIALG_H
