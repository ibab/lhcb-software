/********************************************************************************/
// Author: L.Abadie
// version:v3.4
// changes: handling error such that can be extended to C++: (create a class SQLException) (strlen ck + update stmt)
// file which performs select statements
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"
#include "system_info.h"
#if defined(__cplusplus) || defined(c_plusplus)
extern "C" {
#endif


#if defined( _WIN32) || defined(WIN32)
#define EXTERN_CONFDB  _declspec(dllexport)
#else
#include <stdlib.h> 
#define EXTERN_CONFDB 
#endif

	extern   OCIError* ociError ;
	extern   OCIEnv* ociEnv ;
	extern   OCISvcCtx* ociHdbc ;
	extern   OCIServer *mysrvhp;
	extern   OCISession *myusrhp; 

	int FIRST_TIME_DEVICE_BOOTING_IMAGE=0;
	char* _DeviceListBootingImage;
	int _max_deviceLengthBootingImage;
	int _deviceListLengthBootingImage;
	int* _deviceListBootingImage_nullvalue;
	int _deviceBootingImageNbElement;

	char* _bootImListBootingImage;
	int _max_bootImLengthBootingImage;
	int _bootImListLengthBootingImage;
	int* _bootImListBootingImage_nullvalue;
	int _bootImBootingImageNbElement;

	char* _kernelImListBootingImage;
	int _max_kernelImLengthBootingImage;
	int _kernelImListLengthBootingImage;
	int _kernelImBootingImageNbElement;
	int * _kernelImListBootingImage_nullvalue;

	char* _initrdImListBootingImage;
	int _max_initrdImLengthBootingImage;
	int _initrdImListLengthBootingImage;
	int _initrdImBootingImageNbElement;
	int * _initrdImListBootingImage_nullvalue;

	char* _physicalImListBootingImage;
	int _max_physicalImLengthBootingImage;
	int _physicalImListLengthBootingImage;
	int _physicalImBootingImageNbElement;
	int * _physicalImListBootingImage_nullvalue;

	char* _bootProtocolListBootingImage;
	int _max_bootProtocolLengthBootingImage;
	int _bootProtocolListLengthBootingImage;
	int _bootProtocolBootingImageNbElement;
	int * _bootProtocolListBootingImage_nullvalue;

	int freeDeviceBootImage()
	{
		int status=0;
		_DeviceListBootingImage=(char*)realloc(_DeviceListBootingImage,0*sizeof(char));
		_bootImListBootingImage=(char*)realloc(_bootImListBootingImage,0*sizeof(char));
		_bootProtocolListBootingImage=(char*)realloc(_bootProtocolListBootingImage,0*sizeof(char));
		_physicalImListBootingImage=(char*)realloc(_physicalImListBootingImage,0*sizeof(char));
		_initrdImListBootingImage=(char*)realloc(_initrdImListBootingImage,0*sizeof(char));
		_kernelImListBootingImage=(char*)realloc(_kernelImListBootingImage,0*sizeof(char));

		_deviceListBootingImage_nullvalue=(int*)realloc(_deviceListBootingImage_nullvalue,0*sizeof(int));
		_bootImListBootingImage_nullvalue=(int*)realloc(_bootImListBootingImage_nullvalue,0*sizeof(int));
		_initrdImListBootingImage_nullvalue=(int*)realloc(_initrdImListBootingImage_nullvalue,0*sizeof(int));
		_kernelImListBootingImage_nullvalue=(int*)realloc(_kernelImListBootingImage_nullvalue,0*sizeof(int));
		_bootProtocolListBootingImage_nullvalue=(int*)realloc(_bootProtocolListBootingImage_nullvalue,0*sizeof(int));
		_physicalImListBootingImage_nullvalue=(int*)realloc(_physicalImListBootingImage_nullvalue,0*sizeof(int));

		_deviceListBootingImage_nullvalue=NULL;
		_physicalImListBootingImage_nullvalue=NULL;
		_kernelImListBootingImage_nullvalue=NULL;
		_bootImListBootingImage_nullvalue=NULL;
		_initrdImListBootingImage_nullvalue=NULL;
		_bootProtocolListBootingImage_nullvalue=NULL;

		_DeviceListBootingImage=NULL;
		_bootImListBootingImage=NULL;
		_bootProtocolListBootingImage=NULL;
		_physicalImListBootingImage=NULL;
		_initrdImListBootingImage=NULL;
		_kernelImListBootingImage=NULL;

		FIRST_TIME_DEVICE_BOOTING_IMAGE=0;
		return status;
	}
	/************************************************************************/
	// insert the boot image of a functional device or a functional device type
	/***********************************************************************/
	/**
	* insert the boot image information of a given device or device type returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device or of the device type 
	* @param boot_image : will be used to configure the DHCP server. It is mandatory
	* @param kernel_image_location : Put "" if absent.
	* @param initrd_image_location : Put "" if absent.
	* @param physical_location : Put "" if absent.
	* @param boot_protocol : usually it;s BOOTP
	* @param first_time1 : 1 if it's your first MultipleDevice insert (save it into the database), 0 otherwise
	* @param last_rows1 : 1 if it's your MultipleDevice last insert (save it into the database), 0 otherwise  Maximum in one go: 10000 rows.( we force the insert)
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int InsertMultipleBootImages(char* devicename,char* boot_image,char* kernel_image_location,char* initrd_image_location,char* physical_location,char*  boot_protocol,int first_time1, int last_rows1,char* ErrMess)
	{
		char appliName[100]="InsertMultipleBootImages";
		char sqlstmt[2000];
		OCIStmt *hstmt;
		OCIBind  *bndp[6]; 
		int rescode=0;
		int free_mem=0;
		int res_query=0;
		sword status=0;
		char* dname=NULL;
		int i=0;
		char* dboot_image=NULL;
		char* dinitrd_image_location=NULL;
		char* dkernel_image_location=NULL;
		char* dphysical_location=NULL;
		char* dboot_protocol=NULL;
		int force_insert=0;
		int first_time=first_time1;
		//std:://cout<<"value of first_time "<<first_time <<std::endl;
		int last_rows=last_rows1;
		int* numrows_inserted=NULL;
		int numrows=0;
		OCIBind  *bndp3 = (OCIBind *) 0;

		int dname1=null_charptr(devicename);
		int dboot_image1=null_charptr(boot_image);
		int dkernel_image_location1=null_charptr(kernel_image_location);
		int dinitrd_image_location1=null_charptr(initrd_image_location);
		int dphysical_location1=null_charptr(physical_location);
		int dboot_protocol1=null_charptr(boot_protocol);
		if(FIRST_TIME_DEVICE_BOOTING_IMAGE==1 && _DeviceListBootingImage==NULL)
		{
			first_time=1;

		}
		if(first_time==1)
		{
			status=freeDeviceBootImage(); //delete the cache in case it was exiting with error
			FIRST_TIME_DEVICE_BOOTING_IMAGE=1;

		}


		res_query=AppendString(devicename,_DeviceListBootingImage,_deviceListLengthBootingImage,_deviceBootingImageNbElement,_max_deviceLengthBootingImage,first_time);
		res_query+=AppendString( boot_image,_bootImListBootingImage,_bootImListLengthBootingImage,_bootImBootingImageNbElement,_max_bootImLengthBootingImage,first_time);
		res_query+=AppendString(kernel_image_location,_kernelImListBootingImage,_kernelImListLengthBootingImage, _kernelImBootingImageNbElement,_max_kernelImLengthBootingImage,first_time);
		res_query+=AppendString(initrd_image_location,_initrdImListBootingImage,_initrdImListLengthBootingImage,_initrdImBootingImageNbElement,_max_initrdImLengthBootingImage,first_time);
		res_query+=AppendString(physical_location,_physicalImListBootingImage,_physicalImListLengthBootingImage,_physicalImBootingImageNbElement,_max_physicalImLengthBootingImage,first_time);
		res_query+=AppendString(boot_protocol,_bootProtocolListBootingImage,_bootProtocolListLengthBootingImage,_bootProtocolBootingImageNbElement,_max_bootProtocolLengthBootingImage,first_time);



		res_query+=AppendInt(dname1,_deviceListBootingImage_nullvalue,_deviceBootingImageNbElement,first_time);
		res_query+=AppendInt(dboot_image1,_bootImListBootingImage_nullvalue,_bootImBootingImageNbElement,first_time);
		res_query+=AppendInt(dinitrd_image_location1,_initrdImListBootingImage_nullvalue,_initrdImBootingImageNbElement,first_time);
		res_query+=AppendInt(dkernel_image_location1,_kernelImListBootingImage_nullvalue, _kernelImBootingImageNbElement,first_time);
		res_query+=AppendInt(dphysical_location1,_physicalImListBootingImage_nullvalue,_physicalImBootingImageNbElement,first_time);
		res_query+=AppendInt(dboot_protocol1,_bootProtocolListBootingImage_nullvalue,_bootProtocolBootingImageNbElement,first_time);

		if(_deviceBootingImageNbElement==MAXROWS && last_rows!=1)
		{
			force_insert=1;
			last_rows=1;
		}
		status=res_query;
		if(last_rows==1 && res_query==0 && ociEnv!=0 && FIRST_TIME_DEVICE_BOOTING_IMAGE==1)
		{
			int len_host=LOGIN_LEN;
			char login[LOGIN_LEN];
			char host[LOGIN_LEN];
			char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

			if(errmessg1==NULL)
			{
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
			if(errmessg2==NULL)
			{
				free(errmessg1);
				GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
				return -1;
			}
			res_query=getTerminalName(host,len_host,errmessg1);
			len_host=LOGIN_LEN;
			res_query=getLoginUser(login,len_host,errmessg2);

			if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
			{
				free(errmessg1);
				free(errmessg2);
				rescode=0;
			}
			else
			{
				status=-1;
				rescode=-1;
				if(strncmp("NO_ERROR",errmessg1,8)!=0)
				{
					strcpy(ErrMess,errmessg1);
					rescode=-2;
				}
				if(strncmp("NO_ERROR",errmessg2,8)!=0)
				{
					if(rescode==-2)
						sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
					else
						strcpy(ErrMess,errmessg2);
				}
				rescode=-1;
				status+=freeDeviceBootImage();
				if(dname!=NULL)
					free(dname);
				if(dboot_image!=NULL)
					free(dboot_image);
				if(dinitrd_image_location!=NULL)
					free(dinitrd_image_location);
				if(dkernel_image_location!=NULL)
					free(dkernel_image_location);
				if(dboot_protocol!=NULL)
					free(dboot_protocol);
				if(dphysical_location!=NULL)
					free(dphysical_location);


				free(errmessg1);
				free(errmessg2);
				return rescode;
			}
			//need to proceed with messages

			status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIHandleAlloc unsuccessful");
			}
			else
			{
				sprintf(sqlstmt,
					"BEGIN insert all into %s(deviceid,kernel_image_location,initrd_image_location,physical_location,boot_image_location,boot_protocol,created,author,user_update,terminal_name) values(devid,:kernelIm,:initrdIm,:physicalLoc,:bootImLoc,:bootProt,sysdate,'%s','%s','%s') select deviceid as devid from %s where devicename=:dname; if %s=0 then insert all into %s(devicetypeid,kernel_image_location,initrd_image_location,physical_location,boot_image_location,boot_protocol,created,author,user_update,terminal_name) values(devtypeid,:kernelIm,:initrdIm,:physicalLoc,:bootImLoc,:bootProt,sysdate,'%s','%s','%s') select devicetypeid as devtypeid from %s where devicetype=:dname; end if; :numrows:=%s ; END;",DEVICEBOOTING_TABLE,login,login,host,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,DEVICETYPEBOOTING_TABLE,login,login,host,DEVICETYPE_TABLE,SQLROWCOUNT);
				status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt), (ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
			}

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
			}
			else
			{

				dname=(char*)malloc(_deviceBootingImageNbElement*_max_deviceLengthBootingImage*sizeof(char));
				if(dname!=NULL)
				{
					status=NormalizeVector(_DeviceListBootingImage, _deviceBootingImageNbElement,_max_deviceLengthBootingImage,dname);
					//std:://cout<<"dname not null "<<_max_deviceLength<<std::endl;
				}
				dboot_image=(char*)malloc( _bootImBootingImageNbElement*_max_bootImLengthBootingImage*sizeof(char));
				if(dboot_image!=NULL)
				{
					status+=NormalizeVector(_bootImListBootingImage,_bootImBootingImageNbElement,_max_bootImLengthBootingImage,dboot_image);
					//std:://cout<<"dserialnb not null "<<_max_serialnb1_length<<std::endl;
				}
				dinitrd_image_location=(char*)malloc( _initrdImBootingImageNbElement*_max_initrdImLengthBootingImage*sizeof(char));
				if(dinitrd_image_location!=NULL)
				{
					status+=NormalizeVector(_initrdImListBootingImage, _initrdImBootingImageNbElement,_max_initrdImLengthBootingImage,dinitrd_image_location);
					//std:://cout<<"dlocation not null "<<_max_location_length<<std::endl;
				}
				dphysical_location=(char*)malloc(_physicalImBootingImageNbElement*_max_physicalImLengthBootingImage*sizeof(char));
				if(dphysical_location!=NULL)
				{
					status+=NormalizeVector(_physicalImListBootingImage, _physicalImBootingImageNbElement,_max_physicalImLengthBootingImage,dphysical_location);
					//std:://cout<<"dtype not null "<<_max_devtype_length<<std::endl;
				}
				dkernel_image_location=(char*)malloc(_kernelImBootingImageNbElement*_max_kernelImLengthBootingImage*sizeof(char));
				if(dkernel_image_location!=NULL)
					status+=NormalizeVector(_kernelImListBootingImage, _kernelImBootingImageNbElement,_max_kernelImLengthBootingImage,dkernel_image_location);
				dboot_protocol=(char*)malloc(_bootProtocolBootingImageNbElement*_max_bootProtocolLengthBootingImage*sizeof(char));
				if(dboot_protocol!=NULL)
					status+=NormalizeVector(_bootProtocolListBootingImage, _bootProtocolBootingImageNbElement,_max_bootProtocolLengthBootingImage,dboot_protocol);

				numrows_inserted=(int*)malloc(sizeof(int)*_deviceBootingImageNbElement);
				//std:://cout<<"after memory allocation "<<_deviceNbElement<<std::endl;
				if( (dkernel_image_location==NULL) || (dname==NULL) || (dboot_protocol==NULL) || (dphysical_location==NULL) || (dinitrd_image_location==NULL)||dboot_image==NULL||numrows_inserted==NULL)
				{
					//std:://cout<<"memory pb "<<std::endl;

					rescode=ShowErrors (status, ociError, "Invalid pointer allocation unsuccessful");
					GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					//std:://cout<<"memory pb 1"<<std::endl;
					status+=freeDeviceBootImage();
					//std:://cout<<"memory pb 2"<<std::endl;
					if(dname!=NULL)
						free(dname);
					if(dboot_image!=NULL)
						free(dboot_image);
					if(dinitrd_image_location!=NULL)
						free(dinitrd_image_location);
					if(dkernel_image_location!=NULL)
						free(dkernel_image_location);
					if(dboot_protocol!=NULL)
						free(dboot_protocol);
					if(dphysical_location!=NULL)
						free(dphysical_location);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);

					return -1;
				}
				else
				{
					for(i=0;i<_deviceBootingImageNbElement;i++)
						numrows_inserted[i]=0;
					//std:://cout<<"before binding 0"<<std::endl;
					status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)dname, _max_deviceLengthBootingImage,  SQLT_STR, (dvoid *) & _deviceListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
				}
			}
			//std:://cout<<"before binding "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":kernelIm", -1,(dvoid*)dkernel_image_location, _max_kernelImLengthBootingImage,  SQLT_STR, (dvoid *) & _kernelImListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":initrdIm", -1,(dvoid*)dinitrd_image_location, _max_initrdImLengthBootingImage,  SQLT_STR, (dvoid *)&_initrdImListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":physicalLoc", -1,(dvoid*)dphysical_location, _max_physicalImLengthBootingImage,  SQLT_STR, (dvoid *) &_physicalImListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":bootImLoc", -1,(dvoid*)dboot_image, _max_bootImLengthBootingImage,  SQLT_STR, (dvoid *) & _bootImListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":bootProt", -1,(dvoid*)dboot_protocol,_max_bootProtocolLengthBootingImage,  SQLT_STR, (dvoid *)& _bootProtocolListBootingImage_nullvalue[0],(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status =OCIBindByName(hstmt, &bndp3, ociError,(text*) ":numrows", -1,(dvoid*)&numrows_inserted[0], sizeof(int),SQLT_INT, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);




			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[0], ociError,_max_deviceLengthBootingImage, sizeof(int),0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[1], ociError,  _max_kernelImLengthBootingImage, sizeof(int), 0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[2], ociError,_max_initrdImLengthBootingImage ,sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[3], ociError, _max_physicalImLengthBootingImage  ,sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[4], ociError, _max_bootImLengthBootingImage ,sizeof(int),  0, 0);
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp[5], ociError,_max_bootProtocolLengthBootingImage, sizeof(int), 0, 0);


			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status=OCIBindArrayOfStruct(bndp3, ociError, sizeof(int),0, 0, 0);

			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIBindArrayOfStruct unsuccessful");
			}
			else
				status= OCIStmtExecute(ociHdbc, hstmt, ociError, _deviceBootingImageNbElement, 0, 0, 0, OCI_DEFAULT );
			//std:://cout<<"after execute "<<std::endl;
			if(status!=OCI_SUCCESS)
			{
				if(rescode==0)	
					rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
			}
			else
			{
				numrows=-1;
				for(i=0;i< _deviceBootingImageNbElement;i++)
				{
					if(numrows_inserted[i]==0)
					{
						numrows=0;
						i= _deviceBootingImageNbElement+5;
					}
					else
						numrows=numrows_inserted[i];
				}
				if(numrows==0)
				{
					status = OCITransCommit(ociHdbc, ociError, 0);
					status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
					status+=freeDeviceBootImage();
					if(dname!=NULL)
						free(dname);
					if(dboot_image!=NULL)
						free(dboot_image);
					if(dinitrd_image_location!=NULL)
						free(dinitrd_image_location);
					if(dkernel_image_location!=NULL)
						free(dkernel_image_location);
					if(dboot_protocol!=NULL)
						free(dboot_protocol);
					if(dphysical_location!=NULL)
						free(dphysical_location);
					if(numrows_inserted!=NULL)
						free(numrows_inserted);

					GetErrorMess(appliName, "COULDNOT_INSERT_ALL_ROWS",ErrMess,1);
					return -1;
				}
				else
					status = OCITransCommit(ociHdbc, ociError, 0);

				if(status!=OCI_SUCCESS)
				{
					if(rescode==0)	
						rescode=ShowErrors (status, ociError, "OCITransCommit unsuccessful");
				}		
			}

			status+=freeDeviceBootImage();
			//std:://cout<<"after free device final"<<std::endl;
			if(dname!=NULL)
				free(dname);
			if(dboot_image!=NULL)
				free(dboot_image);
			if(dinitrd_image_location!=NULL)
				free(dinitrd_image_location);
			if(dkernel_image_location!=NULL)
				free(dkernel_image_location);
			if(dboot_protocol!=NULL)
				free(dboot_protocol);
			if(dphysical_location!=NULL)
				free(dphysical_location);
			if(numrows_inserted!=NULL)
				free(numrows_inserted);

			status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

			if(rescode!=0)
				if(ociError!=0)
					OCIReportError(ociError,appliName, ErrMess,1); 
				else
					GetErrorMess(appliName,  "NOT CONNECTED TO ANY DB",ErrMess,1); 
			else
				OCIReportError(ociError,appliName, ErrMess,0); 
			if(force_insert==1 && rescode==0)
			{
				status=freeDeviceBootImage();
				FIRST_TIME_DEVICE_BOOTING_IMAGE=1;
				force_insert=0;

			}
		}
		else
		{
			if(res_query!=0)
			{
				status=freeDeviceBootImage();
				GetErrorMess(appliName, "Cache Problem",ErrMess,1);
			}
			if(FIRST_TIME_DEVICE_BOOTING_IMAGE!=1)
			{
				status=freeDeviceBootImage();

				res_query=-1;
				GetErrorMess(appliName, "CACHE HAS NOT BEEN INITIALIZED",ErrMess,1);
			}
			if(ociEnv==0)
			{
				status=freeDeviceBootImage();
				res_query=-1;
				GetErrorMess(appliName, "NOT CONNECTED TO ANY DB",ErrMess,1);
			}
			status+=res_query;
			if(ociEnv!=0 && FIRST_TIME_DEVICE_BOOTING_IMAGE==1 && res_query==0)
			{
				status=0;
				GetErrorMess(appliName, "NO_ERROR",ErrMess,0);
			}

		}
		//std:://cout<<"end of fct "<<std::endl;
		return (status+rescode);
	}


	/************************************************************************/
	// delete boot image information for a given device name or device type name
	/***********************************************************************/
	/**
	* delete boot image information returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : devicename or device type for which you want to delete the boot information
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int DeleteBootImage(char* devicename,char* ErrMess)
	{
		char appliName[100]="DeleteBootImage";
		int numrows=1;
		int numrows_bis=1;
		char sqlstmt[1000];
		OCIStmt *hstmt;
		OCIBind  *bndp[2]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int devid=0;
		int pway=0;
		int len_string=101;
		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
		len_host=LOGIN_LEN;
		res_query=getLoginUser(login,len_host,errmessg2);
		if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
		{
			free(errmessg1);
			free(errmessg2);
			rescode=0;
		}
		else
		{
			status=-1;
			rescode=-1;
			if(strncmp("NO_ERROR",errmessg1,8)!=0)
			{
				strcpy(ErrMess,errmessg1);
				rescode=-2;
			}
			if(strncmp("NO_ERROR",errmessg2,8)!=0)
			{
				if(rescode==-2)
					sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
				else
					strcpy(ErrMess,errmessg2);
			}
			rescode=-1;
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin delete from %s where deviceid=(select deviceid from %s where devicename=:dname); if %s=0 then  delete from %s where devicetypeid=(select devicetypeid from %s where devicetype=:dname); end if; :numrows:=%s; end;",DEVICEBOOTING_TABLE,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,DEVICETYPEBOOTING_TABLE,DEVICETYPE_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)devicename, strlen(devicename)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		



		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[1], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.

			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				sprintf(ErrMess,"NOTHING COULDN'T BE DELETED : CHECK GIVEN NAME",appliName);
				return 0;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);

			}


		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}

	/************************************************************************/
	// update the boot image of a functional device or a functional device type
	/***********************************************************************/
	/**
	* upadte the boot image information of a given device or device type returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param devicename : name of the device or of the device type 
	* @param boot_image : Put "-1" to keep the old value or the new value
	* @param kernel_image_location : Put "-1" to keep the old value or the new value or "none" if you want it to be NULL.
	* @param initrd_image_location : Put "-1" to keep the old value or the new value or "none" if you want it to be NULL.
	* @param physical_location : Put "-1" to keep the old value or the new value or "none" if you want it to be NULL.
	* @param boot_protocol : Put "-1" to keep the old value or the new value.
	* @param ErrMess : error message in case of failure
	* @return 0 if it is successful
	*/
	EXTERN_CONFDB
		int UpdateBootImage(char* devicename,char* boot_image,char* kernel_image_location,char* initrd_image_location,char* physical_location,char*  boot_protocol,char* ErrMess)
	{
		char appliName[100]="UpdateBootImage";
		int numrows=1;
		int numrows_bis=1;
		char sqlstmt[1500];
		OCIStmt *hstmt;
		OCIBind  *bndp[7]; 
		int ptoid=0;
		int pfromid=0;
		int i=0;
		int rescode=0;
		int resquery=-1;
		sword status;
		int len_host=LOGIN_LEN;
		char login[LOGIN_LEN];
		char host[LOGIN_LEN];
		int devid=0;
		int pway=0;
		int len_string=101;
		char ptype[100]="blfsl";
		char pnb[100]="jfsk";
		if(boot_image==NULL ||kernel_image_location==NULL||initrd_image_location==NULL||physical_location==NULL||boot_protocol==NULL)
		{
			GetErrorMess(appliName, "NONE OF THE INPUT PARAMETERS CAN BE NULL",ErrMess,1);
			return -1;
		}


		char* errmessg1=(char*)malloc(ERROR_BUFFER*sizeof(char));

		if(errmessg1==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			return -1;
		}
		char* errmessg2=(char*)malloc(ERROR_BUFFER*sizeof(char));
		if(errmessg2==NULL)
		{
			GetErrorMess(appliName, "Malloc unsuccessful",ErrMess,1);
			free(errmessg1);
			return -1;
		}

		int res_query=getTerminalName(host,len_host,errmessg1);
		len_host=LOGIN_LEN;
		res_query=getLoginUser(login,len_host,errmessg2);
		if(strncmp("NO_ERROR",errmessg1,8)==0 && strncmp("NO_ERROR",errmessg2,8)==0 )
		{
			free(errmessg1);
			free(errmessg2);
			rescode=0;
		}
		else
		{
			status=-1;
			rescode=-1;
			if(strncmp("NO_ERROR",errmessg1,8)!=0)
			{
				strcpy(ErrMess,errmessg1);
				rescode=-2;
			}
			if(strncmp("NO_ERROR",errmessg2,8)!=0)
			{
				if(rescode==-2)
					sprintf(ErrMess,"%s , %s",errmessg1,errmessg2);
				else
					strcpy(ErrMess,errmessg2);
			}
			rescode=-1;
			free(errmessg1);
			free(errmessg2);
			return -1;
		}
		//need to proceed with messages

		status =OCIHandleAlloc (ociEnv, (void**)&hstmt, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrMess,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrMess,1); 
			return -1;
		}
		else
		{

			sprintf(sqlstmt,"begin update %s set boot_image_location=decode(:bootIm,'-1',boot_image_location,'none',NULL,:bootIm),kernel_image_location=decode(:kernel,'-1',kernel_image_location,'none',NULL,:kernel),boot_protocol=decode(:bootPro,'-1',boot_protocol,'none',NULL,:bootPro),initrd_image_location=decode(:initrd,'-1',initrd_image_location,'none',NULL,:initrd),physical_location=decode(:physical,'-1',physical_location,'none',NULL,:physical),user_update='%s', last_update=sysdate where deviceid=(select deviceid from %s where devicename=:dname); if %s=0 then  update %s set boot_image_location=decode(:bootIm,'-1',boot_image_location,'none',NULL,:bootIm),kernel_image_location=decode(:kernel,'-1',kernel_image_location,'none',NULL,:kernel),boot_protocol=decode(:bootPro,'-1',boot_protocol,'none',NULL,:bootPro),initrd_image_location=decode(:initrd,'-1',initrd_image_location,'none',NULL,:initrd),physical_location=decode(:physical,'-1',physical_location,'none',NULL,:physical),user_update='%s', last_update=sysdate where devicetypeid=(select devicetypeid from %s where devicetype=:dname); end if; :numrows:=%s; end;",DEVICEBOOTING_TABLE,login,LOGICAL_DEVICE_TABLE,SQLROWCOUNT,DEVICETYPEBOOTING_TABLE,login,DEVICETYPE_TABLE,SQLROWCOUNT);

			status=OCIStmtPrepare(hstmt, ociError, (text*)sqlstmt, (ub4)strlen((char *)sqlstmt),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		//std::cout<<"slqstmt="<<sqlstmt<<" and len="<<strlen(sqlstmt)<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[0], ociError,(text*) ":dname", -1,(dvoid*)devicename, strlen(devicename)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[1], ociError,(text*) ":bootIm", -1,(dvoid*)boot_image, strlen(boot_image)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}		

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[2], ociError,(text*) ":kernel", -1,(dvoid*)kernel_image_location, strlen(kernel_image_location)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[3], ociError,(text*) ":bootPro", -1,(dvoid*)boot_protocol, strlen(boot_protocol)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[4], ociError,(text*) ":physical", -1,(dvoid*)physical_location, strlen(physical_location)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		{	

			status =OCIBindByName(hstmt, &bndp[5], ociError,(text*) ":initrd", -1,(dvoid*)initrd_image_location, strlen(initrd_image_location)+1,  SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0,  OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else	
			status = OCIBindByName(hstmt, &bndp[6], ociError, (text*) ":numrows", -1,(dvoid*)&numrows,sizeof(int), SQLT_INT, (dvoid*) 0, 0, 0, 0, 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIBindByName unsuccessful");
		}
		else
			status= OCIStmtExecute(ociHdbc, hstmt, ociError, 1, 0, 0, 0, OCI_DEFAULT );
		//std::cout<<"after execution"<<std::endl;   
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)	
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			//even if it's not good for performance, we commit the transaction one by one... for consistency reasons.

			if(numrows==0)
			{
				status = OCITransCommit(ociHdbc, ociError, 0);
				status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);
				sprintf(ErrMess,"NOTHING COULDN'T BE UPDATED : CHECK GIVEN NAME",appliName);
				return 0;
			}
			else
			{

				status = OCITransCommit(ociHdbc, ociError, 0);

			}


		}
		status =OCIHandleFree (hstmt,OCI_HTYPE_STMT);

		if(rescode!=0)
			OCIReportError(ociError,appliName, ErrMess,1); 
		else
			OCIReportError(ociError,appliName, ErrMess,0); 
		return rescode;
	}
	/*************************************************************************************/

	/**
	* Get the row of the boot image information for a given device or device type name returning an integer value.
	* The user should manage the memory : there is no memory allocation.
	* @param functionaldeviname : functional device name or functional device type name. it's  case sensitive.
	* @param len_device : length of the devtype_result. If the buffer is too small, we put the size needed here.
	* @param device_result :char* buffer where to put the result
	* @param device_result : formatted as follows |db_column_name (db_column_type):db_column_value|
	* @param device_result : db_column_type is I for int and C for char*
	* @param device_result : equals to "NO_ROWS_SELECTED" if no rows selected
	* @param ErrorMessage : error message in case of failure
	* @return 0 if it is successful
	*/

	EXTERN_CONFDB
		int GetBootImageRow_devicename(char* functionaldeviname,int &len_device, char* device_result,char* ErrorMessage)
	{
		char appliName[100]="GetBootImageRow_devicename";
		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int pos3=0;
		int pos4=0;
		int pos5=0;
		int pos6=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		sword status;
		int rescode=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[8];
		sb4  kernellen=0;
		sb4 devlen=0;
		sb4  physicallen=0;
		sb4 bootProlen=0;
		sb4  initrdlen=0;
		sb4 bootImlen=0;

		OCIParam *parmdp;
		int deviceid=0;
		int status1=0;
		char buffer[20];
		int len=0;
		char * devicename_temp=NULL;
		char * bootIm_temp=NULL;
		char * physical_temp=NULL;
		char * bootPro_temp=NULL;
		char* initrd_temp=NULL;
		char* kernel_temp=NULL;

		int dname_null=0;
		int bootIm_null=0;
		int bootPro_null=0;
		int physical_null=0;
		int initrd_null=0;
		int kernel_null=0;
		int devicetype_given=0;
		char logmessage[100];
		int res1=0;
		if(functionaldeviname==NULL)
		{
			GetErrorMess(appliName,"devicename MUST be given",ErrorMessage,1);
			return -1;
		}	
		char selectdev[2000];
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
			if(ociError!=0)
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			else
				GetErrorMess(appliName,"NOT CONNECTED TO ANY DB",ErrorMessage,1);
			return -1;

		}
		else
		{
			sprintf(selectdev,"select t.devicename||'?',nvl(e.kernel_image_location,'none')||'?',nvl(e.physical_location,'none'),nvl(e.initrd_image_location,'none'),nvl(e.boot_image_location,'none'),e.deviceid,nvl(e.boot_protocol,'none')||'?',1 from %s e,%s t where t.devicename=:dname and t.deviceid=e.deviceid union select t.devicetype||'?',nvl(e.kernel_image_location,'none')||'?',nvl(e.physical_location,'none'),nvl(e.initrd_image_location,'none'),nvl(e.boot_image_location,'none'),e.devicetypeid,nvl(e.boot_protocol,'none')||'?',2 from %s e,%s t where t.devicetype=:dname and t.devicetypeid=e.devicetypeid",DEVICEBOOTING_TABLE,LOGICAL_DEVICE_TABLE,DEVICETYPEBOOTING_TABLE,DEVICETYPE_TABLE);
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdev,(ub4) strlen(selectdev),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}
		//std:://cout<<"select="<<selectdev<<" and strlen(select)="<<strlen(selectdev)<<std::endl;
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else	
			status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":dname",-1,(dvoid*) functionaldeviname,strlen(functionaldeviname)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByPos unsuccessful");
		}
		else	
			status=OCIStmtExecute(ociHdbc, stmthp, ociError, 0, 0,(OCISnapshot *) 0, (OCISnapshot *) 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIStmtExecute unsuccessful");
		}
		else
		{
			/* The next two statements describe the select-list item, dept, and return its length to allocate the memory*/
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 1);
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet1 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &devlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 2);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet2 unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &kernellen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet2 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 3);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet3 unsuccessful");
		}
		else 
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &physicallen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet3 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 4);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &initrdlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet3 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 5);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bootImlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet3 unsuccessful");
		}
		else
			status=OCIParamGet(stmthp, OCI_HTYPE_STMT, ociError,(dvoid **) &parmdp, (ub4) 7);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIParamGet4  unsuccessful");
		}
		else
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &bootProlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet4 unsuccessful");
		}
		else
		{
			/* Use the retrieved length of dept to allocate an output buffer, and
			then define the output variable. If the define call returns an error,
			exit the application */
			MinStringLength(bootImlen);
			MinStringLength(devlen);
			MinStringLength(physicallen);
			MinStringLength(bootProlen);
			MinStringLength(initrdlen);
			MinStringLength(kernellen);

			//std:://cout << "bootImlen" << bootImlen<< std::endl;
			bootIm_temp = (char *) realloc(bootIm_temp,(bootImlen + 1)*sizeof(char));
			devicename_temp= (char *) realloc(devicename_temp,(devlen + 1)*sizeof(char));
			physical_temp = (char *) realloc(physical_temp,(physicallen + 1)*sizeof(char));
			bootPro_temp= (char *) realloc(bootPro_temp,(bootProlen + 1)*sizeof(char));
			kernel_temp = (char *) realloc(kernel_temp,(kernellen + 1)*sizeof(char));
			initrd_temp= (char *) realloc(initrd_temp,(initrdlen + 1)*sizeof(char));

			if(initrd_temp==NULL || devicename_temp==NULL || kernel_temp==NULL || physical_temp==NULL||bootIm_temp ==NULL||bootPro_temp==NULL)
			{

				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1;
				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				if(initrd_temp!=NULL)
					free(initrd_temp);
				if(devicename_temp!=NULL)
					free(devicename_temp);
				if(kernel_temp!=NULL)
					free(kernel_temp);
				if(physical_temp!=NULL)
					free(physical_temp);
				if(bootIm_temp!=NULL)
					free(bootIm_temp);
				if(bootPro_temp!=NULL)
					free(bootPro_temp);

				GetErrorMess(appliName,"REALLOC UNSUCCESSFUL",ErrorMessage,1);
				return rescode;

			}
			else
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (devicename_temp), devlen + 1,SQLT_STR, (dvoid *) &dname_null,(ub2 *) 0,0, OCI_DEFAULT);
		}
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos0 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1], ociError, 2,kernel_temp,kernellen+1, SQLT_STR, (dvoid *) &kernel_null,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos1 unsuccessful");

			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[2], ociError, 3,physical_temp,physicallen+1, SQLT_STR, (dvoid *) &physical_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos2 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[3], ociError, 4,initrd_temp,initrdlen+1, SQLT_STR,(dvoid *) &initrd_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos3 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[4], ociError, 5,bootIm_temp,bootImlen+1, SQLT_STR, (dvoid *) &bootIm_null, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos4 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[5], ociError,6,&deviceid,sizeof(deviceid), SQLT_INT, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[6], ociError, 7,bootPro_temp,bootProlen+1, SQLT_STR, (dvoid *) &bootPro_null,0, 0,OCI_DEFAULT);


		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos5 unsuccessful");
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[7], ociError, 8,&devicetype_given,sizeof(int), SQLT_INT, 0,0, 0,OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "DefineByPos8 unsuccessful");

			}
		}
		else
			status =OCIStmtFetch2(stmthp, ociError, 1, OCI_FETCH_NEXT,1,OCI_DEFAULT);
		//std:://cout << "after fetching" <<  std::endl;
		if (status==OCI_SUCCESS || status==OCI_SUCCESS_WITH_INFO) 
		{	
			Format_output(bootPro_null,bootPro_temp, logmessage,'?');
			Format_output(bootIm_null,bootIm_temp,logmessage,'?');
			Format_output(physical_null,physical_temp,logmessage,'?');
			Format_output(dname_null,devicename_temp,logmessage,'?');
			Format_output(kernel_null,kernel_temp,logmessage,'?');
			Format_output(initrd_null,initrd_temp,logmessage,'?');

			pos1=strcspn(devicename_temp,"?");
			pos2=strcspn(initrd_temp,"?");
			pos3=strcspn(physical_temp,"?");
			pos4=strcspn(kernel_temp,"?");
			pos5=strcspn(bootIm_temp,"?");
			pos6=strcspn(bootPro_temp,"?");

			devicename_temp[pos1]='\0';
			initrd_temp[pos2]='\0';
			physical_temp[pos3]='\0';
			kernel_temp[pos4]='\0';
			bootIm_temp[pos5]='\0';
			bootPro_temp[pos6]='\0';

			if(devicetype_given==1)
				len=strlen(bootPro_temp)+strlen(bootIm_temp)+strlen(devicename_temp)+strlen(initrd_temp)+strlen(kernel_temp)+strlen(physical_temp)+strlen("|devicename (C): |deviceid (I): |kernel_image_location (C): |physical_location (C): |initrd_image_location (C): |boot_image_location  (C): |boot_protocol (C): |");
			else
			{
				if(devicetype_given==2)
					len=strlen(bootPro_temp)+strlen(bootIm_temp)+strlen(devicename_temp)+strlen(initrd_temp)+strlen(kernel_temp)+strlen(physical_temp)+strlen("|devicetype (C): |devicetypeid (I): |kernel_image_location (C): |physical_location (C): |initrd_image_location (C): |boot_image_location  (C): |boot_protocol (C): |");
			}
			sprintf(buffer,"%d",deviceid);
			len+=strlen(buffer);
			//std:://cout << "devicetype_given=" <<devicetype_given<<  std::endl;

			if(len_device<len)
			{
				len++;
				len_device=len;
				GetErrorMess(appliName, "BUFFER_TOO_SMALL",ErrorMessage,1);
				if(initrd_temp!=NULL)
					initrd_temp = (char *) realloc(initrd_temp,(0)*sizeof(char));
				if(devicename_temp!=NULL)
					devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
				if(physical_temp!=NULL)
					physical_temp = (char *) realloc(physical_temp,(0)*sizeof(char));
				if(bootIm_temp!=NULL)
					bootIm_temp= (char *) realloc(bootIm_temp,(0)*sizeof(char));
				if(bootPro_temp!=NULL)
					bootPro_temp= (char *) realloc(bootPro_temp,(0)*sizeof(char));
				if(kernel_temp!=NULL)
					kernel_temp= (char *) realloc(kernel_temp,(0)*sizeof(char));

				status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
				return -1;

			}
			else
			{
				if(devicetype_given==1)
					sprintf(device_result,"|devicename (C):%s |deviceid (I):%d |kernel_image_location (C) :%s |physical_location (C):%s |initrd_image_location  (C):%s |boot_image_location (C):%s |boot_protocol (C):%s |" ,devicename_temp,deviceid,kernel_temp,physical_temp,initrd_temp,bootIm_temp,bootPro_temp);
				else
					sprintf(device_result,"|devicetype (C):%s |devicetypeid (I):%d |kernel_image_location (C) :%s |physical_location (C):%s |initrd_image_location  (C):%s |boot_image_location (C):%s |boot_protocol (C):%s |" ,devicename_temp,deviceid,kernel_temp,physical_temp,initrd_temp,bootIm_temp,bootPro_temp);
			}

		} 
		else 
		{
			if(rescode==0)
			{
				rescode=0;
				strcpy(device_result,"NO_ROWS_SELECTED");
				GetErrorMess(appliName, "NO_ROWS_SELECTED",ErrorMessage,1);
			}
		}

		if(initrd_temp!=NULL)
			initrd_temp = (char *) realloc(initrd_temp,(0)*sizeof(char));
		if(devicename_temp!=NULL)
			devicename_temp= (char *) realloc(devicename_temp,(0)*sizeof(char));
		if(physical_temp!=NULL)
			physical_temp = (char *) realloc(physical_temp,(0)*sizeof(char));
		if(bootIm_temp!=NULL)
			bootIm_temp= (char *) realloc(bootIm_temp,(0)*sizeof(char));
		if(bootPro_temp!=NULL)
			bootPro_temp= (char *) realloc(bootPro_temp,(0)*sizeof(char));
		if(kernel_temp!=NULL)
			kernel_temp= (char *) realloc(kernel_temp,(0)*sizeof(char));
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		if(strstr(ErrorMessage,"NO_ROWS_SELECTED")==NULL)
		{
			if(rescode==0)
			{
				OCIReportError(ociError,appliName,ErrorMessage,0); 
			}
			else
			{
				OCIReportError(ociError,appliName,ErrorMessage,1); 
			}
		}
		return rescode;
	}
#if defined(__cplusplus) || defined(c_plusplus)
}
#endif