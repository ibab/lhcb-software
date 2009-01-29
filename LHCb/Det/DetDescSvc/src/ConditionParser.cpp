#include "ConditionParser.h"

#include <boost/version.hpp>
#if BOOST_VERSION >= 103800
#define BOOST_SPIRIT_USE_OLD_NAMESPACE
#include <boost/spirit/include/classic_core.hpp>
#else
#include <boost/spirit/core.hpp>
#endif

using namespace std;
using namespace boost::spirit;

namespace {

  /// Structure to easily prepare the parameter to add to the condition
  struct TempParameter {
    TempParameter():name(),param(){}
    ~TempParameter() { }
    
    std::string name;
    BasicParam* param;
    
    /// helper function
    template <class T>
    void setValue(T value){
      if (param) delete param;
      param = new Param<T>(value);
    }

    /// helper function
    template <class T>
    void newVector(){
      if (param) delete param;
      param = new Param<std::vector<T> >();
    }

    /// helper functor
    struct SetName {
      SetName(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (char const*b, char const*e) const {
        temp.name = std::string(b,e);
        //std::cout << "SetName " << temp.name << std::endl;
      }
    };

    /// helper functor
    template <class T>
    struct SetValue {
      SetValue(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (T val) const {
        //std::cout << "SetValue " << val << std::endl;
        temp.setValue<T>(val);
      }
    };

    /// helper functor
    struct SetValueString {
      SetValueString(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (char const*b, char const*e) const {
        temp.setValue<std::string>(std::string(b,e));
        //std::cout << "SetValueString " << std::string(b,e) << std::endl;
      }
    };

    /// helper functor
    template <class T>
    struct AddVector {
      AddVector(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (char const*, char const*) const {
        //std::cout << "AddVector " << std::string(b,e) << std::endl;
        temp.newVector<T>();
      }
    };

    /// helper functor
    template <class T>
    struct PushBack {
      PushBack(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (T val) const {
        //std::cout << "PushBack " << val << std::endl;
        temp.param->template get<std::vector<T> >().push_back(val);
      }
    };

    /// helper functor
    struct PushBackString {
      PushBackString(TempParameter &par):temp(par){}
      TempParameter &temp;
      void operator () (char const*b, char const*e) const {
        //std::cout << "PushBack " << val << std::endl;
        temp.param->get<std::vector<std::string> >().push_back(std::string(b,e));
      }
    };

    /// helper functor
    struct AddParameter {
      AddParameter(ParamValidDataObject &c, TempParameter &par):temp(par),cond(c){}
      TempParameter &temp;
      ParamValidDataObject &cond;
      void operator () (char const*, char const*) const {
        //std::cout << "AddParam " << temp.name << " " << temp.param->toStr() << std::endl; 
        cond.addBasicParam(temp.name,temp.param);
      }
    };

  };

  /// helper functor
  struct SetString {
    SetString(std::string &s):temp(s){}
    std::string &temp;
    void operator () (char const*b, char const*e) const {
      temp = std::string(b,e);
      //std::cout << "SetString " << temp << std::endl;
    }
  };
  
  /// Class implementing the rules to parse the string.
  class ConditionGrammar : public grammar<ConditionGrammar> {
  public:

    ConditionGrammar() {}

    /// data member to hold the name (path) of the condition
    std::string name;
    /// data member to hold the condition
    ParamValidDataObject condition;

    
    /// Actual definitions of the rules (required by boost::spirit::grammar).
    template <class ScannerT>
    struct definition {

      /// type of elements
      enum Types {  
        Integer = 0,
        Double,
        String,
        IntegerVector,
        DoubleVector,
        StringVector,
        NTypes};
    
      /// reference to the condition to modify
      ParamValidDataObject &cond;
      /// reference to the string where to put the name of the condition
      std::string &condition_name;
      /// temporary object to prepare the parameter before insertion
      TempParameter tmp;
    
      /// commodity rules
      rule<ScannerT> identifier, name, parameter, type[NTypes], value[NTypes];
      rule<ScannerT> spaces, eq, sep;
      /// main rule
      rule<ScannerT> condition;
      
      /// constructor
      definition ( const ConditionGrammar &self ):
        cond(const_cast<ConditionGrammar &>(self).condition),
        condition_name(const_cast<ConditionGrammar &>(self).name)
      {
        // an identifier is a sequence of alphanumeric characters + '/' + '_' + '-'
        identifier = *(alnum_p | ch_p('/') | ch_p('_') | ch_p('-') | ch_p(':'));
        
        // name of a parameter
        name = identifier[TempParameter::SetName(tmp)];

        // I define the keyword used to identify the types
        // basic types:
        type[Integer]       = str_p("int");
        type[Double]        = (str_p("double") | str_p("dbl"));
        type[String]        = (str_p("string") | str_p("str"));
        // vectors: (vectors need a special action to create the container before parsing the values)
        type[IntegerVector] = str_p("int_v")
                              [TempParameter::AddVector<int>(tmp)];
        type[DoubleVector]  = (str_p("double_v") | str_p("dbl_v"))
                              [TempParameter::AddVector<double>(tmp)];
        type[StringVector]  = (str_p("string_v") | str_p("str_v"))
                              [TempParameter::AddVector<std::string>(tmp)];
      
        // just to simplify
        spaces = *space_p;
        eq = ch_p('=');
        sep = ch_p(';');
      
        // descriptions of the values and relative actions
        value[Integer]       = int_p
                               [TempParameter::SetValue<int>(tmp)];
        value[Double]        = real_p
                               [TempParameter::SetValue<double>(tmp)];
        value[String]        = ((+alnum_p) % spaces)
                               [TempParameter::SetValueString(tmp)];
        // vectors are space separated lists
        value[IntegerVector] = int_p[TempParameter::PushBack<int>(tmp)] % spaces;
        value[DoubleVector]  = real_p[TempParameter::PushBack<double>(tmp)] % spaces;
        value[StringVector]  = (+~space_p)[TempParameter::PushBackString(tmp)] % spaces;
      
        // Definition of the parameter
        parameter =
          (
           // vector<int>
           ( type[IntegerVector] >> spaces >> name >> spaces >> eq >> spaces >> value[IntegerVector] ) |
           // vector<double>
           ( type[DoubleVector] >> spaces >> name >> spaces >> eq >> spaces >> value[DoubleVector] ) |
           // vector<string>
           ( type[StringVector] >> spaces >> name >> spaces >> eq >> spaces >> value[StringVector] ) |
           // int
           ( type[Integer] >> spaces >> name >> spaces >> eq >> spaces >> value[Integer] ) |
           // double
           ( type[Double] >> spaces >> name >> spaces >> eq >> spaces >> value[Double] ) |
           // string
           ( type[String] >> spaces >> name >> spaces >> eq >> spaces >> value[String] )
           )
          // add the parameter to the condition
          [TempParameter::AddParameter(cond,tmp)];
      
        // the whole condition
        condition =
          // name (path) of the condition 
          identifier[SetString(condition_name)] >> spaces >>
          // assignment operator (from Pascal)
          str_p(":=") >> spaces >>
          // sequence of parameters separated by (regexp) ' *; *'
          ( parameter % ( spaces >> sep >> spaces ) ) >>
          // optional closing (regexp) ' *(; *)?'
          spaces >> !(sep >> spaces); 
      }
    
      /// function used to access the main rule.
      rule<ScannerT> const& start() const { return condition; }
    };
  };
}

//======================================================================
// function to parse the string describing a condition
//======================================================================
bool ConditionParser(const std::string &str, std::string &name, ParamValidDataObject &cond){
  ConditionGrammar g;
  // TODO: improve error messages
  if ( parse(str.c_str(),g).full ){
    name = g.name;
    cond.update(g.condition);
    return true;
  }
  return false;
}
