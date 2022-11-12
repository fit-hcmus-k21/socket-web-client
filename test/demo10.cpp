#include <stdio.h>
#include <string.h>
#include <conio.h>
 
char str1[20] = "vncoding.net";
 
int  main( )
{
    printf("The string: %s\n", str1);
    memcpy(str1 + 2, str1, 6);
    printf("New string: %s\n", str1);
 
    strncpy(str1, "vncoding.net", sizeof(str1));   // reset string
 
    printf("The string: %s\n", str1);
    memmove(str1 + 2, str1, 6);
    printf("New string: %s\n", str1);
    getch();

    return 0;
}