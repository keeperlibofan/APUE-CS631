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
+ 未初始化数据段(bss)。在程序开始执行之前，内核将此段中的数据初始化为0或者空指针。比方说在所有函数外的声明`long sum[100];`，初始化由函数exec来实现
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

##### voltile 变量--- 易失变量 介绍


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

更改资源限制
+ 任何一个进程可以更改到小于或者等于其硬限制。
+ 任何进程可以降低硬限制，但是必须要大于软限制值
+ 超级用户才可以提高硬限制。

资源限制影响到调用进程并由子进程继承，这意味着，为了影响用户所有后续进程，需要将资源限制
设置构造在shell之中。
所以shell就有内置的ulimit命令。

#### 小结
虽然参数表和环境表不是由内核解释，但是内核起到了从exec的调用者将这两者传递给新进程的作用。


### 第八章：进程控制
pid为0的进程就是调度进程，常常被称为*交换进程*，进程id为1的就是init进程，在自举之后启动一个UNIX系统
在一些UNIX的虚拟存储器实现中，进程ID2是页守护进程，此进程负责支持虚拟存储器系统的分页操作。

+ getpid() 返回调用进程的进程id
+ getppid() 返回调用进程的父进程的id
+ geteuid(),geteuid(),getgid(),getegid()

#### fork函数

子进程获得父进程的数据空间，堆栈的副本，父进程和子进程不会共享这些存储空间部分。现在很多实现并不执行一个父进程的`数据段，堆栈`的完全副本
。作为代替，使用了*写时复制*，当父进程的任何一个试图修改这些区域时，内核只为修改区域的那块内存做一个副本，通常是虚拟存储系统的一"页"。
fork后的子进程连流的缓冲区都会有一份。父进程打开的所有文件描述符都被复制到子进程中，我们这里说的复制是因为对每个文件描述符来说，就好像执行了dup函数一样。

fork之后处理文件描述符通常有一下两种情况比较常见
+ 父进程等待子进程完成。由于父子进程共享同样的文件表，而文件表中指定了当前偏移量，所以子进程曾经做过的读写操作在父进程运行时已经更新了。
+ 父子进程各自执行不同的程序段。各自关闭它们不需要使用的文件描述符。

父进程的
+ real uid，read gid, effective uid, effective gid
+ 附属组id
+ 进程组id
+ ......

也被继承给了子进程

但是
+ 


进程fork失败的原因
+ 系统太多的进程
+ 实际用户的id数的进程总数已经超过了系统的限制。其中CHILD_MAX规定了每个实际用户在任意时刻可拥有的最大进程数目。

通常我们将子进程从fork返回后立即调用exec,有时将这两个操作组合成一个操作，就叫spawn。

#### vfork函数
vfork创建一个新进程，而该进程的目的就是exec一个新的程序，如同shell执行某个可执行文件一样，vfork不将父进程的地址空间
完全复制到子进程中，因为子进程会立即调用exec，如实也就不会引用该地址空间，不过在调用exec或exit函数之前，它是在父进程的空间
中运行的。另外vfork保证子进程先运行，在`调用exec或exit后`父进程才可能被调度运行，在这段时间中，父进程是处于休眠状态的。如果子进程依赖父进程的进一步动作就会导致死锁。

#### exit函数

5种正常终止
+ 在main函数调用return等效于调用exit函数。
+ 调用exit函数
+ 调用_exit和_Exit函数
+ 进程的最后的一个例程执行了return语句。该线程的返回值不用作为进程的返回值。当最后一个启动例程序返回的时候，该进程以0状态返回。
+ 进程的最后一个线程调用pthread_exit函数。进程的终止状态是0，这与传给pthread_exit的参数无关。

后面两种都跟线程有关，现在线程概念还不太清晰。

异常终止的几种状态

+ 调用abort。产生SIGABRT信号，这是下一种异常终止的特例。
+ 当进程接受到某些信号时。异常终止的信号来源
    + 自身abort函数调用
    + 其他进程或者由内核产生，由内核产生的异常信号比方说
        + 进程引用地址空间之外的存储单元
        + 除以0
        + ...
+ 最后一个线程对"取消"请求做出相应。取消会以*延迟*方式发生：一个线程要求取消另一个线程，若干时间后，目标线程终止。

进程终止后，会执行同一段代码，关闭所有打开的描述符，释放进程所使用的存储器。
将其*退出状态*作为参数传递给函数。其父进程能够用wait和waitpid函数来获取其*终止状态*。

+ 退出状态：传递给3个终止函数的参数，或者是main的返回值
+ 终止状态：内核将退出状态装化为终止状态。

当子进程的父进程比子进程先终止，那么在内核逐个检查所有活动的进程的时候，判断其是否为正要终止进程的子进程，如果是，就将改进程的父进程
ID修改为1(init进程的ID)。保证了每个进程都有父进程。

内核为每个终止的子进程保留了一定量的信息，所以当其父进程调用wait和waitpid的时候，就可以得到这些信息
这些信息包括
+ pid
+ 终止状态
+ 该进程使用CPU的时间总量。

僵尸进程：一个已经终止的，但是父进程没有对齐进行善后处理的进程(没有获取其终止子进程的有关信息，释放掉它占用的资源)
init收养进程。

### wait 和 waitpid

wait函数如果执行成功则返回子进程识别码(PID), 如果有错误发生则返回-1. 失败原因存于errno 中。

waitpid参数做出以下解释
+ `pid==-1`, 等待任意一个子进程
+ `pid>0`,等待指定的子进程
+ `pid==0`,等待组id等于调用进程
+ `pid<-1`,等待组id等于pid绝对值得子进程

wait出错的唯一原因就是调用的进程没有子进程。

waitpid出错的情况
+ 就是当指定的进程或进程组不存在
+ 或者被某个信号中断了，这个时候errno的值被设置为`EINTR`
waitpid第三个参数
+ waitpid提供了一个wait的非阻塞版本，有时需要获得一个子进程的状态，但是不想阻塞
+ waitpid还能通过`WUNTRACED`(wait的第三个参数)（pid参数指定的一个子进程处于停止状态，而且这个状态还没有被报告过，就会返回其状态）和`WCONTINUED`（pid参数指定的进程停止后继续运行，就会返回状态）选项支持作业控制

WNOHANG: 指定waitpid不阻塞，返回值是0

对进程有规划的停止和继续这就叫做*作业控制*

不让子进程成为僵死进程的方法就是让其被init进程收养，这样的话，子进程退出，就会被init回收。

#### waitid函数

### 竞争条件
在我们无法判断哪一个进程先运行的时候，因为这很依赖于系统负载和内核的调度算法。

如果一个进程要等待其父进程终止，可以执行以下循环，判断其是否已经被init进程收养。
```cpp
while (getppid() != 1) {
    sleep(1);
}
```
这种循环叫做*轮询*,但我们还是要尽量避免轮询和竞争条件，因为这个会占用用户CPU时间。


### exec 函数

子进程往往调用exec函数以执行另外一个程序，当进程调用exec函数时，该进程被替换为一个新的
程序，而且新的程序从其main函数开始执行。exec不创建新的进程，pid不变，但是当前进程的正文段，
数据段，堆段和栈段都被替换了。

exec 前后实际用户id和实际组id是保持不变的，而有效id是否改变取决于程序文件的设置用户id位和设置组id位
是否被设置。如果设置用户id位被设置了，那么进程的有效用户id位变成文件所有者的id，否则不变，组id也一样。

在执行shell将执行启动配置文件(start-up file)命令，而非登录shell则不会执行这些命令。

##### extern
关键字可以在一个文件中引用另一个文件中定义的变量或者函数。

echoall函数
```cpp
#include "apue/apue.h"

int main(int argc, char *argv[]) {
    int         i;
    char        **ptr;
    extern char **environ; // 这样这个变量可以引用 exec 函数传给他的environ函数

    for (i = 0; i < argc; i++) {
        printf("argv[%d]: %s\n", i, argv[i]);
    }

    for (ptr = environ; *ptr != 0/*解引出来的是一个字符串指针，如果它指向0地址*/; ptr++) {
        printf("%s\n", *ptr);
    }
    exit(0);
}
```

exec调用者两种方式
```cpp
execle("/home/sar/bin/echoall", "echoall", "myarg1", (char*)0, env_init) // 自定义环境变量

execlp("echoall", "echoall", "olny 1 arg", (char*)0) // 使用shell传给它的环境变量
```

#### 更改用户ID和更改组ID

最小特权模型：我们的程序应当只具有完成给定任务的最小权限。

setuid的规则

+ 如果进程有超级用户权限，则setuid将实际用户id，euid，saved set-user-id 都设置为 uid，这就意味着该进程将自己的权限降低了
+ 如果没有超级用户权限，但是uid等于实际用户ID或者保存的设置用户id，则setuid系统调用只将进程的euid改为uid
+ 如果都不满足，errno被设置为EPERM，返回-1

一下几点

+ 只有超级用户进程才能够修改实际用户的id。通常，实际用户的id实在用户登录的时候，由login(1)程序设置的，而且不会改变他，所以login是一个
超级用户进程，调用setuid的时候，设置3种用户id。
+ 当程序文件设置了设置用户id位的时候，exec才能设置有效用户id，如果没有，exec不会改变其有效id。任何时候都可以调用setuid函数将有效uid设置为
euid和ruid。
+ saved set-uid 是由exec复制euid得到的。如果文件设置了set-uid 位，exec根据文件的uid设置euid以后，这个副本就被保护了起来。

所以之所有进程要有saved set-uid 位，是因为进程会有通过`setuid函数`在两种uid（ruid和进程当前使用的文件的所有者id）进行切换的需要。

```cpp
exec("login") // 设置为程序文件的uid，saved set-uid 副本拷贝
```

setuid 只能降低或者保持权限，而exec要根据文件进行升高权限(但不更改ruid)，而且退出后权限位会复原。

进程的saved set-uid位永远都是从进程的euid中获取的，它就是euid的副本。

函数 getuid和geteuid只能获取ruid和euid，没有可移植的方法获取saved set-uid的值。

##### 函数setreuid和setregid

功能是交换实际用户id和有效用户id的值。(不知道为什么要说成交换)

+ 如果参数设置为-1，保持不变

程序在子进程调用exec之前，将子进程的ruid,euid都设置成为普通用户id。

##### 组id

附属组id不受setgid，setregid和setegid函数的影响

at 命令

at命令用于指定将来某个时刻运行程序，由它启动的程序同样作为守护进程

mac os上安装at程序的设置用户id是root用户，这容许at命令对守护进程拥有特权文件的写权限，守护
进程代表用户运行at命令。

+ 程序文件是由root用户拥有的，设置用户id位已经设置。如果运行程序时，euid和saved set-uid被设置
+ at程序第一个要做的事情就是降低权限，防止对特权的误用。
+ 直到它需要访问控制哪些命令即将运行，这些命令要何时运行的配置文件时


调用execl时，内核取execl调用中的pathname而非第一个参数testinterp，因为一眼而言，pathname有更多信息。

脚本中也可以添加`-f`选项
使用的方式就是
```cpp
#!/bin/awk -f
```

suspend 可以挂起超级用户的shell

##### awk命令

awk命令是文本处理命令，是一种强大的文本分析工具，命名是取自三位创始人的名字。

awk读解释器文件的时候，因为#是awk的注释字段，所以它会忽略掉第一行，不能期望解释器会使用

下面是一个awk的example，shell脚本文件，我们将它命名为awkexample
```
#!/usr/bin/awk -f

BEGIN {
    for (i = 0; i < ARGC; i++)
        print "ARGV[%d] = %s\n", i, ARGV[i]
    exit
}
```
解释器文件可以使用户的效率得到提高，但是其代价就是额外的内核开销(因为识别解释器文件的是内核)。
+ 为了执行上面的脚本程序，只需要使用下列命令行，awkexample \<args\>，而不需要`awk -f awkexample <args>`,
+ shell脚本在效率上提供了一些好处

当把
```bash
awk ` BEGIN {
    for (i = 0; i < ARGC; i++) 
        print "ARGV[%d] = %s\n", i, ARGV[i]
    exit
}` $*
```

如何执行这个脚本，shell读取了此命令，然后调用execlp此文件名，发现这并不是一个机器可执行的文件，所以就返回字一个错误，然后execlp就自动认为这是一个
shell脚本。然后就执行`/bin/sh`来解释这个脚本，shell正确的运行了我们的脚本，但是为了运行awk程序，它还是调用了fork，exec和wait函数。于是，用一个shell脚本代替
解释器解释器脚本需要更多的开销。

shell脚本和解释器脚本的区别就在是否以一个解释器名称开头，而且shell脚本文件名都都必须以`.sh`结尾。

+ 解释器脚本使我们可以使用除了/bin/sh以外的其他shell来编写shell脚本。当execlp执行到一个非机器可执行的可执行文件的时候，它总是会调用/bin/sh来解释执行该文件。但是用解释器脚本
则可以简单的写出

```
#!/bin/csh
```

#### 函数 system

只有当命令处理程序可用的时候，才回返回0。system函数的实现也是调用了fork，exec和waitpid这三个函数，因此会有三个返回值

+ fork失败或者waitpid返回失败除开了EINTR之外的错误，则system函数会返回一个-1，而且设置了errno以指示错误类型。
+ 如果exec失败（system是总是调用/bin/sh来解释传递进入的字符串命令参数）。
+ 如果3个函数都成功，那么，system返回值就是shell子进程的终止状态，返回格式是waitpid的格式。

system进行了所需的各种错误处理以及信号处理。在UNIX早期系统中，都没有waitpid函数，可以用下面语句来替代
```cpp
while ((lastpid = wait(&status)) != pid && lastpid != -1);
```

system就是进行了fork和exec，然而如果进程以一种特殊权限运行，它又想生成一个进程执行另外一个程序，我们不能继续给予这个
子进程特权，然而system没有在fork和exec之间对权限进行修改。set-uid 和set-gid绝对不应该调用system函数。

#### 进程会计

每当进程结束的时候，内核就会写一个会计记录

#### 进程调度

内核决定调度，进程可以通过调整nice的值选择以更低优先级运行，通过调整nice值来降低对CPU的占有
所以该进程是"友好的"。NZERO是系统默认的nice值

nice函数
用与增加nice值，越nice的进程越不会占用太多的CPU时间。

getpriotirty函数，可以获取相应进程的nice值

setpriority可以设置相应进程的nice值

#### 进程时间

times函数，times函数返回墙上进程时间作为其函数值。此值是相对于过去的某一时刻度量的。

> 时钟周期:时钟周期（clock cycle）的频率：8253／8254 PIT的本质就是对由晶体振荡器产生的 时钟周期进行计数，晶体振荡器在1秒时间内产生的时钟脉冲个数就是时钟周期的频率。

>时钟滴答（clock tick）：当PIT通道0的计数器减到0值时，它就在IRQ0上产生一次时钟中断，也即一次时钟滴答。PIT通道0的计数器的初始值决定了要过多少时钟周期才产生一次时钟中断，因此也就决定了一次时钟滴答的时间间隔长度。


      
