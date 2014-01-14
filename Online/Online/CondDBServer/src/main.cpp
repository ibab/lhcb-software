
#include "CoolApplication/DatabaseSvcFactory.h"
#include "CoolKernel/IDatabaseSvc.h"
#include "CoolKernel/IDatabase.h"
#include "CoolKernel/IFolder.h"

#include "CoolKernel/IObjectIterator.h"
#include "CoolKernel/IObject.h"


#include "CoolKernel/StorageType.h"
#include "CoolKernel/RecordSpecification.h"
#include "CoolKernel/Record.h"
#include "CoolKernel/FolderSpecification.h"
#include "CoolKernel/ChannelSelection.h"
#include "CoolKernel/Exception.h"


//DIM 
#include <dis.hxx>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <map>
#include <unistd.h>
#include <list>



//Data type used to receive the conditions data
typedef struct{
  //int  upTime;
        int upTime;
        char name[512];
	char cond[1];	
}CONDITION;


class FolderBuffer
{
  public:
  cool::IFolderPtr folderPtr;
  int  storedObjts; 
  FolderBuffer(cool::IFolderPtr );
};

//Class definition for objects that store the content of a single partition of the CondDB
class Partition
{

 private:
  std::string conexion_str;
  cool::IDatabasePtr db;
  std::map <std::string , FolderBuffer> foldersMap;
  cool::IFolderPtr getFolderPtr(std::string );
  void updateBuffer(std::string );

 public:
  std::list<std::string> folders;
  std::list<std::string> conditions;
  std::string partitionID;
  Partition(std::string , std::string);
  void getContent();
  int storeCondition(CONDITION *);
  void flushFolders();
};


//Class used to work with the partitions of the CondDB
class PartitionSvc 
{
  private:
    std::string config_file;
    std::vector<Partition> partitions;
    int getPartitionsInfo();
    int getPartitionParams(std::string , std::string & , std::string &);
  public:
    void init(char *);
    void publishCondDBInfo();
    void updatePartition(CONDITION *);
    void flushPartitions();
};


//THIS VARIABLES NEED TO BE GLOBAL IN ORDER TO DON'T LOOSE DE DATA
std::string folderBuffer("");
std::string condBuffer("");

// All these const_cast are needed because of the DIM API (uses "char*" instead of "const char*")
DimService foldersService(const_cast<char*>("FOLDERS_SERVICE"), const_cast<char*>("C"),
                          const_cast<char*>(folderBuffer.c_str()), folderBuffer.size()+1);
DimService conditionsService(const_cast<char*>("CONDITIONS_SERVICE"), const_cast<char*>("C"),
                             const_cast<char*> (condBuffer.c_str()), condBuffer.size()+1);

long sleep_time = 60; //sleep time in sc
PartitionSvc partitionsSvc; 


class Command : public DimCommand
{
   	void commandHandler()
	{
	   CONDITION *cond;
           cond = (CONDITION *)getData();
           std::cout<<"RECEIVING COND=" << cond->name << std::endl;
           partitionsSvc.updatePartition(cond);
        }
    public :
       Command() : DimCommand(const_cast<char*>("COND_DB_COMMAND"), const_cast<char*>("I:1;C:512;C")) {} 
};


FolderBuffer::FolderBuffer(cool::IFolderPtr folder)
{
  this->folderPtr = folder;
  this->storedObjts = 0;
}


Partition:: Partition(std::string partitionID , std::string conexion_str)
{
  this->partitionID = partitionID;
  this-> conexion_str = conexion_str;
  cool::IDatabaseSvc &dbs = cool::DatabaseSvcFactory::databaseService();
  db = dbs.openDatabase(conexion_str.c_str(),false);
  getContent();
}
 

void Partition::getContent()
{
 
  std::string strChar = "-";
  cool::IFolderPtr folder;
  cool::IObjectIteratorPtr objs;
  cool::IObjectPtr obj;
  std::string payloadData;
  std::string conditionTag = "<condition name=\"";
  std::string::size_type pos;
  std::string condition_name;
  

  std::cout << "--- CONDITIONS DB: " << conexion_str  << std::endl;   

  this->foldersMap.clear();
  this->folders.clear();
  this->conditions.clear();
  
  // Get all the available nodes to calculate the folders names size and the conditions name size
  std::vector<std::string> nodes = db->listAllNodes();
  for ( std::vector<std::string>::iterator node_name = nodes.begin(); node_name != nodes.end(); ++node_name )
  {
    
    this->folders.push_back(*node_name);
    //REMOVE WHEN FINISHED
    std::cout<< std::endl;
    std::cout << "    NODE:" << *node_name << std::endl;
    //REMOVE WHEN FINISHED

    if ( ( (*node_name).find("/DTD/structure.dtd",0) != 0) || ((*node_name).find("/",0) != 0))
    {
      if(db->existsFolder(*node_name))
      {
           //if the node is a folder (not a folderSet) look for the condition name  inside
           folder = db->getFolder(*node_name);
           this->foldersMap.insert(std::pair<std::string, FolderBuffer> ( *node_name, FolderBuffer(folder)));  //OPTIMITATION
           
           //This is not needed any more, bc the folder name and teh condition name, now are always the same.
           /*objs = folder->browseObjects(cool::ValidityKeyMin, cool:: ValidityKeyMax, cool::ChannelSelection::all() );
           obj = objs->next();
           payloadData = obj->payloadValue("data");
	   pos =  payloadData.find(conditionTag,0);
           if( pos != std::string::npos )
	   {
              pos2 = payloadData.find("\"" , pos + conditionTag.length() );
              condition_name = payloadData.substr( pos + conditionTag.length() , pos2-( pos + conditionTag.length()));
              this->conditions.push_back(condition_name);
           }
           else
           {
		 //If there is no condition in the folder the character "-" is added to the conditions 
                  this->conditions.push_back(strChar);
		  } */
           pos = (*node_name).find_last_of("/");
           if( pos == std::string::npos)
	   {
             std::cout << "ERROR: can't get condition name in the node:" << *node_name << std::endl;
             this->conditions.push_back(strChar);
	   }
           else
	   {
             condition_name = (*node_name).substr(pos+1);
	     //std::cout << "condition_name:" << condition_name; 
             this->conditions.push_back(condition_name);
           }

      }
      else
      {
          //if the node is not a folder then it's a folderSet and there is no condition inside
          this->conditions.push_back(strChar);
      }
    }

  }
}


cool::IFolderPtr Partition::getFolderPtr(std::string folderName)
{
  std::map<std::string , FolderBuffer>::iterator it;
  it = foldersMap.find(folderName);
  if( it  != foldersMap.end())
    return it->second.folderPtr;
  cool::IFolderPtr ptr;
  return ptr;
}

void Partition::updateBuffer(std::string folderName)
{ 
  std::map<std::string , FolderBuffer>::iterator it;
  it = foldersMap.find(folderName);
  if( it  != foldersMap.end())
    it->second.storedObjts++;
}

//This method of the Partition class implements the storage of a condition in a "Single Version" folder
//This method can return 3 differents values:
//                                           -1: if an error occurs during the storage of the condition
//                                            0: if the condition was sucesfully stored in a existing folder
//                                            1: if the condition was succesfully stored in a new folder. 
int Partition::storeCondition(CONDITION * cond)
{ 
   cool::IFolderPtr folder;
   std::string new_path(cond->name);      
   long long nano_sc = cond->upTime;
   nano_sc *= 1000000000;
   cool::ValidityKey start(nano_sc);
   std::string condition( cond->cond );
    
   try 
   {
      // create the payload object
      cool::RecordSpecification recSpec;
      recSpec.extend("data",cool::StorageType::String16M);
      cool::FolderSpecification spec(cool::FolderVersioning::SINGLE_VERSION,
                                     recSpec);
      cool::Record payload(spec.payloadSpecification());
      payload["data"].setValue<std::string>(condition);
          
      folder = this->getFolderPtr(new_path);      
      if( folder == NULL )
      {
         
	 // Create the new folder
         //std::cout << "--- Storing condition in new folder: " << new_path  <<std::endl;
	 std::string desc = "Test Folder <storage_type=7>";
         folder = (this->db)->createFolder(new_path, spec, desc, true);      
         folder->setupStorageBuffer(true);
         folder->storeObject (start, cool::ValidityKeyMax, payload, 0);
         folder->flushStorageBuffer();
         this->getContent(); 
         return 1;          
      }
      else
      {
	//std::cout << "--- Storing condition in folder: " << new_path  <<std::endl;
          folder->storeObject (start, cool::ValidityKeyMax, payload, 0);
          this->updateBuffer(new_path);
          return 0; 
      }
      
  }catch(cool::Exception& e)
  {
    std::cout << "EXCEPTION INSERTING FOLDER: " << cond->name << " time="<< cond->upTime << "exception: " << e.what() <<std::endl;
    return -1;
  }
}



void Partition::flushFolders()
{
  
  std::map<std::string , FolderBuffer>::iterator it = this->foldersMap.begin();
  while(it != this->foldersMap.end())
  {
    if( it->second.storedObjts > 0 )
    {
      it->second.folderPtr->flushStorageBuffer();
      it->second.storedObjts = 0;    
      std::cout << ">>>> FlushPartition ... folder= "<< it->first << std::endl;
    }
    ++it;
  }
}


void PartitionSvc::init(char *file)
{
  config_file.assign(file);
  if(getPartitionsInfo() != -1)
  {
    publishCondDBInfo();
  }
}

void PartitionSvc::publishCondDBInfo()
{
   folderBuffer = "";
   condBuffer = "";

  for (std::vector<Partition>::iterator it = partitions.begin(); it != partitions.end(); ++it )
  {
      std::list<std::string>::iterator it_fldr = it->folders.begin();
      std::list<std::string>::iterator it_cond = it->conditions.begin();
      while( it_fldr != it->folders.end())
      {
	folderBuffer += *it_fldr;
	folderBuffer += '\000';
	
	condBuffer += *it_cond;
        condBuffer += '\000';
	
        ++it_fldr;
        ++it_cond;
      }
  }

  foldersService.updateService(const_cast<char*>(folderBuffer.data()),folderBuffer.size());
  conditionsService.updateService(const_cast<char*>(condBuffer.data()),condBuffer.size());
 
}



void PartitionSvc::updatePartition(CONDITION * cond)
{

  std::string folderName(cond->name);
  std::vector<Partition>::iterator it;
  //std::string newPartID =  folderName.substr(0, (folderName.substr(1)).find_first_of("/")+1);
  folderName = folderName.substr(11);
  std::string newPartID =  folderName.substr(0, (folderName.substr(1)).find_first_of("/")+1);
  //std::cout << "--- PartitionSvc::updatePartition folderName = " << folderName <<" newPartID ->" << newPartID << "<-" << std::endl;
  

  for(it = partitions.begin(); it != partitions.end(); ++it)
  {
    if( (*it).partitionID == newPartID)
    {
      if( (*it).storeCondition(cond) == 1)
	 this->publishCondDBInfo();
      break;
    }
  }
}

//Returns 0 if no errors happenened, -1 in any other case.
int PartitionSvc::getPartitionsInfo()
{
  std::string line, partitionID, conex_str;
  std::ifstream configFile(config_file.c_str() , std::ifstream::in);
  std::vector<Partition>::iterator it; 
  //Partition part;  

  it = partitions.begin();
  if (configFile.is_open())
  {
    
    while (!configFile.eof() )
    {
      getline(configFile,line);
      if( getPartitionParams(line, partitionID, conex_str) != -1)
      {
         
        //std::cout << "--- PartitionSvc::getPartitionsInfo"<<" PartID ->" << partitionID << "<-" << std::endl;
	it = partitions.insert(it, Partition(partitionID, conex_str));
        ++it;
      }
    }
    configFile.close();
    return 0;
  }
  else 
  { 
    std::cout << "Unable to open config file: " << config_file << std::endl;  
    return -1;
  }
}


//Returns -1 if an error occurs while reading the partitions file 
//To read correctly the partitions file the format must be >Parition_identifier conexion_string if there is no white char between 
//the partition identifier and the conexion string is not posible to distinguish between them
int PartitionSvc::getPartitionParams(std::string line , std::string & partitionID , std::string & conexion_str)
{
  if(line.find(" ", 0) == std::string::npos )
  {
     partitionID = "";
     conexion_str = "";
     //std::cout << "-> Reading partitions file: empty line: "<< std::endl;
     return -1;
  }

  //deleting white characters at the begining of the string
  while(line.find_first_of(' ') == 0 )
  {
     line.erase(0 ,1);
  }

  //deleting white characters at the end of the string
  while(line.find_last_of(' ') == line.length()-1)
  {
    line.erase(line.length()-1, 1);
  }

  partitionID = line.substr(0, line.find_first_of(" ", 0));
  conexion_str = line.substr(line.find_last_of(" ")+1); 
  return 0;
}

void PartitionSvc::flushPartitions()
{

  std::vector<Partition>::iterator it;
  for(it = partitions.begin(); it != partitions.end(); ++it)
     (*it).flushFolders();
}


int main (int argc, char* argv[]) 
{

  if ( argc != 2 )
  {
    std::cout << "usage: " << argv[0] << " <partitions_file>" << std::endl;
    return 1;
  }

  int run = 3;  
  DimService runNumber("RUN_NB_SERVICE", run);
  partitionsSvc.init(argv[1]);
  Command recvCommand;
  DimServer::start("COND_DB_INTERFACE");
  std::cout << std::endl;
  std::cout << "CONDITIONS DB INTERFACE ... DIM SERVER: START (PROD MODE 1)" << std::endl;
  std::cout << std::endl;
  while(1)
  {
    sleep(sleep_time);
    dim_lock();
    partitionsSvc.flushPartitions();
    dim_unlock();
  }

}
