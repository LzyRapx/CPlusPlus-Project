## 利用可变参数模拟实现`printf`
- 利用`stdarg`宏来解决可变参数问题
- 头文件：`<stdarg.h>`
### 函数:
- `va_list`;
- `type va_arg( va_list arg_ptr, type );`
- `void va_end( va_list arg_ptr );`
- `void va_start( va_list arg_ptr, prev_param );   (ANSI version)`
