#include "HP.h"
//======================================================================================================================================================//
void print_record(Record record)
{
	printf("id: %d \n", record.id);
	printf("name: %s \n",record.name);
	printf("surname: %s \n", record.surname);
	printf("status: %c \n", record.status[0]);
	printf("birthday: %s \n", record.dateOfBirth);
	printf("salary: %d \n", record.salary);
	printf("section: %c \n", record.section[0]);
	printf("daysoff: %d \n", record.daysOff);
	printf("prevyears: %d \n", record.prevYears);
}

//====================================================================================================================================================//

int HP_CreateFile(char* filename)
{
	int fileDesc, data = 1;
	void *next = NULL;
	void *block;
	BF_Init();		
	if (BF_CreateFile(filename) < 0)		/*dimiourgia arxeiou*/
	{
		BF_PrintError("Error creating the file");
	    return -1;
	}
	fileDesc = BF_OpenFile(filename);		/*anoigma tou arxeiou*/
	if (fileDesc < 0)						/*se periptwsi apotixias*/
	{
		BF_PrintError("Error opening file");
		return -1;
	}
	if (BF_AllocateBlock(fileDesc) < 0)		/*desmeusi block sto arxeio*/
	{
		BF_PrintError("Error allocating a block");
		return -1;
	}
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
	{
		BF_PrintError("Error reading first block of file");
		return -1;
	}
	memmove(block,&data, 4);				/*anagnwristiko arxeiou*/
	memmove(block + 4, &next, 4);			/*deiktis se epomeno block*/
	data = -1;
	memmove(block + 8, &data, 4);			/*plithos eggrafwn*/
	if (BF_WriteBlock(fileDesc, 0) < 0)
	{
		BF_PrintError("Error writing first block of file");
		return -1;
	}
	if (BF_CloseFile(fileDesc) < 0)
	{
		BF_PrintError("Error closing file");
		return -1;
	}
	return 0;
}

/*====================================================================================================================================================*/

int HP_OpenFile(char* filename)
{
	int Desc,heapnum; 	/*Desc=anagnwristiko anoigmatos arxeiou,heapnum=arithmos mesa sto prwto block (an einai 1 shmainei oti einai typou heapfile)*/
	void* block; 		/*anfora sta stoixeia toy prwtou block*/
	int heapinfo;		/*ta stoixeia tou prwtou block se morfh char*/
	Desc=BF_OpenFile(filename);
	if	(Desc < 0)
	{
		BF_PrintError("Error opening file");
		return -1;			
	}
	if (BF_ReadBlock(Desc, 0, &block) < 0)
	{
		BF_PrintError("Error in readind  first block of file");
		return -1;
	}
	memcpy(&heapinfo, block, 4);
	heapnum=heapinfo;
	if (heapnum!=1)
	{
		 BF_PrintError("Error not a Heap file");
		 return -1;
	}
	return Desc;
}

/*===================================================================================================================================================*/

int HP_CloseFile(int fileDesc)
{
	if(BF_CloseFile(fileDesc)<0)
	{
		BF_PrintError("Error closing file");
        	return -1;
	}
	return 0;
}

/*==================================================================================================================================================*/

int HP_InsertEntry(int fileDesc,Record record)
{
	void* block,*block_prin,*block_meta;		/*dektes gia ta stoixeia tou twrinou block tou epomenou kai tou prohgoumenou*/
	void *next=NULL;							/*deiktis gia to epomeno block */
	int reccount;								/*stoixeio gia tis diathesimes eggrafes sto block*/
	int full_block=0,zero=0;
	if (BF_GetBlockCounter(fileDesc)==1)		/*an exoume mono ena block desmeuoumai allo ena*/
	{
		if (BF_AllocateBlock(fileDesc) < 0)
		{
			BF_PrintError("Error allocating a block");
			return -1;
		}   
		if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1),&block)<0)
		{
			 BF_PrintError("Error reading a block ");
			 return -1;
		}
		memmove(block,&next, 4);   				/*pernontas mesa tis kattalhles plhrofories gia to block alla kai gia tis eggrafes*/
		memmove(block+4,&zero, 4);
		if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 2),&block_prin)<0)
		{
			 BF_PrintError("Error reading a block previous");
			 return -1;
		}
		memcpy(block_prin+4,&block,4);			/*stin dieythinsh_epomenou_block  tou prohgoumenou apo ton teleutaio bale ton teleutaio*/
		if (BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 2)) < 0)
		{
			BF_PrintError("Error writing a block previous");
			return -1;
		}
		if (BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1)) < 0)
		{
			BF_PrintError("Error writing a block");
			return -1;
		}
	}
	if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1),&block)<0)
	{
		 BF_PrintError("Error reading a block ");
		 return -1;
	}
	memcpy(&reccount,block+4,4); 				/*pare thn plhroforia reccount apo to teleutaio block*/
	if (BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1)) < 0)
	{
		BF_PrintError("Error writing a block");
		return -1;
	}
	if (reccount > 6) 							/*an oles oi eggrafes full sto teleutaio block*/
	{
		if (BF_AllocateBlock(fileDesc) < 0)		/*desmeuse neo block*/
		{
			BF_PrintError("Error allocating a block");
			return -1;
		}   
		if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1),&block_meta)<0)
		{
			 BF_PrintError("Error reading a block ");
			 return -1;
		}
		if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 2),&block)<0)
		{
			 BF_PrintError("Error reading a block 1");
			 return -1;
		}
		memmove(block_meta,&next, 4);			/*kai kane tis katlhles alleges san prin*/
		memmove(block_meta+4,&zero, 4);
		memcpy(block,&block_meta,4);
		if (BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 2)) < 0)
		{
			BF_PrintError("Error writing a block");
			return -1;
		}
		if (BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1)) < 0)
		{
			BF_PrintError("Error writing a block");
			return -1;
		}
		reccount=0;
	}
	if(BF_ReadBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1),&block)<0)
	{
		 BF_PrintError("Error reading a block ");
		 return -1;
	}
	memcpy(((block+8)+(reccount)*sizeof(Record)),&record,sizeof(Record));	/*copy the record at the end of the file*/
	reccount++;
	memcpy(block + 4,&(reccount),4);      		/*anebase kata ena thn plhroforia reccount apo to teleutaio block*/
	if(BF_WriteBlock(fileDesc,(BF_GetBlockCounter(fileDesc) - 1))<0)
	{
		 BF_PrintError("Error writing a block");
         return -1;
	}
	return 0;
}

/*===================================================================================================================================================*/

int compare(void* block, char* fieldName, void* value)
{
	unsigned int bytes_to_compare;
	void* info;
	Record temp;
	memcpy(&temp, block, sizeof(Record));				/*apothikeusi tis eggrafis sto temp apo to block*/
	/*Stis parakatw periptwsis elenxetai to onoma tou pediou kai:
		1.desmeuetai o katallilos xwros gia to info
		2.antigrafetai se auton to antistoixo pedio
		3.filassontai ta bytes ws pros ta opoia tha ginei i sigrisi*/
	if (strcmp(fieldName, "id") == 0)
	{
		info = malloc(sizeof(temp.id));
		memcpy(info, &temp.id, sizeof(temp.id));
		bytes_to_compare = sizeof(temp.id);
	}
	else if (strcmp(fieldName, "name") == 0)
	{
		info = malloc(sizeof(temp.name));
		memcpy(info, &temp.name, sizeof(temp.name));
		bytes_to_compare = strlen(info) + 1;
	}
	else if (strcmp(fieldName, "surname") == 0)
	{
		info = malloc(sizeof(temp.surname));
		memcpy(info, &temp.surname, sizeof(temp.surname));
		bytes_to_compare = strlen(info) + 1;
	}
	else if (strcmp(fieldName, "status") == 0)
	{
		info = malloc(sizeof(temp.status));
		memcpy(info, &temp.status, sizeof(temp.status));
		bytes_to_compare = sizeof(temp.status);
	}
	else if (strcmp(fieldName, "dateOfBirth") == 0)
	{
		info = malloc(sizeof(temp.dateOfBirth));
		memcpy(info, &temp.dateOfBirth, sizeof(temp.dateOfBirth));
		bytes_to_compare = strlen(info) + 1;
	}
	else if (strcmp(fieldName, "salary") == 0)
	{
		info = malloc(sizeof(temp.salary));
		memcpy(info, &temp.salary, sizeof(temp.salary));
		bytes_to_compare = sizeof(temp.salary);
	}
	else if (strcmp(fieldName, "section") == 0)
	{
		info = malloc(sizeof(temp.section));
		memcpy(info, &temp.section, sizeof(temp.section));
		bytes_to_compare = sizeof(temp.status);
	}
	else if (strcmp(fieldName, "daysOff") == 0)
	{
		info = malloc(sizeof(temp.daysOff));
		memcpy(info, &temp.daysOff, sizeof(temp.daysOff));
		bytes_to_compare = sizeof(temp.daysOff);
	}
	else if (strcmp(fieldName, "prevYears") == 0)
	{
		info = malloc(sizeof(temp.prevYears));
		memcpy(info, &temp.prevYears, sizeof(temp.prevYears));
		bytes_to_compare = sizeof(temp.prevYears);
	}
	else											/*an den einai tipota apo ta parapanw, error*/
		return -1;
	if (memcmp(info, value, bytes_to_compare) == 0)	/*se periptwsi isotitas sto pedio kleidi*/
	{
		print_record(temp);							/*ektipwse tin eggrafi*/
		free(info);									/*apodesmeusi mnimis*/
		return 1;									/*epistrefw 1 an eixa isotita me to kleidi*/
	}
	free(info);										/*apodesmeusi mnimis*/
	return 0;										/*epistrefw 0 an den eixa isotita me to kleidi*/
}

/*=================================================================================================================================================*/

void HP_GetAllEntries(int fileDesc, char* fieldName, void* value)
{
	unsigned int block_counter = 1, record_counter = 0;
	int reccount, i, j, flag, found = 0;
	void *block;
	Record temp;
	if (BF_GetBlockCounter(fileDesc) == 1)			//an iparxei ena block sto arxeio, den mporw na diavasw
	{
		printf("In fuction GetAllEntries: No available blocks to search\n");
		return;
	}
	else if (fieldName == NULL && value == NULL)	//periptwsi pou ektipwnw oles tis eggrafes
	{
		for (i = 1; i <= BF_GetBlockCounter(fileDesc) - 1; i++)
		{											//perasma apo ola ta blocks
			if(BF_ReadBlock(fileDesc, i, &block) < 0)
			{										//diavasma tou kathe block
				BF_PrintError("Error reading a block");
				return;
			}
			block_counter++;						//an to block diavastike, auksisi toy metriti
			memcpy(&reccount, block + 4, 4);		//pairnw to plithos twn eggrafwn toy block
			for (j = 0; j < reccount; j++)
			{										//gia kathe eggrafi sto block
				memcpy(&temp, block + 8 + j*sizeof(Record), sizeof(Record));
				print_record(temp);					//ektipwse tin eggrafi
				record_counter++;
			}
		}
	}
	else
	{
		for (i = 1; i <= BF_GetBlockCounter(fileDesc) - 1; i++)
		{											/*perasma apo ola ta blocks*/
			if(BF_ReadBlock(fileDesc, i, &block) < 0)
			{										/*diavasma toy kathe block*/
				BF_PrintError("Error reading a block");
				return;
			}
			block_counter++;						/*an to block diavastike, auksisi toy metriti*/
			memcpy(&reccount, block + 4, 4);		/*pairnw to plithos twn eggrafwn toy block*/
			for (j = 0; j < reccount; j++)
			{										/*gia kathe eggrafi sto block*/
				flag = compare(block + 8 + j*sizeof(Record), fieldName, value);
				if (flag == -1)						/*an dothei lathos fieldName*/
				{									/*ektipwnetai minima lathous*/
					printf("In fuction GetAllEntries: Wrong values given\n");
					return;							/*kai termatizetai i sinartisi*/
				}
				else if (flag == 1)					/*an eixa isotita me to kleidi*/
				{
					found = 1;						/*exw estw kai ena apotelesma stin anazitisi*/
					record_counter++;
				}
			}
		}
		if (found == 0)								/*an den vrika kammia eggrafi stin anazitisi*/
			printf("Den vrethike kamia eggrafi sto arxeio me to kleidi pou dwsate\n");
	}
	printf("%d blocks read.\n", block_counter);
	printf("%d records printed.\n", record_counter);
}
