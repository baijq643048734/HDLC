/*********************************************
@ author : Bai Jiaqi 白嘉麒 
@ ID : 161720328
@ class: 1617203
@ date :2020/5/11
@ instructor :Teacher Ye Haibo 叶海波老师 
**********************************************/
#ifndef _FRAME_H
#define _FRAME_H

#include<stdio.h>
#include<string.h>
#include<stdlib.h>

//#define I 0				//控制字段信息帧编号 
//#define S 2				//控制字段监控帧编号 
//#define U 3				//控制字段无编号帧编号 

//bool IF_INFO_EXIST = 1;			//全局变量 在组帧过程中告知是否存在监控帧

#define BYTE 8								//1字节 
#define MAX_FRAME 12000 					//数据链路层最大帧长为1500 byte 即 12000bit 
#define MAX_INFO  11952						/*最大帧长 1500 byte - 帧头帧尾 2 byte - 地址字段 1 byte - 控制字段 1 byte - FCS字段 2 byte = 1494 byte 
												即 11952bit */
												
// 此处函数均为组帧操作内部调用函数********************************************************************************												
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

void Frameing(char *Address, char *Control, char *Info, int Info_len, char *FCS, char *frame);		//组帧操作
// ******************************************************************************************************************
/*
int init_control_fld(char *Control,char control_type); 			//控制字段生成 返回控制字段帧类型编号 
int init_info_fld(char *Info, int info_type,char *Info_temp);	//根据控制字段帧类型编号 填充数据字段 返回数据字段长度 
*/				
#endif
