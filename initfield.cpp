/*********************************************
@ author : Bai Jiaqi 白嘉麒 
@ ID : 161720328
@ class: 1617203
@ date : 2020/5/11
@ instructor :Teacher Ye Haibo 叶海波老师 
**********************************************/
#include "initfield.h"
 
int init_control_fld(char *Control,char control_type){						//控制字段生成 返回控制字段帧类型编号 
	switch(control_type){
		case 'I':			//信息帧 
		case 'i':
			Control[0] = '0';
			//默认N(s) = 1, N(r) = 1, P/F bit = 1 
				Control[1] = '0';	//N(s) = Control[1,2,3] 
				Control[2] = '0';
				Control[3] = '1';
				
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			return 0;		//信息帧编号为0 
		case 'S':			//监控帧 
		case 's':
			Control[0] = '1';
			Control[1] = '0';
			//默认 bit2 bit3 为 00(RR), P/F bit = 1, N(r) = 1
				Control[2] = '0';
				Control[3] = '0';
					
				Control[4] = '1'; 	//P/F bit = Control[4]
				
				Control[5] = '0';	//N(r) = Control[5,6,7] 
				Control[6] = '0';
				Control[7] = '1';	
			 return 2;		//监控帧编号为2 
		case 'U':			//无编号帧 
		case 'u':
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
		case 0:			//信息帧 写入信息并转换为比特串 
			printf("要传输的数据:\n");
			scanf("%s",Info);
			info_len = strlen(Info);
			printf("转换为比特串:\n");
			info_len = info_to_bit(Info,info_len,Info_temp);
			print_info_bit(Info,info_len);
			return info_len;
		case 2:			//监控帧 信息字段不存在
			Info[0] = '#';			//#表示数据字段不存在 
//			IF_INFO_EXIST = 0;		//全局变量 在组帧过程中告知是否存在监控帧
			return 0;
		case 3:			//无编号帧 默认信息字段全为0 
			for(int i = 0; i < BYTE; i++){
				Info[i] = '0';
			}
			info_len = BYTE;
			return info_len;
	}
}	
