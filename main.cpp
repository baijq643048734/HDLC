/*********************************************
@ author : Bai Jiaqi °×¼Î÷è 
@ ID : 161720328
@ class: 1617203
@ date : 2020/5/11
@ instructor :Teacher Ye Haibo Ò¶º£²¨ÀÏÊ¦ 
**********************************************/

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#define I 0				//¿ØÖÆ×Ö¶ÎĞÅÏ¢Ö¡±àºÅ 
//#define S 2				//¿ØÖÆ×Ö¶Î¼à¿ØÖ¡±àºÅ 
//#define U 3				//¿ØÖÆ×Ö¶ÎÎŞ±àºÅÖ¡±àºÅ 

//bool IF_INFO_EXIST = 1;			//È«¾Ö±äÁ¿ ÔÚ×éÖ¡¹ı³ÌÖĞ¸æÖªÊÇ·ñ´æÔÚ¼à¿ØÖ¡

#define BYTE 8								//1×Ö½Ú 
#define MAX_FRAME 12000 					//Êı¾İÁ´Â·²ã×î´óÖ¡³¤Îª1500 byte ¼´ 12000bit 
#define MAX_INFO  11952						/*×î´óÖ¡³¤ 1500 byte - Ö¡Í·Ö¡Î² 2 byte - µØÖ·×Ö¶Î 1 byte - ¿ØÖÆ×Ö¶Î 1 byte - FCS×Ö¶Î 2 byte = 1494 byte 
												¼´ 11952bit */					/*ï¿½ï¿½ï¿½Ö¡ï¿½ï¿½ 1500 byte - Ö¡Í·Ö¡Î² 2 byte - ï¿½ï¿½Ö·ï¿½Ö¶ï¿½ 1 byte - ï¿½ï¿½ï¿½ï¿½ï¿½Ö¶ï¿½ 1 byte - FCSï¿½Ö¶ï¿½ 2 byte = 1494 byte 
												ï¿½ï¿½ 11952bit */
												
// frame.h´Ë´¦º¯Êı¾ùÎª×éÖ¡²Ù×÷ÄÚ²¿µ÷ÓÃº¯Êı********************************************************************************												
int _2_x(int x);										//2µÄx´Î·½
void char_to_bit(char letter, char *bit);				//½«µ¥¶À×ÖÄ¸×ª»¯Îª±ÈÌØ´®
int info_to_bit(char *Info, int Info_len, char *Info_temp);			//½«´«ÊäÊı¾İ×ª»¯Îª±ÈÌØ´® 
void print_info_bit(char *Info,int Info_len);						//´òÓ¡Êı¾İ±ÈÌØ´®												
void flag_fld_header(char *frame);						//³õÊ¼»¯×éÖ¡Ö¡Í· 
void addr_fld(char *frame, char *Address);				//³õÊ¼»¯×éÖ¡µØÖ·×Ö¶Î 												
void control_fld(char *frame, char *Control);			//³õÊ¼»¯×éÖ¡¿ØÖÆ×Ö¶Î
//int bit_stf(char *Info, int Info_len);					//Êı¾İ×Ö¶Î×Ö½ÚÀ©³ä
int info_fld(char *frame, char *Info, int Info_len);	//³õÊ¼»¯×éÖ¡Êı¾İ×Ö¶Î 
int fcs_fld(char *frame,char *FCS,int frame_length);	//³õÊ¼»¯×éÖ¡FCS×Ö¶Î
int bit_stf(char *frame, int frame_length);				//0bitÀ©³ä
int flag_fld_end(char *frame, int frame_length);		//³õÊ¼»¯×éÖ¡Ö¡Î² 
void print_frame(char *frame, int frame_length);		//´òÓ¡×éÖ¡

void Frameing(char *Address, char *Control, char *Info, int Info_len, char *FCS, char *frame);		//×éÖ¡²Ù×÷
// ******************************************************************************************************************

// initfield.h**********************************************************************************
int init_control_fld(char *Control,char control_type); 			//¿ØÖÆ×Ö¶ÎÉú³É ·µ»Ø¿ØÖÆ×Ö¶ÎÖ¡ÀàĞÍ±àºÅ 
int init_info_fld(char *Info, int info_type,char *Info_temp);	//¸ù¾İ¿ØÖÆ×Ö¶ÎÖ¡ÀàĞÍ±àºÅ Ìî³äÊı¾İ×Ö¶Î ·µ»ØÊı¾İ×Ö¶Î³¤¶È 
void init_fcs_fld(char *Info, char *FCS,int info_type);			//¸ù¾İÊı¾İÉú³ÉCRCĞ£ÑéÂë 
void bin_div(char *inStr,char *outStr);
void checkCRC(char *inStr, char *outStr);
//***********************************************************************************************


int _2_x(int x){		//2µÄx´Î·½
	int num = 1;
	for(int i = 0; i < x; i++){
		num *= 2;
	}
	return num;
}


void char_to_bit(char letter, char *bit){		//½«µ¥¶À×ÖÄ¸×ª»¯Îª±ÈÌØ´®	
	char temp = letter;
	for(int i = 0; i < 8; i++)
	{
		if(temp / _2_x(7 - i) != 0){
			bit[i] = '1';
			temp = temp - _2_x(7 - i);
		}
		else
			bit[i] = '0';
	}
}

int info_to_bit(char *Info, int Info_len, char *Info_temp){		//½«´«ÊäÊı¾İ×ª»¯Îª±ÈÌØ´® 
	char bit[8];
	for(int i = 0; i < Info_len; i++){
		char_to_bit(Info[i],bit);
		for(int j = 0; j < 8; j++){
			Info_temp[i*8+j] = bit[j];			
		}
	}
	strcpy(Info,Info_temp);
	return strlen(Info);
}

void print_info_bit(char *Info,int Info_len){			//´òÓ¡Êı¾İ±ÈÌØ´®
	for(int i = 0; i < Info_len; i++){
		printf("%c",Info[i]);
	}
	printf("\n");
}

void flag_fld_header(char *frame){						//³õÊ¼»¯×éÖ¡Ö¡Í· 
	frame[0] = '0';
	for(int i = 1; i < 7; i++){
		frame[i] = '1';
	}
	frame[7] = '0';
}

void addr_fld(char *frame, char *Address){				//³õÊ¼»¯×éÖ¡µØÖ·×Ö¶Î 
	int j = 0;
	for(int i = 8; i <= 15; i++){
		frame[i] = Address[j++];
	}
}

void control_fld(char *frame, char *Control){			//³õÊ¼»¯×éÖ¡¿ØÖÆ×Ö¶Î 
	int j = 0;
	for(int i = 16; i <= 23; i++){
		frame[i] = Control[j++];
	}
}


int info_fld(char *frame, char *Info, int Info_len){	//³õÊ¼»¯×éÖ¡Êı¾İ×Ö¶Î 
	int frame_length = 24 + Info_len;
	int j = 0;
	for(int i = 24; i < 24 + Info_len; i++){
		frame[i] = Info[j++];
	}
	return frame_length;
}

int fcs_fld(char *frame,char *FCS,int frame_length){	//³õÊ¼»¯×éÖ¡FCS×Ö¶Î 
	int j = 0;
	for(int i = frame_length; i < frame_length + 16; i++){
		frame[i] = FCS[j++];
	}
	frame_length = frame_length + 16;
	return frame_length;
}

int bit_stf(char *frame, int frame_length){					//0bitÀ©³ä 
	int cnt_1 = 0;
	for(int i = 8; i < frame_length; i++){
		if(frame[i] == '1') cnt_1++;
		else cnt_1 = 0;
		if(cnt_1 == 5){
			int temp1 = frame[i+1];
			frame[i+1] = '0';
			frame_length++;
			int temp2;
			for(int j = i + 2; j < frame_length; j++){
				temp2 = frame[j];
				frame[j] = temp1;
				temp1 = temp2;
			}
		}
	}
	return frame_length;
}

int flag_fld_end(char *frame, int frame_length){		//³õÊ¼»¯×éÖ¡Ö¡Î² 
	frame[frame_length] = '0';
	for(int i = frame_length + 1; i < frame_length + 7; i++){
		frame[i] = '1';
	}
	frame_length = frame_length + 7;
	frame[frame_length++] = '0';
	return frame_length;
}

void print_frame(char *frame, int frame_length){		//´òÓ¡×éÖ¡ 
	printf("×éÖ¡: \n");
	for(int i = 0; i < frame_length; i++){
		printf("%c",frame[i]);
	}
	printf("\nÖ¡³¤: %d(bit)\n",frame_length);
}

void Frameing(char *Address, char *Control, char *Info, int Info_len, char *FCS, char *frame){		//×éÖ¡²Ù×÷ 
//            address,		 control,	  	information,infobytelength, FCS,		Frame
	int frame_length;							//×éÖ¡³¤¶È 
//Flag field 
	flag_fld_header(frame);
//Address field 
	addr_fld(frame,Address);		
//Control field
	control_fld(frame,Control);
//Information field
	frame_length = info_fld(frame,Info,Info_len);
//FCS filed
	frame_length = fcs_fld(frame,FCS,frame_length);
//Bit stuffing
	frame_length = bit_stf(frame,frame_length);
//Flag field
	frame_length = flag_fld_end(frame,frame_length);
//Print Frame
	print_frame(frame,frame_length);
}

int init_control_fld(char *Control,char control_type){						//¿ØÖÆ×Ö¶ÎÉú³É ·µ»Ø¿ØÖÆ×Ö¶ÎÖ¡ÀàĞÍ±àºÅ 
	switch(control_type){
		case 'I':			//ĞÅÏ¢Ö¡ 
		case 'i':
			Control[0] = '0';
			//Ä¬ÈÏN(s) = 1, N(r) = 1, P/F bit = 1 
				Control[1] = '0';	//N(s) = Control[1,2,3] 
				Control[2] = '0';
				Control[3] = '1';
				
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			return 0;		//ĞÅÏ¢Ö¡±àºÅÎª0 
		case 'S':			//¼à¿ØÖ¡ 
		case 's':
			Control[0] = '1';
			Control[1] = '0';
			//Ä¬ÈÏ bit2 bit3 Îª 00(RR), P/F bit = 1, N(r) = 1
				Control[2] = '0';
				Control[3] = '0';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			 return 2;		//¼à¿ØÖ¡±àºÅÎª2 
		case 'U':			//ÎŞ±àºÅÖ¡ 
		case 'u':
			Control[0] = '1';
			Control[1] = '1';
			//Ä¬ÈÏÖÃÕı³£ÏìÓ¦Ä£Ê½SNRM(M1 = 00, M2 = 001), P/F bit = 1
				Control[2] = '0';	//M1
				Control[3] = '0';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//M2
				Control[6] = '0';
				Control[7] = '1';
			return 3;		//ÎŞ±àºÅÖ¡±àºÅÎª3 
		default:
			printf("HDLCÖ¡ÀàĞÍÊäÈëÓĞÎó!\n");
			exit(0);
	}
}


int init_info_fld(char *Info, int info_type, char *Info_temp){								//¸ù¾İ¿ØÖÆ×Ö¶ÎÖ¡ÀàĞÍ±àºÅ Ìî³äÊı¾İ×Ö¶Î
	int info_len;
	switch(info_type){
		case 0:			//ĞÅÏ¢Ö¡ Ğ´ÈëĞÅÏ¢²¢×ª»»Îª±ÈÌØ´® 
			printf("Òª´«ÊäµÄÊı¾İ:\n");
			scanf("%s",Info);
			info_len = strlen(Info);
			printf("×ª»»Îª±ÈÌØ´®:\n");
			info_len = info_to_bit(Info,info_len,Info_temp);
			print_info_bit(Info,info_len);
			return info_len;
		case 2:			//¼à¿ØÖ¡ ĞÅÏ¢×Ö¶Î²»´æÔÚ
			Info[0] = '#';			//#±íÊ¾Êı¾İ×Ö¶Î²»´æÔÚ 
//			IF_INFO_EXIST = 0;		//È«¾Ö±äÁ¿ ÔÚ×éÖ¡¹ı³ÌÖĞ¸æÖªÊÇ·ñ´æÔÚ¼à¿ØÖ¡
			return 0;
		case 3:			//ÎŞ±àºÅÖ¡ Ä¬ÈÏĞÅÏ¢×Ö¶ÎÈ«Îª0 
			for(int i = 0; i < BYTE; i++){
				Info[i] = '0';
			}
			info_len = BYTE;
			return info_len;
	}
}	

void init_fcs_fld(char *Info,char *FCS,int info_type){			//³õÊ¼»¯FCS×Ö¶Î 
	switch(info_type){
		case 2:			//ÈôÎª¼à¿ØÖ¡ FCS×Ö¶ÎÖÃÎªÈ«0
			for(int i = 0; i < 2*BYTE; i++){
				FCS[i] = '0';
			}
			printf("CRCĞ£ÑéÂë:\n");
			for(int i = 0; i < 2*BYTE; i++){
				printf("%c",FCS[i]);
			}
			printf("\n");
			break;
		case 0:			//ÈôÎªÊı¾İÖ¡»òÎŞ±àºÅÖ¡£¬ÔòÉú³ÉCRCÈßÓàĞ£ÑéÂë 
		case 3:
			checkCRC(Info,FCS);
			printf("CRCĞ£ÑéÂë:\n");
			for(int i = 0; i < 2*BYTE; i++){
				printf("%c",FCS[i]);
			}
			printf("\n");
			break; 
			
	}
}

void bin_div(char *inStr,char *outStr){
	int ii = 0;
	int flag = 0;
	int ff = 0;
	int len = 0;
	int maxlen = MAX_INFO;
	int div_n = 0;
	int n = 16;						//¶àÏîÊ½³¤¶È 
	char p[]="10001000000100001";	//³ıÊ½£¨CRCÖĞµÄ¶àÏîÊ½£© 
	char temp[maxlen];
	char temp2[maxlen];
	for(int i = 0; (inStr[i] == '0' || inStr[i] == '1') && i < maxlen; i++, len++){
		temp[i]=inStr[i];	
	}
	ii=0;
	for(int i = 0; i < len; i++){
		if(temp[i] == '1'){		//±»³ıÊıµÄµÚÒ»¸ö1
			div_n = i;
			break;
		}
	}
	ii=0;
	ff=0;
	if(len-div_n>=16){	//Èç¹û±»³ıÊıµÄµÚÒ»¸ö1µ½±»³ıÊıµÄ×îºóÒ»Î»Ö®¼äµÄ³¤¶È´óÓÚµÈÓÚ16£¨Êµ¼ÊÊıÑ§ÒâÒåÊÇ±»³ıÊı´óÓÚ³ıÊı£©
		//´Ó±»³ıÊıµÄµÚÒ»¸ö1¿ªÊ¼Óë11000000 00000011 ×öÒì»òÔËËãµÃµ½½á¹û´æ·ÅÔÚÁÙÊ±Êı×étemp[]ÖĞ
		for(int i = div_n; i < len; i++){
			if(i < div_n + 16){
				if(temp[i] != p[i-div_n]) temp2[i-div_n] = '1';
				else temp2[i-div_n] = '0';
			}
			else temp2[i-div_n] = temp[i];
		} 
		
	}
	else{//±»³ıÊıµÄµÚÒ»¸ö1µ½±»³ıÊıµÄ×îºóÒ»Î»Ö®¼äµÄ³¤¶ÈĞ¡ÓÚµÈÓÚ16£¨Êµ¼ÊÊıÑ§ÒâÒå±»³ıÊı±È³ıÊıĞ¡£¬Ö±½Ó×öÎªÓàÊı£©
		for(int i = 0; i < 16; i++){
			temp2[i] = temp[len-16+i];
		}
	}
	//É¾³ıÁãÇ°×º
	for(int i = 0; i < len && (temp2[i] == '1' || temp2[i] == '0'); i++){
		if(ff == 0 && temp2[i] == '1') ff=1;		
		if(ff==1){
			outStr[ii] = temp2[i];
			ii++;
		}
	}		
}

void checkCRC(char *inStr, char *outStr){
	int i;
	int maxlen = MAX_INFO;
	int len;
	char temp[MAX_INFO];
	char tempRx[MAX_INFO];

	for(i = 0; (inStr[i] == '0' || inStr[i] == '1') && i < maxlen ; i++){		//Îªf(x)ºóÃæ¼Ó15¸ö0ÁÙÊ±´æ·ÅÔÚtemp[]
		temp[i]=inStr[i];		
	}
	len = i;
	for(i = len; i < len + 15 ; i++){
			temp[i]='0';
	}
	len = i;
	bin_div(temp,tempRx);
	for(i = 0, len = i; i < maxlen && (tempRx[i] == '0' || tempRx[i] == '1'); i++);
	len = i;
	while(len > 15){
		for(i = 0; i < maxlen; i++){
			temp[i] = tempRx[i];
			tempRx[i] = 0;
		}
		
		bin_div(temp,tempRx);
		
		for(i = 0; i < maxlen && (tempRx[i] == '0' || tempRx[i] == '1'); i++);
		len = i;
	}
	for(i = 0; i < 16 - len; i++){
		outStr[i] = '0';		
	}
	for(i = 16 - len; i < 16; i++){
		outStr[i] = tempRx[len + i - 16];		
	}
}

char Address[BYTE+1];						//µØÖ·×Ö¶ÎA
char Control[BYTE+1];						//¿ØÖÆ×Ö¶ÎC(Ö»¿¼ÂÇµ¥×Ö½Ú)
char Info[MAX_INFO];						//Êı¾İ×Ö¶ÎS 
char Info_temp[MAX_INFO];					//×ª»»³É±ÈÌØ´®µÄÊı¾İ×Ö¶Î¸¨ÖúÊı×é 
int Info_len;								//Êı¾İ×Ö¶Î³¤¶È 
char FCS[BYTE * 2 + 1];						//Ö¡Ğ£ÑéĞòÁĞ×Ö¶ÎFCS 
char frame[MAX_FRAME];						//´æ´¢×îÖÕ×éÖ¡

int main(){
	char control_type;
	printf("ÊäÈëÒªHDLCÖ¡ÀàĞÍ(I or S or U):\n");
	scanf("%c",&control_type);
	int info_type = init_control_fld(Control,control_type);
	Info_len = init_info_fld(Info,info_type,Info_temp);
	printf("Òª´«ÊäµÄµØÖ·(8bit):\n");
	scanf("%s",Address);
//	printf("¿ØÖÆ×Ö¶Î(8bit):\n");
//	scanf("%s",Control);
	init_fcs_fld(Info,FCS,info_type);
//	printf("FCS×Ö¶Î(16bit):\n");
//	scanf("%s",FCS);
	Frameing(Address,Control,Info,Info_len,FCS,frame);
	return 0;
}
 
