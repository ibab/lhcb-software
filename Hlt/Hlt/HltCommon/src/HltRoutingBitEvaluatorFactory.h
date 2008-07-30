//$Id: HltRoutingBitEvaluatorFactory.h,v 1.1 2008-07-30 13:37:32 graven Exp $
#ifndef HLTROUTINGBITEVALUATORFACTORY_H 
#define HLTROUTINGBITEVALUATORFACTORY_H 1

#include "GaudiAlg/GaudiTool.h"
#include <string>

class HltAlgorithm;

static const InterfaceID IID_HltRoutingBitEvaluatorFactory("HltRoutingBitEvaluatorFactory",1,0);

class HltRoutingBitEvaluatorFactory : public GaudiTool {
public:
    static const InterfaceID& interfaceID() { return IID_HltRoutingBitEvaluatorFactory; }

    HltRoutingBitEvaluatorFactory( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

    virtual ~HltRoutingBitEvaluatorFactory();

    struct evaluator {
       virtual ~evaluator();
       virtual bool evaluate() const = 0;
       virtual std::ostream& print(std::ostream&)    const = 0;
    };

    evaluator* create(const std::string& specification, HltAlgorithm& parent) const;
};
#endif
