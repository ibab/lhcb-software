#ifndef TMVAIMPFACTORY_H 
#define TMVAIMPFACTORY_H 1

// gaudi
#include "GaudiKernel/GaudiException.h"

// STL
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <memory>
#include <limits>

// Include VDT here so it gets into all TMVA compilations
//#include "vdt/exp.h"

// TMVA IClassifierReader interface
#include "TMVAIClassifierReader.h"

namespace ANNGlobalPID
{

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

  public:

    // vector of input names
    typedef std::vector<std::string> InputNames;

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
      virtual std::unique_ptr<IClassifierReader> create( const InputNames& inputs ) = 0;
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
      std::unique_ptr<IClassifierReader> create( const InputNames& inputs ) override
      {
        return std::unique_ptr<IClassifierReader>( new TMVATYPE(inputs) ); 
      }
      /// Destructor
      //virtual ~TMVAFactory() { }
    };
    
  public:

    /// Standard constructor
    TMVAImpFactory( );

  private:

    // methods to declare new MVAs to the factory during construction
    // Seperated out to allow seperate compilation units (as the MVAs 
    // themselves can be heavy so compiling all at once does not scale well).

    /// Add MC12TuneV2 networks
    void addMC12TuneV2( );

    /// Add MC12TuneV3 networks
    void addMC12TuneV3( );

    /// Add Bs->MuMu Developmental V1 networks
    void addBs2MuMuDev1( );

    /// Add Bs->MuMu Developmental V2 networks
    void addBs2MuMuDev2( );
    
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
      const auto _id = id(config,particle,track);
      const auto i = m_map.find( _id );
      if ( i != m_map.end() )
      {
        throw GaudiException( _id+" already registered", 
                              "ANNGlobalPID::TMVAImpFactory",
                              StatusCode::FAILURE );
      }
      m_map[ _id ].reset( new TMVAFactory<TMVATYPE>() );
    }

  public:

    /// Get an instance for a given set of parameters
    inline 
    std::unique_ptr<IClassifierReader> create( const std::string & config,
                                               const std::string & particle,
                                               const std::string & track,
                                               const std::vector<std::string>& inputs ) const
    {
      const auto i = m_map.find( id(config,particle,track) );
      return ( i != m_map.end() ? i->second->create(inputs) : nullptr );
    }
    
  private:

    /// Type for internal map
    typedef std::unordered_map< std::string, std::unique_ptr<FactoryBase> > Map;

  private:

    /// The map of known MVA instance generators linked to a given nickname
    Map m_map;

  };

  /// Method to get a static instance of the factory
  const TMVAImpFactory& tmvaFactory();

}

#endif // TMVAIMPFACTORY_H
