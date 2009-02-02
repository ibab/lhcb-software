#include "camline.h"

void  camline::in(std::string & s){
    //old short format
    
    std::size_t pos1 = s.find('/',0);
    //from = 0 -> (pos1-1)
    from = s.substr(0,pos1);
    
    

    std::size_t pos2 = s.find('/',pos1+1);
    //level = pos1+1 -> pos2-1
    slevel = s.substr(pos1+1, pos2-(pos1+1));

    // to wie oben
    
    // ID 
    
    // refID


    std::size_t pos3 = s.find_last_of('\\',pos2+1);
    //message = pos2+1 -> pos3-1;
    if (pos3!=std::string::npos){
      message = s.substr(pos2+1,pos3-(pos2+1)); 
      URL = s.substr(pos3+1); 
    }
    else{
      message = s.substr(pos2+1);
      URL = "";  
    }
    //    std::cout << from <<" "<<slevel<<" "<<message<< " -> " << URL<<std::endl;

 }

std::string camline::out(std::string &sout){
    //old short format
    
  //    std::string sout;
    
    sout+=from;
    sout+="/";
    sout+=slevel;

    //new stuff  
    //sout+="/";
    //sout+=to;
    //sout+="/";
    //sout+=ID;
    //sout+="/";
    //sout+=refID;

    sout+="/";
    sout+=message;
    sout+="\\";
    sout+=URL;
    return sout;
  }
