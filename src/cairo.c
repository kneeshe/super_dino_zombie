#include <cairo/cairo.h>

#define WIDTH 640
#define HEIGTH 480
#define PI 3.14

int main (int argc, char *argv[]){
        cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
        cairo_t *cr = cairo_create (surface);

        cairo_set_source_rgba(cr, 0, 255, 0, 0.6);
        // cairo_rectangle(cr, 90, 0, 20, 300);
        // cairo_rectangle(cr, 0, 200, 200, 30);
        cairo_arc(cr, 200, 200, 200, 0, 2 * PI);
        // cairo_rectangle(cr, 0, 0, 300, 300);
        cairo_fill(cr);


        cairo_surface_write_to_png (surface, "img.png");
        cairo_destroy (cr);
        cairo_surface_destroy (surface);
        return 0;
}

