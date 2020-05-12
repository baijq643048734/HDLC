/*********************************************
@ author : Bai Jiaqi 白嘉麒 
@ ID : 161720328
@ class: 1617203
@ date : 2020/5/11
@ instructor :Teacher Ye Haibo 叶海波老师 
**********************************************/

#include "frame.h"
#include "initfield.h"

										
char Address[BYTE+1];						//地址字段A
char Control[BYTE+1];						//控制字段C(只考虑单字节)
char Info[MAX_INFO];				
		//数据字段S 
char Info_temp[MAX_INFO];					//转换成比特串的数据字段辅助数组 
int Info_len;								//数据字段长度 
char FCS[BYTE * 2 + 1];						//帧校验序列字段FCS 
char frame[MAX_FRAME];						//存储最终组帧

int main(){
	char control_type;
	printf("输入要HDLC帧类型(I or S or U):\n");
	scanf("%c",&control_type);
	int info_type = init_control_fld(Control,control_type);
	Info_len = init_info_fld(Info,info_type,Info_temp);
	printf("要传输的地址(8bit):\n");
	scanf("%s",Address);
//	printf("控制字段(8bit):\n");
//	scanf("%s",Control);
	printf("FCS字段(16bit):\n");
	scanf("%s",FCS);
	Frameing(Address,Control,Info,Info_len,FCS,frame);
	return 0;
}
 
