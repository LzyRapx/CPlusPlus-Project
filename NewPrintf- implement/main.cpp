/**
 * @Author: LzyRapx
 * @Date:   2018-06-24 13:10
 * @Email:  LzyRapx@gmail.com
 * @Last modified by:   LzyRapx
 * @Last modified time: 2018-06-25 16:44
 * @Copyright: (C) 2018 LzyRapx
 */

// 利用可变参数模拟实现 printf

#include <iostream>
#include <stdio.h>
#include <stdarg.h>
using namespace std;

int new_printf(char * str, ...)
{
  va_list arg;
  int cnt= 0;
  char * str_tmp = NULL;
  va_start(arg,str); //初始化
  while(*str != '\0')
  {
    switch (*str) {
      case 'c':
       // 打印下一个参数的字符
        putchar(va_arg(arg,int));
        cnt++;
        break;
      case 's':
       // 取下一个参数的地址
        str_tmp = (char*)va_arg(arg,int);
        while(*str_tmp != '\0') // 利用解引用进行输出
        {
          putchar(*str_tmp);
          cnt++;
          str_tmp++;
        }
        break;
      default: //如果不是'c'或's'，就直接将它打印
        putchar(*str);
        cnt++;
        break;
    }
    str++;
  }
  //arg指向空，防止野指针
  va_end(arg);
  return cnt;
}
int main(int argc, char const *argv[]) {
  new_printf("s \nc c c\n","how cruel the world !", 'l', 'z', 'y');
  return 0;
}
