//$Id: IHltSelectionCombinerFactory.h,v 1.1 2008-08-13 07:11:15 graven Exp $
#ifndef HLTROUTINGBITEVALUATORFACTORY_H 
#define HLTROUTINGBITEVALUATORFACTORY_H 1

#include <string>
#include "GaudiKernel/IAlgTool.h"

class HltAlgorithm;

static const InterfaceID IID_IHltSelectionCombinerFactory("IHltSelectionCombinerFactory",1,0);

class IHltSelectionCombinerFactory : virtual public IAlgTool {
public:
    static const InterfaceID& interfaceID() { return IID_IHltSelectionCombinerFactory; }

    virtual ~IHltSelectionCombinerFactory() {}

    struct selectionCombiner {
       virtual ~selectionCombiner() {};
       virtual bool evaluate() const = 0;
       virtual std::ostream& print(std::ostream&) const = 0;
    };

    // create a 'combiner' object according to the 'specification'
    // which performs a boolean combination of Hlt::Selections.
    // Recognized operators are unary '()', and '!',
    // and binary '&' and '|'. Usual C shortcircuit & precedence rules apply
    virtual selectionCombiner* create(const std::string& specification, HltAlgorithm& parent) const = 0;
};
#endif
