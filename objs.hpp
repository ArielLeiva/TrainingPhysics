#include <cmath>

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

        void add(Vec v2)
        {
            x += v2.x;
            y += v2.y;
        }

        void adds(Vec v2, double c)
        {
            x += v2.x * c;
            y += v2.y * c;
        }

        void sub(Vec v2)
        {
            x -= v2.x;
            y -= v2.y;
        }

        void scale(double c)
        {
            x *= c;
            y *= c;

        }

        double len()
        {
            return sqrt(x*x+y*y);
        }

        double sqlen()
        {
            return x*x+y*y;
        }

        void dump()
        {
            printf("x: %lf, y: %lf\n", x, y);
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

        void dump()
        {
            printf("id: %d, m: %lf, p: (%lf,%lf), v: (%lf,%lf), a: (%lf,%lf)\n", id, m, p.x, p.y, v.x, v.y, a.x, a.y);
        }

        void update(double t)
        {
            p.adds(v, t);
            v.adds(a, t);
        }

        void clean_update(double t)
        {
            p.adds(v, t);
            v.adds(a, t);
            a.x = 0;
            a.y = 0;

        }

        void atract(Obj *o)
        {
            Vec dist(0,0);
            dist.add(p);
            dist.sub(o->p);
            double sq = dist.sqlen();

            // Normalizing and setting the proper acceleration
            dist.scale(m/(sqrt(sq)*sq));
            o->a.add(dist);
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

        int is_safe(int x, int y)
        {
            return (x+1 < fw && x-1 >= 0 && y+1 < fh && y-1 >= 0);
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
            int vol = (int) r;

            for (int y0 = -vol; y0 < vol; ++y0)
            {
                for (int x0 = -vol; x0 < vol; ++x0)
                {
                    x1 = (int)(v.x+x0);
                    y1 = (int)(v.y+y0);

                    if(is_safe(x1, y1) && x0*x0+y0*y0<=vol*vol)
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


