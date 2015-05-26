#include "container.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

container::container(){ }

void container::reset()
{
  contentlist.clear();
  sizelist.clear();
  namelist.clear();
  for (unsigned int i =0;i<objlist.size();++i)
  {
    if (object(i) != NULL)
    {
      free(object(i));
    }
  }
  objlist.clear();
}

void container::add(const std::string& name ,const std::string& content){
  int sz = content.size();
  sizelist.push_back(sz);
  namelist.push_back(name);
  contentlist.push_back(content);
}

void container::add(const std::string& name , void * content,int sz){

  sizelist.push_back(sz);
  namelist.push_back(name);
  contentlist.push_back("");
  for (int i = 0; i<sz; ++i)
    contentlist.back().push_back( ((char *)content)[i]);
}


int container::tofile(char * fname){
  FILE * F = fopen(fname,"wb");
  if (F==NULL){
    perror ("container::tofile()");
    return 0;
  }
  for (int i=0;i<entries();++i){
    fprintf(F,"%s %d\n",name(i).c_str(),size(i));
    fwrite(contentlist[i].c_str(),1,size(i),F);
    fprintf(F,"\n");
  }
  return 1;
}

int container::tostream(std::stringstream &s){
  for (int i=0;i<entries();++i){
    s << name(i) << " "<<size(i) <<"\n"<<content(i)<<"\n";
  }
  //  printf("%d\n",s.str().length());
  return s.str().length();
}


int container::convert(){
  for (unsigned int i=0;i<namelist.size();++i){
    std::string nm =  name(i);
    if (nm=="TEXT"){
      char * v = (char *)(malloc(size(i) * sizeof(char)+1 ));
      strncpy(v,content(i).c_str() ,size(i) * sizeof(char));
      if (v[size(i) * sizeof(char)-1]=='\n')
        v[size(i) * sizeof(char)-1]= '\0';
      v[size(i) * sizeof(char)]= '\0';
      objlist.push_back((void *)v);

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
    objlist.push_back(NULL);
  }
  return namelist.size();
}

int container::fromfile(const char * fname){
  char buf[1024];
  FILE * F = fopen(fname,"rb");
  int cnt=0;
  while (!feof(F)){
    cnt++;
    // std::cout <<cnt++<<std::endl ;
    if (fgets(buf,1023,F)==NULL){
      // fprintf(stderr,"no more headers\n");
      fclose(F);
      return 0;
    }

    int ird = strlen(buf);

    if (buf[ird-1] !='\n'){
      // fprintf(stderr,"header format incorrect: does not end in newline\n");
      fclose(F);   return -1;
    }

    char name[1024];
    int sz;
    int iss = sscanf(buf,"%s %d",name,&sz);
    if (iss != 2){
      // fprintf(stderr,"header format incorrect: %s\n",buf);
      fclose(F);   return -1;
    }
    // std::cout <<name <<" "<<sz<<std::endl;
    char * databuf = (char *)malloc(sz*sizeof(char));

    if (databuf == NULL){
      // fprintf(stderr,"%d bytes could not be allocated\n",sz);
      return -1;
    }
    int got;
    if ( (got = fread(databuf,sizeof(char),sz,F)) != sz){
      // fprintf(stderr,"data incorrect: could not read %d bytes, got %d\n",sz,got);
      free(databuf);
      fclose(F);   return -1;
    }

    char addnl[2];
    if ((got = fread(addnl,sizeof(char),1,F)) != 1){
      // fprintf(stderr,"data incorrect: could not read extra newline",sz,got);
      free(databuf);
      fclose(F);   return -1;
    }
    if (addnl[0] != '\n'){
      // fprintf(stderr,"data incorrect: does not have trailing newline",sz,got);
      free(databuf);
      fclose(F);   return -1;
    }
    sizelist.push_back(sz);
    namelist.push_back((std::string)name);
    contentlist.push_back("");
    contentlist.back().append(databuf,sz);
    free(databuf);
  }
  fclose(F);
  return 1;
}
