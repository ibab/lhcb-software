// $Id: IParticleTisTos.h,v 1.1 2010-07-21 21:19:26 tskwarni Exp $
#ifndef IPARTICLETISTOS_H
#define IPARTICLETISTOS_H 1

// Include files
// from STL
#include <vector>
#include <string>
#include "boost/foreach.hpp"

#include "Kernel/ITisTos.h"

#include "Kernel/LHCbID.h"

//forward declarations
namespace LHCb
{
  class Particle;
  class ProtoParticle;
  class Track;
  class RecVertex;
  class Vertex;
  class HltObjectSummary;
}

/** @class IParticleTisTos IParticleTisTos.h
 *
 *
 *  @author Tomasz Skwarnicki
 *  @date   2010-07-08
 *
 * @par Tool purpose
 *   This tool extends TisTos operations described in @c ITisTos (for LHCbID hit lists)
 *   to reconstruction objects: Particle,  RecVertex, Track and their vectors
 */
class GAUDI_API IParticleTisTos : virtual public ITisTos
{

public:

  /// Interface ID
  DeclareInterfaceID(IParticleTisTos, 1, 0);

  /// Destructor
  virtual ~IParticleTisTos() { }

public:

  // -------------------------------------------------
  // ------------ inputs
  // -------------------------------------------------


  /// Particle input; for composite particles loop over daughters will be executed (true if Signal changed)
  virtual bool addToSignal( const LHCb::Particle & particle ) = 0;

  /// Proto-particle input
  virtual bool addToSignal( const LHCb::ProtoParticle & protoParticle ) = 0;

  /// Track input
  virtual bool addToSignal( const LHCb::Track & track ) = 0;


  /// Hits input
  virtual bool addToSignal( const std::vector<LHCb::LHCbID> & hits ) =0;



  // -------------------------------------------------
  // ------------ outputs
  // -------------------------------------------------

  /// completely classify the Trigger object with respect to the Signal (see ITisTis::TisTosTob for the meaning)
  virtual unsigned int tisTos(const LHCb::Particle & particle)  =0;
  virtual unsigned int tisTos(const LHCb::RecVertex & recVertex )  =0;
  virtual unsigned int tisTos(const LHCb::Vertex & vertex )  =0;
  virtual unsigned int tisTos(const LHCb::Track & track ) =0;
  virtual unsigned int tisTos(const std::vector<LHCb::LHCbID> & hits ) =0;
  virtual unsigned int tisTos(const LHCb::HltObjectSummary & hos ) =0;


  template<class T>
  unsigned int tisTos( const std::vector<T*> & list )
  { 
    unsigned int result(0);
    BOOST_FOREACH( const T* obj, list )
    {
      result |= tisTos( *obj );
      if ( (result&kTPS) && (result&kTOS) && (result&kTIS) ) break;
    }
    return result;
  }

  /// completely classify the Trigger hit sequence with respect to the Signal, return helper class
  template <class T>
  TisTosTob tisTosTob(const T& obj) { return TisTosTob( tisTos(obj) ); }

  template<class T>
  unsigned int tisTosTob( const std::vector<T*> & list )
  {
    unsigned int result(0);
    BOOST_FOREACH( const T* obj, list )
    {
      result |= tisTos( *obj );
      if( (result&kTPS) && (result&kTOS) && (result&kTIS) )break;
    }
    return result;
  }

  /// check for TOS  - may be faster than using tisTos()
  virtual bool tos(const LHCb::Particle & particle) =0;
  virtual bool tos(const LHCb::RecVertex & recVertex) =0;
  virtual bool tos(const LHCb::Vertex & vertex) =0;
  virtual bool tos(const LHCb::Track & track ) =0;
  virtual bool tos(const std::vector<LHCb::LHCbID> & hits ) =0;
  virtual bool tos(const LHCb::HltObjectSummary & hos ) =0;

  template<class T>
  bool tos( const std::vector<T*> & list )
  {
    bool OK = false;
    BOOST_FOREACH( const T* obj, list ) 
    {
      if ( tos(*obj) ) { OK = true; break; }
    }
    return OK;
  }

  /// check for TIS  - may be faster than using tisTos()
  virtual bool tis(const LHCb::Particle & particle) =0;
  virtual bool tis(const LHCb::RecVertex & recVertex) =0;
  virtual bool tis(const LHCb::Vertex & vertex) =0;
  virtual bool tis(const LHCb::Track & track ) =0;
  virtual bool tis(const std::vector<LHCb::LHCbID> & hits ) =0;
  virtual bool tis(const LHCb::HltObjectSummary & hos ) =0;

  template<class T>
  bool tis( const std::vector<T*> & list )
  {
    bool OK = false;
    BOOST_FOREACH( const T* obj, list )
    {
      if ( tis(*obj) ) { OK = true; break; }  
    }
    return OK;
  }

  /// check for TUS  (TPS or TOS) - may be faster than using tisTos()
  virtual bool tus(const LHCb::Particle & particle) =0;
  virtual bool tus(const LHCb::RecVertex & recVertex) =0;
  virtual bool tus(const LHCb::Vertex & vertex) =0;
  virtual bool tus(const LHCb::Track & track ) =0;
  virtual bool tus(const std::vector<LHCb::LHCbID> & hits ) =0;
  virtual bool tus(const LHCb::HltObjectSummary & hos ) =0;


  template<class T>
  bool tus( const std::vector<T*> & list )
  {
    bool OK = false;
    BOOST_FOREACH( const T* obj, list )
    { 
      if ( tus(*obj) ) { OK = true; break; }
    }
    return OK;
  }

  /// analysis string
  virtual std::string analysisReport(const LHCb::Particle & particle) =0 ;
  virtual std::string analysisReport(const LHCb::RecVertex & recVertex) =0 ;
  virtual std::string analysisReport(const LHCb::Vertex & vertex) =0 ;
  virtual std::string analysisReport(const LHCb::Track & track ) =0;
  virtual std::string analysisReport(const std::vector<LHCb::LHCbID> & hits ) =0;
  virtual std::string analysisReport(const LHCb::HltObjectSummary & hos ) =0;


  // --------------------- control calls -------------------------

  virtual void setProjectTracksToCalo(bool onOff) =0;
  virtual void setCaloClustForCharged(bool onOff) =0;
  virtual void setCaloClustForNeutral(bool onOff) =0;
  virtual void setCompositeTPSviaPartialTOSonly(bool onOff) =0;
  virtual void setFullAnalysisReport(bool onOff) =0;

  virtual bool getProjectTracksToCalo() const =0;
  virtual bool getCaloClustForCharged() const =0;
  virtual bool getCaloClustForNeutral() const =0;
  virtual bool getCompositeTPSviaPartialTOSonly() const =0;
  virtual bool getFullAnalysisReport() const =0;

};

#endif // IPARTICLETISTOS_H
