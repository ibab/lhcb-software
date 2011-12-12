// $Id: MicroDSTAlgorithm.h,v 1.5 2009-08-18 09:58:45 jpalac Exp $
#ifndef MICRODSTALGORITHM_H 
#define MICRODSTALGORITHM_H 1

// Include files
// from Gaudi
#include <GaudiAlg/GaudiAlgorithm.h>
#include <GaudiKernel/IRegistry.h>
// Local
#include <MicroDST/MicroDSTCommon.h>
#include <MicroDST/Functors.hpp>


class ObjectContainerBase;

/** @class MicroDSTAlgorithm MicroDSTAlgorithm.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-10-11
 */
class GAUDI_API MicroDSTAlgorithm : public MicroDSTCommon<GaudiAlgorithm>
{

public: 

  /// Standard constructor
  MicroDSTAlgorithm( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MicroDSTAlgorithm( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  inline const std::string& inputTESLocation() const 
  {
    return m_inputTESLocations.empty() 
      ? m_inputTESLocation 
      : m_inputTESLocations[0];
  }

  inline const std::vector<std::string>& inputTESLocations()
  {
    return m_inputTESLocations;
  }


  inline void setInputTESLocation(const std::string& newLocation) 
  {
    m_inputTESLocations.clear();
    m_inputTESLocations.push_back(newLocation);
  }

private:

  std::string m_inputTESLocation;
  std::vector<std::string> m_inputTESLocations;

};
#endif // COPYANDSTOREDATA_H
