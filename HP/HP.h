#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BF.h"

typedef struct
{ 
	int id; 
	char name[15]; 
	char surname[20];
	char status[1];
	char dateOfBirth[10];
	int salary;
	char section[1];
	int daysOff;
	int prevYears;
} Record;

void print_record(Record record);

int HP_CreateFile( char *filename);

int HP_OpenFile( char *filename );

int HP_CloseFile( int fileDesc);

int HP_InsertEntry ( int fileDesc,Record record);

int compare(void* block, char* fieldName, void* value);

void HP_GetAllEntries(int fileDesc, char* fieldName, void* value);
