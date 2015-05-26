#include "container_ROOT.h"

#include "TArrow.h"
#include "TBox.h"
#include "TEllipse.h"
#include "TF1.h"
#include "TF2.h"
#include "TH1.h"
#include "TH2.h"
#include "TLine.h"
#include "TROOT.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;

container_ROOT::container_ROOT() 
  : container(), numHistos(0), numFuncs(0) { }

void container_ROOT::reset(){

  for (unsigned int i =0;i<objlist.size();++i)
  {
    if (object(i) != NULL)
    {
      std::string nma =  name(i);
      std::size_t pos = nma.find('.');
      std::string nm = nma.substr(0,pos);

      if (nm == "DIM2D"){
        // delete
        ((TH2F*)object(i))->Delete();
        continue;
      }

      if (nm == "DIM1D"){
        // delete
        ((TH1F*)object(i))->Delete();
        continue;
      }

      if (nm == "TELLIPSE"){
        // delete
        ((TEllipse*)object(i))->Delete();
        continue;
      }

      if (nm == "TTELLIPSE"){
        // delete
        ((TEllipse*)object(i))->Delete();
        continue;
      }

      if (nm == "TTLINE"){
        // delete
        ((TLine*)object(i))->Delete();
        continue;
      }

      if (nm == "TLINE"){
        // delete
        ((TLine*)object(i))->Delete();
        continue;
      }


      if (nm == "TBOX"){
        // delete
        ((TBox*)object(i))->Delete();
        continue;
      }

      if (nm == "TTBOX"){
        // delete
        ((TBox*)object(i))->Delete();
        continue;
      }


      if (nm == "TTARROW"){
        // delete
        ((TArrow*)object(i))->Delete();
        continue;
      }

      if (nm == "TARROW"){
        // delete
        ((TArrow*)object(i))->Delete();
        continue;
      }

      if (nm == "TF1"){
        // delete
        ((TF1*)object(i))->Delete();
        continue;
      }


      if (nm == "TF2"){
        // delete
        ((TF2*)object(i))->Delete();
        continue;
      }


      free(object(i));
    }
  }
  numHistos=0;
  numFuncs=0;
  contentlist.clear();
  sizelist.clear();
  namelist.clear();
  objlist.clear();
}

int container_ROOT::convert(){

  for (unsigned int i=0;i<namelist.size();++i){
    std::string nma =  name(i);
    std::size_t pos = nma.find('.');
    std::string nm = nma.substr(0,pos);

    if (nm=="TF1"){

      char buf[16];
      sprintf(buf,"F%d",numFuncs);
      numFuncs++;
      // cout <<buf<<endl;
      TF1 * f = new TF1(buf,content(i).c_str());

      objlist.push_back((void *)f);

      continue;
    }

    if (nm=="TF2"){

      char buf[16];
      sprintf(buf,"F%d",numFuncs);
      numFuncs++;

      TF2 * f = new TF2(buf,content(i).c_str());

      objlist.push_back((void *)f);

      continue;
    }

    if (nm=="TEXT"){
      char * v = (char *)(malloc(size(i) * sizeof(char)+1 ));
      strncpy(v,content(i).c_str() ,size(i) * sizeof(char));
      if (v[size(i) * sizeof(char)-1]=='\n')
        v[size(i) * sizeof(char)-1]= '\0';
      v[size(i) * sizeof(char)]= '\0';
      objlist.push_back((void *)v);

      continue;
    }

    if (nm=="EVAL"){
      char * v = (char *)(malloc(size(i) * sizeof(char)+1 ));
      strncpy(v,content(i).c_str() ,size(i) * sizeof(char));
      if (v[size(i) * sizeof(char)-1]=='\n')
        v[size(i) * sizeof(char)-1]= '\0';
      v[size(i) * sizeof(char)]= '\0';
      objlist.push_back((void *)v);

      continue;
    }


    if (nm=="TELLIPSE"){
      char * v = (char *) malloc(4 * sizeof(float));
      unsigned int si = size(i);
      if (si > 4 * sizeof(float)) si =  4 * sizeof(float);
      memcpy(v,content(i).c_str() ,si * sizeof(char));
      float * f = (float *)v;
      //      cout <<f[0]<<" " <<f[1]<<" " <<f[2]<<" " <<f[3]<<endl;
      TEllipse * te = new TEllipse(f[0],f[1],f[2],f[3]);
      te->SetFillStyle(0);
      objlist.push_back((void *)te);
      free(v);
      continue;
    }

    if (nm=="TLINE"){
      char * v = (char *) malloc(4 * sizeof(float));
      unsigned int si = size(i);
      if (si > 4 * sizeof(float)) si =  4 * sizeof(float);
      memcpy(v,content(i).c_str() ,si * sizeof(char));
      float * f = (float *)v;
      //      cout <<f[0]<<" " <<f[1]<<" " <<f[2]<<" " <<f[3]<<endl;
      TLine * te = new TLine(f[0],f[1],f[2],f[3]);

      objlist.push_back((void *)te);
      free(v);
      continue;
    }

    if (nm=="TBOX"){
      char * v = (char *) malloc(4 * sizeof(float));
      unsigned int si = size(i);
      if (si > 4 * sizeof(float)) si =  4 * sizeof(float);
      memcpy(v,content(i).c_str() ,si * sizeof(char));
      float * f = (float *)v;
      //      cout <<f[0]<<" " <<f[1]<<" " <<f[2]<<" " <<f[3]<<endl;
      TBox * te = new TBox(f[0],f[1],f[2],f[3]);
      te->SetLineColor(kBlack);
      te->SetFillStyle(0);
      objlist.push_back((void *)te);
      free(v);
      continue;
    }

    if (nm=="TARROW"){
      char * v = (char *) malloc(4 * sizeof(float));
      unsigned int si = size(i);
      if (si > 4 * sizeof(float)) si =  4 * sizeof(float);
      memcpy(v,content(i).c_str() ,si * sizeof(char));
      float * f = (float *)v;
      //      cout <<f[0]<<" " <<f[1]<<" " <<f[2]<<" " <<f[3]<<endl;
      TArrow * te = new TArrow(f[0],f[1],f[2],f[3]);

      objlist.push_back((void *)te);
      free(v);
      continue;
    }

    if (nm=="TTELLIPSE"){

      float f[4];
      sscanf(content(i).c_str(),"%f %f %f %f",&f[0],&f[1],&f[2],&f[3]);

      TEllipse * te = new TEllipse(f[0],f[1],f[2],f[3]);

      objlist.push_back((void *)te);

      continue;
    }

    if (nm=="TTLINE"){

      float f[4];
      sscanf(content(i).c_str(),"%f %f %f %f",&f[0],&f[1],&f[2],&f[3]);

      TLine * te = new TLine(f[0],f[1],f[2],f[3]);

      objlist.push_back((void *)te);

      continue;
    }

    if (nm=="TTBOX"){

      float f[4];
      sscanf(content(i).c_str(),"%f %f %f %f",&f[0],&f[1],&f[2],&f[3]);

      TBox * te = new TBox(f[0],f[1],f[2],f[3]);
      te->SetLineColor(kBlack);
      te->SetFillStyle(0);

      objlist.push_back((void *)te);

      continue;
    }

    if (nm=="TTARROW"){

      float f[4];
      sscanf(content(i).c_str(),"%f %f %f %f",&f[0],&f[1],&f[2],&f[3]);

      TArrow * te = new TArrow(f[0],f[1],f[2],f[3]);

      objlist.push_back((void *)te);

      continue;
    }

    if (nm=="CTIME"){
      char * v = (char *)(malloc(size(i) * sizeof(char) ));
      strncpy(v,content(i).c_str() ,size(i) * sizeof(char));
      v[size(i) * sizeof(char)-1] = '\0';
      objlist.push_back((void *)v);

      continue;
    }
    if (nm=="DIMA"){
      char * v = (char *)(malloc(size(i) * sizeof(char) +1 ));
      strncpy(v,content(i).c_str() ,size(i) * sizeof(char));
      v[size(i) * sizeof(char)] = '\0';
      objlist.push_back((void *)v);

      continue;
    }
    if (nm=="UTIME"){
      char * v = (char *)(malloc( sizeof(int)  ));
      strncpy(v,content(i).c_str() ,sizeof(int));
      objlist.push_back((void *)v);

      continue;
    }
    if (nm=="DIM2D"){
      int binsx,binsy;
      float x0,x1,y1,y0;

      char buf[16];
      sprintf(buf,"H%d",numHistos);
      numHistos++;

      float * fbuf = (float *)malloc(size(i) * sizeof(char));

      memcpy((char *)fbuf,content(i).c_str(),size(i) * sizeof(char));

      if (fbuf[0] ==2){
        binsx = (int)fbuf[1];
        x0 = fbuf[2];
        x1 = fbuf[3];
        binsy = (int)fbuf[4];
        y0 = fbuf[5];
        y1 = fbuf[6];

        TH2F * v = new TH2F(buf,"2D Histogram",binsx,x0,x1,binsy,y0,y1);

        int   iData = 8;
        for (int i=0; i<= binsx+1; ++i)
        {
          for (int j=0; j <= binsy+1; ++j) 
          {
            float data = (float) fbuf[iData];
            v -> SetBinContent(i,j,data);
            iData ++;
          } //for j
        } //for i

        for (int i=0; i<= binsx+1; ++i) 
        {
          for (int j=0; j <= binsy+1; ++j) 
          {
            float data = (float) fbuf[iData];
            v -> SetBinError(i,j,data);
            iData ++;
          } //for j
        } //for i

        v->SetEntries(fbuf[7]);

        objlist.push_back((void *)v);
      }
      else {
        // std::cerr << "Does not seem to be a 2D Histo"<<std::endl;
        objlist.push_back(NULL);
      }
      free( fbuf);

      continue;
    }
    if (nm=="DIM1D"){
      int binsx;
      float x0,x1;

      char buf[16];
      sprintf(buf,"H%d",numHistos);
      numHistos++;

      float * fbuf = (float *)malloc(size(i) * sizeof(char));
      memcpy((char *)fbuf,content(i).c_str(),size(i) * sizeof(char));

      if (fbuf[0] ==1){
        binsx = (int)fbuf[1];
        x0 = fbuf[2];
        x1 = fbuf[3];

        TH1F * v = new TH1F(buf,"1D Histogram",binsx,x0,x1);
    
        int offsetData  = 5;
        int offsetError = 5+binsx+1;
        // N.B. bin 0: underflow, bin nBins+1 overflow

        // set underflows and overflows:
        v->SetBinContent(0       , fbuf[5]);
        v->SetBinContent(binsx+1 , fbuf[5+binsx+1]);
        for (int i=1; i<= binsx; i++) {
          v->SetBinContent(i, fbuf[offsetData+i]);
          v->SetBinError(i,fbuf[offsetError+i]);
        }
        objlist.push_back((void *)v);

        v->SetEntries(fbuf[4]);

      }
      else {
        objlist.push_back(NULL);
        // std::cerr << "Does not seem to be a 2D Histo"<<std::endl;
      }
      free (fbuf);
      continue;
    }


    objlist.push_back(NULL);

  }
  return objlist.size();
}

