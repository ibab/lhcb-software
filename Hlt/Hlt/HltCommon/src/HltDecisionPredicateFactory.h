//$Id: HltDecisionPredicateFactory.h,v 1.1 2008-09-04 12:24:37 graven Exp $
#ifndef HLTDECISIONPREDICATEFACTORY_H 
#define HLTDECISIONPREDICATEFACTORY_H 1

#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltDecisionPredicateFactory.h"
#include <string>


class HltDecisionPredicateFactory : public GaudiTool ,
                                    virtual public IHltDecisionPredicateFactory
{
public:

    HltDecisionPredicateFactory( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

    virtual ~HltDecisionPredicateFactory();


    IHltDecisionPredicateFactory::decisionPredicate* 
        create(const std::string& specification) const;
};
#endif
