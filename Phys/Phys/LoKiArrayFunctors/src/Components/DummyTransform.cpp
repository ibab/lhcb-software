#include "LoKi/DictTransform.h"
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/MsgStream.h"

#include <iostream>
#include <boost/foreach.hpp>

typedef std::map<std::string,std::string> optmap;


/** @class DummyTransform
 *  Implements a prototype dictionary transform to be used with
 *  DictTransform
 *  fulfilling the following policy
 *  TransForm::Init(const std::map<std::string,std::string>& options, MsgStream& info)
 *  TransForm::operator()(const IParticleDictTool::DICT& inDict, IParticleDictTool::Dict& outDict) const
 *
 *  The dummy transform will return the unchanged source dictionary and print its contents
 *
 *  @author Sebastian Neubert
 *  @date   2013-07-10
 */


class DummyTransform {
public:
 
  bool Init(optmap options, MsgStream& info);
  bool operator()(const IParticleDictTool::DICT& in, IParticleDictTool::DICT& out) const;
  
};

///---------------------------------------------------------------------------------------
bool
DummyTransform::Init(optmap options, MsgStream& info){
  /// do something to configure the tool 
  // print options
  BOOST_FOREACH(optmap::value_type opt, options){
    info << opt.first << " : " << opt.second << endmsg;
  }

  info << "Initializing DummyTransform" << endmsg;
  return true; // true signals success
}
///---------------------------------------------------------------------------------------
bool
DummyTransform::operator()(const IParticleDictTool::DICT& in, IParticleDictTool::DICT& out) const{
  /// most simple transformation is the unit transformation

  // loop over items and print names and values
  // Put the items in the dictionaire into the tuple
  for ( IParticleDictTool::DICT::const_iterator item = in.begin() ; 
        in.end() != item ; ++item ) 
  {
    // fill N-tuple 
    std::cout <<  item->first << "    :    " << item->second << std::endl; 
  }
  //

  out = in;
  return true; // true signals success;
}
///---------------------------------------------------------------------------------------

///---------------------------------------------------------------------------------------
/** @class DictTransformDummy
 *  Implements a concrete DictTransform 
 *  @author Sebastian Neubert
 *  @date   2013-07-10
 */

typedef LoKi::Hybrid::DictTransform<DummyTransform> DictTransformDummy;
DECLARE_TOOL_FACTORY(DictTransformDummy)
///---------------------------------------------------------------------------------------
