#include <allegro5/altime.h>
#include <allegro5/bitmap.h>
#include <allegro5/bitmap_draw.h>
#include <allegro5/bitmap_io.h>
#include <allegro5/color.h>
#include <allegro5/display.h>
#include <allegro5/drawing.h>
#include <allegro5/events.h>
#include <allegro5/keyboard.h>
#include <allegro5/keycodes.h>
#include <allegro5/mouse.h>
#include <allegro5/mouse_cursor.h>
#include <allegro5/system.h>
#include <allegro5/timer.h>
#include <allegro5/transformations.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <unistd.h>
#include <time.h>
#include "cris.c"

#define WIDTH 640
#define HEIGHT 480
#define KEY_SEEN 1
#define KEY_DOWN 2


void must_init(bool test, const char *description) {
  if(test) {
    return;
  } 
  printf("Couldn't initialize %s\n", description);
  exit(1);
}

bool collide(int ax1, int ay1, int ax2, int ay2, int bx1, int by1, int bx2, int by2){
   if(ax1 > bx2) return false;
   if(ax2 < bx1) return false;
   if(ay1 > by2) return false;
   if(ay2 < by1) return false;

   return true;
}

bool mouse_collide(float mouse_x, float mouse_y, float obj_x, float obj_y, float obj_width, float obj_height) {
    if (mouse_x < obj_x) return false;
    if (mouse_x > obj_x + obj_width) return false;
    if (mouse_y < obj_y) return false;
    if (mouse_y > obj_y + obj_height) return false;
    
    return true;
}

typedef struct BOUNCE {
  float x, y;
  float dx, dy;
} BOUNCE;

typedef struct CHAR {
  float x;
  float x2;
  float y;
  float y2;
} CHAR;

enum BOUNCER_TYPE {
    BT_CIRCLE,
    BT_RECTANGLE_2,
    BT_BLEND,
    ENEMY_1,
    ENEMY_2,
    FRASE_1,
    FRASE_2,
    FRASE_3,
    BT_N
};

typedef struct BOUNCER {
    float x, y;
    float dx, dy;
    int type;
} BOUNCER;

bool game_over = false;
float game_over_timer = 0.0;
const float GAME_OVER_DURATION = 10.0;

void check_enemy_collision(CHAR* player, BOUNCER* enemies, int num_enemies, ALLEGRO_SAMPLE *sample) {
    if (game_over) return;
    
    float player_width = 45; 
    float player_height = 45;
    
    for (int i = 0; i < num_enemies; i++) {
        BOUNCER* enemy = &enemies[i];
        
        if (enemy->type != ENEMY_1 && enemy->type != ENEMY_2) continue;
        
        float enemy_width = 45;
        float enemy_height = 45;
        
        if (collide(player->x, player->y, player->x + player_width, player->y + player_height, enemy->x, enemy->y, enemy->x + enemy_width, enemy->y + enemy_height)) {
            game_over = true;
            game_over_timer = 0;
            al_play_sample(sample, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            break;
        }
    }
}

void take_screenshot(ALLEGRO_DISPLAY *display);

const float FPS = 30.0;
const float GRAVITY = 0.5;
float dy = 0;
bool isJumping = false;
float jumpSpeed = -12.0;
const float GROUND = HEIGHT - 55;
float game_timer = 0.0;

int main() {
  must_init(al_init(), "allegro");
  must_init(al_install_keyboard(), "keyboard");
  must_init(al_install_mouse(), "mouse");

  ALLEGRO_TIMER *timer = al_create_timer(1/FPS);
  must_init(timer, "timer");

  ALLEGRO_EVENT_QUEUE *queue = al_create_event_queue();
  must_init(queue, "queue");

  al_set_new_display_option(ALLEGRO_SAMPLE_BUFFERS, 1, ALLEGRO_SUGGEST);
  al_set_new_display_option(ALLEGRO_SAMPLES, 8, ALLEGRO_SUGGEST);
  al_set_new_bitmap_flags(ALLEGRO_MIN_LINEAR | ALLEGRO_MAG_LINEAR);
  al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);
  ALLEGRO_DISPLAY *display = al_create_display(WIDTH, HEIGHT);
  must_init(display, "display");

  al_set_window_title(display, "Super Cthulhu Zombie");
  const float scale_factor_x = ((float)al_get_display_width(display)) / WIDTH;
  const float scale_factor_y = ((float)al_get_display_height(display)) / HEIGHT;

  ALLEGRO_TRANSFORM t;
  al_identity_transform(&t);
  al_scale_transform(&t, scale_factor_x, scale_factor_y);
  al_use_transform(&t);

  ALLEGRO_FONT *font = al_create_builtin_font();
  must_init(font, "font");
  ALLEGRO_FONT *game_over_font = al_create_builtin_font();
  must_init(game_over_font, "game over font");
 
  must_init(al_init_primitives_addon(), "primitives");

  must_init(al_install_audio(), "audio");
  must_init(al_init_acodec_addon(), "audio_codec");
  must_init(al_reserve_samples(16), "sample");

  ALLEGRO_SAMPLE *scream = al_load_sample("./sfx/scream-grind.wav");
  must_init(scream, "scream");
  ALLEGRO_SAMPLE *death = al_load_sample("./sfx/glitch-scream.wav");
  must_init(death, "death");

  // ALLEGRO_AUDIO_STREAM *music = al_load_audio_stream("./sfx/action-loop.wav", 2, 2048);
  // must_init(music, "music");
  // al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
  // al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  //
  // ALLEGRO_AUDIO_STREAM *music_bg = al_load_audio_stream("./sfx/action-effect.wav", 2, 2048);
  // must_init(music_bg, "music_bg");
  // al_set_audio_stream_playmode(music_bg, ALLEGRO_PLAYMODE_LOOP);
  // al_attach_audio_stream_to_mixer(music_bg, al_get_default_mixer());
  
  ALLEGRO_AUDIO_STREAM *music = al_load_audio_stream("./sfx/whisper.wav", 2, 2048);
  must_init(music, "music");
  al_set_audio_stream_gain(music, 1.0);
  al_set_audio_stream_playmode(music, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(music, al_get_default_mixer());
  
  ALLEGRO_AUDIO_STREAM *music_bg = al_load_audio_stream("./sfx/witch.wav", 2, 2048);
  must_init(music_bg, "music_bg");
  al_set_audio_stream_gain(music_bg, 0.2);
  al_set_audio_stream_playmode(music_bg, ALLEGRO_PLAYMODE_LOOP);
  al_attach_audio_stream_to_mixer(music_bg, al_get_default_mixer());


  must_init(al_init_image_addon(), "image");
  ALLEGRO_BITMAP *bgImg = al_load_bitmap("./assets/bg/1.png");
  must_init(bgImg, "1");
  ALLEGRO_BITMAP *midImg = al_load_bitmap("./assets/bg/2.png");
  must_init(midImg, "2");
  ALLEGRO_BITMAP *midImg2 = al_load_bitmap("./assets/bg/3.png");
  must_init(midImg2, "3");
  ALLEGRO_BITMAP *midImg3 = al_load_bitmap("./assets/bg/4.png");
  must_init(midImg3, "4");
  ALLEGRO_BITMAP *fgImg = al_load_bitmap("./assets/bg/5.png");
  must_init(fgImg, "5");
  ALLEGRO_BITMAP *fgImg2 = al_load_bitmap("./assets/bg/6.png");
  must_init(fgImg2, "6");
  ALLEGRO_BITMAP *heror = al_load_bitmap("./assets/heror.png");
  must_init(heror, "hero");
  ALLEGRO_BITMAP *herol = al_load_bitmap("./assets/herol.png");
  must_init(herol, "hero");
  ALLEGRO_BITMAP *demon = al_load_bitmap("./assets/demon.png");
  must_init(demon, "demon");
  ALLEGRO_BITMAP *cross = al_load_bitmap("./assets/cross.png");
  must_init(cross, "crosshair");
  ALLEGRO_BITMAP *bad_block = al_load_bitmap("./assets/bad_block.png");
  must_init(bad_block, "bad_block");
  ALLEGRO_BITMAP *sad_circle = al_load_bitmap("./assets/sad_circle.png");
  must_init(sad_circle, "sad_circle");
  ALLEGRO_BITMAP *grad_circle = al_load_bitmap("./assets/grad_circle.png");
  must_init(grad_circle, "grad_circle");

  al_register_event_source(queue, al_get_keyboard_event_source());
  al_register_event_source(queue, al_get_display_event_source(display));
  al_register_event_source(queue, al_get_timer_event_source(timer));
  al_register_event_source(queue, al_get_mouse_event_source());

  al_hide_mouse_cursor(display);
  al_grab_mouse(display);

  bool done = false;
  bool redraw = true;

  ALLEGRO_EVENT event;

  BOUNCER obj[BT_N];
   for(int i = 0; i < BT_N; i++)
   {
       BOUNCER* b = &obj[i];
       b->x = rand() % WIDTH;
       b->y = rand() % HEIGHT;
       b->dx = ((((float)rand()) / RAND_MAX) - 0.5) * 2 * 4;
       b->dy = ((((float)rand()) / RAND_MAX) - 0.5) * 2 * 4;
       b->type = i;
   }

  CHAR john;
  john.x = 100;
  john.y = 100;
  john.x2 = john.x + 50;
  john.y2 = john.y + 50;

  CHAR enemy;
  enemy.x = 200;
  enemy.y = 200;
  enemy.x2 = enemy.x + 50;
  enemy.y2 = enemy.y + 50;

  CHAR enemy2;
  enemy2.x = 100;
  enemy2.y = 100;
  enemy2.x2 = enemy2.x + 50;
  enemy2.y2 = enemy2.y + 50;


  float vel = 15;

  float mouse_x = 0;
  float mouse_y = 0;

  int ponto = 0;

  unsigned char key[ALLEGRO_KEY_MAX];
  memset(key, 0, sizeof(key));


al_start_timer(timer);
while(!done) {
    al_wait_for_event(queue, &event);

    switch(event.type) {
        case ALLEGRO_EVENT_TIMER:
          if (game_over) {
        game_over_timer += 1.0 / FPS; 
        if(game_over_timer >= GAME_OVER_DURATION){
            done = true; 
        }
        redraw = true;
          }else {
          game_timer += 1.0 / FPS;
          for(int i = 0; i < BT_N; i++)
            {
              BOUNCER* b = &obj[i];
                 b->x += b->dx;
                 b->y += b->dy;

                 if(b->x < 0)
                  {
                    b->x  *= -1;
                    b->dx *= -1;
                  }
                 if(b->x > WIDTH)
                  {
                    b->x = 2*WIDTH - b->x;
                    b->dx *= -1;
                  }
                 if(b->y < 0)
                  {
                    b->y  *= -1;
                    b->dy *= -1;
                  }
                 if(b->y > HEIGHT)
                  {
                    b->y = 2*HEIGHT - b->y;
                    b->dy *= -1;
                  }
                }
          check_enemy_collision(&john, obj, BT_N, death);

          if(key[ALLEGRO_KEY_SPACE] && !isJumping){
            dy = jumpSpeed;
            isJumping = true;
          }
          if(isJumping){
            dy += GRAVITY;
            john.y += dy;
          }
          if(john.y >= GROUND){
            john.y = GROUND;
            dy = 0;
            isJumping = false;
          }else {
            john.y = GROUND;
          }

          if(key[ALLEGRO_KEY_A]){
            john.x -= vel;
            if(john.x < 0){
              john.x *= -1;
            }
          }

          if(key[ALLEGRO_KEY_D]){
            john.x += vel;
            if((john.x + 15) > WIDTH){
              john.x -= (john.x - WIDTH) * 2;
            }
          }

          john.x2 = john.x + 50;
          john.y2 = john.y + 50;

          if(key[ALLEGRO_KEY_P]){
            take_screenshot(display);
          }

          if(key[ALLEGRO_KEY_ESCAPE])
            done = true;

          for(int i = 0; i < ALLEGRO_KEY_MAX; i++)
            key[i] &= ~KEY_SEEN;

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

        case ALLEGRO_EVENT_MOUSE_AXES:
          // al_set_mouse_xy(display, WIDTH/2, HEIGHT/2);
          mouse_x = event.mouse.x;
          mouse_y = event.mouse.y;
          break;

        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
          if(event.mouse.button & 1){
            for(int i = 0; i < BT_N; i++) {
              BOUNCER* b = &obj[i];
              if(b->type == ENEMY_1 || b->type == ENEMY_2) {
                if(mouse_collide(mouse_x, mouse_y, b->x, b->y, WIDTH/4.0, HEIGHT/4.0)) {
                    ponto++;
                    al_play_sample(scream, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
                    break;
                }
              }
            }
          }
        }
          break;
    }

    if(done)
      break;

    if(redraw && al_is_event_queue_empty(queue)) {
      al_clear_to_color(al_map_rgb(0, 0, 0));

      al_draw_scaled_bitmap(bgImg, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      al_draw_scaled_bitmap(midImg, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      al_draw_scaled_bitmap(midImg2, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      al_draw_scaled_bitmap(midImg3, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      al_draw_scaled_bitmap(fgImg, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      al_draw_scaled_bitmap(fgImg2, 0, 0, 620, 360, 0, 0, WIDTH, HEIGHT, 0);
      

      for(int i = 0; i < BT_N; i++)
        {
          BOUNCER* b = &obj[i];
          switch(b->type)
            {
              case BT_CIRCLE: 
                al_draw_scaled_bitmap(sad_circle, 0, 0, 640, 640, b->x, b->y, WIDTH/4.0, HEIGHT/4.0, 0);
               break;

              case BT_RECTANGLE_2:
                al_draw_scaled_bitmap(bad_block, 0, 0, 640, 640, b->x, b->y, WIDTH/4.0, HEIGHT/4.0, 0);
               break;

              case BT_BLEND:
                al_draw_scaled_bitmap(grad_circle, 0, 0, 640, 640, b->x, b->y, WIDTH/4.0, HEIGHT/4.0, 0);
               break;
              
              case ENEMY_1:
                al_draw_scaled_bitmap(demon, 0, 0, 640, 640, b->x, b->y, WIDTH/4.0, HEIGHT/4.0, 0);
               break;

              case ENEMY_2:
                al_draw_scaled_bitmap(demon, 0, 0, 640, 640, b->x, b->y, WIDTH/4.0, HEIGHT/4.0, 0);
               break;

              case FRASE_1:
                al_draw_text(font, al_map_rgb(0, 0, 0), b->x, b->y, 0, "É TUDO CULPA SUA!");
                break;
              case FRASE_2:
                al_draw_text(font, al_map_rgb(0, 0, 0), b->x, b->y, 0, "DESISTA E MORRA!");
                break;
              case FRASE_3:
                al_draw_text(font, al_map_rgb(0, 0, 0), b->x, b->y, 0, "NAO TEM SAIDA!");
                break;
            }
        }

      al_draw_scaled_bitmap(herol, 0, 0, 640, 640, john.x, john.y, WIDTH/4.0, HEIGHT/4.0, 0);

      if (game_over) {
        al_draw_filled_rectangle(0, 0, WIDTH, HEIGHT, al_map_rgba(0, 0, 0, 200));
        al_draw_textf(game_over_font, al_map_rgb(255, 0, 0),WIDTH / 2.0, HEIGHT / 2.0 - 10.0, ALLEGRO_ALIGN_CENTER, "A MORTE É A UNICA SAIDA");
      }

      // al_draw_scaled_bitmap(heror, 0, 0, 640, 640, john.x, john.y, WIDTH/4.0, HEIGHT/4.0, 0);

      al_draw_scaled_bitmap(cross, 0, 0, 1024, 1024, mouse_x, mouse_y, 125, 125, 0);
      al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 10, 0, "Almas exorcizadas: %d", ponto);
      al_draw_textf(font, al_map_rgb(255, 255, 255), 10, 25, 0, "Tempo vivo: %.2f", game_timer);
      
      al_draw_text(font, al_map_rgb(255, 255, 255), WIDTH - 130, HEIGHT - 15, 0, "whynot?! Studios");

      al_flip_display();
      redraw = false;
    }
}
  
  al_destroy_display(display);
  al_destroy_event_queue(queue);
  al_destroy_timer(timer);
  al_destroy_font(font);
  al_destroy_font(game_over_font);
  al_destroy_sample(scream);
  al_destroy_sample(death);
  al_destroy_audio_stream(music);
  al_destroy_audio_stream(music_bg);
  al_destroy_bitmap(bgImg);
  al_destroy_bitmap(midImg);
  al_destroy_bitmap(midImg2);
  al_destroy_bitmap(midImg3);
  al_destroy_bitmap(fgImg);
  al_destroy_bitmap(fgImg2);
  al_destroy_bitmap(heror);
  al_destroy_bitmap(herol);
  al_destroy_bitmap(demon);
  al_destroy_bitmap(cross);
  al_destroy_bitmap(bad_block);
  al_destroy_bitmap(sad_circle);
  al_destroy_bitmap(grad_circle);

  return 0;
}

void take_screenshot(ALLEGRO_DISPLAY *display) {
    time_t rawtime;
    struct tm *timeinfo;
    char filename[64];
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(filename, sizeof(filename), "screenshot_%Y%m%d_%H%M%S.png", timeinfo);
    
    ALLEGRO_BITMAP *screenshot = al_create_bitmap(WIDTH, HEIGHT);
    al_set_target_bitmap(screenshot);
    al_draw_bitmap(al_get_backbuffer(display), 0, 0, 0);
    
    al_set_target_backbuffer(display);
    
    al_save_bitmap(filename, screenshot);
    printf("Screenshot salvo como: %s\n", filename);
    
    al_destroy_bitmap(screenshot);
}
