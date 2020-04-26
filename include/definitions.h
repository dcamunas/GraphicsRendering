#include <string.h>
#include <ctype.h>


/* My constant's */
//#define EMPLOYEES_NUMBER    16
#define EXEC_PATH           "exec/pract2"
#define MASTER_RANK         0
#define IMAGE_SIDE          400
#define IMAGE_SIZE          IMAGE_SIDE * IMAGE_SIDE
#define PRIMARY_COLORS_N    3
#define IMAGE_PATH          "files/foto.dat"
#define NATIVE_MOD          "native"
#define TAG                 1


/* Point info */
#define POINT_INFO_N        5
#define ROW                 3
#define COLUMN              4
#define R                   0
#define G                   1
#define B                   2

/* Colors info*/
#define SEPIA              'S'
#define BLACK_WHITE        'B'
#define DEFAULT            'D'

/* Functions */
int is_integer(char *variable)
{
    int i, is = 1;

    for(i = 0; i < strlen(variable); i++)
    {
        if(!isdigit(variable[i])){is = 0;}
    }

    return is;
}