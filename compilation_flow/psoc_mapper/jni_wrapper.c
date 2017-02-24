#include <stdio.h>
#include <stdlib.h>

#include "com_upscale_PsocMapper.h"

#define _mylog(_s, ...)                           \
{                                              \
	fprintf(stderr, "[PsocMapper] [%s] " _s, __func__, ##__VA_ARGS__); \
	fflush(stderr); \
}

enum RESULT {
	OK = 0,
	ERROR_ARG_MISSING_INPUT_FILES = 1,
	// ERROR_ARG_MISSING_RUNTIME_DIR = 2,
	ERROR_ARG_MISSING_TABLE_NAME = 3,
	ERROR_GENERIC_PSOC_MAPPER = 4
};

// FIXME this is *horrible* but I'd need to modify psoc_mapper..
int main(int argc, char *argv[]);

JNIEXPORT jint JNICALL Java_com_upscale_PsocMapper_GenerateTaskTable
  (JNIEnv * env, jclass clazz, jobjectArray jDotFileNames, jstring jRuntimeDir, jstring jTableFileName) {
	 
	if((*env)->IsSameObject(env, jDotFileNames, NULL) ) {
		_mylog("No input DOT files!\n"); 
		return (jint) ERROR_ARG_MISSING_INPUT_FILES;
	}
	
	// if((*env)->IsSameObject(env, jRuntimeDir, NULL) ) {
		// _mylog("No runtime directory specified!\n"); 
		// return (jint) ERROR_ARG_MISSING_RUNTIME_DIR;
	// }
	
	if((*env)->IsSameObject(env, jTableFileName, NULL) ) {
		_mylog("No table file name specified!\n"); 
		return (jint) ERROR_ARG_MISSING_TABLE_NAME;
	}
	
	
	/* Parse input file names */
	
	int stringCount = (*env)->GetArrayLength(env, jDotFileNames);
	
	char **fileNames = (char **) malloc((stringCount + 1) * sizeof(char **));
	int i;
    for (i=0; i<stringCount; i++) {
        jstring string = (jstring) ((*env)->GetObjectArrayElement(env, jDotFileNames, i));
        const char *rawString = (*env)->GetStringUTFChars(env, string, 0);
        // Don't forget to call `ReleaseStringUTFChars` when you're done.
		fileNames[i] = (char *) rawString;
    }
	
	/* Parse output file name */
	const char *tableFileName = (*env)->GetStringUTFChars(env, jTableFileName, 0);
	// _mylog("Output table filename is '%s'!\n", tableFileName);
	
	/* Prepare input flags */
	int argc = 0;
	char * argv[100];
	argv[argc++] = "psoc_mapper";
	
	argv[argc++] = "-o";
	argv[argc++] = (char *) tableFileName;
	
    for (i=0; i<stringCount; i++)
		argv[argc++] = fileNames[i];
	
	_mylog("argc: %d\n", argc);
    for (i=0; i<argc; i++)
		_mylog("- argv[%d] is '%s'!\n", i, argv[i]);
		
	_mylog("Invoking psoc_mapper...\n");
	int retval = main(argc, argv);
	_mylog("psoc_mapper returned %d...\n", retval);
	
	/* Release everything */
	
	(*env)->ReleaseStringUTFChars(env, jTableFileName, tableFileName);
	
	for (i=0; i<stringCount; i++) {
        jstring string = (jstring) ((*env)->GetObjectArrayElement(env, jDotFileNames, i));
		(*env)->ReleaseStringUTFChars(env, string, fileNames[i]); 
	}
	if(fileNames) free(fileNames);
	
	return (jint) retval ? ERROR_GENERIC_PSOC_MAPPER : OK;
}