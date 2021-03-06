/******************************************************
* Author       : fengzhimin
* Create       : 2018-09-17 01:55
* Last modified: 2018-11-28 00:45
* Email        : 374648064@qq.com
* Filename     : main.c
* Description  : 
******************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>
#include "config.h"
#include "common/confOper.h"
#include "resolution/conflictResolution.h"
#include "common/ioOper.h"
#include "common/procInfo.h"

static char error_info[200];
static char warning_info[200];
static char result_info[200];

static void ExitResourceMonitor()
{
	//删除脚本生产的临时文件
	if(remove("./error.txt") != 0)
	{
		sprintf(warning_info, "remove error.txt failure!(%s)\n", strerror(errno));
		Warning(warning_info);
	}
	if(remove("./tmp.txt") != 0)
	{
		sprintf(warning_info, "remove tmp.txt failure!(%s)\n", strerror(errno));
		Warning(warning_info);
	}
	pthread_mutex_destroy(&showOtherInfo_mutex);
	pthread_mutex_destroy(&conflictProcess_mutex);
	tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
	close(skfd);
	free((void *)nlh);
}

static void printVersion()
{
	system("clear");
	fputs("ResourceMonitor " VERSION " - " COPYRIGHT "\n"
			"Released under the GNU GPL.\n\n",
			stdout);

	char ch = 0;
	bool quit = false;
	while(!quit)
	{
		ch = getch();
		switch(ch)
		{
		case 27:   //ESC
			system("clear");
			quit = true;
			break;
		case 'q':
		case 3:   //Ctrl+C
			ExitResourceMonitor();

			exit(0);
		}
	}
}

static void printHelp()
{
	system("clear");
	fputs("ResourceMonitor " VERSION " - " COPYRIGHT "\n"
			"Released under the GNU GPL.\n\n"
			"-q/Ctrl+C: quit\n"
			"-v: print version info\n"
			"-h/?: print help info\n"
			"ESC: return main menu\n",
			stdout);

	char ch = 0;
	bool quit = false;
	while(!quit)
	{
		ch = getch();
		switch(ch)
		{
		case 27:  //ESC
			system("clear");
			quit = true;
			break;
		case 'q':
		case 3:   //Ctrl+C
			ExitResourceMonitor();

			exit(0);
		}
	}
}

static void showConflictInfo()
{
	time_t timer;
	struct tm *tblock;
	//lock showOtherInfo variable
	pthread_mutex_lock(&showOtherInfo_mutex);
	//lock beginConflictProcess variable
	pthread_mutex_lock(&conflictProcess_mutex);

	if(beginConflictProcess == NULL)
	{
		//不存在竞争信息
		if(!showOtherInfo)
		{
			//clear history info
			system("clear");
			timer = time(NULL);
			tblock = localtime(&timer);
			printf("Local time is: %s\n", asctime(tblock));
			printf("The system is in normal operation!\n");
		}

		conflictCount = 0;   //清空竞争次数
	}
	else
	{
		//存在竞争软件
		if(!showOtherInfo)
		{
			//clear history info
			system("clear");
			timer = time(NULL);
			tblock = localtime(&timer);
			printf("Local time is: %s\n", asctime(tblock));
			printf("The contention time is: %f s\n", (++conflictCount)*SHOW_MESSAGE_RATE*1.0/1000000);

			ConflictProcInfo *currentConflictProcess = beginConflictProcess;
			while(currentConflictProcess != NULL)
			{
				printf("\033[31mconflict type = %2d conflict process = %20s", currentConflictProcess->conflictType, currentConflictProcess->name);
				printf("[");

				if(currentConflictProcess->conflictType & CPU_CONFLICT)
					printf("CPU ");
				if(currentConflictProcess->conflictType & MEM_CONFLICT)
					printf("MEM ");
				if(currentConflictProcess->conflictType & IO_CONFLICT)
					printf("IO ");
				if(currentConflictProcess->conflictType & NET_CONFLICT)
					printf("NET ");
				if(currentConflictProcess->conflictType & PORT_CONFLICT)
					printf("PORT ");
				printf("]\033[0m\n");
				//CPU
				if(currentConflictProcess->conflictType & CPU_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/CPU");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						printf("\033[31mCPU: %s\033[0m\n", name);
					}
				}
				//MEM
				if(currentConflictProcess->conflictType & MEM_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/MEM");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						printf("\033[31mMEM: %s\033[0m\n", name);
					}
				}
				//IO
				if(currentConflictProcess->conflictType & IO_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/IO");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						printf("\033[31mIO: %s\033[0m\n", name);
					}
				}
				//NET
				if(currentConflictProcess->conflictType & NET_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/NET");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						printf("\033[31mNET: %s\033[0m\n", name);
					}
				}
				//PORT
				if(currentConflictProcess->conflictType & PORT_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/PORT");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						printf("\033[31mPORT: %s\033[0m\n", name);
					}
				}

				printf("\033[32m冲突前资源使用: CPU-%d\tMEM-%d\tSWAP-%d\tIO-%lld\tNET-%lld\033[0m\n", currentConflictProcess->normalResUsed.cpuUsed, currentConflictProcess->normalResUsed.memUsed, currentConflictProcess->normalResUsed.swapUsed, currentConflictProcess->normalResUsed.ioDataBytes, currentConflictProcess->normalResUsed.netTotalBytes);

				printf("\033[31m冲突时资源使用: CPU-%d\tMEM-%d\tSWAP-%d\tIO-%lld\tNET-%lld\033[0m\n", currentConflictProcess->conflictResUsed.cpuUsed, currentConflictProcess->conflictResUsed.memUsed, currentConflictProcess->conflictResUsed.swapUsed, currentConflictProcess->conflictResUsed.ioDataBytes, currentConflictProcess->conflictResUsed.netTotalBytes);

				currentConflictProcess = currentConflictProcess->next;
			}
		}
	}

	//unlock beginConflictProcess variable
	pthread_mutex_unlock(&conflictProcess_mutex);
	//unlock showOtherInfo variable
	pthread_mutex_unlock(&showOtherInfo_mutex);
}

void monitorPort()
{
	struct stat statFile;
	char *filePath = PORT_CONTENTION_INFO_PATH;
	//保存/etc/conflictPortInfo.info文件最后读取时间
	long long lastmodification = 0;
	while(1)
	{
		stat(filePath, &statFile);
		//判断/etc/conflictPortInfo.info文件最后更新时间是否与最后读取不一致
		if(lastmodification != statFile.st_mtime)
		{
			lastmodification = statFile.st_mtime;
			int fd = open(filePath, O_RDONLY);
			if(fd >= 0)
			{
				char buf[1024] = {'\0'};
				read(fd, buf, 1024);
				//lock showOtherInfo variable
				pthread_mutex_lock(&showOtherInfo_mutex);
				//clear history info
				system("clear");
				time_t timer;
				struct tm *tblock;
				timer = time(NULL);
				tblock = localtime(&timer);
				printf("Local time is: %s\n", asctime(tblock));
				printf("\033[31m%s\033\[0m\n", buf);
				//显示端口冲突信息1s
				sleep(1);
				//unlock showOtherInfo variable
				pthread_mutex_unlock(&showOtherInfo_mutex);
				//记录端口冲突信息
				Result(buf);
				close(fd);
			}
		}
		usleep(100000);
	}
}

void showInfo()
{
	while(1)
	{
		showConflictInfo();
		usleep(SHOW_MESSAGE_RATE);
	}
}

void increaseConfig()
{
	bool conflictSymbol = false;
	while(1)
	{
		//lock beginConflictProcess variable
		pthread_mutex_lock(&conflictProcess_mutex);
		if(beginConflictProcess != NULL)
		{
			conflictSymbol = true;
		}
		else
		{
			conflictSymbol = false;
		}
		//unlock beginConflictProcess variable
		pthread_mutex_unlock(&conflictProcess_mutex);

		if(!conflictSymbol)
		{
			//increase the value of configuration option
			AutoIncreaseConf();
		}
		usleep(INCREASE_CONFIG_RATE);
	}
}

void monitorResource()
{
    char *data = "request";
    struct sockaddr_nl  local, dest_addr;

    struct _my_msg info;
    int ret;

    skfd = socket(AF_NETLINK, SOCK_RAW, USER_MSG);
    if(skfd == -1)
    {
        sprintf(error_info, "create socket error...%s\n", strerror(errno));
		Error(error_info);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);  //reset terminal setting
		exit(-1);
    }

    memset(&local, 0, sizeof(local));
    local.nl_family = AF_NETLINK;
    local.nl_pid = getpid(); 
    local.nl_groups = 0;
    if(bind(skfd, (struct sockaddr *)&local, sizeof(local)) != 0)
    {
        sprintf(error_info, "bind() error...%s\n", strerror(errno));
		Error(error_info);
        close(skfd);
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);  //reset terminal setting
		exit(-1);
    }

    memset(&dest_addr, 0, sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; // to kernel
    dest_addr.nl_groups = 0;

    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(DATA_SPACE));
    memset(nlh, 0, sizeof(struct nlmsghdr));
    nlh->nlmsg_len = NLMSG_SPACE(DATA_SPACE);
    nlh->nlmsg_flags = 0;
    nlh->nlmsg_type = 0;
    nlh->nlmsg_seq = 0;
    nlh->nlmsg_pid = local.nl_pid; //self port
	time_t timer;
	struct tm *tblock;
    memcpy(NLMSG_DATA(nlh), data, strlen(data));

    while(1)
    {
	    ret = sendto(skfd, nlh, nlh->nlmsg_len, 0, (struct sockaddr *)&dest_addr, sizeof(struct sockaddr_nl));

	    if(!ret)
	    {
			sprintf(error_info, "sendto error...%s\n", strerror(errno));
			Error(error_info);
			ExitResourceMonitor();

			exit(-1);
	    }

		//lock beginConflictProcess variable
		pthread_mutex_lock(&conflictProcess_mutex);
		//释放上一时刻存放的竞争信息
		ConflictProcInfo *currentConflictProcess = beginConflictProcess;
		while(currentConflictProcess != NULL)
		{
			beginConflictProcess = beginConflictProcess->next;
			free(currentConflictProcess);
			currentConflictProcess = beginConflictProcess;
		}

		//接受服务器传来的竞争信息
		while(1)
		{
			memset(&info, 0, sizeof(struct _my_msg));
			socklen_t len = sizeof(dest_addr);
			ret = recvfrom(skfd, &info, sizeof(struct _my_msg), 0, (struct sockaddr *)&dest_addr, &len);
			if(!ret)
			{
				sprintf(error_info, "recv form kernel error...%s\n", strerror(errno));
				Error(error_info);
				ExitResourceMonitor();

				exit(-1);
			}

			//结束标记
			if(info.conflictInfo.conflictType == 0)
			{
				break;
			}
			else
			{
				if(beginConflictProcess == NULL)
					beginConflictProcess = endConflictProcess = malloc(sizeof(ConflictProcInfo));
				else
					endConflictProcess = endConflictProcess->next = malloc(sizeof(ConflictProcInfo));

				memset(endConflictProcess, 0, sizeof(ConflictProcInfo));
				(*endConflictProcess) = info.conflictInfo;
				endConflictProcess->next = NULL;
			}
		}
		//unlock beginConflictProcess variable
		pthread_mutex_unlock(&conflictProcess_mutex);

		if(beginConflictProcess != NULL)
		{
			//竞争消解
			currentConflictProcess = beginConflictProcess;
			while(currentConflictProcess != NULL)
			{
				//CPU
				if(currentConflictProcess->conflictType & CPU_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/CPU");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						if(ReduceConf(currentConflictProcess->name, name))
						{
							if(RecordTunedConfInfo(currentConflictProcess->name, name, "CPU"))
							{
								sprintf(result_info, "Record tuned configuration success(software:%s\tconf:%s\tResourceType:CPU)\n", \
										currentConflictProcess->name, name);
								Result(result_info);
							}
							else
							{
								sprintf(error_info, "Record tuned configuration failure(software:%s\tconf:%s\tResourceType:CPU)\n", \
										currentConflictProcess->name, name);
								Error(error_info);
							}
						}
					}
				}
				//MEM
				if(currentConflictProcess->conflictType & MEM_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/MEM");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						if(ReduceConf(currentConflictProcess->name, name))
						{
							if(RecordTunedConfInfo(currentConflictProcess->name, name, "MEM"))
							{
								sprintf(result_info, "Record tuned configuration success(software:%s\tconf:%s\tResourceType:MEM)\n", \
										currentConflictProcess->name, name);
								Result(result_info);
							}
							else
							{
								sprintf(error_info, "Record tuned configuration failure(software:%s\tconf:%s\tResourceType:MEM)\n", \
										currentConflictProcess->name, name);
								Error(error_info);
							}
						}
					}
				}
				//IO
				if(currentConflictProcess->conflictType & IO_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/IO");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						if(ReduceConf(currentConflictProcess->name, name))
						{
							if(RecordTunedConfInfo(currentConflictProcess->name, name, "IO"))
							{
								sprintf(result_info, "Record tuned configuration success(software:%s\tconf:%s\tResourceType:IO)\n", \
										currentConflictProcess->name, name);
								Result(result_info);
							}
							else
							{
								sprintf(error_info, "Record tuned configuration failure(software:%s\tconf:%s\tResourceType:IO)\n", \
										currentConflictProcess->name, name);
								Error(error_info);
							}
						}
					}
				}
				//NET
				if(currentConflictProcess->conflictType & NET_CONFLICT)
				{
					memset(label, 0, CONFIG_LABEL_MAX_NUM);
					memset(name, 0, CONFIG_VALUE_MAX_NUM);
					strcpy(label, currentConflictProcess->name);
					strcat(label, "/NET");
					if(getConfValueByLabelAndKey(label, "name", name))
					{
						if(ReduceConf(currentConflictProcess->name, name))
						{
							if(RecordTunedConfInfo(currentConflictProcess->name, name, "NET"))
							{
								sprintf(result_info, "Record tuned configuration success(software:%s\tconf:%s\tResourceType:NET)\n", \
										currentConflictProcess->name, name);
								Result(result_info);
							}
							else
							{
								sprintf(error_info, "Record tuned configuration failure(software:%s\tconf:%s\tResourceType:NET)\n", \
										currentConflictProcess->name, name);
								Error(error_info);
							}
						}
					}
				}

				currentConflictProcess = currentConflictProcess->next;
			}
		}


		//设置请求频率
	    usleep(REQUEST_MESSAGE_RATE);
    }
}

int main(int argc, char **argv)
{
	if(getuid() != 0)
	{
		printf("请使用root权限执行\n");
		return -1;
	}

	int pid = getProcIdByName(PROCNAME);
	if(pid == -1)
	{
		sprintf(error_info, "获取进程(%s)id失败!\n", PROCNAME);
		Warning(error_info);
	}
	else if(pid > 0)
	{
		printf("进程%s(%d)已经运行，请先关闭后重新启动!\n", PROCNAME, pid);
		return -1;
	}

	//修改进程名称
	char argv_buf[MAX_ARGV_LENGTH] = {0};  //save argv parameters
	int i;
	for(i = 0; i < argc; i++)
	{
		strcat(argv_buf, argv[i]);
		strcat(argv_buf, " ");
	}

    //修改argv[0]所指向的内存空间的内容
    setproctitle_init(argc, argv, environ);
    
    //调用prctl修改进程名
    setproctitle("%s %s", PROCNAME, argv_buf);

	tcgetattr(STDIN_FILENO, &oldTermios);   //get current terminal setting

	newTermios = oldTermios;
	/*
	 * ~ICANON: don't need to press enter
	 * ~ECHO: prohibit echo
	 * ~ISIG: prohibit to handle the signal
	 */
	newTermios.c_lflag &= ~(ICANON | ECHO | ISIG);
	newTermios.c_cc[VTIME] = 0;
	newTermios.c_cc[VMIN] = 1;
	//cfmakeraw(&newTermios);
	tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);  //set terminal

	if(pthread_mutex_init(&showOtherInfo_mutex, NULL) != 0)
	{
		Error("init showOtherInfo_mutex failure!\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
		return -1;
	}

	if(pthread_mutex_init(&conflictProcess_mutex, NULL) != 0)
	{
		Error("init conflictProcess_mutex failure!\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
		return -1;
	}

	//创建线程用于监控端口冲突信息
	pthread_t pthreadPort_t;
	if(pthread_create(&pthreadPort_t, NULL, (void *)monitorPort, NULL) != 0)
	{
		Error("create monitorPort thread failure!\n");
		return -1;
	}

	//创建线程用于监控共享资源竞争信息并消解冲突
	pthread_t pthreadResource_t;
	if(pthread_create(&pthreadResource_t, NULL, (void *)monitorResource, NULL) != 0)
	{
		Error("create monitorResource thread failure!\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
		return -1;
	}

	//创建线程用于增加配置项
	pthread_t pthreadIncrease_t;
	if(pthread_create(&pthreadIncrease_t, NULL, (void *)increaseConfig, NULL) != 0)
	{
		Error("create increase config thread failure!\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
		return -1;
	}

	//创建线程用于显示竞争信息
	pthread_t pthreadShowConflictInfo_t;
	if(pthread_create(&pthreadShowConflictInfo_t, NULL, (void *)showInfo, NULL) != 0)
	{
		Error("create showInfo thread failure!\n");
		tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);   //reset terminal setting
		return -1;
	}

	bool quit = false;  //whether exit current process or not
	char inputChar = 0;
	while(!quit)
	{
		inputChar = getch();
		switch(inputChar)
		{
		case 'q':
		case 3:      //Ctrl+C
			quit = true;
			break;
		case 'h':
		case '?':
			//lock showOtherInfo variable
			pthread_mutex_lock(&showOtherInfo_mutex);
			showOtherInfo = true;
			//unlock showOtherInfo variable
			pthread_mutex_unlock(&showOtherInfo_mutex);

			printHelp();

			//lock showOtherInfo variable
			pthread_mutex_lock(&showOtherInfo_mutex);
			showOtherInfo = false;
			//unlock showOtherInfo variable
			pthread_mutex_unlock(&showOtherInfo_mutex);
			//显示竞争信息
			showConflictInfo();
			break;
		case 'v':
			//lock showOtherInfo variable
			pthread_mutex_lock(&showOtherInfo_mutex);
			showOtherInfo = true;
			//unlock showOtherInfo variable
			pthread_mutex_unlock(&showOtherInfo_mutex);

			printVersion();

			//lock showOtherInfo variable
			pthread_mutex_lock(&showOtherInfo_mutex);
			showOtherInfo = false;
			//unlock showOtherInfo variable
			pthread_mutex_unlock(&showOtherInfo_mutex);
			//显示竞争信息
			showConflictInfo();
			break;
		}
	}

	ExitResourceMonitor();

    return 0;

}

