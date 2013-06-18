/*
 * dim_JNI_Native.c : JNI native code for dim
 * routines to be used by Java.
 *
 * Started on        : 2000-08-16
 * Written by        : M.Jonker
 *
 * Version history:
 * 20000911MJJ       First Released Version (Not 100% complete yet)
 * 31-Oct-2007 Adjustments for 64bit platforms Joern Adamczewski, gsi
 * 03-Dec-2008 Fix in dim_Client releaseService Hans Essel, gsi
 *
 */
/* TODO Remove these kludges */
#define dim_Dbg_MEMORY          dim_Dbg_SERIALIZER
#define dim_Dbg_MUTABLE_MEMORY  dim_Dbg_SERIALIZER
#define dim_Dbg_MEMORY_ALLOCATE dim_Dbg_SERIALIZER

/* TODO Split the Native and the serialization into two source files */
/* TODO Release global references when they are not anymore used */

#define DIMLIB
#include <stdio.h>
#include "dim.h"
#include "dic.h"
#include "dis.h"
#include "dim_jni.h"


#ifdef  JNI_VERSION_1_4
#define JNI_VERSION JNI_VERSION_1_4
#else
#ifdef  JNI_VERSION_1_3
#define JNI_VERSION JNI_VERSION_1_3
#else
#ifdef  JNI_VERSION_1_2
#define JNI_VERSION JNI_VERSION_1_2
#else
#define JNI_VERSION 0x00010001
#endif
#endif
#endif
#define dim_JNI_version DIM_VERSION_NUMBER


// Debug/tracing support ===============================================================================
// The DBGe (Entry) DBGx (exit) DBGm (middle) DBG (Message only) macros allow for tracing and trapping
// of native method calls.
// DBGe, DBGx and DBGm have corresponding trap mask to active entry exits and middle traps
// By setting DBG_filter to something restrictive, we suppress the code generation of the conditional
// printouts for all dbg conditions except the conditions specified in the DBG_filter. (Provided the
// optimiser does its job.)
#ifndef DBG_filter
  #ifdef _DEBUG
    // no filter
    #define DBG_filter 0xFFFFFFFF
  #else
    // filter all but module loading and unloading
    #define DBG_filter dim_Dbg_MODULE
  #endif
#endif

#define DBG(test)  if(((test&DBG_filter) & DBG_mask  ) !=0) /* etc . */
#define DBGe(test) if(((test&DBG_filter) & DBGe_trap ) !=0) DBG_Trap(test); DBG(test) /* etc ; */
#define DBGm(test) if(((test&DBG_filter) & DBGm_trap ) !=0) DBG_Trap(test); DBG(test) /* etc ; */
#define DBGx(test) if(((test&DBG_filter) & DBGx_trap ) !=0) DBG_Trap(test); DBG(test) /* etc ; */

static int DBG_mask = dim_Dbg_MODULE;
static int DBGe_trap = 0;
static int DBGm_trap = 0;
static int DBGx_trap = 0;

static void DBG_Trap(int code)
{
  /* if you set a break point here you can trap all */
  /* native calls that are activated by the mask DBG_trap */
// TODO DBG_Trap should invoke the debugger
	if(code){}
  return;
}
// ===============================================================================Debug/tracing support=


// Static module variables
JavaVM*		theJavaVM;

jclass		NativeDataMemory;
jmethodID NativeDataMemory_new;
jmethodID NativeDataMemory_decodeData;
jfieldID	NativeDataMemory_dataAddress;
jfieldID	NativeDataMemory_dataSize;

jclass		ObjectDescriptor;

jclass		SendSynchronizer;
jmethodID	SendSynchronizer_new;
jmethodID	SendSynchronizer_setCompletionCode;
jmethodID	SendSynchronizer_getCompletionCode;

jclass		ReceiveSynchronizer;
jmethodID	ReceiveSynchronizer_new;
jmethodID	ReceiveSynchronizer_decodeNativeData;
jmethodID	ReceiveSynchronizer_getCompletionCode;

jclass		CompletionHandler;
jmethodID	CompletionHandler_setCompletionCode;

jclass		NativeDataDecoder;
jmethodID	NativeDataDecoder_decodeNativeData;

jclass		NativeDataEncoder;
jmethodID	NativeDataEncoder_encodeNativeData;

jclass		NativeDimTimer;
jmethodID   NativeDimTimer_timerHandler;

jclass		NativeDimSrvError;
jmethodID   NativeDimSrvError_errorHandler;
jmethodID	NativeDimSrvError_new;
jobject		ourNativeDimSrvError;

jclass		NativeDimCltError;
jmethodID   NativeDimCltError_errorHandler;
jmethodID	NativeDimCltError_new;
jobject		ourNativeDimCltError;

jclass		NativeDimExit;
jmethodID   NativeDimExit_exitHandler;
jmethodID	NativeDimExit_new;
jobject		ourNativeDimExit;

jobject   ourNativeMemoryObject;

#ifdef WIN32
	DWORD MainThreadId = 0;
#else
#ifdef __linux__
	pthread_t MainThreadId = 0;
#else
	int MainThreadId = 0;
#endif
#endif
JNIEnv* TheEnv;

#define NOT_STAMPED 0

// TODO need to get a hook into dim release services so that I can release global references
// Note: I can test on the user library beeing one of the four DIM_JNI specific callbacks to
// decide whether the tag was a global reference.
// I also have to control the copying of the tags by DIM libraries:
// There may be actually some complications because in certain cases the release service is
// not invoked but the service is kept on a 'hot-spare' list.
// In case of DIS, information from the servep structure maybe copied to a dtq structure.

// Forward defintions =====================================================================

void info_service_callback(             jobject* _aDataDecoder, void* data, int* _size);
void info_service_callback_with_cleanup(jobject* _aDataDecoder, void* data, int* _size);
void send_callback(jobject* _aCompletionHandler, int* _status);

void server_getInfo_callback(jobject* _aDataEncoder, void* *address, int *size);
void server_setCmnd_callback(jobject* _aDataDecoder, void  *address, int *size);
void timer_callback(             jobject* _aDimTimer);
void server_error_callback(int severity, int code, char *msg);
void client_error_callback(int severity, int code, char *msg);
void server_exit_callback(int *code);


// DLL load, unload and init ==============================================================

int dim_jni_attachThread(JNIEnv **env)
{
#ifdef WIN32
	DWORD tid;
	tid = GetCurrentThreadId();
#else
#ifdef __linux__
	pthread_t tid;
	tid = pthread_self();
#else
	int tid = 0;
#endif
#endif 

	if(tid == MainThreadId)
	{
		*env = TheEnv;
		return 0;
	}
	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)env, NULL);
	if(MainThreadId == 0)
	{
		MainThreadId = tid;
	}
	return 1;
}

JNIEXPORT jint JNICALL
JNI_OnLoad(JavaVM* jvm, void* reserved)
{
  int    bugs =0;
  JNIEnv *env;

//  DBGe(dim_Dbg_MODULE) ; /* trap only, report on exit */

  if(reserved){}
  theJavaVM = jvm;

  dim_jni_attachThread(&env);
  TheEnv = env;
//  (*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);

#ifdef develop_a_better_understanding_of_java
  {
    jclass		test_cid;
    jmethodID	test_mid;

    test_cid  = (*env)->FindClass(env,"dim/test/Ntest$static_class");
    test_mid  = (*env)->GetMethodID(env,test_cid, "instance_method", "()V");
    test_mid  = (*env)->GetMethodID(env,test_cid, "<init>", "()V");
    test_mid  = (*env)->GetStaticMethodID(env,test_cid, "static_method",   "()V");
    if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env);

    test_cid  = (*env)->FindClass(env,"dim/test/Ntest$instance_class");
    test_mid  = (*env)->GetMethodID(env,test_cid, "instance_method", "()V");
    test_mid  = (*env)->GetMethodID(env,test_cid, "<init>", "()V");
    if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env);

    test_cid  = (*env)->FindClass(env,"dim/test/Ntest$method_class");
    test_mid  = (*env)->GetMethodID(env,test_cid, "instance_method", "()V");
    test_mid  = (*env)->GetMethodID(env,test_cid, "<init>", "()V");
    if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env);
  }
#endif

  NativeDataMemory                    = (*env)->FindClass(env, "dim/Memory");
  NativeDataMemory_new                = (*env)->GetMethodID (env, NativeDataMemory, "<init>", "()V");
  NativeDataMemory_decodeData         = (*env)->GetMethodID (env, NativeDataMemory, "decodeData", "(JILdim/DataDecoder;)V");
  NativeDataMemory_dataAddress        = (*env)->GetFieldID  (env, NativeDataMemory, "dataAddress", "J");
  NativeDataMemory_dataSize           = (*env)->GetFieldID  (env, NativeDataMemory, "highWaterMark",    "I");
  NativeDataMemory                    = (*env)->NewGlobalRef(env, NativeDataMemory);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  ourNativeMemoryObject = (*env)->NewObject(env, NativeDataMemory, NativeDataMemory_new);
  ourNativeMemoryObject = (*env)->NewGlobalRef(env, ourNativeMemoryObject);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  SendSynchronizer                    = (*env)->FindClass(env, "dim/Client$SendSynchronizer");
  SendSynchronizer_new                = (*env)->GetMethodID (env, SendSynchronizer, "<init>", "(Ldim/CompletionHandler;)V");
  SendSynchronizer_setCompletionCode  = (*env)->GetMethodID (env, SendSynchronizer, "setCompletionCode", "(I)I");
  SendSynchronizer_getCompletionCode  = (*env)->GetMethodID (env, SendSynchronizer, "getCompletionCode", "(I)I");
  SendSynchronizer                    = (*env)->NewGlobalRef(env, SendSynchronizer);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  ReceiveSynchronizer                    = (*env)->FindClass(env, "dim/Client$ReceiveSynchronizer");
  ReceiveSynchronizer_new                = (*env)->GetMethodID (env, ReceiveSynchronizer, "<init>", "(Ldim/DataDecoder;)V");
  ReceiveSynchronizer_decodeNativeData   = (*env)->GetMethodID (env, ReceiveSynchronizer, "decodeData", "(Ldim/Memory;)V");
  ReceiveSynchronizer_getCompletionCode  = (*env)->GetMethodID (env, ReceiveSynchronizer, "getCompletionCode","(I)I" );
  ReceiveSynchronizer                    = (*env)->NewGlobalRef(env, ReceiveSynchronizer);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  CompletionHandler                   = (*env)->FindClass(env, "dim/CompletionHandler");
  CompletionHandler_setCompletionCode = (*env)->GetMethodID(env, CompletionHandler, "setCompletionCode", "(I)I");
  CompletionHandler                   = (*env)->NewGlobalRef(env, CompletionHandler);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  NativeDataDecoder                   = (*env)->FindClass(env, "dim/DataDecoder");
  NativeDataDecoder_decodeNativeData  = (*env)->GetMethodID(env,  NativeDataDecoder, "decodeData",   "(Ldim/Memory;)V");
  NativeDataDecoder                   = (*env)->NewGlobalRef(env, NativeDataDecoder);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  NativeDataEncoder                      = (*env)->FindClass(env, "dim/DataEncoder");
  NativeDataEncoder_encodeNativeData     = (*env)->GetMethodID(env,  NativeDataEncoder, "encodeData",   "()Ldim/Memory;");
  NativeDataEncoder                      = (*env)->NewGlobalRef(env, NativeDataEncoder);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  NativeDimTimer					  = (*env)->FindClass(env, "dim/DimTimer");
  NativeDimTimer_timerHandler         = (*env)->GetMethodID (env, NativeDimTimer,   "timerHandler", "()V");
  NativeDimTimer                      = (*env)->NewGlobalRef(env, NativeDimTimer);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}

  NativeDimSrvError					= (*env)->FindClass(env, "dim/DimErrorHandler$DimSrvError");
  NativeDimSrvError_new                  = (*env)->GetMethodID (env, NativeDimSrvError, "<init>", "()V");
  NativeDimSrvError_errorHandler         = (*env)->GetMethodID (env, NativeDimSrvError, "errorHandler", "(IILjava/lang/String;)V");
  NativeDimSrvError                      = (*env)->NewGlobalRef(env, NativeDimSrvError);
  ourNativeDimSrvError = (*env)->NewObject(env, NativeDimSrvError, NativeDimSrvError_new);
  ourNativeDimSrvError = (*env)->NewGlobalRef(env, ourNativeDimSrvError);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}
  
  NativeDimCltError					= (*env)->FindClass(env, "dim/DimErrorHandler$DimCltError");
  NativeDimCltError_new                  = (*env)->GetMethodID (env, NativeDimCltError, "<init>", "()V");
  NativeDimCltError_errorHandler         = (*env)->GetMethodID (env, NativeDimCltError, "errorHandler", "(IILjava/lang/String;)V");
  NativeDimCltError                      = (*env)->NewGlobalRef(env, NativeDimCltError);
  ourNativeDimCltError = (*env)->NewObject(env, NativeDimCltError, NativeDimCltError_new);
  ourNativeDimCltError = (*env)->NewGlobalRef(env, ourNativeDimCltError);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}
  
  NativeDimExit						= (*env)->FindClass(env, "dim/DimExitHandler$DimExit");
  NativeDimExit_new                 = (*env)->GetMethodID (env, NativeDimExit, "<init>", "()V");
  NativeDimExit_exitHandler         = (*env)->GetMethodID (env, NativeDimExit, "exitHandler", "(I)V");
  NativeDimExit                     = (*env)->NewGlobalRef(env, NativeDimExit);
  ourNativeDimExit = (*env)->NewObject(env, NativeDimExit, NativeDimExit_new);
  ourNativeDimExit = (*env)->NewGlobalRef(env, ourNativeDimExit);
  if ((*env)->ExceptionOccurred(env)) {bugs++; (*env)->ExceptionDescribe(env);}
  
  DBGx(dim_Dbg_MODULE) printf("DimJNI: loaded DLL with dim version %d and JNI %d.%d\n", dim_JNI_version,JNI_VERSION>>16,JNI_VERSION&0xFFFF);

  return(JNI_VERSION);
}

JNIEXPORT jint JNICALL
JNI_OnUnLoad(JNIEnv* env, void* reserved)
{
//  static JNIEnv* env;

//  DBGe(dim_Dbg_MODULE) ; /* trap only, report on exit */

  if(reserved){}
//  (*jvm)->AttachCurrentThread(jvm, (void *)&env, NULL);
  (*env)->DeleteGlobalRef(env, NativeDataMemory);
  (*env)->DeleteGlobalRef(env, SendSynchronizer);
  (*env)->DeleteGlobalRef(env, ReceiveSynchronizer);
  (*env)->DeleteGlobalRef(env, CompletionHandler);
  (*env)->DeleteGlobalRef(env, NativeDataDecoder);
  (*env)->DeleteGlobalRef(env, NativeDataEncoder);
  (*env)->DeleteGlobalRef(env, ourNativeMemoryObject);

  (*env)->DeleteGlobalRef(env, NativeDimTimer);
  (*env)->DeleteGlobalRef(env, NativeDimSrvError);
  (*env)->DeleteGlobalRef(env, ourNativeDimSrvError);
  (*env)->DeleteGlobalRef(env, NativeDimCltError);
  (*env)->DeleteGlobalRef(env, ourNativeDimCltError);
  (*env)->DeleteGlobalRef(env, NativeDimExit);
  (*env)->DeleteGlobalRef(env, ourNativeDimExit);

  DBGx(dim_Dbg_MODULE) printf("DimJNI: DLL unloaded\n");
  return(0);
}




/* implementation of dim_native.h =============================================================== */

/*
 * Class:     dim_Native
 * Method:    init
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Native_init
  (JNIEnv* env, jclass nativeClass)
{
  JavaVM* jvm;

  if(nativeClass){}
  if(theJavaVM!=NULL) return JNI_VERSION;
  (*env)->GetJavaVM(env, &jvm);
  return JNI_OnLoad(jvm, 0);
}

/*
 * Class:     dim_Native
 * Method:    stop
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Native_stop
  (JNIEnv* env, jclass nativeClass)
{

  if(nativeClass){}
  return JNI_OnUnLoad(env, 0);
}


/* implementation of dim_dbg.h ================================================================== */

/*
 * Class:     dim_Dbg
 * Method:    setMask
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_dim_Dbg_setMask
  (JNIEnv *env, jclass nativeClass, jint dbg_mask)
{
	if(env){}
	if(nativeClass){}
  if(dim_Dbg_TRANSACTIONS & (DBG_mask|dbg_mask))
    printf("DimJNI: debug mask changed from %08x to %08x\n", DBG_mask, dbg_mask);
  DBG_mask = dbg_mask;
}

/*
 * Class:     dim_Dbg
 * Method:    getMask
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Dbg_getMask
  (JNIEnv *env, jclass nativeClass)
{
	if(env){}
	if(nativeClass){}
  return DBG_mask;
}




/* implementation of dim_client.h =============================================================== */


/* the send callback */

void send_callback(jobject* _theCompletionHandler, int* _status)
{
  jobject theCompletionHandler = *_theCompletionHandler;
  JNIEnv* env;
  int doit;

  DBGe(dim_Dbg_SEND_CALLBACK) printf("DimJNI: client SEND_CALLBACK status %08lx:%d\n", (dim_long)_status, *_status);

  doit = dim_jni_attachThread(&env);
//  (*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);

  (*env)->CallIntMethod(env, theCompletionHandler, CompletionHandler_setCompletionCode, *_status);
  (*env)->DeleteGlobalRef(env, theCompletionHandler);

  if(doit)
	  (*theJavaVM)->DetachCurrentThread(theJavaVM);
  return;
}

/* for debuging messages only, so I do not care about reentrance (which could potentially happen) */
static char* send_data_format;

/* general send service */
jint send_data
  (JNIEnv *env, jstring name, jobject theCompletionHandler, jint mode, jint timeout, void* data_address, int data_size)
{
  jint    ret;
  int     stamped = 0;
  void    (*callback_funct)();
  jobject callback_param;
  jobject theSendSynchronizer;
  
  extern int request_command(char *, void *, int , void (*)(), dim_long, int);

  const char* cmnd = (*env)->GetStringUTFChars(env, name, 0);

//  DBGe(dim_Dbg_SEND_NATIVE) ; /* trap only, report later */

  if(timeout){}
  if(mode & dim_Native_F_STAMPED) stamped = 1;
  if(mode & dim_Native_F_WAIT)  // note: dim_Native_F_WAIT defined as -2147483648L //(0x80000000)
  {
    // Create a SendSynchronizer object using theCompletionHandler
    theSendSynchronizer = (*env)->NewObject(env, SendSynchronizer, SendSynchronizer_new, theCompletionHandler);
    callback_param = (*env)->NewGlobalRef(env, theSendSynchronizer);
    callback_funct = &send_callback;
  }
  else if(theCompletionHandler)
  {
    // create a global reference of the CompletionHandler if present
    callback_param = (*env)->NewGlobalRef(env, theCompletionHandler);
    callback_funct = &send_callback;
  }
  else
  {
    callback_param = 0;
    callback_funct = 0;
  }

  // Send the request
  ret = request_command((char *)cmnd, data_address, data_size, callback_funct, (dim_long)callback_param, stamped);
  DBGx(dim_Dbg_SEND_NATIVE) printf("DimJNI: Client.Send(%s,(%s) 0x%x) returns %d \n", cmnd, send_data_format, * (int*) data_address, ret);

  // release the String
  (*env)->ReleaseStringUTFChars(env, name, cmnd);

//if the send request was not queued and there is a CompletionHandler, we call the send_callback
  if(!ret && callback_param)
  {
    DBGm(dim_Dbg_SEND_NATIVE) printf("DimJNI: Native.send calls callback as bug fix.\n");
//  TODO we do not have to call send_callback(&callback_param, &ret); when not queued? //Apparently we do not need this.
  }

  if(mode & dim_Native_F_WAIT)
  {
    ret=(*env)->CallIntMethod(env, theSendSynchronizer, SendSynchronizer_getCompletionCode, 0);
    DBGx(dim_Dbg_SEND_NATIVE) printf("DimJNI:        SEND returns (after wait) %08x\n",ret);
  }

  return ret;
}



/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIZ)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIZ
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jboolean data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jboolean";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIC)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIC
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jchar data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jchar";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIB)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIB
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jbyte data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jbyte";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIS)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIS
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jshort data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jshort";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;III)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2III
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jint data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jint";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIJ)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIJ
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jlong data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jlong";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIF)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIF
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jfloat data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jfloat";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IID)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IID
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jdouble data)
{
  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jdouble";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, &data, sizeof(data));
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IILjava_lang_String_2
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jstring sdata)
{
	jint ret;
	const char* data = (*env)->GetStringUTFChars(env, sdata, 0);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "String";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, (void*) data, (int)strlen(data)+1);

	(*env)->ReleaseStringUTFChars(env,sdata, data);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[Z)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3Z
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jbooleanArray dataArray)
{
	jboolean*	nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetBooleanArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "boolean[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseBooleanArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[C)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3C
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jcharArray dataArray)
{
	jchar*	  nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetCharArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jchar[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseCharArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[B)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3B
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jbyteArray dataArray)
{
	jbyte*	  nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetByteArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jbyte[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseByteArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[S)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3S
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jshortArray dataArray)
{
	jshort*	  nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetShortArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jshort[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseShortArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[I)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3I
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jintArray dataArray)
{
	jint*	    nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetIntArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jint[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseIntArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[J)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3J
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jlongArray dataArray)
{
	jlong*	  nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetLongArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jlong[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseLongArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[F)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3F
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jfloatArray dataArray)
{
	jfloat*	  nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetFloatArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jfloat[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseFloatArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;II[D)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2II_3D
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jdoubleArray dataArray)
{
	jdouble*	nativeDataArray;
	jint		  length;
	jint		  ret;

	nativeDataArray = (*env)->GetDoubleArrayElements(env,dataArray,0);
	length          = (jint)(*env)->GetArrayLength(env,dataArray) * (jint)sizeof(*nativeDataArray);

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "jdouble[]";
  if(This){}
  ret = send_data(env, name, theCompletionHandler, mode, timeout, nativeDataArray, length);

	(*env)->ReleaseDoubleArrayElements(env,dataArray,nativeDataArray,JNI_ABORT);
	return ret;
}


/*
 * Class:     dim_Client
 * Method:    send
 * Signature: (Ljava/lang/String;Ldim/CompletionHandler;IIJI)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_send__Ljava_lang_String_2Ldim_CompletionHandler_2IIJI
  (JNIEnv *env, jclass This, jstring name, jobject theCompletionHandler, jint mode, jint timeout, jlong nativeDataBlock, jint nativeDataSize)
{

  DBG(dim_Dbg_SEND_NATIVE) send_data_format = "nativeDataBlock";
  if(This){}
  return send_data(env, name, theCompletionHandler, mode, timeout, (void*) nativeDataBlock, nativeDataSize);
}



/* This function is called by callback there is new data to be decoded */
void decodeData(jobject* _theDataDecoder, void* dataAddress, int* _dataSize, int cleanup)
{
	jobject theDataDecoder = *_theDataDecoder;
	int		  dataSize		   = *_dataSize;
	JNIEnv* env;
	int doit;

  doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);
  if(dataAddress == NULL)
  {
  	(*env)->CallVoidMethod(env, theDataDecoder, NativeDataDecoder_decodeNativeData, NULL);
  }
  else
  {
    /* the decode method will further complete the Memory object and call the decodeData method of the DataDecoder object */
    (*env)->CallVoidMethod(env, ourNativeMemoryObject, NativeDataMemory_decodeData, (jlong) dataAddress, (jint) dataSize, theDataDecoder);
  }

  /* and cleanup */
  if(cleanup) (*env)->DeleteGlobalRef(env, theDataDecoder);
  if(doit)
	(*theJavaVM)->DetachCurrentThread(theJavaVM);

  return;
}



/* This call back is called when there is new data for a client subscription */
void info_service_callback(jobject* _theDataDecoder, void* dataAddress, int* _dataSize)
{
	DBGe(dim_Dbg_INFO_CALLBACK) printf("DimJNI: INFO_CALLBACK(data: %08lx(%08x))\n", (dim_long) dataAddress, *_dataSize);

  decodeData(_theDataDecoder, dataAddress, _dataSize, 0);
}


/* This call back is called when a client once_only subscription completes (so we clean up) */
void info_service_callback_with_cleanup(jobject* _theDataDecoder, void* dataAddress, int* _dataSize)
{
	DBGe(dim_Dbg_INFO_CALLBACK) printf("DimJNI: INFO_CALLBACK/ONCE_ONLY(data: %08lx(%08x))\n", (dim_long)dataAddress, *_dataSize);

  decodeData(_theDataDecoder, dataAddress, _dataSize, 1);

}

/* This function is called by callback when timer fires */
void callTimerHandler(jobject* _aDimTimer)
{
//	jobject aDimTimer = *_aDimTimer;
	JNIEnv* env;
	int doit;

	doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);
//printf("Got callback %08x\n", _aDimTimer);
  	(*env)->CallVoidMethod(env, (jobject)_aDimTimer, NativeDimTimer_timerHandler, NULL);
	if(doit)
		(*theJavaVM)->DetachCurrentThread(theJavaVM);
 
  return;
}

/* This call back is called when the timer expires */
void timer_callback(jobject* _aDimTimer)
{
	callTimerHandler(_aDimTimer);
}

/* This function is called by callback when Server Error detected */
void callServerErrorHandler(int severity, int code, char *msg)
{
	JNIEnv* env;
	int doit;

	doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);
  	(*env)->CallVoidMethod(env, ourNativeDimSrvError, NativeDimSrvError_errorHandler,
		(jint)severity, (jint)code, (jstring)(*env)->NewStringUTF(env, msg));
	if(doit)
		(*theJavaVM)->DetachCurrentThread(theJavaVM);
	return;
}

/* This callback is called when the server gets an error*/
void server_error_callback(int severity, int code, char *msg)
{
	callServerErrorHandler(severity, code, msg);
}

/* This function is called by callback when Client Error detected */
void callClientErrorHandler(int severity, int code, char *msg)
{
	JNIEnv* env;
	int doit;

	doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);
  	(*env)->CallVoidMethod(env, ourNativeDimCltError, NativeDimCltError_errorHandler,
		(jint)severity, (jint)code, (jstring)(*env)->NewStringUTF(env, msg));
	if(doit)
		(*theJavaVM)->DetachCurrentThread(theJavaVM);
	return;
}

/* This callback is called when the client gets an error*/
void client_error_callback(int severity, int code, char *msg)
{
	callClientErrorHandler(severity, code, msg);
}

void callServerExitHandler(int code)
{
//	jobject aDimTimer = *_aDimTimer;
	JNIEnv* env;
	int doit;

	doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);
  	(*env)->CallVoidMethod(env, ourNativeDimExit, NativeDimExit_exitHandler,
		(jint)code);
	if(doit)
		(*theJavaVM)->DetachCurrentThread(theJavaVM);
	return;
}

/* This call back is called when the timer expires */
void server_exit_callback(int *code)
{
	callServerExitHandler(*code);
}

/*
 * Class:     dim_Client
 * Method:    infoService
 * Signature: (Ljava/lang/String;Ldim/DataDecoder;II)I
 */
JNIEXPORT jint JNICALL Java_dim_Client_infoService
  (JNIEnv *env, jclass This, jstring name, jobject theNativeDataDecoder, jint mode, jint timeout)
{
	jint  ret;
	int   no_link;
	int   stamped = 0;
	int   service_type = mode & 0x0FFF;
  void  (*callback_function)();
  jobject callback_param;
  jobject theReceiveSynchronizer;
  const char* info = (*env)->GetStringUTFChars(env, name, 0);
  extern unsigned request_service(char *, int, int , void *, int , void (*)(),
				    dim_long, void *, int, int);

//  DBGe(dim_Dbg_INFO_SERVICE); /* trap only, we report on exit */

  if(This){}
  if(mode & dim_Native_F_STAMPED) stamped = 1;
	if(mode & dim_Native_F_WAIT)
  {
    // Create a ReceiveSynchronizer object using the dataDecoder
    theReceiveSynchronizer = (*env)->NewObject(env, ReceiveSynchronizer, ReceiveSynchronizer_new, theNativeDataDecoder);
    callback_param = (*env)->NewGlobalRef(env, theReceiveSynchronizer);
  }
  else
  {
    callback_param = (*env)->NewGlobalRef(env, theNativeDataDecoder);
  }

  if(service_type == dim_Native_ONCE_ONLY) 
	  callback_function = &info_service_callback_with_cleanup;
  else                                     
	  callback_function = &info_service_callback; //TODO who should do the cleanup?


  ret = (jint)request_service((char *)info, service_type, timeout, 0, 0, callback_function, (dim_long)callback_param, &no_link, 0, stamped);
  DBGx(dim_Dbg_INFO_SERVICE) printf("DimJNI: client infoService(%s, DataDecoder@0x%08lx, mode=%d, timeout=%d ) returns %d\n", info, (dim_long)theNativeDataDecoder, mode, timeout, ret);
  (*env)->ReleaseStringUTFChars(env, name, info);

  if(mode & dim_Native_F_WAIT)
  {
    /* we synchronize by calling the getCompletionCode method of the ReceiveSynchronizer */ 
    (*env)->CallIntMethod(env, theReceiveSynchronizer, ReceiveSynchronizer_getCompletionCode, 0);
    DBGx(dim_Dbg_INFO_SERVICE) printf("DimJNI:         infoService(%s) completed with %08x\n",info, ret);
  }

  return ret;
}

/*
 * Class:     dim_DimTimer
 * Method:    start
 * Signature: (Ldim/DimTimer;I)V
 */
JNIEXPORT jlong JNICALL Java_dim_DimTimer_start
  (JNIEnv *env, jclass This, jobject aDimTimer, jint secs)
{
   jobject callback_param;
   void  (*callback_function)();
 
  if(This){}
  callback_param = (*env)->NewGlobalRef(env, aDimTimer);
  callback_function = &timer_callback; //TODO who should do the cleanup?

//printf("Starting timer %d, %08x %08x\n", secs, (long)callback_param, aDimTimer);
  dtq_start_timer(secs, callback_function, callback_param); 

  return (jlong)callback_param;
}

/*
 * Class:     dim_DimTimer
 * Method:    stop
 * Signature: (Ldim/DimTimer)V
 */
JNIEXPORT void JNICALL Java_dim_DimTimer_stop
//  (JNIEnv *env, jclass This, jobject aDimTimer)
  (JNIEnv *env, jclass This, jlong aDimTimer)
{
   jobject callback_param;
   int ret;
 
   if(env){}
   if(This){}
//  callback_param = (*env)->NewGlobalRef(env, aDimTimer);
   callback_param = (jobject) aDimTimer;

//printf("Stopping timer %08x %08X\n", callback_param, aDimTimer);
  ret = dtq_stop_timer((dim_long)callback_param);
 //printf("ret = %d\n", ret);

  return;
}


/*
 * Class:     dim_Client
 * Method:    releaseService
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_dim_Client_releaseService
  (JNIEnv* env, jclass This, jint sid)
{

  DIC_SERVICE *servp;

//  DBGe(dim_Dbg_INFO_SERVICE) ; /* Trap only, report later */

  if(This){}
  servp = (DIC_SERVICE *)id_get_ptr(sid, SRC_DIC);

/*
Hans Essel, 3.12.08
Without deleting the global reference, the Java GC could not free the object!
Any DimInfo object would stay forever. This is a memory leak.
*/
  if(servp != NULL)
  {
	  //  DBGx(dim_Dbg_INFO_SERVICE) printf("DimJNI: Client.releaseService(%d (%s))\n", sid, servp->serv_name);
	  servp->user_routine = NULL; // make sure this is not called anymore
	  (*env)->DeleteGlobalRef(env, (jobject) servp->tag);
	  servp->tag = 0;
  }

  dic_release_service(sid);
	return;
}

/*
 * Class:     dim_Client
 * Method:    noPadding
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_Client_noPadding
  (JNIEnv* env, jclass This)
{

  if(env){}
  if(This){}
  dic_disable_padding();
	return;
}

/*
 * Class:     dim_Client
 * Method:    getFormat
 * Signature: (I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Client_getFormat
  (JNIEnv* env, jclass This, jint sid)
{

  if(This){}
	return (*env)->NewStringUTF(env, (char*)dic_get_format(sid));
}

/*
 * Class:     dim_Client
 * Method:    stop
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_Client_stop
  (JNIEnv* env, jclass This)
{
	extern void dim_stop();

	if(env){}
	if(This){}
	dim_stop();
	return;
}

/*
 * Class:     dim_DimInfo
 * Method:    getQuality
 * Signature: (I)I;
 */
JNIEXPORT jint JNICALL Java_dim_DimInfo_getQuality
  (JNIEnv* env, jclass This, jint sid)
{
	int ret;
	
	if(env){}
	if(This){}
	ret = dic_get_quality(sid);
	return ret;
}

/*
 * Class:     dim_DimInfo
 * Method:    getTimestamp
 * Signature: (I)I;
 */
JNIEXPORT jint JNICALL Java_dim_DimInfo_getTimestamp
  (JNIEnv* env, jclass This, jint sid)
{
	int mysecs, mymilli;
  if(env){}
  if(This){}
	dic_get_timestamp(sid, &mysecs, &mymilli);
	return mysecs;
}

/*
 * Class:     dim_DimInfo
 * Method:    getTimestampMillisecs
 * Signature: (I)I;
 */
JNIEXPORT jint JNICALL Java_dim_DimInfo_getTimestampMillisecs
  (JNIEnv* env, jclass This, jint sid)
{
	int mysecs, mymilli;
  if(env){}
  if(This){}
	dic_get_timestamp(sid, &mysecs, &mymilli);
	return mymilli;
}

/* implementation of dim_server.h =============================================================== */


/*
 * Class:     dim_Server
 * Method:    startServing
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_dim_Server_startServing
  (JNIEnv* env, jclass This, jstring serverName)
{
	const char* serverNameUTF = (*env)->GetStringUTFChars(env, serverName, 0);

	if(This){}

	dis_start_serving(serverNameUTF);

	DBGe(dim_Dbg_SERVER) printf("DimJNI: Start serving\n");

	(*env)->ReleaseStringUTFChars(env, serverName, serverNameUTF);
	return 0;
}

/*
 * Class:     dim_Server
 * Method:    stopServing
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_Server_stopServing
  (JNIEnv* env, jclass This)
{
	DBGe(dim_Dbg_SERVER) printf("DimJNI: Stop serving\n");
	if(env){}
	if(This){}
	dis_stop_serving();
	return;
}

void server_getInfo_callback(jobject* _dataEncoder, void* *address, int *size)
{
  /* server_getInfo_callback is invoked when dim needs the data for a service.
   * The data is obtained by calling the encodeData method of the DataEncoder
   * interface which returns us a Memory object. Next we extract the data address
   * and size from the returned Memory object.
   */
  /* Note, the DataEncoder can obtain the identity of the client by calling
   * getClient or getClientConnID */

  /* thou shall not not use volatile storage to return info to dim */
	jobject dataEncoder = *_dataEncoder;
	jobject theMemory;
	JNIEnv* env;
	int doit;

//	DBGe(dim_Dbg_SERVICE_CALLBACK) ; /* no report, only trap */

	doit = dim_jni_attachThread(&env);
//	(*theJavaVM)->AttachCurrentThread(theJavaVM, (void *)&env, NULL);

	theMemory = (*env)->CallObjectMethod(env, dataEncoder, NativeDataEncoder_encodeNativeData);
	if(theMemory == NULL)
	{
		*address  = 0;
		*size     = 0;
	}
	else
	{
		*address  = (void*) (*env)->GetLongField(env, theMemory, NativeDataMemory_dataAddress);
		*size     = (*env)->GetIntField(env, theMemory, NativeDataMemory_dataSize);
//		printf("data address = %x, data size = %d\n",*address, *size);
	}
	DBGx(dim_Dbg_SERVICE_CALLBACK) printf("DimJNI: server_SERVICE_CALLBACK(dataEncoder=%08lx)\n        ==>    data: %08lx size %08x\n", (dim_long)dataEncoder, (dim_long) *address, *size); 

	if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env); // clear any possible exception, if we do not do this, all further methods will fail!!
	if(doit)
		(*theJavaVM)->DetachCurrentThread(theJavaVM);

	return;
}


/*
 * Class:     dim_Server
 * Method:    getClientConnID
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Server_getClientConnID
  (JNIEnv* env, jclass This)
{
	DBGe(dim_Dbg_GETCLIENT) printf("DimJNI: Server.getClientConnID\n");

	if(env){}
	if(This){}
	return dis_get_conn_id();
}

/*
 * Class:     dim_Server
 * Method:    getClient
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Server_getClient
  (JNIEnv* env, jclass This)
{
	char name[MAX_NODE_NAME+MAX_TASK_NAME+4];

	DBGe(dim_Dbg_GETCLIENT) printf("DimJNI: Server.getClient\n");

	if(This){}
	if(dis_get_client(name)) 
		return (*env)->NewStringUTF(env, name);
	else					 
		return NULL;
}

/*
 * Class:     dim_Server
 * Method:    getServices
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Server_getServices
  (JNIEnv* env, jclass This)
{
	int id;

	DBGe(dim_Dbg_GETCLIENT) printf("DimJNI: Server.getClientServices\n");

	if(This){}
	if( (id = dis_get_conn_id()) ) 
		return (*env)->NewStringUTF(env, dis_get_client_services(id));
	else
		return NULL;
}

/*
 * Class:     dim_Client
 * Method:    getServerPID
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Client_getServerPID
  (JNIEnv* env, jclass This)
{
	int pid, ret;

	if(env){}
	if(This){}
	ret = dic_get_server_pid(&pid);
	if(!ret)
		return 0;
	return pid;
}

/*
 * Class:     dim_Client
 * Method:    getServerConnID
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_dim_Client_getServerConnID
  (JNIEnv* env, jclass This)
{
	if(env){}
	if(This){}
	return dic_get_conn_id();
}

/*
 * Class:     dim_Client
 * Method:    getServer
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Client_getServer
  (JNIEnv* env, jclass This)
{
	char name[MAX_NODE_NAME+MAX_TASK_NAME+4];

	if(This){}
	if(dic_get_server(name)) 
		return (*env)->NewStringUTF(env, name);
	else					 
		return NULL;
}

/*
 * Class:     dim_Client
 * Method:    getServices
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Client_getServices
  (JNIEnv* env, jclass This)
{
	int id;

	if(This){}
	if( (id = dic_get_conn_id()) ) 
		return (*env)->NewStringUTF(env, dic_get_server_services(id));
	else
		return NULL;
}

/*
 * Class:     dim_Server
 * Method:    addService
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ldim/DataEncoder;)I
 */
JNIEXPORT jint JNICALL Java_dim_Server_addService
  (JNIEnv* env, jclass This, jstring serviceName, jstring serviceType, jobject dataEncoder)
{
	const char* serviceNameUTF = (*env)->GetStringUTFChars(env, serviceName, 0);
	const char* serviceTypeUTF = (*env)->GetStringUTFChars(env, serviceType, 0);
	jint sid;

//  DBGe(dim_Dbg_ADD_SERVICE) ; /* no reporting, for trap only */

	if(This){}
 	dataEncoder = (*env)->NewGlobalRef(env, dataEncoder);
	sid = (jint)dis_add_service(serviceNameUTF, serviceTypeUTF, 0, 0, server_getInfo_callback, dataEncoder);

	DBGx(dim_Dbg_ADD_SERVICE) printf("DimJNI: Server.addService(%s,%s, @%08lx)=%d\n",serviceNameUTF, serviceTypeUTF, (dim_long)dataEncoder, sid);

	(*env)->ReleaseStringUTFChars(env, serviceName, serviceNameUTF);
	(*env)->ReleaseStringUTFChars(env, serviceType, serviceTypeUTF);
	return sid;
}


void server_cmnd_callback(jobject* _theDataDecoder, void* dataAddress, int* _dataSize)
{

	DBGe(dim_Dbg_CMND_CALLBACK) printf("DimJNI: server CMND_CALLBACK(data: %08lx(%08x))\n", (dim_long) dataAddress, *_dataSize);

  decodeData(_theDataDecoder, dataAddress, _dataSize, 0);
}


/*
 * Class:     dim_Server
 * Method:    addCommand
 * Signature: (Ljava/lang/String;Ljava/lang/String;Ldim/DataDecoder;)I
 */
JNIEXPORT jint JNICALL Java_dim_Server_addCommand
  (JNIEnv* env, jclass This, jstring serviceName, jstring serviceType, jobject dataDecoder)
{
	const char* serviceNameUTF = (*env)->GetStringUTFChars(env, serviceName, 0);
	const char* serviceTypeUTF = (*env)->GetStringUTFChars(env, serviceType, 0);
	jint sid;

//	DBGe(dim_Dbg_ADD_CMND) ; /* trap only, repot later */
	if(This){}
 	dataDecoder = (*env)->NewGlobalRef(env, dataDecoder);
	sid = (jint)dis_add_cmnd(serviceNameUTF, serviceTypeUTF, server_cmnd_callback, dataDecoder);

	DBGx(dim_Dbg_ADD_CMND) printf("DimJNI: Server.addCmnd(%s,%s, @%08lx) = %d\n",serviceNameUTF, serviceTypeUTF, (dim_long) dataDecoder, sid);

	(*env)->ReleaseStringUTFChars(env, serviceName, serviceNameUTF);
	(*env)->ReleaseStringUTFChars(env, serviceType, serviceTypeUTF);
	return sid;
}


/*
 * Class:     dim_Server
 * Method:    selectiveUpdateService
 * Signature: (I[I)I
 */
JNIEXPORT jint JNICALL Java_dim_Server_selectiveUpdateService
  (JNIEnv* env, jclass This, jint sid, jintArray clients)
{
	jint* clientArray;
	extern void do_update_service(unsigned, int *);

	if(This){}
	if(clients==NULL) clientArray = NULL;
	else			  clientArray = (*env)->GetIntArrayElements(env,clients,0);

	DBGe(dim_Dbg_UPDATE_SERVICE) printf("DimJNI: Server.updateService %d\n", sid);
	do_update_service((unsigned int)sid, (int *)clientArray);

	if(clientArray!=NULL) (*env)->ReleaseIntArrayElements(env,clients,clientArray,JNI_ABORT);


	return 0;
}

/*
 * Class:     dim_Server
 * Method:    removeService
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_dim_Server_removeService
  (JNIEnv* env, jclass This, jint sid)
{
	DBGe(dim_Dbg_RELEASE_SERVICE) printf("DimJNI: Server.removedService %d\n", sid);
	if(env){}
	if(This){}
	dis_remove_service(sid);
	return 0;
}

/*
 * Class:     dim_Server
 * Method:    noPadding
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_Server_noPadding
  (JNIEnv* env, jclass This)
{

  if(env){}
  if(This){}
  dis_disable_padding();
	return;
}

/*
 * Class:     dim_DimErrorHandler
 * Method:    addSrvErrorHandler
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_DimErrorHandler_addSrvErrorHandler
  (JNIEnv* env, jclass This)
{

	void  (*callback_function)();

	if(env){}
	if(This){}
	callback_function = &server_error_callback;

	dis_add_error_handler( callback_function );

  return;
}

/*
 * Class:     dim_DimErrorHandler
 * Method:    addCltErrorHandler
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_DimErrorHandler_addCltErrorHandler
  (JNIEnv* env, jclass This)
{

	void  (*callback_function)();

	if(env){}
	if(This){}
	callback_function = &client_error_callback;
 
	dic_add_error_handler( callback_function ); 

  return;
}

/*
 * Class:     dim_DimExitHandler
 * Method:    addExitHandler
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_dim_DimExitHandler_addExitHandler
  (JNIEnv* env, jclass This)
{

	void  (*callback_function)();

	if(env){}
	if(This){}
	callback_function = &server_exit_callback;

	dis_add_exit_handler( callback_function ); 

  return;
}

/*
 * Class:     dim_DimServer
 * Method:    disableAST
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_dim_DimServer_disableAST
  (JNIEnv* env, jclass This)
{
	
	DIM_LOCK
	if(env){}
	if(This){}
	return;
}

/*
 * Class:     dim_DimServer
 * Method:    enableAST
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_dim_DimServer_enableAST
  (JNIEnv* env, jclass This)
{
	
	if(env){}
	if(This){}
	DIM_UNLOCK
	return;
}

/*
 * Class:     dim_DimClient
 * Method:    disableAST
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_dim_DimClient_disableAST
  (JNIEnv* env, jclass This)
{
	
	DIM_LOCK
	if(env){}
	if(This){}
	return;
}

/*
 * Class:     dim_DimClient
 * Method:    enableAST
 * Signature: ()V;
 */
JNIEXPORT void JNICALL Java_dim_DimClient_enableAST
  (JNIEnv* env, jclass This)
{
	
	if(env){}
	if(This){}
	DIM_UNLOCK
	return;
}

/*
 * Class:     dim_DimServer
 * Method:    setDnsNode
 * Signature: (Ljava/lang/String)V;
 */
JNIEXPORT void JNICALL Java_dim_DimServer_setDnsNode
  (JNIEnv* env, jclass This, jstring nodes)
{
	const char* nodesUTF = (*env)->GetStringUTFChars(env, nodes, 0);
	
	if(env){}
	if(This){}
	dis_set_dns_node(nodesUTF);
	return;
}

/*
 * Class:     dim_DimServer
 * Method:    setDnsPort
 * Signature: (I)V;
 */
JNIEXPORT void JNICALL Java_dim_DimServer_setDnsPort
  (JNIEnv* env, jclass This, jint port)
{
	if(env){}
	if(This){}
	dis_set_dns_port(port);
	return;
}

/*
 * Class:     dim_DimClient
 * Method:    setDnsNode
 * Signature: (Ljava/lang/String)V;
 */
JNIEXPORT void JNICALL Java_dim_DimClient_setDnsNode
  (JNIEnv* env, jclass This, jstring nodes)
{
	const char* nodesUTF = (*env)->GetStringUTFChars(env, nodes, 0);
	
	if(env){}
	if(This){}
	dic_set_dns_node(nodesUTF);
	dic_close_dns();
	return;
}

/*
 * Class:     dim_DimClient
 * Method:    setDnsPort
 * Signature: (I)V;
 */
JNIEXPORT void JNICALL Java_dim_DimClient_setDnsPort
  (JNIEnv* env, jclass This, jint port)
{
	if(env){}
	if(This){}
	dic_set_dns_port(port);
	return;
}

/*
 * Class:     dim_DimServer
 * Method:    getDnsNode
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_DimServer_getDnsNode
  (JNIEnv* env, jclass This)
{
	char nodes[255];
	
	if(This){}
	dis_get_dns_node(nodes);
	return (*env)->NewStringUTF(env, (char*)nodes);
}

/*
 * Class:     dim_DimClient
 * Method:    getDnsNode
 * Signature: ()Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_DimClient_getDnsNode
  (JNIEnv* env, jclass This)
{
	char nodes[255];
	
	if(This){}
	dic_get_dns_node(nodes);
	return (*env)->NewStringUTF(env, (char*)nodes);
}

/*
 * Class:     dim_DimServer
 * Method:    getDnsPort
 * Signature: ()I;
 */
JNIEXPORT jint JNICALL Java_dim_DimServer_getDnsPort
  (JNIEnv* env, jclass This)
{
	
	if(env){}
	if(This){}
	return dis_get_dns_port();
}

/*
 * Class:     dim_DimClient
 * Method:    getDnsPort
 * Signature: ()I;
 */
JNIEXPORT jint JNICALL Java_dim_DimClient_getDnsPort
  (JNIEnv* env, jclass This)
{

	if(env){}
	if(This){}
	return dic_get_dns_port();
}

/*
 * Class:     dim_DimService
 * Method:    setQuality
 * Signature: (II)V;
 */
JNIEXPORT void JNICALL Java_dim_DimService_setQuality
  (JNIEnv* env, jclass This, jint sid, jint qual)
{
	
	if(env){}
	if(This){}
	dis_set_quality(sid, qual);
	return;
}


/*
 * Class:     dim_DimService
 * Method:    setTimestamp
 * Signature: (III)V;
 */
JNIEXPORT void JNICALL Java_dim_DimService_setTimestamp
  (JNIEnv* env, jclass This, jint sid, jint secs, jint millisecs)
{

	if(env){}
	if(This){}
	dis_set_timestamp(sid, secs, millisecs);
	return;
}

/* implementation of dim_memory,h =============================================================== */

/*
 * Class:     dim_Memory
 * Method:    dumpInternalData
 * Signature: (III)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_dumpInternalData
  (JNIEnv *env, jclass nativeClass, jlong internalDataAddress, jint internalDataSize, jint dumpOptions)
{
  {
    int* data = (int*) internalDataAddress;
    int leng = (int)internalDataSize/(int)sizeof(int);
    int  i;

	if(env){}
	if(dumpOptions){}
	if(nativeClass){}
    for (i=0;i<leng;i++)
    {
      if((i%8)==0) printf("%04x:",i);
      printf(" %08x", *(data++));
      if((i%8)==7) printf("\n");
    }
    if((leng%8)!=0) printf("\n");
  }
  return;
}


/*
 * Class:     dim_Memory
 * Method:    getBoolean
 * Signature: (I)Z
 */
JNIEXPORT jboolean JNICALL Java_dim_Memory_getBoolean
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getBoolean\n");
	return *(jboolean*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getChar
 * Signature: (I)C
 */
JNIEXPORT jchar JNICALL Java_dim_Memory_getChar
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getChar\n");
	return *(jchar*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getByte
 * Signature: (I)B
 */
JNIEXPORT jbyte JNICALL Java_dim_Memory_getByte
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getByte\n");
	return *(jbyte*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getShort
 * Signature: (I)S
 */
JNIEXPORT jshort JNICALL Java_dim_Memory_getShort
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getShort\n");
	return *(jshort*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getInt
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_dim_Memory_getInt
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getInt\n");
	return *(jint*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getLong
 * Signature: (I)J
 */
JNIEXPORT jlong JNICALL Java_dim_Memory_getLong
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getLong\n");
	return *(jlong*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getFloat
 * Signature: (I)F
 */
JNIEXPORT jfloat JNICALL Java_dim_Memory_getFloat
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getFloat\n");
	return *(jfloat*)nativeDataAddress;
}

/*
 * Class:     dim_Memory
 * Method:    getDouble
 * Signature: (I)D
 */
JNIEXPORT jdouble JNICALL Java_dim_Memory_getDouble
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getDouble\n");
	return *(jdouble*)nativeDataAddress;
}


/*
 * Class:     dim_Memory
 * Method:    getString
 * Signature: (I,I)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_dim_Memory_getString
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jint maxSize)
{
	if(env){}
	if(nativeClass){}
	if(maxSize){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.getString\n");
	return (*env)->NewStringUTF(env, (char*)nativeDataAddress);
}


/*
 * Class:     dim_Memory
 * Method:    copyIntoBooleanArray
 * Signature: (I[ZII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoBooleanArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jbooleanArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoBooleanArray\n");
	(*env)->SetBooleanArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoCharArray
 * Signature: (I[CII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoCharArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jcharArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoCharArray\n");
	(*env)->SetCharArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoByteArray
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoByteArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jbyteArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoByteArray\n");
	(*env)->SetByteArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoShortArray
 * Signature: (I[SII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoShortArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jshortArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoShortArray\n");
	(*env)->SetShortArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoIntArray
 * Signature: (I[III)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoIntArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jintArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoIntArray\n");
	(*env)->SetIntArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoLongArray
 * Signature: (I[JII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoLongArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jlongArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoLongArray\n");
	(*env)->SetLongArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoFloatArray
 * Signature: (I[FII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoFloatArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jfloatArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoFloatArray\n");
	(*env)->SetFloatArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_Memory
 * Method:    copyIntoDoubleArray
 * Signature: (I[DII)V
 */
JNIEXPORT void JNICALL Java_dim_Memory_copyIntoDoubleArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jdoubleArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY) printf("DimJNI: Memory.copyIntoDoubleArray\n");
	(*env)->SetDoubleArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}




/* implementation of dim_mutablememory.h ======================================================== */

/*
 * Class:     dim_MutableMemory
 * Method:    allocateNativeDataBlock
 * Signature: (I)I
 */
JNIEXPORT jlong JNICALL Java_dim_MutableMemory_allocateNativeDataBlock
  (JNIEnv* env, jclass nativeClass, jint size)
{
  jlong address;
	if(env){}
	if(nativeClass){}
//	DBGe(dim_Dbg_MEMORY_ALLOCATE) ; /* report only */
  address = (jlong) malloc((size_t)size);
	DBGx(dim_Dbg_MEMORY_ALLOCATE) printf("DimJNI: MutableMemory.allocateNativeDataBlock of %d bytes at 0x%08lx\n", size, address);
  return address;
}

/*
 * Class:     dim_MutableMemory
 * Method:    releaseNativeDataBlock
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_releaseNativeDataBlock
  (JNIEnv* env, jclass nativeClass, jlong desc)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MEMORY_ALLOCATE) printf("DimJNI: MutableMemory.releaseNativeDataBlock 0x%08lx\n", desc);
 //printf("free %08X\n", desc);
 	free((void*)desc);
	return;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setBoolean
 * Signature: (IZ)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setBoolean
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jboolean data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setBoolean(0x%08lx, %02x)\n", nativeDataAddress, data);
	*(jboolean*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setChar
 * Signature: (IC)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setChar
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jchar data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setChar(0x%08lx, %02x)\n", nativeDataAddress, data);
	*(jchar*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setByte
 * Signature: (IB)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setByte
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jbyte data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setByte(0x%08lx, %02x)\n", nativeDataAddress, data);
	*(jbyte*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setShort
 * Signature: (IS)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setShort
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jshort data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setShort(0x%08lx, %04x)\n", nativeDataAddress, data);
	*(jshort*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setInt
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setInt
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jint data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setInt(0x%08lx, %0x)\n", nativeDataAddress, data);
	*(jint*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setLong
 * Signature: (IJ)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setLong
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jlong data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setLong(0x%08lx, %08x)\n", nativeDataAddress, (unsigned)data);
	*(jlong*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setFloat
 * Signature: (IF)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setFloat
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jfloat data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setFloat(0x%08lx, %f)\n", nativeDataAddress, data);
	*(jfloat*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setDouble
 * Signature: (ID)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setDouble
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jdouble data)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setDouble(0x%08lx, %08x)\n", nativeDataAddress, (unsigned)data);
	*(jdouble*)nativeDataAddress = data;
}

/*
 * Class:     dim_MutableMemory
 * Method:    setString
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_setString
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jstring data)
{
	const char* charData = (*env)->GetStringUTFChars(env, data, 0);

	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.setString(0x%08lx, %s)\n", nativeDataAddress, charData);

	if(nativeClass){}
	strcpy((char*)nativeDataAddress, charData);
	(*env)->ReleaseStringUTFChars(env, data, charData);
}


/*
 * Class:     dim_MutableMemory
 * Method:    copyFromBooleanArray
 * Signature: (I[ZII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromBooleanArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jbooleanArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromBooleanArray\n");
	(*env)->GetBooleanArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromCharArray
 * Signature: (I[CII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromCharArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jcharArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromCharArray\n");
	(*env)->GetCharArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromByteArray
 * Signature: (I[BII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromByteArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jbyteArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromByteArray\n");
	(*env)->GetByteArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromShortArray
 * Signature: (I[SII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromShortArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jshortArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromShortArray\n");
	(*env)->GetShortArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromIntArray
 * Signature: (I[III)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromIntArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jintArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromIntArray\n");
	(*env)->GetIntArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromLongArray
 * Signature: (I[JII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromLongArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jlongArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromLongArray\n");
	(*env)->GetLongArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromFloatArray
 * Signature: (I[FII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromFloatArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jfloatArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromFloatArray\n");
	(*env)->GetFloatArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyFromDoubleArray
 * Signature: (I[DII)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyFromDoubleArray
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jdoubleArray array, jint arrayOffset, jint length)
{
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyFromDoubleArray\n");
	(*env)->GetDoubleArrayRegion(env, array, arrayOffset, length, (void*) nativeDataAddress);
	return ;
}

/*
 * Class:     dim_MutableMemory
 * Method:    copyNativeDataBlock
 * Signature: (II)V
 */
JNIEXPORT void JNICALL Java_dim_MutableMemory_copyNativeDataBlock
  (JNIEnv* env, jclass nativeClass, jlong destinationDataAddress, jlong sourceDataAddress, jint length)
{
	if(env){}
	if(nativeClass){}
	DBGe(dim_Dbg_MUTABLE_MEMORY) printf("DimJNI: MutableMemory.copyNativeDataBlock\n");
	memcpy((void *)destinationDataAddress, (void *)sourceDataAddress, (size_t)length);
	return ;
}




/* implementation of dim_objectdescriptor.h ===================================================== */

/* fancy packing methods */

typedef enum {f_skip,
              f_boolean, f_byte, f_char, f_short, f_int, f_long, f_float, f_double, f_string, f_object,
              a_boolean, a_byte, a_char, a_short, a_int, a_long, a_float, a_double,	a_string, a_object,
              c_boolean, c_byte, c_char, c_short, c_int, c_long, c_float, c_double, c_string, c_object
			} FieldType;
typedef struct objectDescriptorEntry_struct objectDescriptorEntry_type;
struct objectDescriptorEntry_struct
{
	FieldType type;
	int       length;
	int       offset;
	jfieldID  fieldID;
	jarray    array;
	int       arrayOffset;
};

typedef struct objectDescriptor_struct objectDescriptor_type;
struct objectDescriptor_struct
{
	jclass objectClass;
	int	entries;
	int	maxEntries;
	objectDescriptorEntry_type* entry;
};


/*
 * Class:     dim_ObjectDescriptor
 * Method:    newObjectDescriptor
 * Signature: (Ljava/lang/Class;I)I
 */
JNIEXPORT jlong JNICALL Java_dim_ObjectDescriptor_newObjectDescriptor
  (JNIEnv* env, jclass nativeClass, jclass objectClass, jint maxEntries)
{
	objectDescriptor_type* descriptor;

//  DBGe(dim_Dbg_DESCRIPTORS) ; /* trap only, report on exit */
  // todo put object descriptor and entry array in the same malloc (for dump purposes)
//printf("malloc descriptor\n");
	if(env){}
	if(nativeClass){}
	if(maxEntries==0) maxEntries = 10;
	descriptor = (objectDescriptor_type*) malloc(sizeof(objectDescriptor_type));
	descriptor->entry = (objectDescriptorEntry_type*) malloc((size_t)(maxEntries * (jint)sizeof(objectDescriptorEntry_type)));
	descriptor->objectClass = (*env)->NewGlobalRef(env, objectClass);
	descriptor->entries = 0;
	descriptor->maxEntries = maxEntries;

	DBGx(dim_Dbg_DESCRIPTORS) printf("DimJNI: Native.newObjectDescriptor %08lx\n", (dim_long)descriptor);
	return (dim_long) descriptor;
}

objectDescriptorEntry_type* getNextDescriptorEntry(objectDescriptor_type* descriptor)
{
	if(descriptor->entries == descriptor->maxEntries)
	{
		objectDescriptorEntry_type* entry = realloc(descriptor->entry, (size_t)(descriptor->maxEntries+10));
//printf("realloc descriptor\n");
		if(entry==NULL) return NULL;

		descriptor->entry = entry;
		descriptor->maxEntries = descriptor->maxEntries+10;
	}
	return descriptor->entry + descriptor->entries++;
}



/*
 * Class:     dim_ObjectDescriptor
 * Method:    addFieldToObjectDescriptor
 * Signature: (ILjava/lang/String;Ljava/lang/String;I)I
 */
JNIEXPORT jint JNICALL Java_dim_ObjectDescriptor_addFieldToObjectDescriptor
  (JNIEnv* env, jclass nativeClass, jlong desc, jstring fieldName, jstring fieldType, jint offset)
{
//	FieldType field_type = f_int;
	objectDescriptorEntry_type* entry = getNextDescriptorEntry((objectDescriptor_type*) desc);
	const char*                 name  = (*env)->GetStringUTFChars(env, fieldName, 0);
	const char*                 type  = (*env)->GetStringUTFChars(env, fieldType, 0);
	jfieldID fieldID  = (*env)->GetFieldID(env, ((objectDescriptor_type*) desc)->objectClass, name, type);

	// TODO throw an error if there is no such FieldID

	DBGe(dim_Dbg_DESCRIPTORS) printf("DimJNI: Native.addFieldToObjectDescriptor %08lx Field %s Type %s\n", (dim_long) desc, name, type);
	// TODO: if(entry==NULL) throw out-of-memory exception, set length to 0

	// TODO: if(fieldType == "I") field_type = f_int; etc

	if(nativeClass){}
	entry->type         =f_skip;
	entry->length       =0;
	entry->offset       =offset;
	entry->fieldID      =fieldID;
	entry->array        =0;
	entry->arrayOffset  =0;

	switch (*type)
	{
	case 'Z':
	{
		entry->type	  =f_boolean;
		entry->length =sizeof(jboolean);
		break;
	}
	case 'B':
	{
		entry->type	  =f_byte;
		entry->length =sizeof(jbyte);
		break;
	}

	case 'C':
	{
		entry->type	  =f_char;
		entry->length =sizeof(jchar);
		break;
	}

	case 'S':
	{
		entry->type	  =f_short;
		entry->length =sizeof(jshort);
		break;
	}

	case 'I':
	{
		entry->type	  =f_int;
		entry->length =sizeof(jint);
		break;
	}

	case 'J':
	{
		entry->type	  =f_long;
		entry->length =sizeof(jlong);
		break;
	}

	case 'F':
	{
		entry->type	  =f_float;
		entry->length =sizeof(jfloat);
		break;
	}

	case 'D':
	{
		entry->type	  =f_double;
		entry->length =sizeof(jdouble);
		break;
	}

	//case '[':
	//{
	//	// TODO deal with array types
	//}
	default :
	{
		printf("DimJNI: addFieldToObjectDescriptor - type %s not yet supported. (field %s)\n", name, type);
	}
	}


	(*env)->ReleaseStringUTFChars(env, fieldName, name);
	(*env)->ReleaseStringUTFChars(env, fieldType, type);
	return entry->length;
}


/*
 * Class:     dim_ObjectDescriptor
 * Method:    deleteObjectDescriptor
 * Signature: (I)V
 */
JNIEXPORT void JNICALL Java_dim_ObjectDescriptor_deleteObjectDescriptor
   (JNIEnv* env, jclass nativeClass, jlong desc)
{
	objectDescriptor_type* descriptor = (objectDescriptor_type*) desc;
 
	if(nativeClass){}
	DBGe(dim_Dbg_DESCRIPTORS) printf("DimJNI: Native.deleteObjectDescriptor %08lx\n", (dim_long)desc);
	(*env)->DeleteGlobalRef(env, descriptor->objectClass);
//printf("free descriptor\n");
	free(descriptor->entry);
	free(descriptor);
	return;
}


/*
 * Class:     dim_ObjectDescriptor
 * Method:    copyIntoObject
 * Signature: (ILjava/lang/Object;I)V
 */
 
JNIEXPORT void JNICALL Java_dim_ObjectDescriptor_copyIntoObject
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jobject theObject, jlong desc)
{
	int i;
	objectDescriptorEntry_type* entry;

	objectDescriptor_type* descriptor = (objectDescriptor_type*) desc;
	jclass objectClass = descriptor->objectClass;

	DBGe(dim_Dbg_DESCRIPTORS) printf("DimJNI: Native.copyIntoObject %08lx\n", (dim_long)desc);

	if(nativeClass){}
	// test if object can be cast to object class
	if((*env)->IsInstanceOf(env, theObject, objectClass) != JNI_TRUE)
	{
		// throw exception
		jclass exceptionClass = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
		(*env)->ThrowNew(env, exceptionClass, " (Sorry...)");
		return;
	}

	// loop over descriptor entries
	entry = descriptor->entry;
	for (i=0; i<descriptor->entries; i++)
	{
		switch (entry->type)
		{
		case f_boolean:
			(*env)->SetBooleanField(env, theObject, entry->fieldID, *(jboolean*) (nativeDataAddress+entry->offset));
			break;
		case f_byte:
			(*env)->SetByteField(   env, theObject, entry->fieldID, *(jbyte*)    (nativeDataAddress+entry->offset));
			break;
		case f_char:
			(*env)->SetCharField(   env, theObject, entry->fieldID, *(jchar*)    (nativeDataAddress+entry->offset));
			break;
		case f_short:
			(*env)->SetShortField(  env, theObject, entry->fieldID, *(jshort*)   (nativeDataAddress+entry->offset));
			break;
		case f_int:
			(*env)->SetIntField(    env, theObject, entry->fieldID, *(jint*)     (nativeDataAddress+entry->offset));
			break;
		case f_long:
			(*env)->SetLongField(   env, theObject, entry->fieldID, *(jlong*)    (nativeDataAddress+entry->offset));
			break;
		case f_float:
			(*env)->SetFloatField(  env, theObject, entry->fieldID, *(jfloat*)   (nativeDataAddress+entry->offset));
			break;
		case f_double:
			(*env)->SetDoubleField( env, theObject, entry->fieldID, *(jdouble*)  (nativeDataAddress+entry->offset));
			break;
		case a_boolean:
			(*env)->SetBooleanArrayRegion(env, entry->array, entry->arrayOffset, entry->length, (void*) (nativeDataAddress+entry->offset));
			break;
		case c_boolean:
			(*env)->SetBooleanArrayRegion(env, entry->array, entry->arrayOffset, entry->length, (void*) (nativeDataAddress+entry->offset));
		//TODO :: complete this list, including recursive call to this function for objects
			break;
		default:
			break;
		}
		//TODO :: ?? if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env); // clear any possible exception, if we do not do this, all further methods will fail!!
		entry++;
	}
	return;
}

/*
 * Class:     dim_ObjectDescriptor
 * Method:    copyFromObject
 * Signature: (ILjava/lang/Object;I)V
 */
JNIEXPORT void JNICALL Java_dim_ObjectDescriptor_copyFromObject
  (JNIEnv* env, jclass nativeClass, jlong nativeDataAddress, jobject theObject, jlong desc)
{
	int i;
	objectDescriptorEntry_type* entry;

	objectDescriptor_type* descriptor = (objectDescriptor_type*) desc;
	jclass objectClass = descriptor->objectClass;

	DBGe(dim_Dbg_DESCRIPTORS) printf("DimJNI: Native.copyFromObject %08x\n", (int)desc);

	if(nativeClass){}
	// test if object can be cast to object class
	if((*env)->IsInstanceOf(env, theObject, objectClass) != JNI_TRUE)
	{
		// throw exception
		jclass exceptionClass = (*env)->FindClass(env, "java/lang/IllegalArgumentException");
		(*env)->ThrowNew(env, exceptionClass, " (Sorry...)");
		return;
	}

	// loop over descriptor entries
	entry = descriptor->entry;
	for (i=0; i<descriptor->entries; i++)
	{
		switch (entry->type)
		{
		case f_boolean:
			*(jboolean*) (nativeDataAddress+entry->offset) = (*env)->GetBooleanField(env, theObject, entry->fieldID);
			break;
		case f_byte:
			*(jbyte*) (nativeDataAddress+entry->offset)    = (*env)->GetByteField(env, theObject, entry->fieldID);
			break;
		case f_char:
			*(jchar*) (nativeDataAddress+entry->offset)    = (*env)->GetCharField(env, theObject, entry->fieldID);
			break;
		case f_short:
			*(jshort*) (nativeDataAddress+entry->offset)   = (*env)->GetShortField(env, theObject, entry->fieldID);
			break;
		case f_int:
			*(jint*) (nativeDataAddress+entry->offset)     = (*env)->GetIntField(env, theObject, entry->fieldID);
			break;
		case f_long:
			*(jlong*) (nativeDataAddress+entry->offset)    = (*env)->GetLongField(env, theObject, entry->fieldID);
			break;
		case f_float:
			*(jfloat*) (nativeDataAddress+entry->offset)   = (*env)->GetFloatField(env, theObject, entry->fieldID);
			break;
		case f_double:
			*(jdouble*) (nativeDataAddress+entry->offset)  = (*env)->GetDoubleField(env, theObject, entry->fieldID);
			break;
//		case a_boolean:
//      (*env)->GetBooleanArrayRegion(env, array, 0, length, (void*) nativeDataAddress);
//			break;
//		case c_boolean:
//			*(jbyte*) (nativeDataAddress+entry->offset)  = (*env)->GetField(env, theObject, entry->fieldID);
//			(*env)->SetBooleanArrayRegion(env, entry->array, entry->arrayOffset, entry->length, (void*) (nativeDataAddress+entry->offset));
//		//TODO :: complete this list, including recursive call to this function for objects
//			break;
		default:
			break;
		}
		//TODO :: ?? if ((*env)->ExceptionOccurred(env)) (*env)->ExceptionDescribe(env); // clear any possible exception, if we do not do this, all further methods will fail!!
		entry++;
	}
	return;
}


#ifdef zombies


//	jstring	  myFormat;
//	myFormat = (*env)->NewStringUTF(env, "x");
//	(*env)->CallVoidMethod(env, dataObject, setDimFormatMID, myFormat);		// JUST TO TRY

//	jintArray dataArray;
//	int data[10]={1,2,3,4,5,6,7,8,9,-1};
//	dataArray = (*env)->NewIntArray(env,10);
//	(*env)->SetIntArrayRegion(env, dataArray,0,10,data);
//	(*env)->CallVoidMethod(env, dataObject, NativeDataDecoder_decodeNativeData,   dataArray);	// JUST TO TRY

//	jclass     dataClass     = (*env)->GetObjectClass(env, dataObject);

//	(*env)->CallIntMethod(env,dataObject,dimOIlengthMID);
//	(*env)->CallNonvirtualIntMethod(env,dataObject, dataItemInterface, dimOIlengthMID);//	formatString = (jstring) (*env)->CallNonvirtualObjectMethod(env, dataObject, dataItemInterface, getDimFormatMID);

	// Print the current thread ID in the Debug Window
	// TRACE("Current Thread ID = 0x%X\n", AfxGetThread()->m_nThreadID);
            _RPT1(_CRT_ERROR, "Invalid allocation size: %u bytes.\n", nSize);


//	formatString = (jstring) (*env)->CallObjectMethod(env,dataObject,getDimFormatMID);
//	format = (*env)->GetStringUTFChars(env, formatString, 0);
//	printf("Format string in native: %s\n",format);
//	(*env)->ReleaseStringUTFChars(env, formatString, format);

  /* server_getInfo_callback is invoked when dim needs the data for a service.
   * The data is obtained by calling the encodeData method of the DataEncoder
   * interface which returns us a Memory object. Next we extract the data address
   * and size from the returned Memory object.
   *
   * Alternative:
   * I call the following class method: Server.sendMeTheData(theDataEncoder)
   * This method is implemented as
   * sendMeTheData(DataEncoder theDataEncoder)
   * {
   *   Memory theData = theDataEncoder.encodeData();
   *   Server.setDataReference(Memory.dataAddress, Memory.DataSize);
   * }
   * The method setDataReference(int dataAddress, int dataSize) is implemented
   * as a native method and receives directly the data which one can store in
   * a global variable.
   * More complicated, not convinced that it would be more efficient.
   */



#endif
