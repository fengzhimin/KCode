/******************************************************
* Author       : fengzhimin
* Create       : 2017-05-08 13:11
* Last modified: 2017-05-08 13:11
* Email        : 374648064@qq.com
* Filename     : config.h
* Description  : 
******************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <linux/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>

#define VERSION "1.0.0"

#define COPYRIGHT "(C) 2017-2018 zhimin feng"

#define OPENLOG  1      //0=不记录日志   1=记录日志
#define SHOWINFO   0     //0: 表示不在终端显示日志   1: 表示在终端显示

#define ONLINE_RESOLUTION     //0=不在线资源竞争消解    1=在线资源竞争消解

#define REQUEST_MESSAGE_RATE   100000    //request message rate from server(us)

//log file name
#define ERROR_LOG_FILE    "/var/log/ResourceMonitor/Client/errorInfo.log"
#define WARNING_LOG_FILE  "/var/log/ResourceMonitor/Client/warningInfo.log"
#define RESULT_LOG_FILE   "/var/log/ResourceMonitor/Client/resultInfo.log"
#define GET_CONFIG_VALUE_FILE   "/var/log/ResourceMonitor/Client/configValue.txt"

#define MAX_NAMELENGTH           50    //应用程序名称最大字符长度

//定义存放冲突信息的最大字符个数
#define MAX_CONFLICTINFO      256

#define LINE_CHAR_MAX_NUM      1024   //一行最大字符个数
#define CONFIG_LABEL_MAX_NUM     64
#define CONFIG_KEY_MAX_NUM       64     //配置项key的最大值
#define CONFIG_VALUE_MAX_NUM     1024   //配置项value的最大值
#define MAX_SUBSTR               1024   //拆分后子字符串的最大长度
//定义ResourceMonitor-Client配置文件存放的路径
#define ResourceMonitor_Client_CONFIG_PATH     "/etc/ResourceMonitor/Client/ResourceMonitorClient.conf"
#define ResourceMonitor_Client_CONFIG_NOTESYMBOL    '#'
//save the script of contention solution
#define ResourceMonitor_Client_SOLUTION_PATH   "/etc/ResourceMonitor/Client/Resolution"
#define SCRIPT_PATH_MAX_NUM    128    // the max number of the script path

/*************************************
 * function: 存放关于进程调度的时间数值
*************************************/
typedef struct ProcessSchedInfo
{
	int sum_exec_runtime;   //累计运行的物理时间
	int wait_sum;     //在就绪队列里的等待时间
	int iowait_sum;   //io等待时间
} ProcSchedInfo;

/***********************************
 * function: 记录软件执行时的资源使用情况
 * @para cpuUsed: 软件运行时占用CPU情况
 * @para memUsed: 软件运行时占用MEM情况
 * @para swapUsed: 软件运行时占用交换分区的情况
 * @para ioDataBytes: 软件运行时IO读写情况
 * @para netTotalBytes: 软件运行时NET上传下载情况
***********************************/
typedef struct ProcResourceUtilization
{
	int cpuUsed;
	int memUsed;
	int swapUsed;
	unsigned long long ioDataBytes;
	unsigned long long netTotalBytes;
} ProcResUtilization;

/**********************************
 * function: 存放资源冲突的进程信息
 * name: conflict process name
 * pgid: process group id
 * conflictType: 冲突的类型
 * normalResUsed: 正常运行时资源使用情况
 * conflictResUsed: 冲突时资源使用情况
 * conflictInfo: 存放冲突的信息
 * next: 下一个地址
**********************************/
typedef struct ConflictProcess
{
	char name[MAX_NAMELENGTH];
	char conflictType;
	pid_t pgid;
	ProcResUtilization normalResUsed;
	ProcResUtilization conflictResUsed;
	char conflictInfo[MAX_CONFLICTINFO];
	struct ConflictProcess *next;
} ConflictProcInfo;

//每一位代表一种资源是否冲突   1 = 冲突   0 = 不冲突
#define CPU_CONFLICT    1    //CPU资源冲突(00000001)
#define MEM_CONFLICT    2    //MEM资源冲突(00000010)
#define NET_CONFLICT    4    //NET资源冲突(00000100)
#define IO_CONFLICT     8    //IO资源冲突 (00001000)
#define PORT_CONFLICT   16   //端口资源冲突(00010000)

#endif
