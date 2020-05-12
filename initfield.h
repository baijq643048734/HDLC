/*********************************************
@ author : Bai Jiaqi 白嘉麒 
@ ID : 161720328
@ class: 1617203
@ date : 2020/5/11
@ instructor :Teacher Ye Haibo 叶海波老师 
**********************************************/

#ifndef _INITFIELD_H
#define _INITFIELD_H

#include "frame.h"
 

//#define I 0				//控制字段信息帧编号 
//#define S 2				//控制字段监控帧编号 
//#define U 3				//控制字段无编号帧编号 

//bool IF_INFO_EXIST = 1;			//全局变量 在组帧过程中告知是否存在监控帧

int init_control_fld(char *Control,char control_type); 			//控制字段生成 返回控制字段帧类型编号 
int init_info_fld(char *Info, int info_type,char *Info_temp);	//根据控制字段帧类型编号 填充数据字段 返回数据字段长度 

#endif
