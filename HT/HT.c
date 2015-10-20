 # include "HT.h"
//======================================================================================================================================//

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

//====================================================================================================================================//

int HT_CreateIndex(char* fileName, char attrType, char* attrName, int attrLength, int depth)
{
	int fileDesc, i, kadoi = 1, temp;
	void* block;
	void* bucket;
	HT_info hash_info;
	hash_info.attrName = malloc(18 * sizeof(char));
	hash_info.fileDesc = -1;
	hash_info.attrType = attrType;
	strcpy(hash_info.attrName, attrName);
	hash_info.attrLength = attrLength;
	hash_info.depth = depth;
	for (i = 1; i <= depth; i++)
		kadoi = 2 * kadoi;							//kadoi = 2^depth//
	BF_Init();		
	if (BF_CreateFile(fileName) < 0)				//dimiourgia arxeiou//				
	{
		BF_PrintError("Error creating the file");	//se periptwsi apotixias//
	    return -1;
	}
	fileDesc = BF_OpenFile(fileName);				//anoigma tou arxeiou//
	if (fileDesc < 0)								//se periptwsi apotixias//
	{
		BF_PrintError("Error opening file");
		return -1;
	}
	for (i = 1; i <= 10; i++)
	{												//desmeusi twn 10 prwtwn block//
		if (BF_AllocateBlock(fileDesc) < 0)
		{
			BF_PrintError("Error allocating a block");
			return -1;
		}
	}
	if (BF_ReadBlock(fileDesc, 0, &block) < 0)
	{
		BF_PrintError("Error reading first block of file");
		return -1;
	}
	i = 0;											//0 gia hash//
	memcpy(block, &i, sizeof(int));					//pernaw to 0//
	memcpy(block + 4, &hash_info, sizeof(HT_info));	//kai to hash_info sto block//
	if (BF_WriteBlock(fileDesc, 0) < 0)
	{
		BF_PrintError("Error writing a block");
		return -1;
	}
	if (BF_ReadBlock(fileDesc, 1, &block) < 0)
	{												//diavazw to euretirio//
		BF_PrintError("Error reading a block");
		return -1;
	}
	for (i = 0; i <= kadoi - 1; i++)
	{
		memcpy(block + 8*i, &i, sizeof(int));		//pernaw ton arithmo gia to euretirio//
		temp= i + 2;
		memcpy(block + 4 + 8*i, &temp, 4);			//pernaw ton arithmo tou block pou deixnei//
		if (BF_ReadBlock(fileDesc, i + 2, &bucket) < 0)
		{											//diavazw tous kadous//
			BF_PrintError("Error reading a block");
			return -1;
		}
		memcpy(bucket, &depth, 4);				//pernaw to topiko vathos pou einai iso me to oliko//
		temp = 0;
		memcpy(bucket + 4, &temp, 4);			//pernaw to plithos twn eggrafwn pou einai arxika 0//
		memcpy(bucket + 504, &temp, 4);			//pernaw mia endeiksi gia tin ektipwsi tou bucket//
		temp=-1;								
		memcpy(bucket + 508, &temp, 4);			//pernaw ton deikti NULL sto telos tou bucket//
		if(BF_WriteBlock(fileDesc, i + 2) < 0)
		{
			BF_PrintError("Error writing a block");	
			return -1;
		}
	}
	if(BF_WriteBlock(fileDesc, 1) < 0)
	{
		BF_PrintError("Error writing a block");
		return -1;
	}
	if (BF_CloseFile(fileDesc) < 0)
	{
		BF_PrintError("Error closing file");
		return -1;
	}
	return 0;
}

//====================================================================================================================================//

HT_info* HT_OpenIndex( char *fileName )
{
	int fileDesc;        //Desc=anagnwristiko anoigmatos arxeiou,hashinfo=arithmos mesa sto prwto block (an einai 0 shmainei oti einai typou hashfile)//
    void* block;            //anfora sta stoixeia toy prwtou block//
    int hashinfo;   //to prwto stoixeio tou prwtou block se morfh  int//
	HT_info *info;
	info=malloc(sizeof(HT_info));
	fileDesc=BF_OpenFile(fileName);
	if      (fileDesc<0)
	{
		BF_PrintError("Error opening file");
		return NULL;
	}
	if (BF_ReadBlock(fileDesc,0, &block)<0)
	{
		BF_PrintError("Error in readind  first block of file");
		return NULL;
	}
	memcpy( &hashinfo,block,4 );
	if (hashinfo!=0)
	{
		BF_PrintError("Error not a Hash file");
		return NULL;
	}
	memcpy(info,block+4,sizeof(HT_info));
	info->fileDesc=fileDesc;
	memcpy(block+4,info,sizeof(HT_info));
	if(BF_WriteBlock(fileDesc,0)<0)
	{
		BF_PrintError("Error in writing  a block");
                return NULL;
	}
	return info;
}

//=====================================================================================================================================//

int HT_CloseIndex( HT_info* header_info)
{
	int fileDesc;
	fileDesc=header_info->fileDesc;
	if(BF_CloseFile(fileDesc)<0)
	{
		BF_PrintError("Error closing file");
		return -1;
	}
	free(header_info->attrName);
	free(header_info);
	return 0;
}

//======================================================================================================================================//

int HT_InsertEntry ( HT_info* header_info,Record record)
{
	int t,counter2=0,counter3,fileDesc,i,two=1,k,ar_e,block_num,reccount,poin,chane2,tel_ar;
	int gen_depth,mod_class,topic_depth,chane,ngtv=-1,zero=0,c_ptr,counter,dif,metr,ar_egr;
	void* block,* block_ptr,*kouvas,*kouvas2,*first,*hashbin,*hashbin2,*next,*block7;
	Record *record2,*record4;
	Record record3;
	//----------------------------------//
	fileDesc=header_info->fileDesc;			//pernw to fileDesc apo to struct me ta info//
	gen_depth=header_info->depth;			//pernw to oliko bathos apo to struct me ta info
	//---------------------------------//
	mod_class=Mod_Class(gen_depth,header_info,record);	//efarmozw thn synarthsh katakermatismou me periptosiologia typou//
	if (mod_class==-1)
	{
		printf("Error \n ");
                return -1;	
	}
	//-----------------------------------//
	if(BF_ReadBlock(fileDesc,1,&block)<0)			
        {
        	BF_PrintError("Error reading a block \n");
                return -1;
        }
       	memcpy(&ar_e,block+8*mod_class,4);			
        if (ar_e!=mod_class){ printf("Error \n"); return -1;}		//tsekarw gia pithanw error sto shmeio auto//
        memcpy(&block_num,block+8*mod_class+4,4);			//briskw apo to eurethrio ton arithmo tou block pou antistixei se auto to mod_class//
        if(BF_ReadBlock(fileDesc,block_num,&block_ptr)<0)
        {
        	BF_PrintError("Error reading a block ");
        	return -1;
        }
        memcpy(&reccount,block_ptr+4,4);				//pernw to plithos twn eggrafwn//
	//--------------------------------//
        if (reccount>6)							//an exei 7 eggrafes  //
        {
	
		//------------------------------------------------------------------------------------------------//
		memcpy(&topic_depth,block_ptr,4);       //pare to topiko bathos//
		if (AllSame(block_ptr,header_info->attrName,Rec_Key((header_info->attrName),&record))||((gen_depth==topic_depth)&&(gen_depth==6)))
		{
			memcpy(&chane,block_ptr+508,4);						//des an yparxei alysida sto block sou//
			if (chane==-1)								//an oxi//
			{					
				if (BF_AllocateBlock(fileDesc)<0)				//desmeusai mia //
				{
					BF_PrintError("Error allocating a block ");
                			return -1;
				}
				if (BF_ReadBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1, &kouvas) < 0)
                                {                                                                              
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }
                                memcpy(kouvas, &ngtv, 4);                             //kane tis katallhles arxikopoihseis sta block//
                                memcpy(kouvas + 4, &zero, 4);               		//sta paidia tou block pou den tha yparxei tpt yparxei to ngtv=-1//
                                memcpy(kouvas + 504, &zero, 4);                                        
                                memcpy(kouvas + 508, &ngtv, 4);                 
                                if(BF_WriteBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1) < 0)
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }
				chane=BF_GetBlockCounter(fileDesc)-1;
				memcpy(block_ptr+508,&chane,4);		//bale ton arithmo tou block san deikth sto telos tou bucket sou//
				if(BF_WriteBlock(fileDesc,block_num) < 0)
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }

			}

			//---------------------------------------------------------------------//
			chane=Last_Chane(fileDesc,chane);		//pare to teleutaio diathesimo block alysidas//
			if (BF_ReadBlock(fileDesc,chane, &kouvas) < 0)
                        {
                        	BF_PrintError("Error reading a block");
                               	return -1;
                       	}
			memcpy(&ar_egr,kouvas+4,4);	//diabase thn alysida kai des ton arithmo ton eggrafwn ths//

			//------------------------------------------------------------------------//

			if (ar_egr>6)			//an einai megalytero apo 6//
			{
				if (BF_AllocateBlock(fileDesc)<0)	//epekteine thn aludida dhmiourgwntas ena akoma block//
                                {
                                        BF_PrintError("Error allocating a block ");
                                        return -1;
                                }
                                if (BF_ReadBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1, &kouvas2) < 0)
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }						//kai kanwntas tis kattalhlles allages opws kai parapanw//
				tel_ar=BF_GetBlockCounter(fileDesc)-1;
				chane2=chane;
				chane=tel_ar;
				memcpy(kouvas+508,&tel_ar,4);
                                memcpy(kouvas2, &ngtv, 4);
                                memcpy(kouvas2 + 4, &zero, 4);
                                memcpy(kouvas2 + 504, &zero, 4);
                                memcpy(kouvas2 + 508, &ngtv, 4);
				ar_egr=0;
                                if(BF_WriteBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1) < 0)
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }	
				if(BF_WriteBlock(fileDesc,chane2) < 0)
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }

			}
			//----------------------------------------------------------------------//

			if (BF_ReadBlock(fileDesc,chane, &kouvas) < 0)
                       	{
                       		 BF_PrintError("Error reading a block");
                                 return -1;
                        }
			memcpy(((kouvas+8)+(ar_egr)*sizeof(Record)),&record,sizeof(Record));	//perna to record sthn alysida//
			ar_egr++;								
			memcpy(kouvas+4,&ar_egr,4);
			if(BF_WriteBlock(fileDesc,chane) < 0)
                        {
                        	BF_PrintError("Error writing a block");
                               	return -1;
                        }
		}

		//------------------------------------------------------------------------------------------------------------------------------------//

		else	//an den einai ola idia//
		{
			memcpy(&topic_depth,block_ptr,4);	//pare to topiko bathos//
			//------------------------------------------------------------------------------------------------------------------------//

			if ((topic_depth==gen_depth)&&(gen_depth<6))	//an to topiko einai idio me to oliko bathos//
			{
				gen_depth++;
				header_info->depth=gen_depth;		//auksise kata 1 to oliko bathos kai perna to sto info//
				if (BF_ReadBlock(fileDesc, 1, &first) < 0)
                        	{                                                                                      
                                	BF_PrintError("Error reading a block");
                                	return -1;
                        	}
				for (k=1;k<=gen_depth-1;k++)	//two=2^(palio oliko bathos)//
				{
					two=two*2;
				}
                        	for (k = two; k <= 2*two - 1; k++)
				{

                                	memcpy(first+k*8, &k, 4);               // auksise tis theseis tou eurethriou//
                                	if (k!=(ar_e+two))		//an to k den einai  o arithmos tou gematou block + oses theseis prosthesame sto eurethrio//
					{ 
						memcpy(&c_ptr,first+(k-two)*8+4,4);	//oi neoi arithmoi k eurethriou tha deixnoun sta idia bucket me //
                                		memcpy(first+4+k*8, &c_ptr, 4);         // k-aithmoi eurethriou pou prosthethikan// 
					}
					else if (k==(ar_e+two))		//an to k einai  o arithmos tou gematou block + oses theseis prosthesame sto eurethrio//
					{                                              
                                		if (BF_AllocateBlock(fileDesc)<0)
                                		{
                                        		BF_PrintError("Error allocating a block ");
                                        		return -1;
                                		}
                                		if (BF_ReadBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1, &hashbin) < 0)
                                		{
                                        		BF_PrintError("Error reading a block");
                                        		return -1;
                                		}
                                		memcpy(hashbin,&gen_depth, 4);		//desmeusai neo bucket arxikopoihse to kai //                            
                                		memcpy(hashbin + 4, &zero, 4);                   
                               		 	memcpy(hashbin + 504, &zero, 4);                                       
                                		memcpy(hashbin + 508, &ngtv, 4);               
                                		if(BF_WriteBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1) < 0)
                                		{
                                        		BF_PrintError("Error writing a block");
                                        		return -1;
                                		}
						c_ptr=BF_GetBlockCounter(fileDesc)-1;
                                                memcpy(first+4+k*8, &c_ptr, 4);		//bale na deixnei to eurethrio se auto to bucket se auto ton arithmo//
                                                if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                                {
                                                        BF_PrintError("Error reading a block");
                                                        return -1;
                                                }
						memcpy(hashbin2,&gen_depth, 4);		//kane to gemato/provlimatiko block na xei topiko bathos=gen bathos
						if(BF_WriteBlock(fileDesc,block_num) < 0)
                                                {
                                                        BF_PrintError("Error writing a block");
                                                        return -1;
                                                }
					}
                        	}
                        	if(BF_WriteBlock(fileDesc, 1) < 0)	//grapse to eurethrio //
                        	{
                                	BF_PrintError("Error writing a block");
                                	return -1;
                        	}
				if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }
				counter2=0;
				memcpy(&chane2,hashbin2+508,4);
				counter3=Chane_Count(block_num,&counter2,fileDesc);	// sto counter3 epistrefetai o arithmos twn eggrafwn tou teleutaiou//
				memcpy(hashbin2+4,&zero,4);  //mhdenise tis  eggrafes// //block mias alysidas eno sto counter2 o arithmos ton bucket ths //
				if(BF_WriteBlock(fileDesc,block_num) < 0)		//alysidas xwris to teleutaio//
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }
                		if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }                
				if(counter2!=0)						//an yparxei aysida//
				{
					counter=7*counter2+counter3;			
					if (BF_ReadBlock(fileDesc,chane2, &block7) < 0)	//pare thn dieythinsh ths arxhs ths alysidas//
                                	{
                                        	BF_PrintError("Error reading a block");
                                        	return -1;
                                	}
					memcpy(hashbin2+508,&ngtv,4);			//bale sto pedio tou block gia thn alysida thn timh -1//
					if(BF_WriteBlock(fileDesc, block_num) < 0)
                                	{	
                                        	BF_PrintError("Error writing a block");
                                        	return -1;
                                	}
                                	if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                	{
                                        	BF_PrintError("Error reading a block");
                                        	return -1;
                                	}	
				}
				else						//an den yparxei alysida//
				{
						counter=counter3;
				}
				Record record2[counter];
				t=1;
				for(i=0;i<=counter-1;i++)		
				{
					if ((i>=0)&&(i<=6))		//pernw ta stoixeia mesa apo to bucket//
					memcpy(&record2[i],hashbin2+8+i*sizeof(Record),sizeof(Record));
					else
					{	
						k=i-7*t;		//0<=k<=6
						Chane_Rec(block7,&next,k,fileDesc,&record3);	//pernw ta stoixeia mesa apo thn alysida//
						memcpy(&record2[i],&record3,sizeof(Record));
						if ((k==6)&&(next==NULL))			//an teleiwsei h alysida bges//
						{
							break;
						}
						if (k==6)					//an k=6 pane ston epomeno block alysidas mhdenise to k//
						{
							block7=next;
							t++;
						}
					}
				}
				for(i=0;i<=counter-1;i++)
                                {
                                     	HT_InsertEntry (header_info,record2[i]);	//kalese anadromika thn insert gia eisagwgh olwn twn stoixeiwn sto pinaka//
                                }
				HT_InsertEntry (header_info,record);			//bale anadromika kai to record//
			}

			//--------------------------------------------------------------------------------------------------------------------------//

			else if (topic_depth<gen_depth)				//an to oliko bathos einai megalytero apo to topiko//
			{
				memcpy(&topic_depth,block_ptr,4);
				topic_depth++;
				if (BF_ReadBlock(fileDesc,1, &first) < 0)
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }
				for (k=1;k<=gen_depth;k++)
                                {
                                        two=two*2;
                                }
				metr=0;
				for(i=0;i<two-1;i++)
				{
					memcpy(&poin,first+4+i*8,4);		// o deikths sto eurethrio pou deixnei sto block me to provlhma//
					if ((poin==block_num)&&(metr==0))	//an einai o prwtos pou synantame//
					{
						metr++;				//anebazw ton metrhth kata 1//
						if (BF_ReadBlock(fileDesc,block_num, &hashbin) < 0)
                        			{
                         				BF_PrintError("Error reading a block");
                               				return -1;
                        			}
						memcpy(hashbin,&gen_depth,4);	//kanw to topiko bathos isa me to oliko//	
                        			if(BF_WriteBlock(fileDesc,block_num) < 0)
                        			{
                                			BF_PrintError("Error writing a block");
                        				return -1;
                        			}
                        			if (BF_ReadBlock(fileDesc,block_num, &hashbin) < 0)
                        			{
                        				BF_PrintError("Error reading a block");
                              				return -1;
                        			}
					}
					else if ((poin==block_num)&&(metr==1))		//an einai o deuteros//
					{
						metr++;
						if (BF_AllocateBlock(fileDesc)<0)	//desmeuw xorw gia ena bucket kai to arxikopoiw//
                                                {
                                                        BF_PrintError("Error allocating a block ");
                                                        return -1;
                                                }
                                                if (BF_ReadBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1, &hashbin) < 0)
                                                {
                                                        BF_PrintError("Error reading a block");
                                                        return -1;
                                                }
                                                memcpy(hashbin,&topic_depth, 4); //auto tha exei topic_depth(provlimatikou +1(to xw kanei ++ pio panw))//
                                                memcpy(hashbin + 4, &zero, 4);
                                                memcpy(hashbin + 504, &zero, 4);
                                                memcpy(hashbin + 508, &ngtv, 4);
                                                if(BF_WriteBlock(fileDesc,BF_GetBlockCounter(fileDesc)-1) < 0)
                                                {
                                                        BF_PrintError("Error writing a block");
                                                        return -1;
                                                }
                                                c_ptr=BF_GetBlockCounter(fileDesc)-1;	//kane to eurethrio na dexnei ekei//
                                                memcpy(first+4+i*8, &c_ptr, 4);
						 if(BF_WriteBlock(fileDesc,1) < 0)                       //grapse to eurethrio//
                                		{	
                                        		BF_PrintError("Error writing a block");
                                        		return -1;
                                		}
                                		if (BF_ReadBlock(fileDesc,1, &first) < 0)    
                                		{
                                        		BF_PrintError("Error reading a block");
                                        		return -1;
                                		}
					}
					else if ((poin==block_num)&&(metr>1))
					{
						metr++;
						memcpy(first+4+i*8, &c_ptr, 4);		//kane to eurethtio na deixnei sto bucket pou desmeusame//
						if(BF_WriteBlock(fileDesc,1) < 0)                       //grapse to eurethrio//
                                                {
                                                        BF_PrintError("Error writing a block");
                                                        return -1;
                                                }
                                                if (BF_ReadBlock(fileDesc,1, &first) < 0)
                                                {
                                                        BF_PrintError("Error reading a block");
                                                        return -1;
                                                }
					}
				}
				if(BF_WriteBlock(fileDesc,1) < 0)			//grapse to eurethrio//
                                {
                                	BF_PrintError("Error writing a block");
                                	return -1;
                                }	
				if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)	//apo edw  ws...//
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }
                		counter2=0;
				memcpy(&chane2,hashbin2+508,4);
				counter3=Chane_Count(block_num,&counter2,fileDesc);
				memcpy(hashbin2+4,&zero,4);
				if(BF_WriteBlock(fileDesc,block_num) < 0)
                                {
                                        BF_PrintError("Error writing a block");
                                        return -1;
                                }
                		if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                {
                                        BF_PrintError("Error reading a block");
                                        return -1;
                                }                
				if(counter2!=0)
				{
					counter=7*counter2+counter3;
					if (BF_ReadBlock(fileDesc,chane2, &block7) < 0)
                                	{
                                        	BF_PrintError("Error reading a block");
                                        	return -1;
                                	}
					memcpy(hashbin2+508,&ngtv,4);
					if(BF_WriteBlock(fileDesc, block_num) < 0)
                                	{	
                                        	BF_PrintError("Error writing a block");
                                        	return -1;
                                	}
                                	if (BF_ReadBlock(fileDesc,block_num, &hashbin2) < 0)
                                	{
                                        	BF_PrintError("Error reading a block");
                                        	return -1;
                                	}	
				}
				else
				{
						counter=counter3;
				}
				Record record2[counter];
				t=1;
				for(i=0;i<=counter-1;i++)
				{
					if ((i>=0)&&(i<=6))
					memcpy(&record2[i],hashbin2+8+i*sizeof(Record),sizeof(Record));
					else
					{	
						k=i-7*t;
						Chane_Rec(block7,&next,k,fileDesc,&record3);
						memcpy(&record2[i],&record3,sizeof(Record));
						if ((k==6)&&((next)==NULL))
						{
							break;
						}
						if (k==6)
						{
							block7=next;
							t++;
						}
					}
				}
				for(i=0;i<=counter-1;i++)
                                {
                                     	HT_InsertEntry (header_info,record2[i]);
                                }							//...ws edw elsigithikan kai parapanw//
				HT_InsertEntry (header_info,record);		
			}
			//---------------------------------------------------------------------------------------------------------------------------//
		}	
		//---------------------------------------------------------------------------------------------------------------------------//
	}
	//---------------------------------------------------------------------------------------------------------------------------//
	else	//aplh periptwsh//
	{
		memcpy(block_ptr+8+reccount*sizeof(Record),&record,sizeof(Record));	//kane eisagwgh ekei pou prepei//
		reccount++;
		memcpy(block_ptr+4,&reccount,4);
		if(BF_WriteBlock(fileDesc,block_num) < 0)
                {
               		BF_PrintError("Error writing a block");
                       	return -1;
                }
	}
	//---------------------------------------------------------------------------------------------------------------------------//
	return 0;
}

//=======================================================================================================================================//

void HT_GetAllEntries(HT_info* header_info, void* value)
{
	int i, j, block_counter = 0, megethos_euretiriou = 1, bucket_num = -1, reccount, hash_code, found = 0, record_counter = 0;
	char temp, temp2[20];
	void* block;
	void* bucket;
	Record record;
	int* index;
	for (i = 1; i <= header_info->depth; i++)
		megethos_euretiriou = megethos_euretiriou * 2;
	index = malloc(megethos_euretiriou * sizeof(int));	//ftiaxnw ena pinaka opou tha antigrapsw to euretirio//
	if (index == NULL)
	{		//an den mporese na desmeutei xwros, ektipwse error//
		printf("Memory allocation failed\n");
		return;
	}
	if (BF_ReadBlock(header_info->fileDesc, 1, &block) < 0)
	{
		BF_PrintError("Error reading a block");
		return;
	}
	for (i = 0; i < megethos_euretiriou; i++)
	{
		memcpy(&bucket_num, block + 4 + 8*i, 4);
		index[i] = bucket_num;					//antigrafw to euretrio//
	}
	if (value == NULL)							//periptwsi pou ektipwnw oles tis eggrafes//
	{
		for (i = 0; i < megethos_euretiriou; i++)
		{										//gia oles tis theseis toy euretiriou//
			if (BF_ReadBlock(header_info->fileDesc, index[i], &bucket) < 0)
			{									//diavazw to bucket//
				BF_PrintError("Error reading a block");
				return;
			}
			memcpy(&reccount, bucket + 504, 4);	//pairnw sto reccount tin endeiksi gia tin ektipwsi//
			if (reccount == 1)	//an to bucket exei ektipwthei se proigoumeni epanalipsi//
				continue;		//to prospernaw//
			reccount = 1;		//proswrina to reccount ginetai 1//
			memcpy(bucket + 504, &reccount, 4);	//pernaw tin endeiksi oti to bucket auto tha ektipwthei//
			if (BF_WriteBlock(header_info->fileDesc, index[i]) < 0)
			{
				BF_PrintError("Error writing a block");
				return;
			}
			do
			{
				block_counter++;	//auksanetai  o metritis twn bucket pou diavastikan//
				memcpy(&reccount, bucket + 4, 4); //pairnw to plithos twn eggrafwn tou bucket//
				for (j = 0; j < reccount; j++)
				{								//gia oles tis eggrafes tou bucket//
					memcpy(&record, bucket + 8 + j*sizeof(Record), sizeof(Record));	//pairnw tin eggrafi//
					print_record(record);		//kai tin  ektipwnw//
					record_counter++;
				}
				memcpy(&bucket_num, bucket + 508, 4);
				if (bucket_num == -1)	//an den iparxei bucket se alisida//
					break;				//stamataw to do-while//							
				if (BF_ReadBlock(header_info->fileDesc, bucket_num, &bucket) < 0)
				{	//an iparxei alisida, diavazw to epomeno bucket tis alisidas//
					BF_PrintError("Error reading a block");
					return;
				}
			} while (1);
		}
		printf("%d records read in file.\n", record_counter);
	}
	else
	{
		if (header_info->attrType == 'i')
		{	//an prokeitai gia akeraio kleidi//
			memcpy(&i, value, 4);
			hash_code = HT_Function(header_info->depth, i, NULL, 0);
		}
		else if (header_info->attrType == 'c' && header_info->attrLength > 1)
		{	//an prokeitai gia kleidi-simvoloseira//
			if (strcmp(header_info->attrName, "name") == 0)
				hash_code = HT_Function(header_info->depth, 0, value, 15);
			else if (strcmp(header_info->attrName, "surname") == 0)
				hash_code = HT_Function(header_info->depth, 0, value, 20);
			else
				hash_code = HT_Function(header_info->depth, 0, value, 0);
		}
		else if (header_info->attrType == 'c' && header_info->attrLength == 1)
		{	//an proketai gia kleidi-char//
			memcpy(&temp, value, 4);
			hash_code = HT_Function(header_info->depth, 0, &temp, 1);
		}	//pairnw tin thesi tou euretiriou pou antistoixei sto kleidi//
		if (BF_ReadBlock(header_info->fileDesc, index[hash_code], &bucket) < 0)
		{								//diavazw to bucket//
			BF_PrintError("Error reading a block");
			return;
		}
		do
		{
			block_counter++;	//auksanetai  o metritis twn bucket pou diavastikan//
			memcpy(&reccount, bucket + 4, 4); //pairnw to plithos twn eggrafwn tou bucket//
			for (j = 0; j < reccount; j++)
			{								//gia oles tis eggrafes tou bucket//
				i = compare(bucket + 8 + j*sizeof(Record), header_info->attrName, value, 1);
				if (i == -1)
				{
					printf("In fuction GetAllEntries: Wrong values given\n");
					return;
				}
				else if (i == 1)
				{
					record_counter++;
					found = 1;
				}
			}
			memcpy(&bucket_num, bucket + 508, 4);
			if (bucket_num == -1)	//an den iparxei bucket se alisida//
				break;				//stamataw to do-while//
			if (BF_ReadBlock(header_info->fileDesc, bucket_num, &bucket) < 0)
			{	//an iparxei alisida, diavazw to epomeno bucket tis alisidas//
				BF_PrintError("Error reading a block");
				return;
			}
		} while (1);
		if (found == 0)
			printf("Den vrethike eggrafi sto arxeio me to kleidi pou dwsate.\n");
		else
		{
			printf("%d records matched in field '%s' with key ", record_counter, header_info->attrName);
			if (header_info->attrType == 'i')
			{
				memcpy(&i, value, 4);
				printf("'%d'.\n", i);
			}
			else if (header_info->attrType == 'c' && header_info->attrLength == 1)
				printf("'%c'.\n", temp);
			else
			{
				memcpy(&temp2, value, strlen(value) + 1);
				printf("'%s'.\n", temp2);
			}
		}
	}
	printf("%d blocks read.\n", block_counter);
	free(index);
}	

//=========================================================================================================================================//

int HT_Function(int depth, int akeraio_pedio, char* string_pedio, int length)	//sinartisi katakermatismou//
{	/*pairnei orismata: to vathos kai an prokeitai gia kleidi-arithmo pairnei ena arithmo
	kai NULL gia trito orisma, enw an prokeitai gia kleidi-string pairnei san trito
	orisma to string kai san tetarto orisma to length tou*/
	unsigned int i, temp = 1, temp2, sum, max;
	char* str;
	for (i = 1; i <= depth; i++)
		temp = 2 * temp;			//ipologizetai to 2^depth//
	if (string_pedio == NULL)		// an prokeitai gia kleidi-arithmo//
		return (akeraio_pedio % temp);
	else if (string_pedio != NULL && length == 1)
	{								//an prokeitai gia tipo-kleidi char//
		sum = (int) string_pedio[0] - 64;	//filaw to kefalaio gramma//
		return (sum % temp);
	}	
	else
	{								//an prokeitai gia kleidi-string//
		str = malloc((strlen(string_pedio) + 1)*sizeof(char));	//desmeusi aparaititou xwrou//
		strcpy(str, string_pedio);	//filaw sto str to string//
		sum = (int)str[0];			//pairnw ton prwto xaraktira//
		if (atoi(str) != 0)
		{							//an prokeitai gia imerominia//
			sum = sum - 48;			//opws 17-1-1993//
			for (i = 1; i <= strlen(str) - 1; i++)
			{						
				if (str[i] == '-')
					continue;
				temp2 = (int) str[i];
				temp2 = temp2 - 48;
				sum = sum * 10;
				sum = sum + temp2;	//filaw sto sum to 1711993//
			}
			free(str);
			return (sum % temp);
		}							//an proketai gia leksi h gramma//
		sum = sum - 64;				//pairnw enan arithmo gia to prwto kefalaio gramma//
		if (length == 20 || length == 15)
		{
			for (i = 1; i <= strlen(str); i++)
			{
				temp2 = (int) str[i];
				temp2 = temp2 - 96;
				sum = i * sum;
				sum = sum + temp2;
			}
		}
		free(str);
		return (sum % temp);
	}
}

//=========================================================================================================================================//

 int Mod_Class(int gen_depth,HT_info* header_info,Record record)	//periptwseologia typwn gia klhsh ths Hash fuction//
 {
	int mod_class;
	if (header_info->attrType=='c')
        {
                if (strcmp((header_info->attrName),"name")==0)
                {
                        mod_class=HT_Function(gen_depth,0, record.name,15);
                }
                else if (strcmp((header_info->attrName),"surname")==0)
                {
                        mod_class=HT_Function(gen_depth,0, record.surname,20);
                }
                else if (strcmp((header_info->attrName),"status")==0)
                {
                        mod_class=HT_Function(gen_depth,0, (record.status),1);
                }
                else if (strcmp((header_info->attrName),"dateOfBirth")==0)
                {
                        mod_class=HT_Function(gen_depth,0, record.dateOfBirth,0);
                }
                else if (strcmp((header_info->attrName),"section")==0)
                {
                        mod_class=HT_Function(gen_depth,0, (record.section),1);
                }
                else
				{
                        printf("Wrong data \n");
                        return -1;
                }
        }
        else if(header_info->attrType=='i')
        {
                if (strcmp((header_info->attrName),"id")==0)
                {
                        mod_class=HT_Function(gen_depth,record.id,NULL,0);
                }
                else if (strcmp((header_info->attrName),"salary")==0)
                {
                        mod_class=HT_Function(gen_depth,record.salary,NULL,0);
                }
                else if (strcmp((header_info->attrName),"daysOff")==0)
                {
                        mod_class=HT_Function(gen_depth,record.daysOff,NULL,0);
				}
                else if (strcmp((header_info->attrName),"prevYears")==0)
                {
                        mod_class=HT_Function(gen_depth,record.prevYears,NULL,0);
                }
                else
                {
                        printf("Wrong data \n");
                        return -1;
                }
        }
        else
        {
                printf("Wrong data \n");
                return -1;
        }
	return mod_class;
}

//============================================================================================================================================================//

int AllSame(void * block_ptr,char * fieldName,void * value)	//xrhsh ths compare gia epalitheush an ena pedio twn eggrafwn enos block einai idio me to//
{								//pedio value//
	int i,allsame;
	allsame=1;
	for (i=0;i<=6;i++)
	{
		if (compare(block_ptr+8+i*sizeof(Record),fieldName,value,0))
		{
			allsame=0;
			break;
		}
	}
	return allsame;
}

//===========================================================================================================================================================//

int compare(void* block, char* fieldName, void* value, int use)
{
	unsigned int bytes_to_compare;
	Record temp;
	int num, flag;
	char ch, type;
	char word[20];
	memcpy(&temp, block, sizeof(Record));				//apothikeusi tis eggrafis sto temp apo to block
	//Stis parakatw periptwsis elenxetai to onoma tou pediou kai:
	//	1.apothikeuetai o tipos tou pediou kleidiou
	//	2.filassetai stin katallili metavliti to pedio pros sigrisi
	//	3.filassontai ta bytes ws pros ta opoia tha ginei i sigrisi
	if (strcmp(fieldName, "id") == 0)
	{
		type = 'i';
		num = temp.id;
		bytes_to_compare = sizeof(temp.id);
	}
	else if (strcmp(fieldName, "name") == 0)
	{
		type = 's';
		strcpy(word, temp.name);
		bytes_to_compare = strlen(temp.name) + 1;
	}
	else if (strcmp(fieldName, "surname") == 0)
	{
		type = 's';
		strcpy(word, temp.surname);
		bytes_to_compare = strlen(temp.surname) + 1;
	}
	else if (strcmp(fieldName, "status") == 0)
	{
		type = 'c';
		ch = temp.status[0];
		bytes_to_compare = sizeof(temp.status);
	}
	else if (strcmp(fieldName, "dateOfBirth") == 0)
	{
		type = 's';
		strcpy(word, temp.dateOfBirth);
		bytes_to_compare = strlen(temp.dateOfBirth) + 1;
	}
	else if (strcmp(fieldName, "salary") == 0)
	{
		type = 'i';
		num = temp.salary;
		bytes_to_compare = sizeof(temp.salary);
	}
	else if (strcmp(fieldName, "section") == 0)
	{
		type = 'c';
		ch = temp.section[0];
		bytes_to_compare = sizeof(temp.section);
	}
	else if (strcmp(fieldName, "daysOff") == 0)
	{
		type = 'i';
		num = temp.daysOff;
		bytes_to_compare = sizeof(temp.daysOff);
	}
	else if (strcmp(fieldName, "prevYears") == 0)
	{
		type = 'i';
		num = temp.prevYears;
		bytes_to_compare = sizeof(temp.prevYears);
	}
	else											//an den einai tipota apo ta parapanw, error
		return -1;
	if (type == 'i')
		flag = memcmp(&num, value, bytes_to_compare);
	else if (type == 'c')
		flag = memcmp(&ch, value, bytes_to_compare);
	else if (type == 's')
		flag = memcmp(word, value, bytes_to_compare);
	if (use == 0)									//an kalestei apo tin insert
		return flag;
	else if (use == 1)									//an kalestei apo tin getallentries
	{
		if (flag == 0)	//se periptwsi isotitas sto pedio kleidi
		{
			print_record(temp);							//ektipwse tin eggrafi
			return 1;									//an eixa isotita me to kleidi
		}
	}
	return 0;											//an den eixa isotita me to kleidi
}

//=========================================================================================================================================================//

void * Rec_Key(char * fieldName,Record *record)	//epistrofh deikth sthn timh tou pediou pou prosdiorizetai apo to fieldname tou record//
{
	if (strcmp(fieldName, "id") == 0)
	{
		return &(record->id);
	}
	else if (strcmp(fieldName, "name") == 0)
	{
		return &(record->name);
	}
	else if (strcmp(fieldName, "surname") == 0)
	{	
		return &(record->surname);
	}
	else if (strcmp(fieldName, "status") == 0)
	{
		return &(record->status);
	}
	else if (strcmp(fieldName, "dateOfBirth") == 0)
	{
		return &(record->dateOfBirth);
	}
	else if (strcmp(fieldName, "salary") == 0)
	{
		return &(record->salary);
	}
	else if (strcmp(fieldName, "section") == 0)
	{
		return &(record->section);
	}
	else if (strcmp(fieldName, "daysOff") == 0)
	{
		return &(record->daysOff);
	}
	else if (strcmp(fieldName, "prevYears") == 0)
	{
		return &(record->prevYears);
	}
	else
	{
		printf("Error \n ");
		return NULL;
	}
}

//=========================================================================================================================================================//

int Chane_Count(int block_num,int * counter,int fileDesc)	//epistrefei twn arithmo twn block sto *counter xwris to teleutaio mia alysidas//
{
	void * block;
	int chane,counter2;
	if (BF_ReadBlock(fileDesc, block_num, &block) < 0)
        {                           
         	BF_PrintError("Error reading a block");
                return;
        }
	memcpy(&chane,block+508,4);
	if (chane==-1){memcpy(&counter2,block+4,4);return counter2;} 	//epistrefei twn arithmwn ton eggrafwn tou teleutaiou blocj mias alysidas//
	else {(*counter)++; return Chane_Count(chane,counter,fileDesc);}

}

//=================================================================================================================================================//

void Chane_Rec(void * block,void** next,int i,int fileDesc,Record * record)
{
	void * block2;
        int chane;
	memcpy(record,block+8+i*sizeof(Record),sizeof(Record));	 //bale sto record thn k-osth eggrafh tou block me deikth block//
	if (i==6) 
	{
		memcpy(&chane,block+508,4);		//an i=6//
		if (chane==-1){ *next=NULL; return;}		//bale sto next=Null//
        	if (BF_ReadBlock(fileDesc,chane, &block2) < 0)
        	{
                	BF_PrintError("Error reading a block");
                	return;
        	}
		*next=block2;	
		return ;
	}
	(*next)=NULL;	
	return ;
}

//==================================================================================================================================================//

int Last_Chane(int fileDesc,int chane)
{ 
	void* block;							//epistrefei to teleutaio block mias alysidas//
	int next;
	if (BF_ReadBlock(fileDesc,chane, &block) < 0)
        {
        	BF_PrintError("Error reading a block");
                return;
        }
	memcpy(&next,block+508,4);
	if (next!=-1) return Last_Chane(fileDesc,next);
	else return chane;
}
	
//==============================================================================================================================================//
