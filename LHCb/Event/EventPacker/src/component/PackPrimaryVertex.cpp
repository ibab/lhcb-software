// Include files
#include "Event/PrimaryVertex.h"
#include "Event/GenericBuffer.h"
#include "GaudiAlg/GaudiAlgorithm.h"

class PackPrimaryVertices : public GaudiAlgorithm
{
public:
  PackPrimaryVertices( const std::string&, ISvcLocator*) ;
  StatusCode execute() ;
private:
  std::string m_inputName ;
  std::string m_outputName ;
  int m_version ;
} ;

class UnpackPrimaryVertices : public GaudiAlgorithm
{
public:
  UnpackPrimaryVertices( const std::string&, ISvcLocator*) ;
  StatusCode execute() ;
private:
  std::string m_inputName ;
  std::string m_outputName ;
  int m_version ;
} ;

DECLARE_ALGORITHM_FACTORY( PackPrimaryVertices )
DECLARE_ALGORITHM_FACTORY( UnpackPrimaryVertices )

//=============================================================================
// better move this to a separate file, to keep it a bit clean
//=============================================================================

namespace {

  template<class Buffer, typename T>
  void writeFloat( Buffer& b, const T& x)
  {
    float xtmp = x ;
    b.write((char*)&xtmp,sizeof(float)) ;
  }

  template<class Buffer, typename T>
  void readFloat( Buffer& b, T& x)
  {
    float xtmp ;
    b.read((char*)&xtmp,sizeof(float)) ;
    x = xtmp ;
  }

  template<class Buffer, typename T>
  void writeUInt32( Buffer& b, const T& x)
  {
    unsigned int xtmp= x ;
    b.write((char*)&xtmp,sizeof(uint32_t)) ;
  }

  template<class Buffer, typename T>
  void readUInt32( Buffer& b, T& x)
  {
    uint32_t xtmp ;
    b.read((char*)&xtmp,sizeof(uint32_t)) ;
    x = xtmp ;
  }

  template<class Buffer, class Container>
  void writeContainer( Buffer& b, const Container& vec )
  {
    writeUInt32(b,vec.size()) ;
    for( const auto& i : vec ) i.write(b) ;
  }

  template<class Buffer, class Container>
  void readContainer( Buffer& b, Container& vec )
  {
    size_t s;
    readUInt32(b,s) ;
    vec.resize(s) ;
    for(size_t i=0; i<s; ++i) vec[i].read(b) ;
  }

  class CharVectorOStream
  {
  public:
    CharVectorOStream( std::vector<char>& buffer )
      : m_buffer(buffer) {}
    void write( char* value, size_t l ) {
      m_buffer.insert( m_buffer.end(), value, value + l ) ;
    }
    template<typename T> void write( const T& t ) { write((char*)(&t),sizeof(t)) ; }
  private:
    std::vector<char>& m_buffer ;
  } ;

  class CharVectorIStream
  {
  public:
    CharVectorIStream( const std::vector<char>& buffer )
      : m_buffer(buffer),m_pos(0) {}
    void read( char* value, size_t l ) {
      std::copy(& m_buffer[m_pos],&m_buffer[m_pos+l],value ) ;
      m_pos += l ;
    }
    template<typename T> void read( const T& t ) { read((char*)(&t),sizeof(t)) ; }
    size_t pos() const { return m_pos ; }
  private:
    const std::vector<char>& m_buffer ;
    size_t m_pos ;
  } ;

}

//=============================================================================

namespace LHCb
{
  template<class Buffer>
  void PrimaryVertexTrack::write( Buffer& buffer ) const
  {
    buffer.write((char*)&m_id,sizeof(m_id)) ;
    writeFloat(buffer,m_state(0)) ;
    writeFloat(buffer,m_state(1)) ;
    writeFloat(buffer,m_state(2)) ;
    writeFloat(buffer,m_state(3)) ;
    writeFloat(buffer,m_invcov(0,0)) ;
    writeFloat(buffer,m_invcov(1,0)) ;
    writeFloat(buffer,m_invcov(1,1)) ;
    writeFloat(buffer,m_weight) ;
  }

  template<class Buffer>
  void PrimaryVertexTrack::read( Buffer& buffer )
  {
    buffer.read((char*)&m_id,sizeof(m_id)) ;
    readFloat(buffer,m_state(0)) ;
    readFloat(buffer,m_state(1)) ;
    readFloat(buffer,m_state(2)) ;
    readFloat(buffer,m_state(3)) ;
    readFloat(buffer,m_invcov(0,0)) ;
    readFloat(buffer,m_invcov(1,0)) ;
    readFloat(buffer,m_invcov(1,1)) ;
    readFloat(buffer,m_weight) ;
    // recompute derivatives and chi2
    initCache() ;
  }

  template<class Buffer>
  void PrimaryVertex::write( Buffer& b) const
  {
    // vertex base
    writeUInt32(b, key() ) ;
    writeFloat(b, position().x() ) ;
    writeFloat(b, position().y() ) ;
    writeFloat(b, position().z() ) ;
    for(int irow=0; irow<3; ++irow)
      for(int icol=0; icol<=irow; ++icol)
        writeFloat(b, covMatrix()(irow,icol) ) ;
    writeFloat(b, chi2() ) ;
    writeUInt32(b, nDoF() ) ;
    // PV
    writeFloat(b, m_refZ ) ;
    writeContainer(b,m_tracks) ;
  }

  template<class Buffer>
  void PrimaryVertex::read( Buffer& b)
  {
    // vertex base
    int key ;
    readUInt32(b,key) ;
    this->setKey(key ) ;
    double x,y,z ;
    readFloat(b,x) ;
    readFloat(b,y) ;
    readFloat(b,z) ;
    setPosition( Gaudi::XYZPoint(x,y,z) ) ;
    Gaudi::SymMatrix3x3 cov ;
    for(int irow=0; irow<3; ++irow)
      for(int icol=0; icol<=irow; ++icol)
        readFloat(b, cov(irow,icol) ) ;
    setCovMatrix(cov) ;
    double chi2 ;
    int ndof ;
    readFloat(b,chi2) ;
    readUInt32(b,ndof) ;
    this->setChi2AndDoF(chi2,ndof) ;
    // PV
    readFloat(b, m_refZ ) ;
    readContainer(b,m_tracks) ;
    // recreate derivative sum
    initCache() ;
  }
}


//-----------------------------------------------------------------------------
// Implementation file for class : PackVertices
//
// 2015-10-01 : Wouter Hulsbergen
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PackPrimaryVertices::PackPrimaryVertices( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_version(1)
{
  declareProperty( "InputLocation" , m_inputName  = LHCb::PrimaryVertexLocation::Default );
  declareProperty( "OutputLocation", m_outputName = LHCb::PrimaryVertexLocation::Packed );
}

UnpackPrimaryVertices::UnpackPrimaryVertices( const std::string& name,
                                              ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_version(1)
{
  declareProperty( "InputLocation" , m_inputName  = LHCb::PrimaryVertexLocation::Packed );
  declareProperty( "OutputLocation", m_outputName = LHCb::PrimaryVertexLocation::Default );
}

//=============================================================================
// Main execution
//=============================================================================

StatusCode PackPrimaryVertices::execute()
{
  LHCb::PrimaryVertex::Range vertices = get<LHCb::PrimaryVertex::Range>(m_inputName) ;
  LHCb::GenericBuffer* buffer = new LHCb::GenericBuffer() ;
  CharVectorOStream os{buffer->data()} ;
  os.write(m_version) ;
  unsigned int N = vertices.size() ;
  os.write(N) ;
  for( const auto& v : vertices ) v->write(os) ;
  put(buffer,m_outputName) ;
  return StatusCode::SUCCESS ;
}

StatusCode UnpackPrimaryVertices::execute()
{
  StatusCode sc = StatusCode::SUCCESS ;
  const LHCb::GenericBuffer* buffer = get<const LHCb::GenericBuffer*>(m_inputName) ;
  auto vertices = new LHCb::PrimaryVertex::Container() ;
  CharVectorIStream is{buffer->data()} ;
  int version{0} ;
  is.read(version) ;
  if( version==m_version ) {
    unsigned int N{0} ;
    is.read(N) ;
    for(unsigned int i=0; i<N; ++i) {
      LHCb::PrimaryVertex* pv = new LHCb::PrimaryVertex() ;
      pv->read(is) ;
      vertices->insert( pv ) ;
    }
    put(vertices,m_outputName) ;
    if( is.pos() != buffer->data().size() )
      sc = Warning("Did not correctly decode PackedPVContainer",StatusCode::FAILURE) ;
  } else {
    sc = Warning("Wrong version of persisted data",StatusCode::FAILURE) ;
  }
  return sc ;
}

//=============================================================================
