// $Id: GiGaLeaf.cpp,v 1.1 2002-01-22 18:24:42 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
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
{};

// ============================================================================
/** destructor
 */
// ============================================================================
GiGaLeaf::~GiGaLeaf()
{
  m_pars  .clear() ;
  m_ipars .clear() ;
};

// ============================================================================
// End 
// ============================================================================

