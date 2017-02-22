#include "hipert_hg_modelToCode_PsocMapper.h"
#include <stdio.h>

JNIEXPORT void JNICALL Java_hipert_hg_modelToCode_PsocMapper_GenerateTaskTable
  (JNIEnv * env, jclass obj, jstring javaString)
{
	const char *filename = (*env)->GetStringUTFChars(env, javaString, 0);

   // use your string

	fprintf(stderr, "psoc_mapper. Filename is %s!\n", filename);
	fflush(stderr);
	
	FILE *fout = NULL;
	if ((fout = fopen(filename, "w")) == NULL) {
			perror("psoc_mapper: Opening output file");
			return;
		}
	
	fprintf(fout, "// No schedulability analysis neither mapping were performed. Use -s|-d switch to enable them\n");	
		
	if(fout) fclose(fout);
	
	(*env)->ReleaseStringUTFChars(env, javaString, filename);
}