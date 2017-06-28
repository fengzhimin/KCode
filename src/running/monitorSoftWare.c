/******************************************************
* Author       : fengzhimin
* Create       : 2017-06-11 14:32
* Last modified: 2017-06-20 21:40
* Email        : 374648064@qq.com
* Filename     : monitorSoftWare.c
* Description  : 
******************************************************/

#include "running/monitorSoftWare.h"

static char cmdline[MAX_PROCPATH];

void clearMonitorAPPName()
{
	currentMonitorAPPName = beginMonitorAPPName;
	while(currentMonitorAPPName != NULL)
	{
		beginMonitorAPPName = beginMonitorAPPName->next;
		vfree(currentMonitorAPPName);
		currentMonitorAPPName = beginMonitorAPPName;
	}

	endMonitorAPPName = NULL;
	//clear MonitorAPPNameNum 
	MonitorAPPNameNum = 0;
}

bool insertMonitorAPPName(MonitorAPPName obj)
{
	bool insertPoint = true;
	currentMonitorAPPName = beginMonitorAPPName;
	while(currentMonitorAPPName != NULL)
	{
		if(strcasecmp(obj.name, currentMonitorAPPName->name) == 0)
		{
			insertPoint = false;
			break;
		}

		currentMonitorAPPName = currentMonitorAPPName->next;
	}
	if(insertPoint)
	{
		//insert new MonitorAPPName obj into list
		endMonitorAPPName = endMonitorAPPName->next = vmalloc(sizeof(MonitorAPPName));
		*endMonitorAPPName = obj;
	}

	return insertPoint;
}

void getAllMonitorAPPName()
{
	//clear MonitorAPPNameNum 
	MonitorAPPNameNum = 0;
	MonitorAPPName temp;
	memset(&temp, 0, sizeof(MonitorAPPName));
	//associate list header and tail
	beginMonitorAPPName = endMonitorAPPName = currentMonitorAPPName = vmalloc(sizeof(MonitorAPPName));
	memset(currentMonitorAPPName, 0, sizeof(MonitorAPPName));
	struct task_struct *task, *p;
	struct list_head *ps;
	task = &init_task;
	list_for_each(ps, &task->tasks)
	{
		p = list_entry(ps, struct task_struct, tasks);
		memset(cmdline, 0, MAX_PROCPATH);
		sprintf(cmdline, "/proc/%d/cmdline", p->pid);
		//By judge process /proc/pid/cmdline whether space to determine whether the program is user level or kernel level programm
		if(IsEmpty(cmdline) == 1)
		{
			memset(&temp, 0, sizeof(MonitorAPPName));
			strcpy(temp.name, p->comm);
			if(insertMonitorAPPName(temp))
				MonitorAPPNameNum++;
		}
	}

	//in order to improve efficiency
	currentMonitorAPPName = beginMonitorAPPName;
	beginMonitorAPPName = beginMonitorAPPName->next;
	vfree(currentMonitorAPPName);
}