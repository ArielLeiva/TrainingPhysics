#include <iostream>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include "objs.hpp"

using namespace std;

int x_c;
int y_c;
int w;
int h;
char *base;

int main()
{
    
    int fd = open("/dev/fb0", O_RDWR); 

    if(fd == -1)
    {
        cout << "Error openning fb0\n";
        return -1;
    }

    struct fb_var_screeninfo vin;

    if(ioctl(fd, FBIOGET_VSCREENINFO, &vin))
    {
        cout << "Error getting fb info\n";
        return -1;
    }

    w = vin.xres;
    x_c = w/2;
    h = vin.yres;
    y_c = h/2;

    int ssize = vin.yres_virtual * vin.xres_virtual * vin.bits_per_pixel / 8;
    char *ptr = (char *) mmap(0, ssize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    Vec p0(x_c,y_c), v0(0,2), a0(0,0);
    Vec p1(60, 60), v1(40,0), a1(0,0);

    Obj o(p0, v0, a0, 300000, 50, 0, 0x00ff00ff);
    Obj s(p1, v1, a1, 350000, 50, 1, 0x0000ff00);

    base = ptr;

    Fb fb(&vin, base);

    while(1)
    {
        fb.print_obj(o);
        fb.print_obj(s);
        s.atract(&o);
        o.atract(&s);

        //o.dump();
        //s.dump();
        s.clean_update(0.01);
        o.clean_update(0.01);
    }


    return 0;
}
