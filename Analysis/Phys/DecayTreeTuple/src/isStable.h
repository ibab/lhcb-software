// $Id: isStable.h,v 1.1 2009-05-15 10:57:05 pkoppenb Exp $
#ifndef ISSTABLE_H 
#define ISSTABLE_H 1

// Include files

/** @class isStable isStable.h
 *  
 *
 *  @author Patrick Koppenburg
 *  @date   2009-05-14
 */
bool isStable(const LHCb::MCParticle * mc)const
{
    if(!mc) return true;
    if(mc->endVertices().empty()) return true;
    unsigned int anID=abs(mc->particleID().pid());
    
    if(anID == 11 || anID == 13 
       || anID == 22 || anID == 12 
       || anID == 111 ||  anID == 211 
       || anID == 130 ||  anID == 321
       || anID == 2112 ||  anID == 2212) return true;
    return false;
  }
  

#endif // ISSTABLE_H
