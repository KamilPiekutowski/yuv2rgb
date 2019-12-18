#include <stdio.h>
#include <iostream>
#include <bits/stdc++.h>
#include <pthread.h>
	  
const unsigned int  NUM_THREADS  = 16;
const unsigned int  BUFSIZE = 710 * 420 * 4 / NUM_THREADS;

void* yuv2rgb(unsigned char y, unsigned char u, unsigned char v,
        unsigned char* r, unsigned char *g, unsigned char *b);

void* test(void* a);

int main()
{

    int fakeBuff[BUFSIZE];
		pthread_t thr[NUM_THREADS];
    int rc;

		for(int k=0; k < 60 * 20; k++)
		{
		    for(int j=0; j < 60; j++)
		    {
				    unsigned char a;

  					for (int x = 0; x < NUM_THREADS; ++x) {
    						a = x;
    						if ((rc = pthread_create(&thr[x], NULL, test, &a))) 
								     {
      							     fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
      								   return EXIT_FAILURE;
    								 }
						}

						for (int x = 0; x < NUM_THREADS; ++x) 
						{
								pthread_join(thr[x], NULL);
						}
		    }
		}

		return 0;
}

void* yuv2rgb(unsigned char y, unsigned char u, unsigned char v,
        unsigned char* r, unsigned char *g, unsigned char *b) {
    int rTmp =  y + (1.370705 * (float) (v-128));
    int gTmp =  y - (0.698001 * (float) (v-128)) - (0.337633 * (float) ( u-128));
    int bTmp =  y + (1.732446 * (float) (u-128));
		
    *r = std::clamp(rTmp, 0, 255);
    *g = std::clamp(gTmp, 0, 255);
    *b = std::clamp(bTmp, 0, 255);
}


void* test(void* a)
{
	  unsigned char b = 10* *(unsigned char*) a;
    //printf("running thread\n");
		for(int i = 0; i < BUFSIZE; i++)
	  {
				 yuv2rgb(1,1,1,&b,&b,&b);
		}

		pthread_exit(NULL);
}
