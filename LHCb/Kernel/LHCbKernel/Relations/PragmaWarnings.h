// $Id: PragmaWarnings.h,v 1.1 2002-04-25 08:44:03 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef RELATIONS_PRAGMAWARNINGS_H 
#define RELATIONS_PRAGMAWARNINGS_H 1
// Include files
/** @file PragmaWarnings.h Relations/PragmaWarnings.h
 *  
 *  @warning the content of the file is Platform-Dependent! 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   25/04/2002
 */
#ifdef WIN32
/** disable warning C4786: 
 *  identifier was truncated to '255' characters in the debug information
 */
#pragma warning ( disable : 4786 )
#endif 

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_PRAGMAWARNINGS_H
// ============================================================================
