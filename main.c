//as libs vão aqui
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/drawing.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/system.h>
#include <allegro5/timer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>

//aqui vão as variaveis globais e consts
#define WIDTH 720
#define HEIGHT 480
#define KEY_SEEN 1
#define KEY_DOWN 2

const float FPS = 30.0;
const float GRAVITY = 0.5;

//as funções auxiliares vão aqui
void must_init(bool test, const char *description) {
  if(test) {
    return;
  } 
  printf("Couldn't initialize %s\n", description);
  exit(1);
}

//função principal
int main() {
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");

  ALLEGRO_TIMER *timer = al_create_timer(1/FPS);
  must_init(timer, "timer");

  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  must_init(queue, "queue");

  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
  ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);
  must_init(display, "display");

  ALLEGRO_FONT *font = al_create_builtin_font();
  must_init(font, "font");

  must_init(al_init_primitives_addon(), "primitives");

  must_init(al_install_audio(), "audio");
  must_init(al_init_acodec_addon(), "audio_codec");
  must_init(al_reserve_samples(16), "sample");

  ALLEGRO_SAMPLE *scream = al_load_sample("./sfx/scream-grind.wav");
  must_init(scream, "action-loop");

  ALLEGRO_AUDIO_STREAM *music = al_load_audio_stream("./sfx/action-loop.wav", 2, 2048);
  must_init(music, "music");
  al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  
  ALLEGRO_AUDIO_STREAM *music_bg = al_load_audio_stream("./sfx/action-effect.wav", 2, 2048);
  must_init(music_bg, "music_bg");
  al_set_audio_stream_playmode(music_bg, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(music_bg, al_get_default_mixer());


  must_init(al_init_image_addon(), "image");
  //aqui irão os bitmaps
  ALLEGRO_BITMAP *dude = al_load_bitmap("./assets/spritesheet.png");
  must_init(dude, "dude");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(display));
  al_register_event_source(queue, al_get_timer_event_source(timer));

  bool done = false;
  bool redraw = true;

  ALLEGRO_EVENT event;

  //var para o personagem
  float char_x = 100;
  float char_y = 100;
  float vel = 5;

  unsigned char key[ALLEGRO_KEY_MAX];
  memset(key, 0, sizeof(key));

  al_start_timer(timer);
  while(!done) {
    al_wait_for_event(queue, &event);

    switch (event.type) {
      case ALLEGRO_EVENT_TIMER:
        if(key[ALLEGRO_KEY_W]) {
          char_y -= vel;
          if(char_y < 0){
            char_y *= -1;
          }
        }
        if(key[ALLEGRO_KEY_S]){
          char_y += vel;
          if(char_y > HEIGHT){
              char_y -= (char_y - HEIGHT) * 2;
          }
        }
        if(key[ALLEGRO_KEY_A]){
          char_x -= vel;
          if(char_x < 0) {
            char_x *= -1;
          }
        }
        if(key[ALLEGRO_KEY_D]){
          char_x += vel;
          if(char_x > WIDTH) {
            char_x -= (char_x - WIDTH) * 2;
          }
        }

        if(key[ALLEGRO_KEY_M]) {
          al_play_sample(scream, 0.3, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }

        if(key[ALLEGRO_KEY_ESCAPE]){
          done = true;
        }

        for(int i = 0; i < ALLEGRO_KEY_MAX; i++) {
          key[i] &= ~KEY_SEEN;
        }

        redraw = true;
        break;

      case ALLEGRO_EVENT_KEY_DOWN:
        key[event.keyboard.keycode] = KEY_SEEN | KEY_DOWN;
        break;
      case ALLEGRO_EVENT_KEY_UP:
        key[event.keyboard.keycode] &= ~KEY_DOWN;
        break;
      case ALLEGRO_EVENT_DISPLAY_CLOSE:
        done = true;
        break;
    }

    if(redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      al_draw_filled_rectangle(char_x, char_y, char_x + 50, char_y + 50, al_map_rgb(255, 0, 0));
      // al_draw_bitmap(dude, char_x, char_y, 0);
      // al_draw_bitmap_region(dude);
      

      al_flip_display();
    }

    if(done){
      break;
    }
  
  }

  al_destroy_display(display);
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_bitmap(dude);
  al_destroy_sample(scream);
  al_destroy_audio_stream(music);
  al_destroy_audio_stream(music_bg);


  return 0;
}
