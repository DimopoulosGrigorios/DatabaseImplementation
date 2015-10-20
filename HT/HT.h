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

typedef struct 
{ 
	int fileDesc; 
	char attrType;                                                                               
	char* attrName;                 
	int attrLength;      
	int depth; 
} HT_info;

int HT_CreateIndex( char *fileName,char attrType,char* attrName,int attrLength,int depth);

HT_info* HT_OpenIndex( char *fileName );

int HT_CloseIndex( HT_info* header_info);

int HT_Function(int depth, int akeraio_pedio, char* string_pedio, int length);

int HT_InsertEntry ( HT_info* header_info,Record record);

int compare(void* block, char* fieldName, void* value, int use);

void HT_GetAllEntries(HT_info* header_info, void* value);

void print_record(Record record);

int Mod_Class(int gen_depth,HT_info* header_info,Record record);

int AllSame(void * block_ptr,char * fieldName,void * value);

void * Rec_Key(char * fieldName,Record *record);

int Chane_Count(int block_num,int * counter,int fileDesc);

void Chane_Rec(void * block,void** next,int i,int fileDesc,Record * record);

int Last_Chane(int fileDesc,int chane);
