// $Id: Particle2VertexIPSAsct.cpp,v 1.5 2006-06-20 12:08:23 jpalac Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h"

// from DaVinci
#include "Kernel/IGeomDispCalculator.h"

// local
#include "Particle2VertexIPSAsct.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle2VertexIPSAsct
//
// 2006-06-06 : Juan PALACIOS
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Particle2VertexIPSAsct );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Particle2VertexIPSAsct::Particle2VertexIPSAsct( const std::string& type,
                                                const std::string& name,
                                                const IInterface* parent )
  : GaudiTool ( type, name , parent )
{
  declareInterface<IParticle2VertexAsct>(this);
  declareProperty( "UseSignificance",   m_useSignificance=true );
  declareProperty( "MaxToBeAssociated", m_max=-1 );
}
//=============================================================================
template <typename FROMITER, typename TOITER, typename TABLE>
TABLE Particle2VertexIPSAsct::table(const FROMITER pBegin,
                                    const FROMITER pEnd,
                                    const TOITER vBegin,
                                    const TOITER vEnd,
                                    const IGeomDispCalculator* pIPTool) const
  {
    TABLE table;

    std::cout << "Making table!!!!" << std::endl;

    for (FROMITER ip = pBegin; ip!=pEnd; ++ip) {
      for (TOITER iv = vBegin; iv!=vEnd; ++iv){

        //      if( (*iv)->isPrimary() ) continue;
        double x,sx;
        StatusCode sc = pIPTool->calcImpactPar(**ip,**iv, x, sx);
        if (!sc.isSuccess()) {
          std::cout << "pIPTool->calcImpactPar failed" << std::endl;
          continue;
        }
      
        double w = std::fabs(m_useSignificance?x/sx:x);
        std::cout << **iv << std::endl;
        std::cout << " p = @" << **ip
                  << " v = @ " << **iv
                  << " w = " << w
                  << std::endl;
        if (m_max<=0 || w<m_max ) table.relate(*ip,*iv,w);
      }
    }
    return table; 
  }
  
//=============================================================================
// Destructor
//=============================================================================
Particle2VertexIPSAsct::~Particle2VertexIPSAsct() {}

//=============================================================================
