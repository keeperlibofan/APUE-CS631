//
// Created by seadream on 2018/10/27.
//

#include<stdio.h>
void main(void)
{
    FILE* stream;
    char s[81];
    stream = fopen("fscanf.txt","w");
    if(stream == NULL)/*流为空*/
    {
        printf("the file is opeaned error!\n");
    }
    else
    {
        fprintf(stream,"%s %ld %f %c","a_string",6500,3.1415,'x');
        fseek(stream,0L,SEEK_SET);
        fscanf(stream,"%s",s);
        printf("%ld\n",ftell(stream));
        if(ferror(stream))            /*判断是否读取出错*/ {
            printf("Error form reading file.\n");
        }
        if(ferror(stream))/*清楚错误信息*/
        {
            clearerr(stream);
            printf("Error has been cleared.\n");
        }
        if(ferror(stream))/*再次检查是否还有错误信息*/
        {
            printf("Error form reading file.\n");
        }
        fclose(stream);
    }
}