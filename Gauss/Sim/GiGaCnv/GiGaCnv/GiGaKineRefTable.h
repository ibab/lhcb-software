// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2001/07/24 11:13:54  ibelyaev
/// package restructurization(III) and update for newer GiGa
///
// ============================================================================
#ifndef GIGACNV_GIGAKINEREFTABLE_H 
#define GIGACNV_GIGAKINEREFTABLE_H 1
// ============================================================================
// STD & STL 
//#include <map> 
#include <hash_map>  ///< use hash map
/// GiGaCnv 
#include "GiGaCnv/GiGaKineRefTableEntry.h"

/** @class GiGaKineRefTable GiGaKineRefTable.h GiGaCnv/GiGaKineRefTable.h
 *  
 *  An auxilalry class to keep the relations between 
 *  Geant4 track/trajectory TrackID identifier and 
 *  the converted MCParticle object 
 *
 *  @author Ivan Belyaev
 *  @date   21/07/2001
 */

class GiGaKineRefTable: public std::hash_map<int,GiGaKineRefTableEntry>
{
public:
  ///
  typedef GiGaKineRefTableEntry       RefEntry;
  typedef std::hash_map<int,RefEntry> RefBase;
  
  /// Standard constructor
  GiGaKineRefTable();
  /// destructor 
  ~GiGaKineRefTable(); 
  
protected:

private:
  
};

// ============================================================================
#endif ///< GIGACNV_GIGAKINEREFTABLE_H
// ============================================================================
