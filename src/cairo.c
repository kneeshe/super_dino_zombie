#include <cairo/cairo.h>

#define WIDTH 640
#define HEIGTH 480
#define PI 3.14

void cruz_invertida();
void bad_square();
void sad_circle();
void blend();

int main (){
  cruz_invertida();
  bad_square();
  sad_circle();
  blend();

  return 0;
}

void cruz_invertida(){
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
  cairo_t *cr = cairo_create(surface);
  cairo_rectangle(cr, 90, 0, 20, 300);
  cairo_rectangle(cr, 0, 200, 200, 30);
  cairo_set_source_rgba(cr, 255, 255, 255, 0.6);
  cairo_fill(cr);
  cairo_surface_write_to_png (surface, "assets/demo_cairo/cross.png");
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void bad_square(){
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
  cairo_t *cr = cairo_create(surface);
  cairo_rectangle(cr, 0, 0, 300, 300);
  cairo_set_source_rgba(cr, 255, 0, 0, 0.6);
  cairo_fill(cr);
  cairo_surface_write_to_png (surface, "assets/demo_cairo/bad_square.png");
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void sad_circle(){
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
  cairo_t *cr = cairo_create(surface);
  cairo_arc(cr, 200, 200, 200, 0, 2 * PI);
  cairo_set_source_rgba(cr, 0, 255, 0, 0.6);
  cairo_fill(cr);
  cairo_surface_write_to_png (surface, "assets/demo_cairo/sad_circle.png");
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}

void blend(){
  cairo_surface_t *surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, WIDTH, HEIGTH);
  cairo_t *cr = cairo_create(surface);
  cairo_pattern_t *circle;
    circle = cairo_pattern_create_radial(WIDTH/2.0, HEIGTH/2.0, 1.5, WIDTH/4.0, HEIGTH/4.0, HEIGTH);
  cairo_pattern_add_color_stop_rgba(circle, 0,  255.5, 0, 0, 0.6);
  cairo_pattern_add_color_stop_rgba(circle, 1,  0, 0, 255.5, 0.6);
  cairo_rectangle(cr, 0, 0, WIDTH, HEIGTH);
  cairo_set_source(cr, circle);
  cairo_fill(cr);
  cairo_surface_write_to_png (surface, "assets/demo_cairo/grad_circle.png");
  cairo_destroy(cr);
  cairo_surface_destroy(surface);
}
