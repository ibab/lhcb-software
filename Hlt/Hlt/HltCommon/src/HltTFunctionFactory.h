// $Id: HltTFunctionFactory.h,v 1.1 2008-01-22 09:56:37 hernando Exp $
#ifndef HLTTFUNCTIONFACTORY_H 
#define HLTTFUNCTIONFACTORY_H 1

// Include files
// from Gaudi
#include "HltBase/HltBaseTool.h"
#include "HltBase/IFunctionFactory.h"            

/** @class HltTFunctionFactory HltTFunctionFactory.h
 *  
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2007-12-04
 */
namespace Hlt {

  template <class T>
  class IFunctionCreator {
  public:
    typedef typename zen::function<T> TFunction;
    virtual TFunction* create(const std::string& name) = 0;
  };  

  template <class T>
  class FunctionCreator: public IFunctionCreator<T> {
  public:
    typedef typename zen::function<T> TFunction;
    FunctionCreator(const TFunction& fun, HltBaseTool* mbase)
    {function = fun.clone(); base = mbase;}
    virtual ~FunctionCreator() {delete function;}
    TFunction* create(const std::string& name) {
      base->Assert(name.empty(), " create() requieres no name! "+name);
      return function->clone();
    };
  public:
    TFunction* function;
    HltBaseTool* base;
  };

  template <class T1, class T2> 
  class TFunctionCreator : public IFunctionCreator<T1> {
  public:
    typedef typename zen::function<T1> TFunction;
    typedef typename zen::bifunction<T1,T2> BiFunction;
    typedef typename Hlt::TSelection<T2> T2Selection;
    typedef typename zen::binder_function<T1,T2> BinderFunction;
    typedef zen::bifilter<double,double> Comparator;
    TFunctionCreator(const BiFunction& bifun, const Comparator& com,
                     HltBaseTool* mbase, bool mBinderKey = false){
      bifunction = bifun.clone(), comparator = com.clone(); 
      base = mbase; binderKey = mBinderKey;}
    virtual ~TFunctionCreator() {delete bifunction; delete comparator;}
    TFunction* create(const std::string& name) {
      base->Assert(!name.empty(), 
                   "create() null input name in binder function ");
      T2Selection& sel = base->retrieveTSelection<T2>(name);
      TFunction* fun = 0;
      BinderFunction* bfun = new BinderFunction(*bifunction,sel,*comparator);
      if (binderKey) {
        fun = new zen::binder_by_key<T1,T2>(*bfun);
        delete bfun;
      } else fun = bfun;
      return fun;
    }
  public:
    BiFunction* bifunction;
    HltBaseTool* base;
    std::string input;
    Comparator* comparator;
    bool binderKey;
  };  
  
};


template <class T>
class HltTFunctionFactory : public HltBaseTool, 
                            virtual public IFunctionFactory<T> {
public: 
  /// Standard constructor
  HltTFunctionFactory( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);

  virtual ~HltTFunctionFactory( ) {}; 

  virtual bool command(const std::string& 
                       command, const std::string& value = "");

  zen::function<T>* function(const std::string& funtionname);
  
  zen::filter<T>* filter(const std::string& filtername);

  virtual StatusCode initialize();

protected:

  void declare(const std::string& name, Hlt::IFunctionCreator<T>* creator) {
    Assert(m_creators.find(name) == m_creators.end(), 
           "declare() already declared Function Creator "+name);
    m_creators[name] = creator;
    debug() << " declared Function Creator " << name << endreq;
  }
  
  template <class FUNCTION>
  void declare(const std::string& name) {
    declare(name, new Hlt::FunctionCreator<T>(FUNCTION(),this));
  }
  
  template <class FUNCTION,class COMPARATOR,class T2>
  void declare(const std::string& name) {
    declare(name, new Hlt::TFunctionCreator<T,T2>(FUNCTION(),COMPARATOR(),this));
    declare(name+"Key", 
            new Hlt::TFunctionCreator<T,T2>(FUNCTION(),COMPARATOR(),this,true));
  }
  
  template <class INTERFACE>
  void declare(const std::string& name, const std::string& toolname) {
    INTERFACE* it = tool<INTERFACE>(toolname,this);
    declare(name, new Hlt::FunctionCreator<T>
            (Hlt::FunctionTool<T,INTERFACE>(*it),this));
  }
  
  
  template <class INTERFACE,class COMPARATOR,class T2>
  void declare(const std::string& name, const std::string& toolname) {
    INTERFACE* it = tool<INTERFACE>(toolname,this);
    declare(name, 
            new Hlt::TFunctionCreator<T,T2>(Hlt::BiFunctionTool<T,T2,INTERFACE>
                                            (*it),COMPARATOR(),this));
    declare(name+"Key", 
            new Hlt::TFunctionCreator<T,T2>(Hlt::BiFunctionTool<T,T2,INTERFACE>
                                            (*it),COMPARATOR(),this,true));
  }
  
  

  std::map<std::string, Hlt::IFunctionCreator<T>* > m_creators;

protected:

  bool m_smart;

};
#endif // HLTTFUNCTIONFACTORY_H
