// $Id: HltTFilter.h,v 1.1 2008-01-22 09:56:37 hernando Exp $
#ifndef HLTCOMMON_HLTTFILTER_H 
#define HLTCOMMON_HLTTFILTER_H 1

// Include files
// from Gaudi
#include "HltBase/HltAlgorithm.h"
#include "HltBase/IFunctionFactory.h"

/** @class HltTrackFilter HltTrackFilter.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-03-22
 */

namespace Hlt {
  template <class T>
  class TFilterData {
  public:
    TFilterData(const std::string& name) 
    {filtername = name; minvalue = -1e6; maxvalue = 1e6;
    function = 0; filter = 0;}
    virtual ~TFilterData() 
    {if (function) delete function; if (filter) delete filter;}
  public:    
    std::string filtername;
    std::string functionname;
    std::string input2;
    int infoID;
    zen::function<T>* function;
    zen::filter<T>* filter;
    Hlt::Counter counter;
    Hlt::Counter counterCandidates;
    std::string operation;
    float minvalue;
    float maxvalue;
    bool isMin;
    Hlt::Histo* histovalues;
    Hlt::Histo* histobest;
  };  
}

template <class T>
class HltTFilter : public HltAlgorithm {
public: 

  typedef typename Hlt::TSelection<T> TSelection;
  typedef typename Hlt::TFilterData<T> TFilterData;
  typedef typename std::vector<TFilterData*>::iterator TFilterDataIterator;
  typedef typename zen::function<T> TFunction;
  typedef typename zen::filter<T> TFilter;
  //  typedef typename IFunctionFactory<T> ITFactory;
  typedef typename std::vector<T*> TContainer;
  typedef typename std::vector<T*>::iterator TContainerIterator;
  typedef KeyedContainer<T,Containers::HashMap> TESCONT;

  /// Standard constructor
  HltTFilter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltTFilter( );

  virtual StatusCode initialize();
  virtual StatusCode execute   ();
  virtual StatusCode finalize  ();

protected:

  bool m_tesInput;

  TSelection* m_tselinput;
  TSelection* m_tseloutput;

  virtual void saveConfiguration();  

protected:

  StringArrayProperty m_filterDescriptor;

  bool m_addInfo;

  void setInput(std::vector<T*>& cont) 
  {m_tinput = &cont;}

  void setOutput(std::vector<T*>& cont) 
  {m_toutput = &cont;}

  std::vector<T*>* m_tinput;
  std::vector<T*> m_ttemp;
  std::vector<T*>* m_toutput;

  std::vector< TFilterData* > m_tfilters;

protected:

  std::string m_functionFactoryName;
  IFunctionFactory<T>* m_factory;

protected:

  bool addTFilterData(const std::string& filtername);

  void addInfo(TContainer& cont, TFunction& fun, int infoID);
  
  std::vector<double> infos(TContainer& cont, int infoID);
  

};
#endif // HLTTRACKFILTER_H
