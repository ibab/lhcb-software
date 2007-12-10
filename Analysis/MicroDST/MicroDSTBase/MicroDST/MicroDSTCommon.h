// $Id: MicroDSTCommon.h,v 1.1.1.1 2007-12-10 09:12:42 jpalac Exp $
#ifndef MICRODST_MICRODSTCOMMON_H 
#define MICRODST_MICRODSTCOMMON_H 1

// Include files
#include <MicroDST/Types.h>
#include <MicroDST/Functors.hpp>


class ISvcLocator;
class IInterface;

/** @class MicroDSTCommon MicroDSTCommon.h MicroDST/MicroDSTCommon.h
 *  
 *
 *  @author Juan PALACIOS
 *  @date   2007-11-30
 */
template <class PBASE>
class MicroDSTCommon : public PBASE {
public: 
  /// Standard constructor

  MicroDSTCommon();

  /// Standard algorithm-like constructor
  MicroDSTCommon( const std::string& name,
                  ISvcLocator* pSvcLocator )
    : 
    PBASE(name, pSvcLocator),
    m_inputTESLocation(""),
    m_outputPrefix("microDST")
  {
    this->declareProperty( "InputLocation", m_inputTESLocation );
    this->declareProperty( "OutputPrefix",  m_outputPrefix  );
  }
  

  /// Standard tool-like constructor
  MicroDSTCommon( const std::string& type,
                  const std::string& name,
                  const IInterface* parent )
    : 
    PBASE(type, name, parent),
    m_inputTESLocation(""),
    m_outputPrefix("microDST")
  {
    this->declareProperty( "InputLocation", m_inputTESLocation );
    this->declareProperty( "OutputPrefix",  m_outputPrefix  );
  }
  

  virtual ~MicroDSTCommon( ); ///< Destructor

  virtual StatusCode initialize();
  
  virtual StatusCode finalize();

  /**
   * Copy an object of type T from the TES onto another TES location.
   * Uses the copy constructor of type T for copying.
   * 
   * @param from The TES location of the object to be copied
   * @param to   The TES location where the copy should be stored
   * @return     Const pointer to the cloned object.
   *
   * @author Juan Palacios juancho@nikhef.nl
   * @author Ulrich Kerzel
   */
  template <class T, class CopyFunctor>
  const T* copyAndStoreObject( const std::string& from,
                               const std::string& to   );
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const std::string& from,
                               const std::string& to    );


  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const T* from, T* to    ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* copyKeyedContainer( const std::string& from, 
                               ContainedItemCloner* cloner    ) ;



  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class ContainedItemCloner >
  const T* cloneContainerToLocalStore( const std::string& from ) ;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class itemCloner>
  T* cloneKeyedContainerItem( const T* item );

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  const T* getStoredClone( const T* original    ) const;

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getStoredClone( const T* original    ) ;


  /**
   * Get the TES container in the TES location to be stored on the
   * MicroDST. 
   * @param locTES The address in the original TES
   * @return pointer to the container in the MicroDST TES
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getOutputContainer( const std::string& location );

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  const T* getInputContainer( const std::string& location ) ;


  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T, class itemCloner>
  T* cloneKeyedItemToLocalStore( const T* item );

  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getLocalClone( const T* original    ) ;
  
  /**
   *
   * @author Juan Palacios juancho@nikhef.nl
   */
  template <class T>
  T* getLocalContainer( const std::string& location );

  template <class T>
  inline bool exist(const std::string& location) {
    return PBASE:: template exist<T>(location);
  }

  template <class T>
  inline T* get(const std::string& location) {
    return PBASE:: template get<T>(location);
  }


protected:

  inline MicroDST::DataStore* localDataStore() 
  {
    return m_store;
  }
  
  inline const std::string& inputTESLocation() const 
  {
    return m_inputTESLocation;
  }

  inline std::string& inputTESLocation()
  {
    return m_inputTESLocation;
  }

  inline const std::string& outputPrefix() const 
  {
    return m_outputPrefix;
  }

  inline const std::string& fullOutputTESLocation() const 
  {
    return m_fullOutputTESLocation;
  }

  inline std::string& fullOutputTESLocation() 
  {
    return m_fullOutputTESLocation;
  }

  inline void setInputTESLocation(const std::string& newLocation) 
  {
    this->inputTESLocation() = newLocation;
    setFullOutputTESLocation();
  }

  inline void setFullOutputTESLocation() 
  {
    this->fullOutputTESLocation() = "/Event/"+ this->outputPrefix() + 
      "/" + this->inputTESLocation();
  }

  inline const std::string outputTESLocation(const std::string& inputLocation) const
  {
    return "/Event/"+ this->outputPrefix() + "/" + 
      MicroDST::niceLocationName(inputLocation);
  }


protected:

private:

  std::string m_inputTESLocation;
  std::string m_outputPrefix;
  std::string m_fullOutputTESLocation;
  MicroDST::DataStore* m_store;
  
};
// templated method implementations.
#include "MicroDSTCommon.icpp"
#endif // MICRODST_MICRODSTCOMMON_H
