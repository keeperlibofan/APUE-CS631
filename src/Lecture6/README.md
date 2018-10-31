## 第七章：进程环境

#### atexit 函数
一个进程可以登记多达32个函数，这些函数由exit自动调用，并将这些程序称为*终止处理程序*。
```c
#include <stdlib.h>

int atexit(void (*func)(void)); // 传入一个函数地址
```
exit 调用这些函数的顺序与它们被登记时候的顺序相反。同一个函数如果登记多次也会被调用多次
sysconf函数可以确定给定平台的进程最大终止处理函数。
exit首先调用各自的*终止处理函数*，然后关闭(fclose)打开的流。

C程序分为以下几种调用函数堆栈
+ 用户函数  
+ main函数
+ C启动例程
上面三种调用`exit`后都不返回，调用exit后会接连调用登记好的`终止处理函数`，最后还会调用
标准IO的清理函数，然后再调用_exit或_Exit回退到内核。进程也可以非自愿的由一个信号使其终止。

#### C 语言中的静态函数
与普通函数的区别就在于static修饰的函数只能在本文件内使用。

#### 命令行参数
当执行一个程序时，调用exec的进程可以将命令行的参数传递给新的程序。
ISO C要求argv[argc]是一个空指针，所以我们在遍历argv的时候可以写成`argv[argc] != NULL`
#### 环境表
环境表是一个字符指针数组，其中每个指针包含一个以NULL结束的C字符串地址。
全局变量environ则包含了该指针数组的地址。
`extern char **environ`
每个字符串结尾处都显示的指向一个NULL的指针。
环境由`name = value`这样的字符串组成。
历史上的UNIX系统大多支持函数带三个参数，其中第三个就是环境表地址。

`int main(nt argc, char *argv[], char *envp[]);`

但是访问环境变量的话，我们通常还是会选择调用`getenv`和`putenv`,而不是environ,但是如果要访问整个
环境则必须使用environ。

#### C程序存储空间布局
c程序组成部分
+ 正文段，正文端常常是只读的，以防止程序意外而修改其指令。
+ 初始化函数端。如任何函数外定义的变量。
+ 未初始化数据段。在程序开始执行之前，内核将此段中的数据初始化为0或者空指针。比方说在所有函数
外的声明`long sum[100];`
+ stack,自动变量以及每次函数多用所保留的信息都放在此段中。返回地址和调用者的环境信息。然后最近被调用的函数
在栈上为其分配临时空间。
+ 堆。通常在堆中进行动态存储分配。


c语言进程的存储空间安排是从高位地址到低位地址安排如下
`命令行参数和环境变量`，`栈（从高位地址向地位地址延续）`，`堆（从低位地址向高位地址延续）`,`未初始化的数据`，`初始化的数据`，`程序正文`。

a.out中还包含若干其他类型的段，如包含符号表的段，包含符号信息以及包含动态共享库链接表的段。

需要被放在磁盘程序文件中的段是正文段和初始化的段，其他的组成部分都是在加载到内存后才生成的，未初始的段也是在加载到内存后由内核将他们都设置为0;

##### 函数size函数
size(1) 会报告程序的正文段，初始化数据段和未初始化数据段的长度。
`$ size a.out`
```
__TEXT  __DATA  __OBJC  others  dec     hex
4096    4096    0       4294971392      4294979584      100003000   
```
`dec`和`hex`列是用于表示分别是10进制和16进制表示三段总长度。

#### 共享库
共享库使得可执行的文件不再需要包含公用库函数，而只需在进程`都可以引用的存储区`中保存这种库例程的副本就行。
程序第一次执行或者第一次调用某个库函数时，用`动态链接的方法`将程序与共享库函数相互链接。

优点：
+ 减少了可执行文件的长度
+ 可以用库函数的新版本代替老版本而无需对该库的程序重新连接编辑。

缺点
+ 增加了一些运行的开销，这种开销发生在程序第一次被执行的时候，或每个共享库函数第一次被调用的时候。 

对程序使用不同的方法说明是否使用共享库
`ld`命令 -- linker
`cc`命令

1. 先用无共享库的方式创建下列可执行文件（典型的hello.c程序）
```sh
$ gcc -static hello1.c // 阻止 gcc 使用静态链接库,但是 mac 好像不支持 编译不通过
$ gcc hello1.c      // gcc 默认使用共享库
```

#### 存储空间分配

三个用于存储空间动态分配的函数
+ malloc,分配指定字节数的存储区，*此存储区的初始值不确定*。
+ calloc,分配指定数量指定长度的对象分配存储空间。*而空间中的每一位都被初始化为0*。
+ realloc,增加或者减少以前分配区的长度。新增加的区域内初始值不确定。

三个函数返回的指针一定要是适当对其的，可以用于任何的对象。例如在一个特定的系统上,如果最严格
的对齐要求是，double必须在8的倍数的地址单元开始，那么上面三个函数都应该这样对齐。
因为三个函数返回的指针都是void类型，所以我们不需要显示的进行强制类型转化。

free函数释放ptr指向的存储空间。被释放的空间常常被送入可用的存储区池。

调用realloc后，如果原来的存储区没有足够大的空间，将现存的元素数组复制到新分配的存储区，然后释放原有的存储空间。
若ptr是一个空指针，那么，realloc的功能和malloc的功能一样。

这些分配例程通常是通过sbrk(zconf.h)系统调用实现。这一个系统调用扩充或缩小进程的堆。

malloc和free的实现一般不会缩小进程的存储空间，释放的空间可以供后续再分配，所以，函数会将它们保存在
malloc池中而不会返回给内核。

大多数的实现所分配的存储空间会比要求的大一些，这是因为需要一些额外的空间来记录管理信息，比方说：`分配的块的长度`，
`指向下一个分配块的指针`等等。

其他的致命错误包括
+ 释放了一个已经释放的块，调用free时所产用的指针不是3个alloc函数返回的指针。
+ 内存泄漏，如果一个进程调用了alloc函数，却忘记用free来释放它，那么这个进程占用的空间就会
缓慢的增加，直到不再有空闲空间，而且可能会由于过度的换页开销而造成性能下降。

#### 替代的存储空间分配程序
libmalloc,vmalloc,quick-fit,jemalloc,TCMalloc,函数alloca

#### 环境变量
环境字符串的形式是：
```c
name=value
```

内核不会查看这些字符串，解释完全是由各个程序来决定。例如shell中某些登录时自动设置的(HOME和USER等)，

很多时候需要设置环境变量，改变现有的环境变量，新增新的环境变量。能影响到的知识当前进程以及其后生成和调用的任何子进程环境，
但不能影响父进程环境。
macOS X
+ getenv
+ putenv: 取值形式为 *name=value* 的字符串，如果*name*已经定义了，那么删除其原来的定义。这个函数自由的将传递给它的字符串直接放入到环境中。所以不能
将存放在栈中的字符串作为参数传递给它，这样会报错，原因是当前函数返回的时候，栈帧会被重用。
```c
int putenv(char *str);
```
+ setenv： 如果rewrte位非0覆盖现有的定义，如果rewrite为零，如果已经定义，不删除现有的定义。这个函数会分配空间。
+ unsetenv：

*环境表* 是指向实际*name=value*的指针数组，和环境字符串通常放在进程存储空间的顶部。因为环境表是数组实现
所以删除很简单，只要将需要删除的指针的后续指针向前移动一个就行。修改的话就会复杂一些。
但是修改就会很麻烦，这是因为环境表和环境字符串是在进程的顶部，所以不能向上拓展，也不能向下拓展，因为下面就是栈。
 + 修改现有的*name*:
    + 如果新的字符串长度小于或者等于现有的value长度，只要将新的字符串复制到原来字符串上所用的空间就行
    + 如果新的value的长度大于原来的长度，则必须调用malloc在堆中分配新的空间，然后将字符串复制到该空间中，使环境表针对name的指针指向新的分配区。
 + 增加一个新的name，首先就要调用malloc函数为*name=value*分配空间，然后复制到空间中。
    + 新的name,环境表也要被分配到新的分配区间，然后将新的*name=value*字符串指针添加到表尾，并且还加上一个空指针，然后使environ指向新的环境表，所以最终表被放在了堆中，但是
    表中的指针还是会有的指向栈顶上的各个`value=name`字符串。
    + 如果又多加了一个环境变量，那就用realloc函数，重新分配只多一个char指针的空间，同样表的结尾要指向一个空指针。
```c
#include <string.h>

int mysetenv(const char*name, const char*value, int rewrite) {
    char* oldvalue;
    char* newPtr;
    if ((oldvalue = getenv(name)) == NULL) {

    } else {
        if (strlen(oldvalue) >= strlen(value)) {
            strcpy(value, oldvalue);
        } else { //value 长度大于原长度
            newPtr = (char*)malloc(sizeof(char)*(strlen(name)+strlen(value)+2));
            strcpy(newPtr, name);
            strncat(newPtr, '=', 1);
            strncat(newPtr, value, strlen(value));
            // 然后针对name的指针指向新分配的区,添加到环境表中
            
        }
    }
}

int main() {

    getenv();
}
```

#### 函数setjmp和longjmp
goto函数是不能跨越函数的，longjmp和setjmp在处理发生在很深的嵌套层的函数调用中的出错情况是很有用的。

```cpp
//进行命令处理程序的典型骨架部分

#include "apue/apue.h"

void do_line(char *); // do_line 处理该输入行
void cmd_add(void);   // switch 实现了命令选择。
int get_token(void); // 从该输入行中获取下一个token

#define TOK_ADD 5

int
main() {
    char line[MAXLINE];
    // 从标准输入中读一行
    while (fgets(line, MAXLINE, stdin) != NULL) {
        do_line(line);
    }
}

char *tok_ptr;

void do_line(char * ptr) { // 处理一行的输入
    int cmd;
    
    tok_ptr = ptr;
    while ((cmd = get_token()) > 0) {
        switch (cmd) {
            case (TOK_ADD): 
                cmd_add();
        }
    }
    
}

void cmd_add(void) {
    int token;
    
    token = get_token(); // 为了获取下一个token
    // 这个函数的剩余处理
    
}

int get_token(void) {
    // 没有实现，获取下一个token
}
```

当调用到cmd函数的时候，程序的栈型是
 `main的栈` -> `do_line的栈` -> `cmd_add的栈`
如果出现了错误，我们只能一层一层的返回，那就会很麻烦。
所以我们需要setjmp和longjmp跳过若干个调用栈，返回到当前函数调用路径的某个函数上。
在希望返回到的位置调用`setjmp`,直接调用该函数返回值就是0。
+ jmp_buf 是某种类型的数组，所以在存放在调用longjmp时用来恢复栈的所有信息。
在
+ 一个setjmp可以对应多个longjmp

### 自动变量，寄存器变量和易失变量
问题：当longjmp返回到main函数时，这些变量的值能否恢复到以前调用setjmp时候的值呢。
答案：大多数的实现不回滚自动变量和寄存器变量的值。

自动变量就是当你longjmp到的函数栈中的内部变量，如果你有一个自动变量，不想使其回滚，那么需要定义为volatile属性。
全局变量和静态变量在longjmp后不会改变。

```cpp
//进行命令处理程序的典型骨架部分

#include <setjmp.h>
#include "apue/apue.h"

void do_line(char *); // do_line 处理该输入行
void cmd_add(void);   // switch 实现了命令选择。
int get_token(void); // 从该输入行中获取下一个token

jmp_buf jmpbuffer; // 存放恢复栈状态的所有信息
#define TOK_ADD 5

int
main() {
    char line[MAXLINE];
    // 从标准输入中读一行
    if (setjmp(jmpbuffer) != 0) { // 可以对应多个longjmp
        printf("error");
    }
    while (fgets(line, MAXLINE, stdin) != NULL) {
        do_line(line);
    }
    
}

char *tok_ptr;

void do_line(char * ptr) { // 处理一行的输入
    int cmd;

    tok_ptr = ptr;
    while ((cmd = get_token()) > 0) {
        switch (cmd) {
            case (TOK_ADD):
                cmd_add();
        }
    }

}

void cmd_add(void) {
    int token;

    token = get_token(); // 为了获取下一个token
    if (token < 0) {
        longjmp(jmpbuffer, 1); // 调用后会抛弃main 函数栈的下面两个栈
    }        
    // 这个函数的剩余处理
    
}

int get_token(void) {
    // 没有实现，获取下一个token
}
```

longjmp在程序编译优化之后（即为添加`-O`参数之后），jmp_buf类型的全局变量才会
复原自动变量和寄存器变量。
如果程序编译时进行了优化，那么自动变量和寄存器变量才会被放置在寄存器中，如果没有，这两个变量还是
会被安排在存储器中。

#### 自动变量的潜在问题
观察一下这段代码
```cpp
#include <stdio.h>

FILE *
open_data(void) {
    FILE *fp;
    
    char data[BUFSIZ]; // 自动变量 setvbuf让栈中的这一段成为了标准流的缓冲区
    
    if ((fp = fopen("datafile", "r")) == NULL) {
        return(NULL);
    } 
    if (setvbuf(fp, data, _IOLBF, BUFSIZ) != 0) {
        return(NULL);
    } 
    return(fp);
}
```

然而函数返回后，这一段会被下一个被调用的函数的栈帧使用，这时就构成了冲突。为了解决这一问题，应该静态的(static 或 extern 在静态区)
或者动态的(使用alloc函数在动态区)为数组data分配空间。

#### getrlimit 和 setrlimit

用于查询进程的资源限制。

```cpp
struct rlimit {
    rlim_t   rlim_cur;   // 软限制，当前的最大值
    rlim_t   rlim_max;   // 硬限制，rlim的最大限制
}
```




