# 计算机通信基础课程设计实验报告



## 实验内容

​	完成了`HDLC`协议最基本的协议内容，包括:

1. 组帧、拆帧并对组帧进行解析
2. 连接的建立与拆除
3. 实用停等协议的模拟演示
4. 在数据传输过程中实现了`CRC`冗余校验来验证正确性

## 实验步骤

### 一、连接的建立与拆除

#### 1. 相关函数

```c
//链接建立与拆除相关函数
int build_SNRM_frame(char *Address);					//建立要发起连接的SARM连接帧  
int build_UA_frame(char *Address);						//建立确认UA连接帧 
int build_DISC_frame(char *Address);					//建立请求断连的DISC帧 
int connect_to_addr(char * Address);					//与目的地址主机连接 	
//void receive(char *frame);							//接收帧	
```

#### 2. 主要实现功能

 1. 由用户输入要连接的地址赋值给`Address`	数组，然后通过调用函数`connect_to_addr`，来判断用户输入地址是否合法，并且判断地址为有效地址、广播地址（11111111）还是链路测试专用地址（00000000）。若用户输入广播地址或链路测试地址，则输出相关信息；若用户输入有效地址，则调用`build_SNRM_frame`函数建立`SNRM`帧，并发送`SNRM`帧并等待接收`UA`帧与远程地址建立连接。远程地址通过`build_UA_frame`函数建立`UA`帧并发送。

    ![1589429053752](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589429053752.png)

    ![1589429162770](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589429162770.png)

    ![1589429213166](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589429213166.png)

    (注：具体组帧操作见下文)

 2. 在与远程地址建立连接之后，用户可以通过输入来选择继续发送数据还是与远程地址断开连接，若用户选择发送数据，则会进行进一步处理(见下文)，若用户选择断连，则会通过调用`build_DISC_frame`函数发送`DISC`帧请求断连并等待远程地址发送UA帧确认断连。

    ![1589430600246](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589430600246.png)

#### 3. 连接、拆除流程图

​	![1589430677387](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589430677387.png)

### 二、组帧、拆帧、帧解析操作

#### 1. 相关函数

```c
// frame.h此处函数均为组帧操作内部调用函数										
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

// initfield.h 初始化字段
int init_control_fld(char *Control,int control_type); 			//控制字段生成 返回控制字段帧类型编号 
int init_info_fld(char *Info, int info_type,char *Info_temp);	//根据控制字段帧类型编号 填充数据字段 返回数据字段长度 
void init_fcs_fld(char *Info, char *FCS,int info_type);			//根据数据生成CRC校验码 
void bin_div(char *inStr,char *outStr);							//bit除法 
void checkCRC(char *inStr, char *outStr);						//CRC校验码生成 

// 数据发送相关函数
int send(int flag);										//发送帧 返回发送的帧类型 
int frame_parsing(char *frame, int frame_len, int info_len);				//帧解析函数 调用unframe函数 然后对帧进行解析
void unframe(char *frame, int frame_len, int info_len, char *addr, char *cont, char *info, char *fcs);								//拆帧		
int bit_del(char *frame, int len, char *frame1);					//0bit删除	

//*帧解析相关函数
int control_parsing(char *recv_cont);				//解析控制字段 返回帧类型 
void info_parsing(int fcs_flag, int con_type, char *recv_info, int info_len);		//解析数据字段  
int fcs_parsing(int con_type, char *recv_info, char *recv_fcs);		//进行CRC校验 成功返回1 否则返回0
void bit_to_info(char *recv_info, int info_len);		//bit串转字符串 

```

#### 2. 主要实现功能

 	在组帧过程中,最主要的函数是`Frameing` 。

​	首先用户建立连接，发出`SNRM`帧以后，进入一个循环，不断调用`send`函数。

​	进入`send`函数后，若用户需要发送数据，则会调用`init_control_fld`函数并传入生成信息帧的标志`I`，来生成相应的控制帧，在函数中，会通过全局变量`Info_num`来告知这是已发送的第几个数据段，并写给控制字段的`N(S)、N(R)`。在控制字段初始化完成后，紧接着便调用`init_info_fld`函数来初始化数据字段，将用户输入的数据转化为比特串赋值给全局变量`Info`，并返回数据字段比特长度，然后调用`init_fcs_fld`函数对用户输入数据生成相应的`CRC`冗余校验码。

​	在这些初始化工作完成后，便调用`Frameing`函数对之前的全局变量进行0比特填充(`bit_stf`)、拼接、加入帧头帧尾，形成一个完整的组帧。至此，组帧操作全部完成。

​	![1589431457942](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589431457942.png)

​	紧接着，在远程地址收到组帧后，调用`frame_parsing`函数，对帧进行解析。

​	在解析函数中，首先调用`unframe`函数，将一个组帧进行拆除帧头帧尾、0比特删除(`bit_del`)等相关操作，将组帧分成地址、控制、数据、`FCS`字段并赋值给局部变量进行相关信息的分析检查。

​	检查过程大概如下:

​	在拆帧以后，首先输出对应的组帧、帧长以及其中的地址字段、控制字段、数据字段、`FCS`字段。然后调用`control_parsing`进行控制字段分析。之所以要先分析控制字段，因为控制字段中包含帧的类型，先判断出帧的类型，才可以对帧中的信息进行相应的解读。函数分析完控制字段后，返回该帧的帧类型，并传入紧接着要调用的`fcs_parsing`函数，若帧类型为数据帧(`I`)，则在函数中会进行相应的`CRC`冗余校验，来验证信息是否正确，并将验证正确与否的结果返回，传入紧接着的数据字段分析函数`info_parsing`，若`CRC`校验成功，则函数先输出数据比特串，然后调用函数`bit_to_info`将数据比特串转换回字符串结构并显示给用户。	![1589432096251](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589432096251.png)

​	至此，帧解析完成，返回所解析帧的类型给`main`函数，在主函数中，通过判断返回的帧的类型进行相应的操作——若返回类型为数据帧(`I`)，则继续进行实用停等协议的流程；而返回类型若是断连无编号帧(`DISC`)，则发送`UA`帧来结束本次连接。

### 三、实用停等协议的实现与模拟

#### 1.相关函数

```c
//停等协议流程相关函数
void num_to_str(int info_num, char *info_num_str);		//将序号转为二进制存放 
int build_RR_frame(char *Address);						//建立RR确认帧 
int build_SREJ_frame(char *Address);					//建立SREJ请求重发帧  
```

#### 2.主要实现功能

​	由上文，在地址接受完信息以后，由用户对比数据，选择数据是否发送成功。若校验数据成功，则发送`RR`帧，告知主机已收到该信息帧，并改变全局变量`Info_num`的值，也就是改变了`N(S)、N(R)`；若校验数据失败，则发送`SREJ`帧告知主机，重新发送编号为`Info_num`的帧，即完成了一个简易的使用停等协议的模拟。

![1589432858496](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589432858496.png)

![1589432944142](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589432944142.png)

### 四、CRC冗余校验

#### 1. 相关函数

```c
void init_fcs_fld(char *Info, char *FCS,int info_type);			//根据数据生成CRC校验码 
void bin_div(char *inStr,char *outStr);							//bit除法 
void checkCRC(char *inStr, char *outStr);						//CRC校验码生成 
```

#### 2. 主要实现功能

​	通过调用`checkCRC`函数，传入数据字段，生成相应的冗余码。在校验时，通过对比组帧的`FCS`字段和收到数据生成的`CRC`冗余码是否一致来判断数据是否有变动。

![1589433348556](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589433348556.png)

## 实验缺陷

### 一、整体情况

​	由于在代码完成时使用的是`Dev-C++`编译器，最开始多文件连编时，总是出现类似于找不到头文件等稀奇古怪的错误，于是最后我只好把各个文件中的代码全部放入`main.cpp`函数中，整体代码量也较大，可能会使老师读源码比较费劲。

​	其次，并没有实现老师所说的开两个线程来进行连接建立、数据传输、连接拆除的操作，而是在一个线程中进行了这些过程的模拟。

### 二、组帧、拆帧、帧解析阶段

​	在组帧阶段中，为了简化问题，默认地址字段为8比特，没有考虑地址首位为0的地址扩充情况；默认控制字段为8比特，并默认控制字段的`P/F bit`位始终为1；默认`FCS`字段为16比特，也就是默认冗余码的生成规则为`CRC-16`。

### 三、连接建立、拆除过程

​	在连接过程中，对于广播模式和测试链路状态模式中，没有进行具体的函数实现，仅仅输出了相关的提示信息，而仅仅对有效地址进行了连接的建立、数据的传输、连接的拆除等操作。

### 四、传输过程

​	在传输过程中。

​	对于无编号帧`U`：完整的`HDLC`协议有以下多种无编号帧。

​	![1589434007276](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589434007276.png)

​	但我只考虑了`SNRM`、`UA`、`DISC`三种无编号帧来实现基本的连接建立、拆除、确认操作。

​	对于监控帧`S`：完整的`HDLC`协议有以下多种无编号帧。

​	![1589433989128](C:\Users\Administrator\AppData\Roaming\Typora\typora-user-images\1589433989128.png)

​	但我只考虑了`RR`、`SREJ`两种监控帧来实现基本的实用停等协议的过程。

## 实验心得

​	毫不夸张的说，计算机通信基础的课程设计，是我上大学有史以来第一次完全靠自己查阅资料、复习老师上课所讲内容和`PPT`，一行代码一行代码逐字逐句敲过来的，在备战考研期间，我腾出了整整一周的时间来完成代码的编写、调试、报告的书写，可以说是鸭梨山大~

​	虽然这只是一门选修课，但是在完成计算机通信基础课程设计的过程中，我切实了解到了计算机底层的通信协议的实现，体会到了老师上课所讲的内容，也在整整1000行的代码历练中提升了自己的代码能力和思考问题、解决问题的水平。

​	最后，感谢老师在疫情期间通过网课的形式对我们的所做的贡献，老师辛苦了！
