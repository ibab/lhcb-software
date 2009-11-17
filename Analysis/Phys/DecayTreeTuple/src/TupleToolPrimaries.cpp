// $Id: TupleToolPrimaries.cpp,v 1.5 2009-11-17 12:33:12 pkoppenb Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "TupleToolPrimaries.h"

#include "GaudiAlg/Tuple.h"
#include "GaudiAlg/TupleObj.h"

#include <Kernel/GetDVAlgorithm.h>
#include <Kernel/DVAlgorithm.h>
#include <Event/RecVertex.h>
#include "Kernel/IOnOffline.h"

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
  : GaudiTool ( type, name , parent )
{
  declareInterface<IEventTupleTool>(this);
  declareProperty("InputLocation", m_pvLocation = "" , 
                  "PV location to be used. If empty, take context-dependent default");
  
}

//=============================================================================
//=============================================================================

StatusCode TupleToolPrimaries::initialize(){
  if( !GaudiTool::initialize() ) return StatusCode::FAILURE;

  if ( "" == m_pvLocation){
    const IOnOffline* oo = tool<IOnOffline>("OnOfflineTool",this);
    m_pvLocation = oo->primaryVertexLocation();
  } 
  debug() << "Will be looking for PVs at " << m_pvLocation << endmsg ;

  return StatusCode::SUCCESS;
}

//=============================================================================
//=============================================================================

StatusCode TupleToolPrimaries::fill( Tuples::Tuple& tuple ) {
  
  std::vector<flat> pvs;
  std::vector<const flat*> refPvs;

  if (exist<RecVertex::Container>(m_pvLocation)){
    const RecVertex::Container* PV = get<RecVertex::Container>(m_pvLocation);
    unsigned int size = std::min<unsigned int>( PV->size(), 40 );  
    pvs.reserve( size );
    refPvs.reserve( size );
    
    RecVertex::Container::const_iterator it = PV->begin();
    for( ; (PV->end()!=it && it-PV->begin() < size ) ; ++it ){
      pvs.push_back( **it ); // doing this avoid using delete at the end...
      refPvs.push_back( &(pvs.back()) ); // to use mem_fun
    }
  
    if( PV->size() > 40 ){
      Warning("More than 40 primaries, some PVs will not be stored.");
    }
  }
  
  StatusCode test;
  test = tuple->farray( "PVX", std::mem_fun( &flat::x ),
			"PVY", std::mem_fun( &flat::y ),
			"PVZ", std::mem_fun( &flat::z ),
			refPvs.begin(), refPvs.end(),
			"nPV", 40 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;
  
  
  test = tuple->farray( "PVXERR", std::mem_fun( &flat::ex ),
			"PVYERR", std::mem_fun( &flat::ey ),
			"PVZERR", std::mem_fun( &flat::ez ),
			refPvs.begin(), refPvs.end(),
			"nPV", 40 );
  if( !test.isSuccess() ) return StatusCode::FAILURE;
  
  test = tuple->farray( "PVCHI2", std::mem_fun( &flat::chi2 ),
			"PVNDOF", std::mem_fun( &flat::ndof ),
			"PVNTRACKS", std::mem_fun( &flat::nTracks ),
			refPvs.begin(), refPvs.end(),
			"nPV", 40 );

  return StatusCode(test);
  //  return StatusCode::SUCCESS;
}
