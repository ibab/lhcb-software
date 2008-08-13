//$Id: HltSelectionCombinerFactory.h,v 1.1 2008-08-13 07:14:53 graven Exp $
#ifndef HLTSELECTIONCOMBINERFACTORY_H 
#define HLTSELECTIONCOMBINERFACTORY_H 1

#include "GaudiAlg/GaudiTool.h"
#include "HltBase/IHltSelectionCombinerFactory.h"
#include <string>

class HltAlgorithm;


class HltSelectionCombinerFactory : public GaudiTool ,
                                    virtual public IHltSelectionCombinerFactory
{
public:

    HltSelectionCombinerFactory( const std::string& type,
                                   const std::string& name,
                                   const IInterface* parent);

    virtual ~HltSelectionCombinerFactory();


    IHltSelectionCombinerFactory::selectionCombiner* 
        create(const std::string& specification, HltAlgorithm& parent) const;
};
#endif
