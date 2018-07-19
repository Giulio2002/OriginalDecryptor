#include "explorer.h"
#include "menu.h"
#include <unistd.h>

int main(int argc, char **argv)
{
    char dir[500];
    getcwd(dir, sizeof(dir));

    while(1) {
        START:
        chdir(dir);
        switch( menu() ){
            case 0:
                return 0;
            case 1:
                if(explorer() == 0) return 0;
                else goto START;
        }
    }

}
