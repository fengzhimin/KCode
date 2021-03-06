/******************************************************
* Author       : fengzhimin
* Create       : 2016-11-09 11:40
* Last modified: 2017-05-02 18:06
* Email        : 374648064@qq.com
* Filename     : strOper.h
* Description  : 字符串处理函数
******************************************************/

#ifndef __STROPER_H__
#define __STROPER_H__

#include <linux/string.h>
#include <linux/slab.h>
#include "config.h"


#define cutStrByLabel(str, ch, subStr, subStrLength)   \
	cutStrByLabelDebug(str, ch, subStr, subStrLength, __FILE__, __FUNCTION__, __LINE__);
/**************************
 * function: 判断一个字符串是否为数字
 * return: true = 是数字　false = 不是一个数字
 * @para _ch: 要判断的字符串
**************************/
bool isNum(char *_ch);

/**********************************
 * function: 提取字符串中的数字(只提取正整数)
 * return: -1 = 没有数字  >=0 = 返回提取的数字
 * @para _str: 要被提取的字符串
***********************************/
unsigned long long ExtractNumFromStr(char *_str);

/**********************************
 * func: 通过字符ch来拆分字符串
 * return: 1 = 不可拆分   拆分后子字符串的个数 > 1
 * @para str: 要被拆分的字符串
 * @para ch: 分隔字符
 * @para subStr: 存放拆分后的子字符串的数组
 * @para subStrLength: subStr的大小
 * example: str = str1:str2:str3  --->  ch = ':'时 subStr = str1 str2 str3
***********************************/
int cutStrByLabelDebug(char *str, char ch, char subStr[][MAX_SUBSTR], int subStrLength, const char *file, const char *function, const int line);

/**********************************
 * func: 去掉字符串中开头的所有的空格和制表符
 * return: void
 * @para str: 要被处理的字符串数组
 * example: str = "  str"    ---->    str = "str"
***********************************/
void removeBeginSpace(char *str);

/**********************************
 * func: 移除字符串中所有的指定字符
 * return: void
 * @para str: 处理的字符串数组
 * @para ch: 要被移除的字符
 * example: str = "1 2 3" ch = ' '  ------>  str = "123"
***********************************/
void removeChar(char *str, char ch);

/**********************************
 * func: 移除字符串中所有的空白符(空格+TAB)
 * return: void
 * @para str: 处理的字符串数组
 * example: str = "1 2	3" ch = ' '  ------>  str = "123"
***********************************/
void removeSpace(char *str);

/**********************************
 * func: 获取字符串中子字符串的个数
 * return: int
 * @para str: 原字符串
 * @para substr: 要查找的字符串
 * example: str = "str123str23" substr = "str"  -----> return = 2
***********************************/
int GetSubStrNum(char *str, char *substr);

/**********************************
 * func: 将整数转化为字符串
 * return: 存放最后一个字符的下一个字符的地址
 * @para str: 存放转化后的字符串
 * @para num: 整数
 * example: num = 12 -------> str = "12"
***********************************/
char* IntToStr(char *str, int num);

/**********************************
 * func: 将Long long转化为字符串
 * return: 存放最后一个字符的下一个字符的地址
 * @para str: 存放转化后的字符串
 * @para num: long long
 * example: num = 12 -------> str = "12"
***********************************/
char* LongToStr(char *str, long long num);

/**********************************
 * func: 将字符串整数转化为int整数(正数)
 * return: 转化后的int
 * @para str: 存放转化前的字符串
 * example: str = "12" -------> return = 12
***********************************/
int StrToInt(char *str);

/**********************************
 * func: 将字符串浮点数转化为int整数(正数)
 * return: 转化后的int
 * @para strFloat: 存放转化前的字符串
 * example: strFloat = "11.22" ------> return = 11
***********************************/
int StrFloatToInt(char *strFloat);

/**********************************
 * func: 将十进制数转化为十六进制
 * return: 最后一位数的下一位字符地址(用于递归函数)
 * @para str: 存放转化后的十六进制数
 * @para num: 需要转化的十进制数
**********************************/
char *Dec2Hex(char *ch, int num);

#endif
