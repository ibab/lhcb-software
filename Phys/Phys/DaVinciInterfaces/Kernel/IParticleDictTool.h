#ifndef IPARTICLEDICTTOOL_H 
#define IPARTICLEDICTTOOL_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/VectorMap.h"
#include "LoKi/PhysTypes.h"

static const InterfaceID IID_IParticleDictTool ( "IParticleDictTool", 1, 0 );

/** @class IParticleDictTool IParticleDictTool.h
 *  returns a dictionary of variable-names and their values
 *
 *  @author Sebastian Neubert
 *  @date   2013-07-08
 *
 */

class IParticleDictTool : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_IParticleDictTool; }

  typedef GaudiUtils::VectorMap<std::string,double> DICT;
  
  /** fill the dictionary: 
   *  @param p (INPUT) the particle
   *  @param dict (UPDATE) the dictionary to be filled  
   */
  virtual StatusCode  fill ( const LHCb::Particle* p , DICT& dict ) const =0;



};
#endif // IPARTICLEDICTTOOL_H
