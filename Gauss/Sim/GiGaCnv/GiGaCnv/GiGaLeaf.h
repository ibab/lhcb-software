// $Id: GiGaLeaf.h,v 1.2 2002-08-23 08:19:40 witoldp Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/01/22 18:24:41  ibelyaev
//  Vanya: update for newer versions of Geant4 and Gaudi
//
// ============================================================================
#ifndef GIGACNV_GIGALEAF_H 
#define GIGACNV_GIGALEAF_H 1
// Include files
// STD & STL 
#include <string>
#include <vector>
// GaudiKernel
#include "GaudiKernel/ClassID.h"


/** @class GiGaLeaf GiGaLeaf.h GiGaCnv/GiGaLeaf.h
 *  
 *  General description of the IOpaqueAddress content
 *  for GiGaObjects  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru 
 *  @date   15/01/2002
 */

class GiGaLeaf 
{  
public:
  
  typedef std::vector<std::string>   Pars  ;
  typedef std::vector<unsigned long> IPars ;
  
public:
  
  /** Standard constructor
   *  @param   path object path 
   *  @param   clid object CLID 
   *  @param   pars parameters (strings)
   *  @param   ipars parameters (integers) 
   */
  GiGaLeaf( const std::string& path  = ""       , 
            const CLID&        clid  = CLID  () , 
            const Pars&        pars  = Pars  () , 
            const IPars&       ipars = IPars () ); 
  
  /** destructor 
   */
  virtual ~GiGaLeaf();
  
public:

  /** accessor to object path
   *  @return object path 
   */
  inline const std::string&   path  () const { return m_path ; }
  
  /** accessor to object CLID
   *  @return object CLID 
   */
  inline const CLID&          clid  () const { return m_clid ; }
  
  /** get string parameters 
   *  @att trick! be VERY careful!
   *  @return pointer to string parameters 
   */
  inline const std::string*   par   () const 
  { return  pars().empty() ? (const std::string*)   0 :  &(pars().front()); }
  
  /** get integer parameters 
   *  @att trick! be VERY careful!
   *  @return pointer to integer parameters 
   */
  inline const unsigned long* ipar  () const 
  { return ipars().empty() ? (const unsigned long*) 0 : &(ipars().front()); }
  
  /** accessor to object parameters
   *  @return object parameters
   */
  inline const Pars&          pars   () const { return m_pars ; }
  
  /** accessor to object parameters
   *  @return object parameters
   */
  inline const IPars&         ipars () const { return m_ipars; }

  /** set new value for the object path
   *  @param valeu new value for the object path
   */
  inline void setPath  ( const std::string& value ) { m_path  = value ; }

  /** set new value for the object clid
   *  @param valeu new value for the object clid
   */
  inline void setClid  ( const CLID&        value ) { m_clid  = value ; }

  /** set new value for the object parameters 
   *  @param valeu new value for the object parameters 
   */
  inline void setPars  ( const Pars&        value ) 
  { 
    m_pars  = value ;
    if(m_pars.size()==1) { m_pars.push_back(""); }
  }


  /** set new value for the object parameters 
   *  @param valeu new value for the object parameters 
   */
  inline void setIPars ( const IPars&       value ) { m_ipars = value ; }

private:
  
  std::string     m_path   ;
  CLID            m_clid   ;
  Pars            m_pars   ;
  IPars           m_ipars  ;
  
};

// ============================================================================
// End 
// ============================================================================
#endif // GIGACNV_GIGALEAF_H
// ============================================================================
