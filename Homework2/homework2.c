/******************************************************************************************
*	@file		homework2.c
*	@brief		This programs perform basic file-io functions like crerating a
*			file, writing to it, appending it and reading it. Also this
*			program modifies the file permissions for the file being crea-
*			ted. A character and string are extracted from the file in the
*			later section of the code and printed on the console. The pri-
*			purpose of this file is to serve as a testting module for ltrace,
*			strace and perf performed both on native linux and the Beaglebon-
			e Green.
*	@Tools_Used	GCC  
*	@Author		Souvik De
*	@Date		02/10/2019
*
*******************************************************************************************/

#include<stdio.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<string.h>

#define F_NAME 		("homework2.txt")	//File Path
#define READER_LENGTH	(10)			//Size of Array used to read string from the File

int main(void)
{
    	char *string;
    	char c;
    	char rstring[READER_LENGTH];

	/* A fun welcome message */
    	printf("\nI want to write an IPV4 joke, but good ones are already exhausted :-(");

	/* Create a file and open it in write mode */
    	FILE* fptr = fopen(F_NAME, "w");
	/* If file operation is successful */
	if(fptr != NULL)
	{
		/* Modify file permissions */
    		chmod(F_NAME, 00400 | 00200);
    		fputc('A', fptr);	//Write a character to the file
    		fclose(fptr);		//Close file

		/* Attempt openig file in append mode */
		fptr = fopen(F_NAME, "a");
		/* If file operation is successful */
		if(fptr != NULL)
		{
	    		string = malloc(100 * sizeof(char));	//Allocate a string dynamically
	    		printf("\nEnter a String: ");
	    		scanf("%s",string);
	    		fputs(string, fptr);			//Append user inputted string to the file
	    		fflush(fptr);				//Flush file output
	    		fclose(fptr);				//Close file

			/* Attempt opening file in read mode */
	    		fptr = fopen(F_NAME, "r");
			/* If file operation is successful */
			if(fptr != NULL)
			{  
	    			c = getc(fptr);			//Read the first character from the file
	    			printf("Character: %c",c);
	    			printf("\n");
	    			fgets(rstring,7,fptr);		//Read a string from the file
	    			printf("String: %s",rstring);       
	    			printf("\n");   
	    			fclose(fptr);			//Close file
			}
			else
			{
				printf("\nFailed to open file in R Mode!");
			}
	    		free(string);		//Free allocated space
		}
		else
		{
			printf("\nFailed to open file in A Mode!");
		}
	}
	else
	{
		printf("\nFailed to open file in W mode!");
	}

    	return 0;
}
