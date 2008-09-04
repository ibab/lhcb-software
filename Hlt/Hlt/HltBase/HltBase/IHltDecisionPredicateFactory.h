//$Id: IHltDecisionPredicateFactory.h,v 1.1 2008-09-04 12:24:02 graven Exp $
#ifndef HLTDECISIONCOMBINERFACTORY_H 
#define HLTDECISIONCOMBINERFACTORY_H 1

#include <string>
#include <functional>
#include "GaudiKernel/IAlgTool.h"

namespace LHCb { class HltDecReports; };

static const InterfaceID IID_IHltDecisionPredicateFactory("IHltDecisionPredicateFactory",1,0);

class IHltDecisionPredicateFactory : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID() { return IID_IHltDecisionPredicateFactory; }

    virtual ~IHltDecisionPredicateFactory() {}

    struct decisionPredicate : std::unary_function<const LHCb::HltDecReports&,bool>{
       virtual ~decisionPredicate() {};
       virtual bool operator()(const LHCb::HltDecReports&) const = 0;
       virtual std::ostream& print(std::ostream&) const = 0;
    };

    // create a 'Predicate' object according to the 'specification'
    // which performs a boolean combination based on the contents of Hlt::DecReports
    // Recognized operators are unary '()', and '!',
    // and binary '&' and '|'. Usual C shortcircuit & precedence rules apply
    virtual decisionPredicate* create(const std::string& specification) const = 0;
};
#endif
