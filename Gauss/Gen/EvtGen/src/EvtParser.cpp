//--------------------------------------------------------------------------
//
// Environment:
//      This software is part of the EvtGen package developed jointly
//      for the BaBar and CLEO collaborations.  If you use all or part
//      of it, please give an appropriate acknowledgement.
//
// Copyright Information: See EvtGen/COPYRIGHT
//      Copyright (C) 1998      Caltech, UCSB
//
// Module: EvtParser.cc
//
// Description: Reading the decay table and produce a list of tokens.
//
// Modification history:
//
//    RYD     Febuary 11, 1998        Module created
//
//------------------------------------------------------------------------
// 
#include <fstream.h>
#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif
#include <string.h>
#include "EvtGen/EvtParser.hh"
#include "EvtGen/EvtReport.hh"
#include "EvtGen/EvtString.hh"

#define MAXBUF 1024

EvtParser::EvtParser(){
  _ntoken=0;
  _lengthoftokenlist=0;
  _tokenlist=0;
  _linelist=0;
}

EvtParser::~EvtParser(){

  delete [] _tokenlist;
  delete [] _linelist;

}


int EvtParser::getNToken(){

  return _ntoken;

}

const EvtString& EvtParser::getToken(int i){

  return _tokenlist[i];

}

int EvtParser::getLineofToken(int i){

  return _linelist[i];

}

void EvtParser::Read(const EvtString filename){
  ifstream fin;
  
  fin.open(filename.value());
  if (!fin) {
    report(ERROR,"EvtGen") << "Could not open "<<filename<<endl;
  }

  char buf[MAXBUF];
  char buf2[MAXBUF];
  char c;

  int line=0;
  int i;

  while(fin.peek() != EOF){
    line++;
    
    i=0;
    while((c=fin.get()) != '\n' && i<MAXBUF) {
      buf[i]=c;
      i++;
    }
    if(i==MAXBUF) {
      report(ERROR,"EvtGen") << "Error in EvtParser: line:"
			     <<line<<" to long"<<endl;
    }
    else {
      buf[i] = '\0';
    }
    
    //search for '#' which indicates comment for rest of line!
    i=0;
    do{
      if (buf[i]=='#') buf[i]=0;
      i++;
    }while(buf[i-1]!=0);

    //read each token
    istrstream ist(buf,strlen(buf));
    while(ist>>buf2){
      i=0;
      int semicolon=0;
      do{
	if (buf2[i]==';') {
	  buf2[i]=0;
	  semicolon=1;
	}
      }while(buf2[i++]!=0);
      if (buf2[0]!=0){
	addToken(line,buf2);
      }
      if (semicolon) addToken(line,";");
    }
  }

  fin.close();
}



void EvtParser::addToken(int line,const EvtString& string){

  //report(INFO,"EvtGen") <<_ntoken<<" "<<line<<" "<<string<<endl;  

  if (_ntoken==_lengthoftokenlist) {

    int new_length=1000+4*_lengthoftokenlist;


    
    int*     newlinelist= new int[new_length];
    EvtString* newtokenlist= new EvtString[new_length];
  
    int i;

    for(i=0;i<_ntoken;i++){
     newlinelist[i]=_linelist[i];
     newtokenlist[i]=_tokenlist[i];
    }

    delete [] _tokenlist;
    delete [] _linelist;

    _tokenlist=newtokenlist;
    _linelist=newlinelist;    

    _lengthoftokenlist=new_length;

  }


  _tokenlist[_ntoken]=string;

  _linelist[_ntoken]=line;
 
  _ntoken++;  

  //report(INFO,"EvtGen") << "First:"<<_tokenlist[0]<<" last:"<<_tokenlist[_ntoken-1]<<endl;

}
   
