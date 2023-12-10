#include <iostream>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>

using namespace std;

int x_c;
int y_c;
int w;
int h;
char *base;

class Vec 
{
    public:
        double x;
        double y;

        Vec(double nx, double ny)
        {
            x = nx;
            y = ny;
        }

        void copy(Vec v2)
        {
            x = v2.x;
            y = v2.y;
        }

        void add_vec(Vec v2)
        {
            x += v2.x;
            y += v2.y;
        }

};

class Obj
{
    public:
        Vec p = Vec(0,0);
        Vec v = Vec(0,0);
        Vec a = Vec(0,0);
        double m;
        double vol;
        int id;
        int color;

        Obj(Vec p0, Vec v0, Vec a0, double nm, double nvol,int nid, int ncolor)
        {
            p.copy(p0);
            v.copy(v0);
            a.copy(a0);
            m = nm;
            vol = nvol;
            id = nid;
            color = ncolor;
        }

        void dump_obj()
        {
            printf("id: %d, m: %lf, p: (%lf,%lf), v: (%lf,%lf), a: (%lf,%lf)\n", id, m, p.x, p.y, v.x, v.y, a.x, a.y);
        }


};

class Fb 
{
    public:
        struct fb_var_screeninfo *vin;
        char *base;
        int fw;
        int fh;


        Fb(struct fb_var_screeninfo *nvin, char *nbase)
        {
           vin = nvin;
           base = nbase;
           fw = vin->xres;
           fh = vin->yres;
        }
        
        int car(int x, int y)
        {
            return 4*((fh-y)*fw + x);
        }

        void pix(int x, int y, int color)
        {
            int *real = (int *) (base + car(x,y));
            *real = color;
        }

        void circle(Vec v, double r, int color)
        {
            int x1 = 1;
            int y1 = 1;

            for (int y0 = -r; y0 < r; ++y0)
            {
                for (int x0 = -r; x0 < r; ++x0)
                {
                    x1 = (int)(v.x+x0);
                    y1 = (int)(v.y+y0);

                    if(x0*x0+y0*y0<=r*r)
                    {
                        pix(x1, y1, color);
                    }
                }
            }

        }

        void print_obj(Obj o)
        {
            circle(o.p, o.vol, o.color);
        }
};

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

    Vec v1(x_c,y_c), v2(1,3), v3(0,0);

    Obj o(v1,v2,v3,3, 50, 0, 0x00ff00ff);
    o.dump_obj();

    base = ptr;

    Fb fb(&vin, base);

    Vec v4(x_c, y_c);

    fb.print_obj(o);


    return 0;
}
