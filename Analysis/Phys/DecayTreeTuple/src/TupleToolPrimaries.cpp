// $Id: TupleToolPrimaries.cpp,v 1.7 2010-02-09 09:40:49 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolPrimaries.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"
#include "Kernel/DVAlgorithm.h"

#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/DVAlgorithm.h>
#include <Event/RecVertex.h>

#include <functional>

//-----------------------------------------------------------------------------
// Implementation file for class : TupleToolPrimaries
//
// 2007-11-07 : Jeremie Borel
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
// actually acts as a using namespace TupleTool
DECLARE_TOOL_FACTORY( TupleToolPrimaries );

using namespace LHCb;

namespace {

  class flat {
  public:
    flat( const RecVertex & r ) 
      : pos( r.position() )
      , mchi2( r.chi2() )
      , mndof( r.nDoF() )
      , ntracks( r.tracks().size() )
    {
      const Gaudi::SymMatrix3x3 & e = r.covMatrix ();
      err = Gaudi::XYZPoint( std::sqrt( e(0,0) )
			     ,std::sqrt( e(1,1) )
			     ,std::sqrt( e(2,2) ) );
    }
    
    double x() const { return pos.x(); }
    double y() const { return pos.y(); }
    double z() const { return pos.z(); }
    
    double ex() const { return err.x(); }
    double ey() const { return err.y(); }
    double ez() const { return err.z(); }
    
    double chi2() const { return mchi2; }
    int ndof() const { return mndof; }

    int nTracks() const { return ntracks; }

    flat(){};
  private:
    Gaudi::XYZPoint pos,err;
    double mchi2;
    int mndof, ntracks;
  };

}

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolPrimaries::TupleToolPrimaries( const std::string& type,
					const std::string& name,
					const IInterface* parent )
  : TupleToolBase ( type, name , parent )
  , m_dva(0)
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("InputLocation", m_pvLocation = "" , 
                  "PV location to be used. If empty, take context-dependent default");
  
}

//=============================================================================
//=============================================================================

StatusCode TupleToolPrimaries::initialize(){
  if( !TupleToolBase::initialize() ) return StatusCode::FAILURE;

  if ( "" == m_pvLocation){
    const IOnOffline* oo = tool<IOnOffline>("OnOfflineTool",this);
    m_pvLocation = oo->primaryVertexLocation();
    debug() << "Will be looking for PVs at " << m_pvLocation << endmsg ;
  } 
  m_dva = Gaudi::Utils::getDVAlgorithm ( contextSvc() ) ;
  if (0==m_dva) return Error("Couldn't get parent DVAlgorithm", StatusCode::FAILURE);

  return StatusCode::SUCCESS;
}

//=============================================================================
//=============================================================================

StatusCode TupleToolPrimaries::fill( Tuples::Tuple& tuple ) 
{
  const std::string prefix=fullName();
  
  std::vector<flat> pvs;
  std::vector<const flat*> refPvs;

  const RecVertex::Container* PV = 0 ;
  if ( ""==m_pvLocation ) PV = m_dva->primaryVertices();   // default
  else if (exist<RecVertex::Container>(m_pvLocation)){     // user given
    PV = get<RecVertex::Container>(m_pvLocation);
  }
  if (0!=PV){
    unsigned int size = std::min<unsigned int>( PV->size(), 40 );  
    pvs.reserve( size );
    refPvs.reserve( size );
    
    RecVertex::Container::const_iterator it = PV->begin();
    for( ; (PV->end()!=it && int(it-PV->begin()) < int(size) ) ; ++it ){
      pvs.push_back( **it ); // doing this avoid using delete at the end...
      refPvs.push_back( &(pvs.back()) ); // to use mem_fun
    }
  
    if( PV->size() > 40 ){
      Warning("More than 40 primaries, some PVs will not be stored.");
    }
  }
  
  StatusCode test;
  test = tuple->farray( prefix+"PVX", std::mem_fun( &flat::x ),
			prefix+"PVY", std::mem_fun( &flat::y ),
			prefix+"PVZ", std::mem_fun( &flat::z ),
			refPvs.begin(), refPvs.end(),
			prefix+"nPV", 40 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;
  
  
  test = tuple->farray( prefix+"PVXERR", std::mem_fun( &flat::ex ),
			prefix+"PVYERR", std::mem_fun( &flat::ey ),
			prefix+"PVZERR", std::mem_fun( &flat::ez ),
			refPvs.begin(), refPvs.end(),
			prefix+"nPV", 40 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;
  
  test = tuple->farray( prefix+"PVCHI2", std::mem_fun( &flat::chi2 ),
			prefix+"PVNDOF", std::mem_fun( &flat::ndof ),
			prefix+"PVNTRACKS", std::mem_fun( &flat::nTracks ),
			refPvs.begin(), refPvs.end(),
			prefix+"nPV", 40 );

  return StatusCode(test);
  //  return StatusCode::SUCCESS;
}
