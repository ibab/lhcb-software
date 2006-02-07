// $Id: Relations.cpp,v 1.3 2006-02-07 13:42:54 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.3 $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ====================================================================
// STD & STL 
// ====================================================================
#include <string>
#include <vector>
// ============================================================================
// Relations
// ============================================================================
#include "Relations/IRelation.h"
#include "Relations/IRelationWeighted.h"
#include "Relations/IRelation2D.h"
#include "Relations/IRelationWeighted2D.h"
#include "Relations/Relation1D.h"
#include "Relations/Relation2D.h"
#include "Relations/RelationWeighted1D.h"
#include "Relations/RelationWeighted2D.h"
#include "Relations/RelationUtils.h"
#include "Relations/RelationsDict.h"
// ============================================================================

// ============================================================================
/** @file 
 *
 *  Auxillary implementation file for the package Kernel/Relations
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-03 
 */
// ============================================================================

// ============================================================================
/// anonymous namespace to hide local instantiations 
// ============================================================================
namespace 
{
  Relations::BaseEntry                       _0_0_1 ;
  Relations::BaseRange                       _0_0_2 ;
  Relations::BaseTable                       _0_0_3 ;  
  Relations::BaseWeightedEntry               _0_1_1 ;
  Relations::BaseWeightedRange               _0_1_2 ;
  Relations::BaseWeightedTable               _0_1_3 ;
  
  LHCb::Relation1D<int,int>                  _1 ;
  LHCb::Relation2D<std::string,int>          _2 ;
  LHCb::RelationWeighted1D<int,float,double> _3 ;
  LHCb::RelationWeighted2D<int,float,double> _4 ;
} ;
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
