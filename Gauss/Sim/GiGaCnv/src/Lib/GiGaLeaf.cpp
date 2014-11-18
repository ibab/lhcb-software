// $Id: GiGaLeaf.cpp,v 1.2 2002-08-23 08:19:41 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:42  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
// 
// ============================================================================
// Include files
// local
#include "GiGaCnv/GiGaLeaf.h"

// ============================================================================
/** @file GiGaLeaf.cpp
 * 
 * Implementation file for class : GiGaLeaf
 * 
 * @author Vanya Belyaev Ivan.Belyaev@itep.ru
 * @date 15/01/2002 
 */
// ============================================================================

// ============================================================================
/** Standard constructor
 *  @param   path object path 
 *  @param   clid object CLID 
 *  @param   pars parameters (strings)
 *  @param   ipars parameters (integers) 
 */
// ============================================================================
GiGaLeaf::GiGaLeaf( const std::string& path  , 
                    const CLID&        clid  , 
                    const Pars&        pars  , 
                    const IPars&       ipars )
  : m_path  ( path  ) 
  , m_clid  ( clid  ) 
  , m_pars  ( pars  ) 
  , m_ipars ( ipars )
{      
    if(m_pars.size()==1) { m_pars.push_back(""); };
}

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaLeaf::~GiGaLeaf()
{
  m_pars  .clear() ;
  m_ipars .clear() ;
}

// ============================================================================
// End 
// ============================================================================

