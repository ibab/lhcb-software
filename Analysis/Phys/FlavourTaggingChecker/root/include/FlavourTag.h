#ifndef FlavourTag_H
#define FlavourTag_H 1

#include <vector>
#include "Tagger.h"

class FlavourTag;
typedef std::vector<FlavourTag*> FlavourTags;

/** @class FlavourTag
 *
 *  This class stores the global result of the taggers
 *   decisions once they got combined by the corresponding tool
 *   (like for ex. the default CombineTaggersProbability)
 *
 *  @author Marco Musy
 *  @date   2010-07-15
 */
class FlavourTag {

 public:

  FlavourTag( ) {  
    reset();
  }///< Constructor

  ~FlavourTag( ){    
    this->reset();
  } ///< Destructor
  
  void reset() {
    m_omega=0.5;
    m_omegaOS=0.5;
    m_decision=0; 
    m_category=0;
    m_decisionOS=0; 
    m_categoryOS=0;
    m_TrueTag=0;
    m_taggers.clear();
  }///< reset tag content

  enum TagResult{ b    =-1,
		  bbar = 1,
		  none = 0
  };

  int    decision(){ return m_decision; }///< tag global decision on flavour
  int    category(){ return m_category; }///< tagging categorization
  int    TrueTag() { return m_TrueTag; }///< true tag coming from MC knowledge
  double omega()   { return m_omega; }///< estimator of the mistag (must be tuned by the user on real data to obtain a reliable number)

  int    decisionOS(){ return m_decisionOS; } 
  int    categoryOS(){ return m_categoryOS; } 
  double omegaOS()   { return m_omegaOS; } 
  void setOmegaOS(double a)   { m_omegaOS = a; } 
  void setDecisionOS(int a)   { m_decisionOS = a; } 
  void setCategoryOS(int a)   { m_categoryOS = a; } 

  Taggers taggers()  {return m_taggers;}///< retrieve container of Tagger pointers
  void addTagger(Tagger* a) {m_taggers.push_back(a);} ///< store this tagger

  void setTaggers(Taggers a) {m_taggers=a;} 
  void setOmega(double a)   { m_omega = a; }
  void setDecision(int a)   { m_decision = a; }
  void setCategory(int a)   { m_category = a; }
  void setTrueTag(int a)    { m_TrueTag = a; }

  void Print() {
    if(DBGLEVEL<3) {
      cout<< " - FLAVOURTAG:  "
	  <<std::setw(5)<<m_decision<<std::setw(3)<<m_category
	  <<std::setw(5)<<m_taggers.at(0)->decision()
	  <<std::setw(3)<<m_taggers.at(1)->decision()
	  <<std::setw(3)<<m_taggers.at(2)->decision()
	  <<std::setw(3)<<m_taggers.at(3)->decision()
	  <<std::setw(3)<<m_taggers.at(4)->decision()
	  <<"\n              decision= "<<m_decision 
	  << (m_TrueTag==m_decision ? " RIGHT":" WRONG")
	  << ",   cat="<<m_category
 	  <<"   w="<<m_omega
	  << endl;
      for(Taggers::iterator i=m_taggers.begin();i!=m_taggers.end(); i++ ){
	if((*i)->type()) 
	  cout << "    -> tagger found:" << (*i)->taggerparts().at(0)->ID()
	       << (m_TrueTag==(*i)->decision() ? " Right":" Wrong")
	       << "  NNet="<< (*i)->RawNNetOutput()
	       << "  w="<< (*i)->omega()
	       << "  Pt="<<(*i)->taggerparts().at(0)->pt()
	       << endl;
      }
    }
  }///< dump FlavourTag object information

 private:
  double m_omega, m_omegaOS;
  int    m_decision, m_category, m_TrueTag;
  int    m_decisionOS, m_categoryOS;
  Taggers m_taggers;

};

#endif 
