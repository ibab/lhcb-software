// $Id: ContainerClonerAlg.h,v 1.3 2010-05-26 09:56:56 jpalac Exp $
#ifndef MICRODST_CONTAINERCLONERALG_H 
#define MICRODST_CONTAINERCLONERALG_H 1

// Include files
// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
// From MicroDST
#include "MicroDST/MicroDSTAlgorithm.h"
#include "MicroDST/BindType2ClonerDef.h"
#include "MicroDST/Defaults.h"
/** @class ContainerClonerAlg ContainerClonerAlg.h MicroDST/ContainerClonerAlg.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2008-08-29
 */

namespace MicroDST 
{

template <typename T>
class ContainerClonerAlg : public MicroDSTAlgorithm {

private:
  typedef Defaults<T> DEFAULTS;
  typedef Location<T> LOCATION;
  typedef typename BindType2Cloner<T>::cloner CLONER;
public:

  /// Standard constructor
  ContainerClonerAlg( const std::string& name, ISvcLocator* pSvcLocator )
    :
    MicroDSTAlgorithm ( name , pSvcLocator ),
    m_cloner(0),
    m_clonerType(DEFAULTS::clonerType)
  {
    declareProperty("ClonerType", m_clonerType);
  }

  ~ContainerClonerAlg( ) {} ///< Destructor

  StatusCode initialize() 
  {
    StatusCode sc = MicroDSTAlgorithm::initialize(); // must be executed first

    debug() << "==> Initialize" << endmsg;

    if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

    if ( inputTESLocation()=="")  {
      verbose() << "Setting input TES location default: " 
                << LOCATION::Default << endmsg;
      setInputTESLocation(LOCATION::Default);
    }
    verbose() << "inputTESLocation() is " << inputTESLocation() << endmsg;

    m_cloner = tool<CLONER>(m_clonerType, this);

    if (m_cloner) {
      debug() << "Found cloner " << m_clonerType << endmsg;
    } else {
      sc = Error("Failed to find cloner " + m_clonerType,
                 StatusCode::FAILURE, 10);
    }

    return sc;

  }
  
  StatusCode execute() 
  {
    
    debug() << "==> Execute" << endmsg;

    typedef std::vector<std::string>::const_iterator stringIter;
    stringIter locBegin = this->inputTESLocations().begin();
    stringIter locEnd = this->inputTESLocations().end();
    
    for (stringIter iLoc = locBegin; iLoc != locEnd; ++iLoc) {

      const std::string inputLocation = MicroDST::niceLocationName(*iLoc);
      const std::string outputLocation = 
        this->outputTESLocation( inputLocation );
        
      verbose() << "Going to clone Container from " << inputLocation
                << " into " << outputLocation << endmsg;

      const typename T::Container* cont = 
        copyContainer<T, CLONER>(inputLocation, m_cloner );

      if (0==cont) {
        Warning("Unable clone or get container from "+ inputLocation,
                StatusCode::FAILURE, 0).ignore();
      } else {
        verbose() <<"Cloned Container " << outputLocation 
                  << " has " << cont->size() 
                  << " elements." << endmsg;
      }
      
    }

    setFilterPassed(true);
    return StatusCode::SUCCESS;

  }
  
  StatusCode finalize() 
  {
    debug() << "==> Finalize" << endmsg;
    return MicroDSTAlgorithm::finalize();
  }
  
private:

  CLONER* m_cloner;
  std::string m_clonerType;
  
};
 
}

#endif // MICRODST_CONTAINERCLONERALG_H
