/*********************************************
@ author : Bai Jiaqi 白嘉麒 
@ ID : 161720328
@ class: 1617203
@ date : 2020/5/11
@ instructor :Teacher Ye Haibo 叶海波老师 
**********************************************/

/*********************************************
@ 完成了HDLC最基本的协议内容,包括:
@ 1. 组帧 拆帧 对帧进行分析
@ 2. 连接的建立与拆除
@ 3. 实用停等协议的模拟演示
@ 4. CRC校验 
@ Attention: 由于Dev-C++ 编译器多文件连编时
@            总是找不到头文件内容 故把所有
@			 函数都放入main.cpp中，造成阅读
@			 不便 尽情谅解! ^_^
**********************************************/

 
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define I 0				//控制字段信息帧编号 
#define S 2				//控制字段监控帧编号 
#define U 3				//控制字段无编号帧编号 
#define SNRM 30 		//无编号帧中SARM建立连接帧
#define UA 31			//无编号帧中UA确认帧
#define DISC 32			//无编号帧中DISC拆连帧 
#define RR 20			//监控帧中确认收到的N(r)帧 
#define SREJ 21			//监控帧中否认收到的N(r)帧 

bool IF_INFO_EXIST = 1;			//全局变量 在组帧过程中告知是否存在监控帧

#define BYTE 8								//1字节 
#define MAX_FRAME 12000 					//数据链路层最大帧长为1500 byte 即 12000bit 
#define MAX_INFO 11952						/*最大帧长 1500 byte - 帧头帧尾 2 byte - 地址字段 1 byte - 控制字段 1 byte - FCS字段 2 byte = 1494 byte 
												即 11952bit */	
#define MAX_STR 1449
												
// frame.h此处函数均为组帧操作内部调用函数***************************************************************************											
int _2_x(int x);										//2的x次方
void char_to_bit(char letter, char *bit);				//将单独字母转化为比特串
int info_to_bit(char *Info, int Info_len, char *Info_temp);			//将传输数据转化为比特串 
void print_info_bit(char *Info,int Info_len);						//打印数据比特串												
void flag_fld_header(char *frame);						//初始化组帧帧头 
void addr_fld(char *frame, char *Address);				//初始化组帧地址字段 												
void control_fld(char *frame, char *Control);			//初始化组帧控制字段
//int bit_stf(char *Info, int Info_len);					//数据字段字节扩充
int info_fld(char *frame, char *Info, int Info_len);	//初始化组帧数据字段 
int fcs_fld(char *frame,char *FCS,int frame_length);	//初始化组帧FCS字段
int bit_stf(char *frame, int frame_length);				//0bit扩充
int flag_fld_end(char *frame, int frame_length);		//初始化组帧帧尾 
void print_frame(char *frame, int frame_length);		//打印组帧

int Frameing(char *Address, char *Control, char *Info, int Info_len, char *FCS, char *frame);		//组帧操作 返回帧长 
// ******************************************************************************************************************

// initfield.h 初始化字段**********************************************************************************
int init_control_fld(char *Control,int control_type); 			//控制字段生成 返回控制字段帧类型编号 
int init_info_fld(char *Info, int info_type,char *Info_temp);	//根据控制字段帧类型编号 填充数据字段 返回数据字段长度 
void init_fcs_fld(char *Info, char *FCS,int info_type);			//根据数据生成CRC校验码 
void bin_div(char *inStr,char *outStr);							//bit除法 
void checkCRC(char *inStr, char *outStr);						//CRC校验码生成 
//*********************************************************************************************************

//链接建立与拆除相关函数************************************************************************* 
int build_SNRM_frame(char *Address);					//建立要发起连接的SARM连接帧  
int build_UA_frame(char *Address);						//建立确认UA连接帧 
int build_DISC_frame(char *Address);					//建立请求断连的DISC帧 
int connect_to_addr(char * Address);					//与目的地址主机连接 	
//void receive(char *frame);							//接收帧	
//**************************************************************************************************************** 

// 数据发送相关函数*********************************************************************************************** 
int send(int flag);										//发送帧 返回发送的帧类型 
int frame_parsing(char *frame, int frame_len, int info_len);				//帧解析函数 调用unframe函数 然后对帧进行解析
void unframe(char *frame, int frame_len, int info_len, char *addr, char *cont, char *info, char *fcs);								//拆帧		
int bit_del(char *frame, int len, char *frame1);					//0bit删除	
// ***************************************************************************************************************
 
//帧结构初始化函数 *********************************************************************************************** 
void  init_frame();   			//清空全局变量Address,Control,Info,Info_temp,Info_len,FCS,frame以便下一帧传输使用 
// *************************************************************************************************************** 

//*帧解析相关函数*************************************************************************************************
int control_parsing(char *recv_cont);				//解析控制字段 返回帧类型 
void info_parsing(int fcs_flag, int con_type, char *recv_info, int info_len);		//解析数据字段  
int fcs_parsing(int con_type, char *recv_info, char *recv_fcs);		//进行CRC校验 成功返回1 否则返回0
void bit_to_info(char *recv_info, int info_len);		//bit串转字符串 
//****************************************************************************************************************

//停等协议流程相关函数********************************************************************************************
void num_to_str(int info_num, char *info_num_str);		//将序号转为二进制存放 
int build_RR_frame(char *Address);						//建立RR确认帧 
int build_SREJ_frame(char *Address);					//建立SREJ请求重发帧  
//****************************************************************************************************************

//全局变量(重复使用 节省空间)*************************************************************************************
char Address[BYTE+1];						//地址字段A
char Control[BYTE+1];						//控制字段C(只考虑单字节)
char Info[MAX_INFO];						//数据字段S 
char Info_temp[MAX_INFO];					//转换成比特串的数据字段辅助数组 
int Info_len;								//数据字段长度 
char FCS[BYTE*2+1];							//帧校验序列字段FCS 
char CRC[BYTE*2+1];							//存放CRC冗余码 在校验时与FCS比较 
char frame[MAX_FRAME];						//存储最终组帧
int Info_num = 0;							//存储发送的数据次数 用来控制N(s) N(r) 
char Info_num_str[BYTE/2];					//将序号转为二进制存放所用数组 
int RR_flag;								//发送RR帧标志
//*****************************************************************************************************************


// frame.h此处函数均为组帧操作内部调用函数************************************************
 
int _2_x(int x){		//2的x次方
	int num = 1;
	for(int i = 0; i < x; i++){
		num *= 2;
	}
	return num;
}


void char_to_bit(char letter, char *bit){		//将单独字母转化为比特串	
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

int info_to_bit(char *Info, int Info_len, char *Info_temp){		//将传输数据转化为比特串 
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

void print_info_bit(char *Info,int Info_len){			//打印数据比特串
	for(int i = 0; i < Info_len; i++){
		printf("%c",Info[i]);
	}
	printf("\n");
}

void flag_fld_header(char *frame){						//初始化组帧帧头 
	frame[0] = '0';
	for(int i = 1; i < 7; i++){
		frame[i] = '1';
	}
	frame[7] = '0';
}

void addr_fld(char *frame, char *Address){				//初始化组帧地址字段 
	int j = 0;
	for(int i = 8; i <= 15; i++){
		frame[i] = Address[j++];
	}
}

void control_fld(char *frame, char *Control){			//初始化组帧控制字段 
	int j = 0;
	for(int i = 16; i <= 23; i++){
		frame[i] = Control[j++];
	}
}


int info_fld(char *frame, char *Info, int Info_len){	//初始化组帧数据字段 
	int frame_length = 24 + Info_len;
	int j = 0;
	for(int i = 24; i < 24 + Info_len; i++){
		frame[i] = Info[j++];
	}
	return frame_length;
}

int fcs_fld(char *frame,char *FCS,int frame_length){	//初始化组帧FCS字段 
	int j = 0;
	for(int i = frame_length; i < frame_length + 16; i++){
		frame[i] = FCS[j++];
	}
	frame_length = frame_length + 16;
	return frame_length;
}

int bit_stf(char *frame, int frame_length){					//0bit扩充 
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

int flag_fld_end(char *frame, int frame_length){		//初始化组帧帧尾 
	frame[frame_length] = '0';
	for(int i = frame_length + 1; i < frame_length + 7; i++){
		frame[i] = '1';
	}
	frame_length = frame_length + 7;
	frame[frame_length++] = '0';
	return frame_length;
}

void print_frame(char *frame, int frame_length){		//打印组帧 
	printf("组帧: \n");
	for(int i = 0; i < frame_length; i++){
		printf("%c",frame[i]);
	}
	printf("\n帧长: %d(bit)\n",frame_length);
}

int Frameing(char *Address, char *Control, char *Info, int Info_len, char *FCS, char *frame){		//组帧操作 
//            address,		 control,	  	information,infobytelength, FCS,		Frame
	int frame_length;							//组帧长度 
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
	return frame_length;
}

//***************************************************************************

// initfield.h 初始化字段**********************************************************************************
int init_control_fld(char *Control,int control_type){						//控制字段生成 返回控制字段帧类型编号 
	switch(control_type){
		case I:			//信息帧 
			Control[0] = '0';
			//默认N(s) N(r)每次+1, P/F bit = 1 
				num_to_str(Info_num, Info_num_str);
				Control[1] = Info_num_str[0];
				Control[2] = Info_num_str[1];
				Control[3] = Info_num_str[2];
//				Control[1] = '0';	//N(s) = Control[1,2,3] 
//				Control[2] = '0';
//				Control[3] = '1';
				
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = Info_num_str[0];	
				Control[6] = Info_num_str[1];
				Control[7] = Info_num_str[2];
//				Control[5] = '0';	//N(r) = Control[5,6,7] 
//				Control[6] = '0';
//				Control[7] = '1';	
				if(Info_num > 7) Info_num = 0;		//若发送的信息帧个数大于7 就从0开始重新编号 
			return I;		//信息帧编号为I 
		case RR:			//监控帧 RR
			Control[0] = '1';
			Control[1] = '0';
			// bit2 bit3 为 00(RR), P/F bit = 1, N(r) = 1
				Control[2] = '0';
				Control[3] = '0';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			 return RR;		//监控帧编号为RR
		case SREJ:			//监控帧 SREJ
			Control[0] = '1';
			Control[1] = '0';
			// bit2 bit3 为 11(SREJ), P/F bit = 1, N(r) = 1
				Control[2] = '1';
				Control[3] = '1';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			 return SREJ;		//监控帧编号为RR
		case 'U':			//无编号帧 
			Control[0] = '1';
			Control[1] = '1';
			//默认置正常响应模式SNRM(M1 = 00, M2 = 001), P/F bit = 1
				Control[2] = '0';	//M1
				Control[3] = '0';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//M2
				Control[6] = '0';
				Control[7] = '1';
			return 3;		//无编号帧编号为3 
		default:
			printf("HDLC帧类型输入有误!\n");
			exit(0);
	}
}


int init_info_fld(char *Info, int info_type, char *Info_temp){								//根据控制字段帧类型编号 填充数据字段
	int info_len;
	switch(info_type){
		case I:			//信息帧 写入信息并转换为比特串 
			printf("要传输的数据:\n");
			gets(Info);
			info_len = strlen(Info);
			printf("转换为比特串:\n");
			info_len = info_to_bit(Info,info_len,Info_temp);
			print_info_bit(Info,info_len);
			return info_len;
		case S:			//监控帧 信息字段不存在
//			Info[0] = '#';			//#表示数据字段不存在 
			IF_INFO_EXIST = 0;		//全局变量 在组帧过程中告知是否存在监控帧
			return 0;
		case U:			//无编号帧 默认信息字段全为0 
			for(int i = 0; i < BYTE; i++){
				Info[i] = '0';
			}
			info_len = BYTE;
			return info_len;
	}
}	

void init_fcs_fld(char *Info,char *FCS,int info_type){			//初始化FCS字段 
	switch(info_type){
		case S:			//若为监控帧 FCS字段置为全0
			for(int i = 0; i < 2*BYTE; i++){
				FCS[i] = '0';
			}
			printf("CRC校验码:\n");
			for(int i = 0; i < 2*BYTE; i++){
				printf("%c",FCS[i]);
			}
			printf("\n");
			break;
		case I:			//若为数据帧或无编号帧，则生成CRC冗余校验码 
		case U:
			checkCRC(Info,FCS);
			printf("CRC校验码:\n");
			for(int i = 0; i < 2*BYTE; i++){
				printf("%c",FCS[i]);
			}
			printf("\n");
			break; 
			
	}
}

void bin_div(char *inStr,char *outStr){				//bit除法 
	int ii = 0;
	int flag = 0;
	int ff = 0;
	int len = 0;
	int maxlen = MAX_INFO;
	int div_n = 0;
	int n = 16;						//多项式长度 
	char p[]="10001000000100001";	//除式（CRC中的多项式） 
	char temp[maxlen];
	char temp2[maxlen];
	for(int i = 0; (inStr[i] == '0' || inStr[i] == '1') && i < maxlen; i++, len++){
		temp[i]=inStr[i];	
	}
	ii=0;
	for(int i = 0; i < len; i++){
		if(temp[i] == '1'){		//被除数的第一个1
			div_n = i;
			break;
		}
	}
	ii=0;
	ff=0;
	if(len-div_n>=16){	//如果被除数的第一个1到被除数的最后一位之间的长度大于等于16（实际数学意义是被除数大于除数）
		//从被除数的第一个1开始与11000000 00000011 做异或运算得到结果存放在临时数组temp[]中
		for(int i = div_n; i < len; i++){
			if(i < div_n + 16){
				if(temp[i] != p[i-div_n]) temp2[i-div_n] = '1';
				else temp2[i-div_n] = '0';
			}
			else temp2[i-div_n] = temp[i];
		} 
		
	}
	else{//被除数的第一个1到被除数的最后一位之间的长度小于等于16（实际数学意义被除数比除数小，直接做为余数）
		for(int i = 0; i < 16; i++){
			temp2[i] = temp[len-16+i];
		}
	}
	//删除零前缀
	for(int i = 0; i < len && (temp2[i] == '1' || temp2[i] == '0'); i++){
		if(ff == 0 && temp2[i] == '1') ff=1;		
		if(ff==1){
			outStr[ii] = temp2[i];
			ii++;
		}
	}		
}

void checkCRC(char *inStr, char *outStr){			//CRC校验码生成 
	int i;
	int maxlen = MAX_INFO;
	int len;
	char temp[MAX_INFO];
	char tempRx[MAX_INFO];

	for(i = 0; (inStr[i] == '0' || inStr[i] == '1') && i < maxlen ; i++){		//为f(x)后面加15个0临时存放在temp[]
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
//****************************************************************************************

//链接建立与拆除相关函数************************************************************************* 

int build_SNRM_frame(char *Address){					//建立连接到Address的连接帧 
	//char linframe[MAX_FRAME];
	char lincontrol[BYTE+1] = "11001001";
	char lininfo[BYTE+1] = "00000000";
	int lininfo_len = BYTE;
	char linfcs[BYTE*2+1] = "0000000000000000";
	strcpy(Control,lincontrol);
	strcpy(Info,lininfo);
	Info_len = lininfo_len;
	strcpy(FCS,linfcs);
	int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
	return frame_len;
} 

int build_UA_frame(char *Address){					//建立确认UA连接帧 
	char linframe[MAX_FRAME];
	char lincontrol[BYTE+1] = "11001110";
	char lininfo[BYTE+1] = "00000000";
	int lininfo_len = BYTE;
	char linfcs[BYTE*2+1] = "0000000000000000";
	strcpy(Control,lincontrol);
	strcpy(Info,lininfo);
	Info_len = lininfo_len;
	strcpy(FCS,linfcs);
	int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
	return frame_len;
} 

int build_DISC_frame(char *Address){					//建立请求断连的DISC帧 
	char linframe[MAX_FRAME];
	char lincontrol[BYTE+1] = "11001010";
	char lininfo[BYTE+1] = "00000000";
	int lininfo_len = BYTE;
	char linfcs[BYTE*2+1] = "0000000000000000";
	strcpy(Control,lincontrol);
	strcpy(Info,lininfo);
	Info_len = lininfo_len;
	strcpy(FCS,linfcs);
	int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
	return frame_len;
}

int connect_to_addr(char *Address){
	if(strlen(Address) != 8){
		printf("找不到此地址(可能原因:输入地址位数错误 应为8位!)\n");
		exit(0);
	} 
	char flag = Address[0];							//解析地址字段flag 查看地址是有效地址还是全0或是广播地址 
	for(int i = 0; i < BYTE; i++){
		if(Address[i] != flag){
			flag = '2';
			break;	
		}
	}
	switch(flag){
		case '0':
			printf("测试数据链路状态成功!\n");
			return 0;
		case '1':
			printf("广播模式!\n");
			return 1;
		case '2':
			printf("正在建立连接!\n");
			return -1;
	}
}

//******************************************************************************************

 
// 数据发送相关函数*************************************************************************
int send(int flag){					//发送帧 
	if(flag == 0){
		int frame_len = build_SNRM_frame(Address);
//		int info_type = init_control_fld(Control,'U');
//		Info_len = init_info_fld(Info,info_type,Info_temp);
//		init_fcs_fld(Info,FCS,info_type);
//		int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
		int con_type = frame_parsing(frame,frame_len,Info_len);
		return con_type;
	}
	else if(flag == 1){
		int frame_len = build_UA_frame(Address);
//		int info_type = init_control_fld(Control,'U');
//		Info_len = init_info_fld(Info,info_type,Info_temp);
//		init_fcs_fld(Info,FCS,info_type);
//		int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
		int con_type = frame_parsing(frame,frame_len,Info_len);
		return con_type;
	}
	else{
		while(1){
			printf("\n继续发送数据(输入1),或与远程地址断开连接(输入2):\n");
			int choice;
			scanf("%d",&choice);
			getchar();
			if(choice == 1){
				int info_type = init_control_fld(Control,I);
				Info_len = init_info_fld(Info,info_type,Info_temp);
				init_fcs_fld(Info,FCS,info_type);
				int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
				printf("发出此帧!\n");
				system("pause");
				printf("进行帧解析:\n");
				int con_type = frame_parsing(frame,frame_len,Info_len);
//				if(con_type == I){		//若发送了数据帧 
//					printf("请校验数据是否一致(一致输入1,不一致输入2),若一致发送RR帧确认,若不一致发送SREJ帧请求重发!\n");
//					int choice2;
//					scanf("%d",&choice2);
//					send_Sframe(choice2);
//				}
				return con_type;
			}
			else if(choice == 2){
				int frame_len = build_DISC_frame(Address);
				int con_type = frame_parsing(frame,frame_len,Info_len);
				return con_type;
			}
			else{
				printf("请输入正确序号!\n");
			}	
		}
	}
}

int frame_parsing(char *frame, int frame_len, int info_len){
	char recv_cont[BYTE+1] = {'\0'};
	char recv_addr[BYTE+1] = {'\0'};				//void unframe(char *frame, int frame_len, int info_len, char *addr, char *cont, char *info, char *fcs)
	char recv_info[MAX_INFO] = {'\0'};
	char recv_fcs[BYTE*2+1] = {'\0'};
	unframe(frame,frame_len,Info_len,recv_addr,recv_cont,recv_info,recv_fcs);
	printf("接收到帧:%s 共%dbit\n",frame,frame_len);
	printf("其中:\n"); 
	printf("地址字段:%s\n",recv_addr);
	printf("控制字段:%s\n",recv_cont);
	printf("数据字段:%s,帧长:%d\n",recv_info,info_len);
	printf("FCS字段:%s\n",recv_fcs);
	int con_type = control_parsing(recv_cont);
	int fcs_flag = fcs_parsing(con_type,recv_info,recv_fcs);
	info_parsing(fcs_flag,con_type,recv_info,info_len);
	return con_type; 
}

void unframe(char *frame, int frame_len, int info_len, char *addr, char *cont, char *info, char *fcs){		//拆帧 
	char frame1[MAX_FRAME];
	int i = 0;
	int j = 0;
	int len = 0;
	for(i = 8; i < frame_len - 8; i++){
		frame1[j++] = frame[i];
		len++;
	}
	char frame2[MAX_FRAME];
	int len2 = bit_del(frame1,len,frame2);
	for(i = 0; i < BYTE; i++){
		addr[i] = frame2[i];
	}
	addr[i] = '\0';
	j = i;
	int k = 0;
	for(; i < j + BYTE; i++){
		cont[k++] = frame2[i];
	}
	cont[k] = '\0';
	j = i;
	k = 0;
	for(; i < j + info_len; i++){
		info[k++] = frame2[i];
	}
	info[k] = '\0';
	j = i;
	k = 0;
	for(; i < j + BYTE * 2; i++){
		fcs[k++] = frame2[i];
	}
	fcs[k] = '\0';
} 

int bit_del(char *frame, int len, char *frame1){		//0bit删除 
	int cnt = 0;
	int j = 0;
	for(int i = 0 ; i < len ; i++){
		if(frame[i] == '1') cnt++;
		else cnt = 0; 
		if(cnt != 5){
			frame1[j] = frame[i];	
			j++;
		}
		else if(cnt == 5){
			frame1[j] = frame[i];	
			j++;
			if(frame[i+1] == '0'){
				i++;
				cnt = 0;
			}
			else if(frame[i+1] == 1 && frame[i+2] == 1){
				printf("数据字段有误!\n");
				exit(0);
			}
		}
	}
	return j;
}

//***************************************************************************

//帧结构初始化函数 ***********************************************************************************************

void init_frame(){
	for(int i = 0; i < BYTE; i++){
//		Address[i] = '\0';
		Control[i] = '\0';
	}
	for(int i = 0; i < BYTE * 2; i++){
		FCS[i] = '\0';
		CRC[i] = '\0'; 
	}
	for(int i = 0; i < MAX_INFO; i++){
		Info[i] = '\0';
		Info_temp[i] = '\0';
	}
	for(int i = 0; i < MAX_FRAME; i++){
		frame[i] = '\0';
	}
	Info_len = 0;
} 

//****************************************************************************************************

//*帧解析相关函数*************************************************************************************************

int control_parsing(char *recv_cont){				//解析控制字段 返回帧类型 
	if(recv_cont[0] == '0'){		//判断为信息帧 
		printf("信息帧编号:%c%c%c\n",recv_cont[1],recv_cont[2],recv_cont[3]);
		return I;
	}
	else if(recv_cont[0] == '1'){		 
//		if(recv_cont[1] == '0') return S;		//判断为监控帧 
		if(recv_cont[1] == '0'){		//判断为监控帧 
			char bit2 = recv_cont[2];
			char bit3 = recv_cont[3];
			if(bit2 == '0' && bit3 == '0'){		//RR帧 
				return RR;
			} 
			else if(bit2 == '1' && bit3 == '1'){		//SREJ帧 
				return SREJ;
			} 
		} 
		else{		//判断为无编号帧 
			char bit3 = recv_cont[2];
			char bit4 = recv_cont[3];
			char bit6 = recv_cont[5];
			char bit7 = recv_cont[6];
			char bit8 = recv_cont[7];
			if(bit3 == '0' && bit4 == '0' && bit6 == '0'
				&& bit7 == '0' && bit8 == '1'){
					return SNRM;
				}
			else if(bit3 == '0' && bit4 == '0' && bit6 == '1'
					&& bit7 == '1' && bit8 == '0'){
						return UA;
					}
			else if(bit3 == '0' && bit4 == '0' && bit6 == '0'
					&& bit7 == '1' && bit8 == '0'){
						return DISC;
					}
		}
	}
	printf("控制帧解析错误!\n");
	exit(0);
}

//void bit_to_info(char *inStr_bit, char *outStr_char){		//bit串转回字符 
//	int i, j;
//	int n = 0;
//	int maxlen = MAX_INFO;
//	char temp[MAX_INFO];
//	char bin[8];
//	char sumchar;
//	for(i = 0; i < maxlen  && (inStr_bit[i] == '0' || inStr_bit[i] == '1'); i++){			//获取输入串长度
//		temp[i]=inStr_bit[i];
//	}
//	n = i;
//	for(i = 0; i < n % 8 && i + n < maxlen; i++){			//给不是8的倍数比特串后面加0
//		temp[i+n] = '0';
//	}
//	for(i = 0; i < maxlen && (temp[i] == '0' || temp[i] == '1'); i = i + 8){			//转换成ASCII码值
//		sumchar = 0;
//		for(j = 0; j < 8; j++){		//将8位比特转换成十进制的ASCII码值
//			sumchar = (temp[i+j] - '0') * _2_x(7 - j);
//			outStr_char[i/8] = outStr_char[i/8] + sumchar;	
//		}
//	}
//}

void info_parsing(int fcs_flag, int con_type, char *recv_info, int info_len){			//解析数据字段 
	if(!fcs_flag){
		printf("CRC校验码校验失败!请重新发送!\n"); 
	}
	else{
		switch(con_type){
			case I:
				printf("正在解析数据字段!\n");
				printf("比特数据为:%s\n",recv_info);
//				for(int i = 0; i < info_len; i++){
//					printf("%c",recv_info[i]);
//				}
//				char str[MAX_STR]; 
				printf("解析出原数据为:");
				bit_to_info(recv_info,info_len);
//				printf("字符串数据为:%s\n",str);
				break;
			case S:
			case RR:
			case SREJ: 
				printf("此帧为监控帧,不具有数据字段!\n");
				break;
			case SNRM:
				printf("此帧为SNRM帧!\n");
				break; 
			case UA:
				printf("此帧为UA帧!\n");
				break;
			case DISC:
				printf("此帧为DISC帧!正在请求与远程地址断开连接!\n"); 
				break;
		}	
	}
}

int fcs_parsing(int con_type, char *recv_info, char *recv_fcs){		//进行CRC校验 成功返回1 否则返回0
	switch(con_type){
		case I:
			printf("正在进行CRC校验!\n");
			checkCRC(recv_info,CRC);
			strcpy(CRC,recv_fcs);
			printf("FCS:%s\n",recv_fcs);
			printf("CRC:%s\n",CRC);
			system("pause");
			if(strcmp(CRC,recv_fcs) == 0){
				printf("CRC校验成功!\n");
				return 1;
			}
			else {
				printf("CRC校验失败!\n");
				return 0;
			}
		case S:
		case RR:
		case SREJ:
		case U:
		case SNRM:
		case DISC:
		case UA:
			return 1;
	}
}

void bit_to_info(char *recv_info, int info_len){			//bit串转字符串 
	char str[BYTE+1];
	int k, sum;
	for(int i = 0; i < info_len; i = i + 8){
		int i1 = 0;
		for(int j = i; j < i + 8; j++){
			str[i1++] = recv_info[j];
		}
		str[i1] = '\0';
    	for(sum = k = 0; str[k] ; (sum *= 2) += str[k++] - '0');
    	printf("%c",sum);
	}
	printf("\n");
}

//************************************************************************************* 

//*停等协议流程相关函数***************************************************************** 
void num_to_str(int info_num, char *info_num_str){
	switch(info_num){
		case 0:
			info_num_str[0] = '0';
			info_num_str[1] = '0';
			info_num_str[2] = '0';
			info_num_str[3] = '\0';
			break;
		case 1:
			info_num_str[0] = '0';
			info_num_str[1] = '0';
			info_num_str[2] = '1';
			info_num_str[3] = '\0';
			break;
		case 2:
			info_num_str[0] = '0';
			info_num_str[1] = '1';
			info_num_str[2] = '0';
			info_num_str[3] = '\0';
			break;
		case 3:
			info_num_str[0] = '0';
			info_num_str[1] = '1';
			info_num_str[2] = '1';
			info_num_str[3] = '\0';
			break;
		case 4:
			info_num_str[0] = '1';
			info_num_str[1] = '0';
			info_num_str[2] = '0';
			info_num_str[3] = '\0';
			break;
		case 5:
			info_num_str[0] = '1';
			info_num_str[1] = '0';
			info_num_str[2] = '1';
			info_num_str[3] = '\0';
			break;
		case 6:
			info_num_str[0] = '1';
			info_num_str[1] = '1';
			info_num_str[2] = '0';
			info_num_str[3] = '\0';
			break;
		case 7:
			info_num_str[0] = '1';
			info_num_str[1] = '1';
			info_num_str[2] = '1';
			info_num_str[3] = '\0';
			break;
	}
} 

int build_RR_frame(char *Address){					//建立RR确认帧 
	//char linframe[MAX_FRAME];
	char lincontrol[BYTE+1] = "10001001";
	char lininfo[BYTE+1] = "00000000";
	int lininfo_len = 0;		//监控帧无信息字段 
	char linfcs[BYTE*2+1] = "0000000000000000";
	strcpy(Control,lincontrol);
	strcpy(Info,lininfo);
	Info_len = lininfo_len;
	strcpy(FCS,linfcs);
	int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
//	RR_flag = 1; 
	return frame_len;
}

int build_SREJ_frame(char *Address){					//建立SREJ请求重发帧 
	//char linframe[MAX_FRAME];
	char lincontrol[BYTE+1] = "10111001";
	char lininfo[BYTE+1] = "00000000";
	int lininfo_len = 0;		//监控帧无信息字段 
	char linfcs[BYTE*2+1] = "0000000000000000";
	strcpy(Control,lincontrol);
	strcpy(Info,lininfo);
	Info_len = lininfo_len;
	strcpy(FCS,linfcs);
	int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
//	RR_flag = 0;
	return frame_len;
}

//********************************************************************************* 

int main(){
	char control_type;
//	printf("输入HDLC帧类型(I or S or U):\n");
//	scanf("%c",&control_type);
//	int info_type = init_control_fld(Control,control_type);
//	Info_len = init_info_fld(Info,info_type,Info_temp);
	printf("要传输的地址(8bit):\n");
	scanf("%s",Address);
	getchar();
	int cta = connect_to_addr(Address); 
	if(cta == -1){
		printf("发送SNRM帧与目标地址建立连接:\n");
//		build_SARM_frame(Address);
//		printf("连接成功！\n");
		int end_flag = 0;			//帧序号 
		int con_type;
		init_frame();
		while(1){
			if(end_flag == 0){
				con_type = send(end_flag++);
				system("pause");
			}
			else if(end_flag == 1){
				printf("收到SNRM帧,即将发送UA帧确认:\n");
				con_type = send(end_flag++);
				printf("与远程地址建立连接成功!\n");
				system("pause");
			}
			else{
				con_type = send(end_flag++);
				if(con_type == I){
					printf("请校验数据是否一致(一致输入1,不一致输入2),若一致发送RR帧确认,若不一致发送SREJ帧请求重发!\n");
					int choice2;
					scanf("%d",&choice2);
					while(1){
						if(choice2 == 1){		//RR帧 
							Info_num++;
							int frame_len = build_RR_frame(Address);
							int con_type = frame_parsing(frame,frame_len,Info_len);
							printf("收到RR帧,数据已成功发送至远程地址!\n");
							break;
						}
						else if(choice2 == 2){
							int frame_len = build_SREJ_frame(Address);
							int con_type = frame_parsing(frame,frame_len,Info_len);
							printf("收到SREJ帧,上条数据发送失败,请重新发送!\n");
							break; 
						}
					}
					 
				}
				if(con_type == DISC){
					printf("收到DISC帧,发出UA帧确认拆连!\n");
					system("pause");
			//		build_UA_frame(frame);
					int frame_len = build_UA_frame(Address);
					int con_type = frame_parsing(frame,frame_len,Info_len);
					if(con_type == UA){
						printf("已断开连接,再会! ^-^\n");
					} 
					break;
				}
			}
			init_frame(); 
//			printf("输入第%d帧的\n",end_flag);
//			printf("HDLC帧类型(I or S or U):");
//			scanf("%c",&control_type);
//			printf("%d",control_type);
//			int info_type = init_control_fld(Control,control_type);
//			Info_len = init_info_fld(Info,info_type,Info_temp);
//			init_fcs_fld(Info,FCS,info_type);
//			int frame_len = Frameing(Address,Control,Info,Info_len,FCS,frame);
//			frame_parsing(frame,frame_len,Info_len);
//			end_flag++;
//			init_frame();
		}		
	}
//	printf("控制字段(8bit):\n");
//	scanf("%s",Control);
//	init_fcs_fld(Info,FCS,info_type);
//	printf("FCS字段(16bit):\n");
//	scanf("%s",FCS);
//	Frameing(Address,Control,Info,Info_len,FCS,frame);
	return 0;
}
 
