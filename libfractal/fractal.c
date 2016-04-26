/*Autheur : AMAN SINGH
 *Version : 26/04/16
 */

#include <stdlib.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
    struct fractal *f = (struct fractal*)malloc(sizeof(struct fractal));
    f->value[height][width];
    f->name = name;
    f->width = (unsigned int)width;
    f->height = (unsigned int)height;
    f->a = a;
    f->b = b;

    return f;
}

void fractal_free(struct fractal *f)
{
    free(f);
}

const char *fractal_get_name(const struct fractal *f)
{
    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    return f->value[y][x];
}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
    f->value[y][x] = val;
}

int fractal_get_width(const struct fractal *f)
{
    return (int)(f->width);
}

int fractal_get_height(const struct fractal *f)
{
    return (int)(f->height);
}

double fractal_get_a(const struct fractal *f)
{
    return f->a;
}

double fractal_get_b(const struct fractal *f)
{
    return f->b;
}
