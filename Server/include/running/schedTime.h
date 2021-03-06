/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-09 20:44
* Last modified: 2017-06-09 20:44
* Email        : 374648064@qq.com
* Filename     : schedTime.h
* Description  : 关于进程调度(/proc/pid/sched)
******************************************************/

#ifndef __SCHEDTIME_H__
#define __SCHEDTIME_H__

#include <linux/sched.h>
#include "config.h"
#include "common/fileOper.h"
#include "common/strOper.h"
#include "log/logOper.h"

#define getProcSchedInfo(pid, schedInfo) getProcSchedInfoDebug(pid, schedInfo, __FILE__, __FUNCTION__, __LINE__)

long long nsec_high(unsigned long long nsec);

unsigned long nsec_low(unsigned long long nsec);

/**************************************
 * func: 获取一个进程的ProcSchedInfo结构体数据
 * return true = 成功　　　false = 失败
 * @para pid: process id
 * @para schedInfo: 存放SchedInfo
**************************************/
bool getProcSchedInfoDebug(pid_t pid, ProcSchedInfo *schedInfo, const char *file, const char *function, const int line);

/**************************************
 * func: 将两个ProcSchedInfo类型的对象相加
 * return: 返回相加后的结果
 * @para value1: 被加的ProcSchedInfo
 * @para value2: 加的ProcSchedInfo
****************************************/
ProcSchedInfo add(ProcSchedInfo value1, ProcSchedInfo value2);

/**************************************
 * func: 将两个ProcSchedInfo类型的对象相减
 * return: 返回相减后的结果
 * @para value1: 被减数
 * @para value2: 减数
 * example: value1 - value2
****************************************/
ProcSchedInfo sub(ProcSchedInfo value1, ProcSchedInfo value2);

#endif
