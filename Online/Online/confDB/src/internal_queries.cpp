/********************************************************************************/
//	Author: L.Abadie
// version:v3.4
// file which performs internal querie such as statistics measurements,name of tables, columns ...
// N.B : Memory management (allocation+release) should be handled by the user application thus, for all the fct provided by this lib
/********************************************************************************/
#include "list_structures.h"

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
	/************************************************************************/
	// return the cpu and db time useful to meaure perf. need access to the v$service_stats
	/***********************************************************************/

	EXTERN_CONFDB
		int GetCPUDBTime(char* servicename,int &len_statres,double* statname_result)
	{


		int i = 0;
		int j=0;
		int pos1=0;
		int pos2=0;
		int len1=0;
		int rescode=0;
		int free_mem=0;
		ub4 numcols = 0;
		ub4 collen=0;
		ub2 type = 0;
		int prefetch_rows=5;
		sword status=0;
		OCIStmt *stmthp;
		OCIBind  *bnd1p = (OCIBind *) 0; 
		OCIDefine* def[2];
		ub4 statname_len=0;
		OCIParam *parmdp;
		char * statname_temp=NULL;
		int statlen=0;
		double statvalue=0;

		if(len_statres!=2)
		{
			rescode=ShowErrors (status, ociError, "The size of this array is 2!");
			status=-10;
			rescode=-10;
			return -10;
		}
		status =OCIHandleAlloc (ociEnv, (void**)&stmthp, OCI_HTYPE_STMT , 0, 0);
		if(status!=OCI_SUCCESS)
		{	
			rescode=ShowErrors (status, ociError, "OCIStmtHAndleStmt unsuccessful");
		}
		else
		{
			char selectdevtype[1000];
			sprintf(selectdevtype,"select stat_name||'?',value from v$service_stats where service_name='INTDB10g.cern.ch'  and stat_name in ('DB time','DB CPU')");
			status=OCIStmtPrepare(stmthp, ociError, (text*) selectdevtype,(ub4) strlen(selectdevtype),(ub4) OCI_NTV_SYNTAX, (ub4) OCI_DEFAULT);
		}


		/*if(status!=OCI_SUCCESS )
		{
		if(rescode==0)
		rescode=ShowErrors (status, ociError, "OCIStmtPrepare unsuccessful");
		}
		else
		status=OCIBindByName(stmthp, &bnd1p, ociError,(text*)":servname ",-1,(dvoid*) servicename,strlen(servicename)+1, SQLT_STR, (dvoid *) 0,(ub2 *) 0, (ub2*) 0, (ub4) 0, (ub4 *) 0, OCI_DEFAULT);
		*/
		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIBindByName internal unsuccessful");
		}
		else
			status=OCIAttrSet (stmthp,OCI_HTYPE_STMT,&prefetch_rows,0,OCI_ATTR_PREFETCH_ROWS,ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrSet unsuccessful");
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
			status=OCIAttrGet((dvoid*) parmdp, (ub4) OCI_DTYPE_PARAM,(dvoid*) &statlen, (ub4 *) 0, (ub4) OCI_ATTR_DATA_SIZE,(OCIError *) ociError);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
				rescode=ShowErrors (status, ociError, "OCIAttrGet1 unsuccessful");
		}
		else 
		{
			/* Use the retrieved length of dept to allocate an output buffer, and then define the output variable. If the define call returns an error,exit the application */
			statname_temp = (char *) realloc(statname_temp,(statlen + 1)*sizeof(char));

			if(statname_temp ==NULL)
			{
				status=-10;
				rescode=ShowErrors (status, ociError, "  pointer allocation unsuccessful");
				rescode=-1000;
			}
			else
			{
				status =OCIDefineByPos(stmthp, &def[0], ociError,1, (ub1 *) (statname_temp), statlen + 1,SQLT_STR, (dvoid *) 0,(ub2 *) 0,0, OCI_DEFAULT);
				free_mem=1;
			}
		}

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos0 unsuccessful");		
			}
		}
		else
			status=OCIDefineByPos (stmthp, &def[1],ociError, 2,&statvalue,sizeof(statvalue), SQLT_NUM, 0, 0, 0, OCI_DEFAULT);

		if(status!=OCI_SUCCESS)
		{
			if(rescode==0)
			{
				rescode=ShowErrors (status, ociError, "OCIDefineByPos1 unsuccessful");		
			}
		}
		else
		{	
			while(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
			{
				status =OCIStmtFetch (stmthp, ociError, 1, OCI_FETCH_NEXT, OCI_DEFAULT);
				//std::cout<<"value of statname "<<statname_temp<<" and time "<< statvalue <<std::endl;
				if(status==OCI_SUCCESS || status== OCI_SUCCESS_WITH_INFO)
				{
					pos1=strcspn(statname_temp,"?");
					statname_temp[pos1]='\0';
					if(strcmp(statname_temp,"DB time")==0)
						statname_result[0]=statvalue;
					else
						statname_result[1]=statvalue;
				}
				else
				{
					if(i==0) //it means it  fails due to empty cursors
					{
						if(rescode==0)
							rescode=ShowErrors (status, ociError, "OCIFETCH  unsuccessful bis");
					}
					else
					{
						len_statres=2;
					}
				}
				i++;
			}

		}
		if(free_mem==1)
		{
			statname_temp = (char *) realloc(statname_temp,(0)*sizeof(char));
		}
		status =OCIHandleFree (stmthp,OCI_HTYPE_STMT);
		rescode+=status;
		return rescode;
	}


#if defined(__cplusplus) || defined(c_plusplus)
}
#endif