#include <cairo/cairo.h>

#define WIDTH 640
#define HEIGTH 480
#define PI 3.14

void cruz_invertida(cairo_t *cr);
void bad_square(cairo_t *cr);
void sad_circle(cairo_t *cr);
void blend(cairo_t *cr);

int main (int argc, char *argv[]){
        cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
        cairo_t *cr = cairo_create (surface);

        cairo_set_source_rgba(cr, 0, 255, 0, 0.6);
        
        cruz_invertida(cr);
        bad_square(cr);
        sad_circle(cr);
        cairo_fill(cr);


        cairo_surface_write_to_png (surface, "img.png");
        cairo_destroy (cr);
        cairo_surface_destroy (surface);
        return 0;
}

void cruz_invertida(cairo_t *cr){
  cairo_rectangle(cr, 90, 0, 20, 300);
  cairo_rectangle(cr, 0, 200, 200, 30);
}

void bad_square(cairo_t *cr){
  cairo_rectangle(cr, 0, 0, 300, 300);
}

void sad_circle(cairo_t *cr){
  cairo_arc(cr, 200, 200, 200, 0, 2 * PI);
}

