#include "CombineTaggersPID.h"
#include "taggingutils.h"

CombineTaggersPID::CombineTaggersPID(){

  declareProperty( "CombineTaggersPID_isBs", m_isBs = false );

  declareProperty( "CombineTaggersPID_cat1", m_cat1 = 0.405 );
  declareProperty( "CombineTaggersPID_cat2", m_cat2 = 0.371 );
  declareProperty( "CombineTaggersPID_cat3", m_cat3 = 0.281 );
  declareProperty( "CombineTaggersPID_cat4", m_cat4 = 0.257 );
  declareProperty( "CombineTaggersPID_cat5", m_cat5 = 0.180 );

  //test for strip10
  //declareProperty( "CombineTaggersPID_cat1", m_cat1 = 0.414 );
  //declareProperty( "CombineTaggersPID_cat2", m_cat2 = 0.3465 );
  //declareProperty( "CombineTaggersPID_cat3", m_cat3 = 0.3075 );
  //declareProperty( "CombineTaggersPID_cat4", m_cat4 = 0.2695 );
  //declareProperty( "CombineTaggersPID_cat5", m_cat5 = 0.1965 );

  //Sort the possible combination of particle identification (PID) 
  //according to omega
  const unsigned long CvSize = 41;
  const int pid_cats_bs[CvSize] = {
    111111,
    11111,
    111010,
    11100,
    101110,
    110110,
    101111,
    101011,
    11010,
    111011,
    101100,
    110101,
    110100,
    101101,
    100111,
    110010,
    101010,
    110011,
    110001,
    100110,
    110111,
    101001,
    1000,
    111101,
    100101,
    10000,
    100011,
    10101,
    111000,
    10,
    10110,
    10011,
    11001,
    1110,
    100,
    1011,
    111,
    1,
    1101,
    111001,
    111110
  };
  const int pid_cats_bu[CvSize] = {
    110110,
    101111,
    110101,
    110011,
    101101,
    101011,
    101110,
    111001,
    100111,
    11100,
    110010,
    101100,
    110111,
    100110,
    111010,
    110100,
    110001,
    101001,
    1000,
    111011,
    111101,
    100101,
    10000,
    101010,
    1101,
    1011,
    111111,
    11010,
    100011,
    11001,
    10011,
    100,
    1110,
    111000,
    10110,
    10101,
    10,
    111,
    1,
    11111,
    111100
  };
  //strip10
  const int pid_cats_new_bs[CvSize] = {
    110110,
    110101,
    110100,
    110011,
    110111,
    111000,
    111011,
    110010,
    101110,
    110001,
    101101,
    11010,
    100111,
    10000,
    101100,
    101111,
    101011,
    111101,
    11100,
    10011,
    100110,
    10110,
    100101,
    11001,
    101010,
    101001,
    10101,
    100,
    100011,
    1000,
    111,
    1110,
    1101,
    1011,
    10,
    1
  };
  for(unsigned int i=0; i<CvSize; i++) {
    m_pid_cats_bu.push_back(pid_cats_bu[i]);
    m_pid_cats_bs.push_back(pid_cats_bs[i]);
    //m_pid_cats_bu.push_back(pid_cats_new_bs[i]);
    //m_pid_cats_bs.push_back(pid_cats_new_bs[i]);
    m_index.push_back(0);
  }
  
  theTag = new FlavourTag();

}


FlavourTag* CombineTaggersPID::combineTaggers( Taggers& vtg ){
  theTag->reset();

  theTag->setTaggers(vtg);

  if(vtg.empty()) return theTag; 

  if(msgLevel(MSG::DEBUG)) debug() << "Running CombineTaggersPID::combineTaggers" << endmsg;

  Taggers ostaggers;
  std::vector<int> itag;
  for(int j=0; j!=5; ++j) itag.push_back(0);

  if(msgLevel(MSG::DEBUG)) debug() << "Number of taggers = " << vtg.size() << endmsg;

  //put pionSS if Bu, and kaonSS if Bs
  bool kaonSS=false;
  bool pionSS=false;
  if(m_isBs){
    kaonSS=true;
    if(msgLevel(MSG::DEBUG)) debug() << "has strange" << endmsg;
  }
  else{
    pionSS=true;
    if(msgLevel(MSG::DEBUG)) debug() << "hasdown" << endmsg;
  }
  if ((kaonSS==false) && (pionSS==false)) return theTag;
  
  //Fill the taggers
  for(Taggers::iterator iter = vtg.begin(); iter != vtg.end(); ++iter){
    if((*iter)->type() == Tagger::OS_Muon){ 
      ostaggers.push_back(*iter);
      itag[0] = (*iter)->decision();
      if(msgLevel(MSG::DEBUG)) debug() << "Muon decision =      " << itag[0] << endmsg;
      //if(itag[0] != 0) theTag->addTagger(*iter);//not necessary here
    }
    if((*iter)->type() == Tagger::OS_Electron){      
      ostaggers.push_back(*iter);
      itag[1] = (*iter)->decision();
      if(msgLevel(MSG::DEBUG)) debug() << "Electron decision =  " << itag[1] << endmsg;
      //if(itag[1] != 0) theTag->addTagger(*iter);
    }
    if((*iter)->type() == Tagger::OS_Kaon){      
      ostaggers.push_back(*iter);
      itag[2] = (*iter)->decision();
      if(msgLevel(MSG::DEBUG)) debug() << "Kaon decision =      " << itag[2] << endmsg;
      //if(itag[2] !=0 ) theTag->addTagger(*iter);
    }
    if(((*iter)->type() == Tagger::SS_Kaon) || ((*iter)->type() == Tagger::SS_Pion)){
      ostaggers.push_back(*iter);
      itag[3] = (*iter)->decision();
      if(msgLevel(MSG::DEBUG)) debug() << "Kaon/Pion SS decision =      " << itag[3] << endmsg;
      //if(itag[3] !=0 ) theTag->addTagger(*iter);
    }
    if((*iter)->type() == Tagger::VtxCharge){      
      ostaggers.push_back(*iter);
      itag[4] = (*iter)->decision();
      if(msgLevel(MSG::DEBUG)) debug() << "Vertex decision =    " << itag[4] << endmsg;
      //if(itag[4] != 0) theTag->addTagger(*iter);
    }
  }

  //Want to combine opposite side muon, electron, kaon, same side kaon/pion, vertex  
  int index=0;
  int tagsum=0;
  int tagdecision=0;
  int catt=0;

  if(itag[0]!=0) index+=10000;
  if(itag[1]!=0) index+= 1000;
  if(itag[2]!=0) index+=  100;
  if(itag[3]!=0) index+=   10;
  if(itag[4]!=0) index+=    1;
  tagsum=(itag[0]+itag[1]+itag[2]+itag[3]+itag[4]); //Sum of all possible taggers
  if(abs(tagsum)>1) index += 100000;

  //Give a tagger decision
  if (tagsum > 0)  tagdecision=1;
  if (tagsum < 0)  tagdecision=-1;
  if (tagsum == 0) tagdecision=0;

  //sort into categories
  double omega=0.5;
  if (tagdecision==0) catt=0;
  if (tagdecision!=0) {
    for (int ic=0; ic<41; ic++){
      //Select the index vector according to kaon/pion SS
      if (kaonSS==true) m_index[ic]=m_pid_cats_bs[ic];
      if (pionSS==true) m_index[ic]=m_pid_cats_bu[ic];
      if (m_index[ic]==index){
        if(msgLevel(MSG::DEBUG)) debug() << "Index =    " << m_index[ic] << " = " << index << endmsg;     
        if      (ic<=10) { catt=5; omega=m_cat5; }
        else if (ic<=18) { catt=4; omega=m_cat4; }
        else if (ic<=25) { catt=3; omega=m_cat3; }
        else if (ic<=32) { catt=2; omega=m_cat2; }
        else if (ic<=39) { catt=1; omega=m_cat1; }
        //strip10
        //if      (ic<=7) { catt=5; omega=m_cat5; }
        //else if (ic<=14) { catt=4; omega=m_cat4; }
        //else if (ic<=21) { catt=3; omega=m_cat3; }
        //else if (ic<=28) { catt=2; omega=m_cat2; }
        //else if (ic<=36) { catt=1; omega=m_cat1; }
        else catt=0;
      }
    }
  }
  if(msgLevel(MSG::DEBUG)) debug() << "Category= " << catt << " omega= " << omega << endmsg;

  if(!catt) return theTag;

  ///fill FlavourTag object
  if(      tagdecision ==  1 ) theTag->setDecision( FlavourTag::bbar );
  else if( tagdecision == -1 ) theTag->setDecision( FlavourTag::b );
  else theTag->setDecision( 0 );
  theTag->setCategory( catt );
  theTag->setOmega( omega );

  return theTag;
}
