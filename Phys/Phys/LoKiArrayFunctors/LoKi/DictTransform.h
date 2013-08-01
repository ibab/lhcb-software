#ifndef DICTTRANSFORMTOOL_H 
#define DICTTRANSFORMTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IParticleDictTool.h"            // Interface used by this tool
#include "LoKi/PhysTypes.h"

// std::lib
#include <map>
#include <string>

namespace LoKi 
{
  namespace Hybrid 
  {
    
/** @class DictTransform
 *  Generic implementation of a Dict->Dict Transformation
 *  The input dictionairy is aquired through an IParticleDictTool
 *  which has to be added to the tool
 *  the transformed dictionary is returned through IParticleDictTool
 *  The transformation is templated
 *  The template parameter Transform needs to fulfill the following policy
 *  TransForm::Init(const std::map<std::string,std::string>& options)
 *  TransForm::operator()(const IParticleDictTool::DICT& inDict, IParticleDictTool::Dict& outDict) const
 *
 *  @author Sebastian Neubert
 *  @date   2013-07-10
 */

template<class Transform>
class DictTransform : public GaudiTool, virtual public IParticleDictTool {
public: 
 
// ======================================================================    
  /// Standard constructor
  DictTransform( const std::string& type, 
		 const std::string& name,
		 const IInterface* parent) 
    : GaudiTool ( type, name , parent ), m_transform()
  {
    declareInterface<IParticleDictTool>(this);
    
    declareProperty 
      ( "Source" , m_sourcename , 
	"Type/Name for Dictionary Tool" );

    declareProperty
      ( "Options" , m_options ,
	"dictionary of options passed to the Transform"); 
  
  }

// ======================================================================    

  virtual StatusCode initialize()
  {
    StatusCode sc = GaudiTool::initialize() ;
    if ( sc.isFailure() ) {  return sc ; }               // RETURN 
    // acquire the DictTool
    m_source = tool<IParticleDictTool>(m_sourcename, this);
    if(m_source == NULL) return Error("Unable to find the source DictTool " + m_sourcename , sc ) ;

    
    if(!m_transform.Init(m_options,info())){
      Error("Initialization of Transform failed.",StatusCode::FAILURE);
    }
  
    Info("Successfully initialized DictTransform");
    return StatusCode::SUCCESS;
  }
  
// ======================================================================    

  virtual StatusCode  fill ( const LHCb::Particle* p , 
                             IParticleDictTool::DICT& dict ) const {

  /// request the dictionary s from the source DictTool
  IParticleDictTool::DICT sourceDict;
  m_source->fill(p,sourceDict).ignore();

  /// apply the transformation to the dictionary
  /// this is delegated to the Transform
  m_transform(sourceDict,dict);
    
  return StatusCode::SUCCESS;
  }

// ======================================================================    

  virtual ~DictTransform( ){}; ///< Destructor

// ======================================================================    

protected:
  
  IParticleDictTool* m_source;
  std::string m_sourcename;
  
  /// Templated worker class to encapsulate the transformation algorithm
  Transform m_transform;  
  std::map<std::string,std::string> m_options;
  
};
  } //end namespace Hybrid
} //end namespace LoKi

#endif 
