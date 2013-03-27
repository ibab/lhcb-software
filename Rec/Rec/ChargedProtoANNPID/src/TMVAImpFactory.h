#ifndef TMVAIMPFACTORY_H 
#define TMVAIMPFACTORY_H 1

// gaudi
#include "GaudiKernel/GaudiException.h"

// STL
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <cmath>

namespace ANNGlobalPID
{

#ifndef IClassifierReader__def
#define IClassifierReader__def
  /** @class TMVAImpFactory TMVAImpFactory.h
   *
   *  Interface class for TMVA C++ implementation of networks
   */
  class IClassifierReader
  {
  public:
    /// constructor
    IClassifierReader() : fStatusIsClean( true ) { }
    /// Destructor
    virtual ~IClassifierReader() { }
    /// return classifier response
    virtual double GetMvaValue( const std::vector<double>& inputValues ) const = 0;
    /// returns classifier status
    bool IsStatusClean() const { return fStatusIsClean; }
  protected:
    bool fStatusIsClean; ///< Status flag
  };
#endif

  /** @class TMVAImpFactory TMVAImpFactory.h
   *
   *  Factory to create instances of standalone C++ TVMA objects for
   *  a given track, hypothesis and network tuning.
   *
   *  @author Chris Jones
   *  @date   2013-03-12
   */
  class TMVAImpFactory
  {

  private:

    /** @class FactoryBase TMVAImpFactory.h
     *
     *  Base class for factories
     *
     *  @author Chris Jones
     *  @date   2013-03-12
     */
    class FactoryBase
    {
    public:
      /// Create an instance of the TMVA classifier for this factory
      virtual IClassifierReader* create( std::vector<std::string> inputs ) = 0;
      /// Destructor
      virtual ~FactoryBase() { }
    };

    /** @class TMVAFactory TMVAImpFactory.h
     *
     *  Templated class for specific TMVA factories
     *
     *  @author Chris Jones
     *  @date   2013-03-12
     */
    template <class TMVATYPE>
    class TMVAFactory : public FactoryBase
    {
    public:
      /// Create an instance of the TMVA classifier for this factory
      inline IClassifierReader* create( std::vector<std::string> inputs ) 
      {
        return new TMVATYPE(inputs); 
      }
      /// Destructor
      virtual ~TMVAFactory() { }
    };
    
  public:

    /// Standard constructor
    TMVAImpFactory( );

    /// Destructor
    ~TMVAImpFactory( );

  private:

    /// Returns the id string for a given configuration
    inline std::string id( const std::string & config,
                           const std::string & particle,
                           const std::string & track ) const
    {
      return config+"-"+particle+"-"+track ;
    }

    /// Register a new TMVA instance with the factory
    template <class TMVATYPE>
    inline void add( const std::string & config,
                     const std::string & particle,
                     const std::string & track )
    {
      const std::string _id = id(config,particle,track);
      Map::const_iterator i = m_map.find( _id );
      if ( i != m_map.end() )
      {
        throw GaudiException( _id+" already registered", 
                              "ANNGlobalPID::TMVAImpFactory",
                              StatusCode::FAILURE );
      }
      m_map[ _id ] = new TMVAFactory<TMVATYPE>();
    }

  public:

    /// Get an instance for a given set of parameters
    inline 
    IClassifierReader* create( const std::string & config,
                               const std::string & particle,
                               const std::string & track,
                               const std::vector<std::string>& inputs ) const
    {
      Map::const_iterator i = m_map.find( id(config,particle,track) );
      return ( i != m_map.end() ? i->second->create(inputs) : NULL );
    }
    
  private:

    /// Type for internal map
    typedef std::map< std::string, FactoryBase* > Map;

    /// The map
    Map m_map;

  };

  /// Method to get a static instance of the factory
  const TMVAImpFactory& tmvaFactory();

}

#endif // TMVAIMPFACTORY_H
