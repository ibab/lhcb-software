// CreShrt1.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"





#include <windows.h>
//#include <atlbase.h>
#include <atlcomcli.h>
#include <objbase.h>
#include <shlguid.h>
#include <shobjidl.h>
/**********************************************************************
* Function......: CreateShortcut
* Parameters....: lpszFileName - string that specifies a valid file name
*          lpszDesc - string that specifies a description for a 
                             shortcut
*          lpszShortcutPath - string that specifies a path and 
                                     file name of a shortcut
* Returns.......: S_OK on success, error code on failure
* Description...: Creates a Shell link object (shortcut)
**********************************************************************/
HRESULT CreateShortcut(/*in*/ LPCTSTR lpszFileName, 
                    /*in*/ LPCTSTR lpszDesc, 
                    /*in*/ LPCTSTR lpszShortcutPath,
										/*in*/ LPCTSTR lpszTargetCommandLineArguments,
                    /*in*/ LPCTSTR icon)
{
    HRESULT hRes = E_FAIL;
    DWORD dwRet = 0;
    CComPtr<IShellLink> ipShellLink;
        // buffer that receives the null-terminated string 
        // for the drive and path
    TCHAR szPath[MAX_PATH];    
        // buffer that receives the address of the final 
        //file name component in the path
    LPTSTR lpszFilePart;    
    WCHAR wszTemp[MAX_PATH];
        
    // Retrieve the full path and file name of a specified file
    dwRet = GetFullPathName(lpszFileName, 
                       sizeof(szPath) / sizeof(TCHAR), 
                       szPath, &lpszFilePart);
    if (!dwRet)                                        
        return hRes;

    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance(CLSID_ShellLink,
                            NULL, 
                            CLSCTX_INPROC_SERVER,
                            IID_IShellLink,
                            (void**)&ipShellLink);

    if (SUCCEEDED(hRes))
    {
        // Get a pointer to the IPersistFile interface
        CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

        // Set the path to the shortcut target and add the description
        hRes = ipShellLink->SetPath(szPath);
        if (FAILED(hRes))
            return hRes;

        hRes = ipShellLink->SetDescription(lpszDesc);
        if (FAILED(hRes))
            return hRes;
				hRes	= ipShellLink->SetArguments(lpszTargetCommandLineArguments);
        if (FAILED(hRes))
            return hRes;
        // IPersistFile is using LPCOLESTR, so make sure 
                // that the string is Unicode
        if (strlen(icon) != 0)
        {
          hRes = ipShellLink->SetIconLocation(icon,0);
          if (FAILED(hRes))
            return hRes;
        }

#if !defined _UNICODE
        MultiByteToWideChar(CP_ACP, 0, 
                       lpszShortcutPath, -1, wszTemp, MAX_PATH);
#else
        wcsncpy(wszTemp, lpszShortcutPath, MAX_PATH);
#endif

        // Write the shortcut to disk
        hRes = ipPersistFile->Save(wszTemp, TRUE);
    }

    return hRes;
}

/*********************************************************************
* Function......: ResolveShortcut
* Parameters....: lpszShortcutPath - string that specifies a path 
                                     and file name of a shortcut
*          lpszFilePath - string that will contain a file name
* Returns.......: S_OK on success, error code on failure
* Description...: Resolves a Shell link object (shortcut)
*********************************************************************/
HRESULT ResolveShortcut(/*in*/ LPCTSTR lpszShortcutPath,
                        /*out*/ LPTSTR lpszFilePath)
{
    HRESULT hRes = E_FAIL;
    CComPtr<IShellLink> ipShellLink;
        // buffer that receives the null-terminated string 
        // for the drive and path
    TCHAR szPath[MAX_PATH];     
        // buffer that receives the null-terminated 
        // string for the description
    TCHAR szDesc[MAX_PATH]; 
        // structure that receives the information about the shortcut
    WIN32_FIND_DATA wfd;    
    WCHAR wszTemp[MAX_PATH];

    lpszFilePath[0] = '\0';

    // Get a pointer to the IShellLink interface
    hRes = CoCreateInstance(CLSID_ShellLink,
                            NULL, 
                            CLSCTX_INPROC_SERVER,
                            IID_IShellLink,
                            (void**)&ipShellLink); 

    if (SUCCEEDED(hRes)) 
    { 
        // Get a pointer to the IPersistFile interface
        CComQIPtr<IPersistFile> ipPersistFile(ipShellLink);

        // IPersistFile is using LPCOLESTR, 
                // so make sure that the string is Unicode
#if !defined _UNICODE
        MultiByteToWideChar(CP_ACP, 0, lpszShortcutPath,
                                       -1, wszTemp, MAX_PATH);
#else
        wcsncpy(wszTemp, lpszShortcutPath, MAX_PATH);
#endif

        // Open the shortcut file and initialize it from its contents
        hRes = ipPersistFile->Load(wszTemp, STGM_READ); 
        if (SUCCEEDED(hRes)) 
        {
            // Try to find the target of a shortcut, 
                        // even if it has been moved or renamed
            hRes = ipShellLink->Resolve(NULL, SLR_UPDATE); 
            if (SUCCEEDED(hRes)) 
            {
                // Get the path to the shortcut target
                hRes = ipShellLink->GetPath(szPath, 
                                     MAX_PATH, &wfd, SLGP_RAWPATH); 
                if (FAILED(hRes))
                    return hRes;

                // Get the description of the target
                hRes = ipShellLink->GetDescription(szDesc,
                                             MAX_PATH); 
                if (FAILED(hRes))
                    return hRes;

                lstrcpyn(lpszFilePath, szPath, MAX_PATH); 
            } 
        } 
    } 

    return hRes;
}
#include <stdio.h>
int main(int argc, _TCHAR* argv[])
// argv[1] = target file name
// argv[2] = the shortcut file name (full path)
// argv[3] = (optional) command line arguments for target
// argv[4] = (optional) Shortcut description
// argv[5] = (optional) path to file containing icon for the shortcut. Index 0 is used...
{
	int status;
	void *p = 0;
		if ((argc <=2) || (argc>6))
		{
			printf("Too many or missing Arguments\n\tUsage: CreShrt <targetFileName> <Shortcut_full_Path_with_filename> (optional)<CommandLineArguments> (optional)<Description Text> (optional) <Path to Icon file> \n\tExample: CreShrt \"C:\\WINDOWS\\explorer.exe\" \"C:\\tst short cut for explorer.lnk\" \"C:\" \"Description of Shortcut\" \"C:\\WINDOWS\\Coffee Bean.bmp\" \n" );
			status = 0;
      return (status);
		}

	CoInitialize(p);
	if (argc == 6 )
	{
    status = CreateShortcut(argv[1], argv[4], argv[2], argv[3], argv[5]);
	}
	else if (argc == 5)
	{
    status = CreateShortcut(argv[1], argv[4], argv[2], argv[3], "");
	}
	else if (argc == 4)
	{
    status = CreateShortcut(argv[1], " ", argv[2], argv[3], "");
	}
	else if (argc == 3)
	{
    status = CreateShortcut(argv[1], " ", argv[2], " ", "");
	}
		return(status);
}
