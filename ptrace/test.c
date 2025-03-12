#include <stdio.h>
char str[] = {"Hello World"};
int main()
{
    puts("Working");
    while (1)
    {
        puts(str);
        getchar();
    }
    puts("Done");
}