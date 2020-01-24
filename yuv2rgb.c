#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define WIDTH 720
#define HEIGHT 406
#define YPL_SIZE WIDTH * HEIGHT
#define UVPL_SIZE WIDTH * HEIGHT 
#define RGBPL_SIZE WIDTH * HEIGHT * 3 //3 bytes per pixel 

typedef unsigned char uchar_t;

float clamp(float d, float min, float max);
void convert_cpu(uchar_t* y_plane,uchar_t* uv_plane, 
            uchar_t* rgb_plane);
void yuv2rgb(uint8_t y, uint8_t u, uint8_t v,
        uint8_t *r, uint8_t *g, uint8_t *b);

int main(int argc, char** argv)
{
   FILE* fd;
   uchar_t y_plane[YPL_SIZE];
   uchar_t uv_plane[UVPL_SIZE];
   uchar_t rgb_plane[RGBPL_SIZE];
   
   //get uv-plane frome the file frame192.y
   fd = fopen("frame192.y", "r");
   if(!fd)
   {
      printf("Error opening frame192.y\n");
      return -1;
   }

   fread(y_plane, YPL_SIZE, 1, fd);;
   fclose(fd);

  
   fd = fopen("frame192.uv", "r");
   if(!fd)
   {
      printf("Error opening frame192.uv\n");
      return -1;
   }

   fread(uv_plane, UVPL_SIZE, 1, fd);;
   fclose(fd);
   
   convert_cpu(y_plane, uv_plane, rgb_plane);

#ifdef DEBUG
   printf("Done converting.\n");
#endif

   
   fd = fopen("frame192.data", "w+");
   if(!fd)
   {
      printf("Error opening frame192.uv\n");
      return -1;
   }

   fwrite(rgb_plane, RGBPL_SIZE, 1, fd);;
   fclose(fd);

#ifdef DEBUG
   printf("Done writing to file.\n");
#endif

   return 0;
}

float clamp(float d, float min, float max) 
{
   const float t = d < min ? min : d;
   return t > max ? max : t;
}

void yuv2rgb(uint8_t y, uint8_t u, uint8_t v, 
        uint8_t *r, uint8_t *g, uint8_t *b)
{
   float r_tmp = (float) y + (1.370705 * (float) (v-128));
   float g_tmp = (float) y - (0.698001 * (float) (v-128)) - (0.337633 * (u-128));
   float b_tmp = (float) y + (1.732446 * (float) (u-128));
   *r = (uint8_t) clamp(r_tmp, 0.0f, 255.0f);
   *g = (uint8_t) clamp(g_tmp, 0.0f, 255.0f);
   *b = (uint8_t) clamp(b_tmp, 0.0f, 255.0f);
#ifdef DEBUG
      if(*b > 250)
      {
         printf("b value overflow %d\n ", *b);
         printf("b_tmp value overflow %4.1f\n ", b_tmp);
      }
#endif
}

void convert_cpu(uchar_t* y_plane,uchar_t* uv_plane, 
            uchar_t* rgb_plane)
{
   uchar_t v_plane[UVPL_SIZE/4];
   uchar_t u_plane[UVPL_SIZE/4];

   for(int i =0; i < UVPL_SIZE/2; i += 2 )
   {
      u_plane[i/2] = uv_plane[i];
#ifdef DEBUG
      //printf("UVPL_SIZE / 2 = %d, i/2 %d\n", UVPL_SIZE/2, i/2);
#endif
   }

   for(int i =0; i < UVPL_SIZE/2; i += 2 )
   {
      v_plane[i/2] = uv_plane[i+1];
   }

   for(int i =0; i < UVPL_SIZE; i++)
   {
      uint8_t y;
      uint8_t u;
      uint8_t v;

      uint8_t r;
      uint8_t g;
      uint8_t b;

      int w = (i % WIDTH) / 2;
      int h = (i / WIDTH) / 2;
   
      int width_v_plane = WIDTH/2;

      int idx = w + (width_v_plane * h);

      y = y_plane[i];
      
      u = u_plane[idx];
      v = v_plane[idx];
    
      yuv2rgb(y, u, v, &r, &g, &b);

      rgb_plane[3*i + 0] = r;
      rgb_plane[3*i + 1] = g;
      rgb_plane[3*i + 2] = b;

#ifdef DEBUG
      //printf("RGB = (%d,%d,%d)\n", r,g,b);
      //printf("i = (%d), idx = (%d)\n",i, idx);
      //printf("v_plane size = (%d)\n", UVPL_SIZE/4);
      //printf("(w,h) = (%d,%d)\n", w, h);
#endif
   }
}
