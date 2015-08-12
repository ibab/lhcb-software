// $Id:$ 
// ============================================================================
#include "LoKi/DictTransform.h"
// ============================================================================
#include <iostream>
#include <boost/foreach.hpp>
// ============================================================================
typedef std::map<std::string,std::string> optmap;
// ============================================================================
/** @class DummyTransform
 *  Implements a prototype dictionary transform to be used with
 *  DictTransform
 *  fulfilling the following policy
 *  TransForm::Init(const std::map<std::string,std::string>& options, ostream& info)
 *  TransForm::operator()(const IParticleDictTool::DICT& inDict, IParticleDictTool::Dict& outDict) const
 *
 *  The dummy transform will return the unchanged source dictionary and print its contents
 *
 *  @author Sebastian Neubert
 *  @date   2013-07-10
 */


class DummyTransform 
{
private:
  
  bool m_debug;
  
public:
  
  DummyTransform() : m_debug(false) { }
  
  bool Init(const optmap& options, std::ostream& info, const bool debug = false );
  bool operator()(const IParticleDictTool::DICT& in, IParticleDictTool::DICT& out) const;

};

///---------------------------------------------------------------------------------------
bool
DummyTransform::Init(const optmap& options, std::ostream& info, const bool debug ){
  m_debug = debug;
  /// do something to configure the tool
  // print options
  BOOST_FOREACH(optmap::value_type opt, options){
    if ( m_debug ) info << opt.first << " : " << opt.second << std::endl;
  }

  if ( m_debug ) info << "Initializing DummyTransform" << std::endl;
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
    if ( m_debug ) std::cout <<  item->first << "    :    " << item->second << std::endl;
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
