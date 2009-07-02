// $Id: HltTFunctionFactory.h,v 1.9 2009-07-02 09:46:39 graven Exp $
#ifndef HLTTFUNCTIONFACTORY_H 
#define HLTTFUNCTIONFACTORY_H 1

// Include files
#include <cassert>
#include "boost/type_traits/remove_pointer.hpp"
#include "boost/lambda/lambda.hpp"
#include "boost/lambda/bind.hpp"
#include "boost/lambda/construct.hpp"

using boost::lambda::new_ptr;
using boost::lambda::constructor;
using boost::lambda::bind;


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


 

  template <typename T,typename Constructor>
  class FunctionCreator_: public IFunctionCreator<T> {
  public:

    FunctionCreator_(const Constructor& constructor, GaudiTool* mbase)
        : m_constructor(constructor)
        , m_base(mbase) 
    {}

    virtual ~FunctionCreator_() {}

    zen::function<T>* create(const std::string& name, HltAlgorithm* ) {
      m_base->Assert(name.empty(), " create() requires no name! "+name);
      return m_constructor();
    };

  private:
    // Copy c'tor NOT implemented... avoid compiler generated one..
    FunctionCreator_(const FunctionCreator_<T,Constructor>&) ;

    Constructor m_constructor; 
    GaudiTool* m_base;
  };


  template <typename T, typename U>
  IFunctionCreator<T>* FunctionCreator(const U& u, GaudiTool* b) { return new Hlt::FunctionCreator_<T,U>(u,b); }


  template <class T1, class T2Selection, typename COMPARATOR,typename Constructor> 
  class TFunctionCreator_ : public IFunctionCreator<T1> {
  public:
    typedef typename zen::function<T1> TFunction;
    typedef typename boost::remove_pointer<typename T2Selection::value_type>::type T2;
    typedef typename zen::bifunction<T1,T2> BiFunction;
    typedef zen::bifilter<double,double> Comparator;

    TFunctionCreator_(const Constructor& constructor, bool mBinderKey = false)
        : m_constructor(constructor)
        , m_binderKey(mBinderKey)
    { }

    virtual ~TFunctionCreator_() {}

    TFunction* create(const std::string& name, HltAlgorithm* algo) {
      assert(algo!=0);
      algo->Assert(!name.empty(), "create() must have input name in binder function ");
      T2Selection& sel = algo->retrieveTSelection<T2>(name);
      TFunction *fun(0);
      typedef typename zen::binder_function<T1,T2Selection> BinderFunction;
      BinderFunction* bfun = new BinderFunction(m_constructor(),sel,COMPARATOR());
      if (m_binderKey) {
        fun =  new zen::binder_by_key<T1,T2Selection>(*bfun) ;
        delete bfun;
      } else{
         fun=bfun;
      }
      return fun;
    }
  private:
    // Copy c'tor NOT implemented... avoid compiler generated one..
    TFunctionCreator_(const TFunctionCreator_<T1,T2Selection,COMPARATOR,Constructor>& rhs);

    Constructor m_constructor;
    bool m_binderKey;
  };  

  template <class T1, class T2Selection, typename COMPARATOR,typename Constructor> 
  IFunctionCreator<T1>* TFunctionCreator(const Constructor& c, bool binderKey=false) { return new Hlt::TFunctionCreator_<T1,T2Selection,COMPARATOR,Constructor>(c,binderKey); }
  
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
  void declare(const std::string& name, Hlt::IFunctionCreator<T>* creator) {
    Assert(m_creators.find(name) == m_creators.end(), 
           "declare() already declared Function Creator "+name);
    m_creators[name] = creator;
    debug() << " declared Function Creator " << name << endreq;
  }
  

  template <typename FUNCTION>
  void declare(const std::string& name) {
    declare(name, Hlt::FunctionCreator<T>(new_ptr<FUNCTION>() ,this));
  }
  void declare(const std::string& name) {
    int id = hltInfoID(name);
    typedef Hlt::Info<T> FUNCTION;
    declare(name, Hlt::FunctionCreator<T>(bind(new_ptr<FUNCTION>(),id) ,this));  
  }
  template <typename INTERFACE>
  void declare(const std::string& name, const std::string& toolname, bool delay=true) {
    typedef Hlt::FunctionTool<T,INTERFACE> FUNCTION;
    if (delay) {
       debug()<< " delaying tool creation: " << this->name()<< "." << toolname << endmsg;
       declare(name, Hlt::FunctionCreator<T>(bind(new_ptr<FUNCTION>(),toolname,this) ,this));
    } else {
       debug()<< " creating tool: " << this->name()<< "." << toolname << endmsg;
       INTERFACE* it = tool<INTERFACE>(toolname,this);
       declare(name, Hlt::FunctionCreator<T>(bind(new_ptr<FUNCTION>(),it) ,this));
    }
  }
  
  
  template <class FUNCTION,class COMPARATOR,class T2Selection>
  void declare(const std::string& name) {
    declare(name,       Hlt::TFunctionCreator<T,T2Selection,COMPARATOR>(constructor<FUNCTION>()));
    declare(name+"Key", Hlt::TFunctionCreator<T,T2Selection,COMPARATOR>(constructor<FUNCTION>(),true));
  }
  
  
  template <class INTERFACE,class COMPARATOR,class T2Selection>
  void declare(const std::string& name, const std::string& toolname) {
    typedef typename boost::remove_pointer<typename T2Selection::value_type>::type T2;
    declare(name,       Hlt::TFunctionCreator<T,T2Selection,COMPARATOR>(bind(constructor<Hlt::BiFunctionTool<T,T2,INTERFACE> >(),toolname,this)));
    declare(name+"Key", Hlt::TFunctionCreator<T,T2Selection,COMPARATOR>(bind(constructor<Hlt::BiFunctionTool<T,T2,INTERFACE> >(),toolname,this),true));
  }

private:
  std::map<std::string, Hlt::IFunctionCreator<T>* > m_creators;
  bool m_smart;

};
#endif // HLTTFUNCTIONFACTORY_H
