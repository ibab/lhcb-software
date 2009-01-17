// $Id: HltTFunctionFactory.h,v 1.6 2009-01-17 23:27:25 jonrob Exp $
#ifndef HLTTFUNCTIONFACTORY_H 
#define HLTTFUNCTIONFACTORY_H 1

// Include files
#include <cassert>
#include "boost/type_traits/remove_pointer.hpp"

// from Gaudi
#include "HltBase/HltBaseTool.h"
#include "HltBase/HltAlgorithm.h"
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
    virtual TFunction* create(const std::string& name, HltAlgorithm*) = 0;
    virtual ~IFunctionCreator() { }
  };  

  template <class T>
  class FunctionCreator: public IFunctionCreator<T> {
  public:
    typedef typename zen::function<T> TFunction;

    FunctionCreator(const TFunction& fun, HltBaseTool* mbase)
        : m_function(fun.clone())
        , m_base(mbase) 
    {}

    virtual ~FunctionCreator() {}

    TFunction* create(const std::string& name, HltAlgorithm* ) {
      m_base->Assert(name.empty(), " create() requires no name! "+name);
      return m_function->clone();
    };

  private:
    // Copy c'tor NOT implemented... avoid compiler generated one..
    FunctionCreator(const FunctionCreator<T>&) ;

    std::auto_ptr<TFunction> m_function;
    HltBaseTool* m_base;
  };

  template <class T1, class T2Selection> 
  class TFunctionCreator : public IFunctionCreator<T1> {
  public:
    typedef typename zen::function<T1> TFunction;
    typedef typename boost::remove_pointer<typename T2Selection::value_type>::type T2;
    typedef typename zen::bifunction<T1,T2> BiFunction;
    typedef zen::bifilter<double,double> Comparator;

    TFunctionCreator(const BiFunction& bifun, const Comparator& com, bool mBinderKey = false)
        : m_bifunction( bifun.clone() )
        , m_comparator( com.clone() )
        , m_binderKey(mBinderKey)
    { }
    TFunctionCreator(const TFunctionCreator<T1,T2Selection>& rhs)
        : m_bifunction( rhs.m_bifunction->clone() )
        , m_comparator( rhs.m_comparator->clone() )
        , m_binderKey(  rhs.m_binderKey)
    { }

    virtual ~TFunctionCreator() {}

    TFunction* create(const std::string& name, HltAlgorithm* algo) {
      assert(algo!=0);
      algo->Assert(!name.empty(), "create() must have input name in binder function ");
      T2Selection& sel = algo->retrieveTSelection<T2>(name);
      TFunction *fun(0);
      typedef typename zen::binder_function<T1,T2Selection> BinderFunction;
      BinderFunction* bfun = new BinderFunction(*m_bifunction,sel,*m_comparator);
      if (m_binderKey) {
        fun =  new zen::binder_by_key<T1,T2Selection>(*bfun) ;
        delete bfun;
      } else{
         fun=bfun;
      }
      return fun;
    }
  private:
   

    std::auto_ptr<BiFunction> m_bifunction;
    std::auto_ptr<Comparator> m_comparator;
    bool m_binderKey;
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

  virtual ~HltTFunctionFactory( );

  virtual bool command(const std::string& command, const std::string& value = "");

  zen::function<T>* function(const std::string& funtionname);
  zen::filter<T>*   filter  (const std::string& filtername);

  virtual StatusCode initialize();

protected:

  void declare(const std::string& name) {
    int id = hltInfoID(name);
    declare(name, new Hlt::FunctionCreator<T>(Hlt::Info<T>(id),this));  
  }

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
  
  template <class FUNCTION,class COMPARATOR,class T2Selection>
  void declare(const std::string& name) {
    declare(name,       new Hlt::TFunctionCreator<T,T2Selection>(FUNCTION(),COMPARATOR()));
    declare(name+"Key", new Hlt::TFunctionCreator<T,T2Selection>(FUNCTION(),COMPARATOR(),true));
  }
  
  template <class INTERFACE>
  void declare(const std::string& name, const std::string& toolname) {
    INTERFACE* it = tool<INTERFACE>(toolname,this);
    declare(name, new Hlt::FunctionCreator<T>(Hlt::FunctionTool<T,INTERFACE>(*it),this));
  }
  
  
  template <class INTERFACE,class COMPARATOR,class T2Selection>
  void declare(const std::string& name, const std::string& toolname) {
    INTERFACE* it = tool<INTERFACE>(toolname,this);
    typedef typename boost::remove_pointer<typename T2Selection::value_type>::type T2;
    declare(name,       new Hlt::TFunctionCreator<T,T2Selection>(Hlt::BiFunctionTool<T,T2,INTERFACE>(*it),COMPARATOR()));
    declare(name+"Key", new Hlt::TFunctionCreator<T,T2Selection>(Hlt::BiFunctionTool<T,T2,INTERFACE>(*it),COMPARATOR(),true));
  }
  
  


private:
  std::map<std::string, Hlt::IFunctionCreator<T>* > m_creators;

  bool m_smart;

};
#endif // HLTTFUNCTIONFACTORY_H
