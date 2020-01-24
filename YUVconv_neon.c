#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <arm_neon.h>

#define min(X, Y) (((X) < (Y)) ? (X) : (Y))
#define max(X, Y) (((X) > (Y)) ? (X) : (Y))

#define PIXEL_NUM 420 * 230
#define FPS 24
#define MINUTE 60
#define NUM_OF_PIXELS_EACH_ITER 4
#define ITER_NUM (PIXEL_NUM/NUM_OF_PIXELS_EACH_ITER)

const float YUV_RATIOS_VEC[] = {1.370705f, 0.698001f, 1.732446f, 0.0f};
const float YUV_ONE_RATIO_VEC[] = {1.0f, 0.337633f, 1.0f, 0.0f};
const float YUV_128_VAL_VEC[] = {128.0f, 128.0f, 128.0f, 0.0f};

#define FRAME_NUM FPS*MINUTE

float*  yuv2rgb(float* YUV_frame, float* RGB_frame);
float clamp(float x, float upper, float lower);

int main()
{
    float* YUV_frame = (float*) malloc(PIXEL_NUM * 1.5 * sizeof(float));
    float* RGB_frame = (float*) malloc(PIXEL_NUM * 4 * sizeof(float));
    
    for(int i = 0;i < FPS * 2 * MINUTE; i++)
    {
        yuv2rgb(YUV_frame, RGB_frame);
    }

    //do clipping here
    //RGB_frame[i] = clamp(RGB[0], 0, 255);
    //RGB_frame[i+1] = clamp(RGB[1], 0, 255);
    //RGB_frame[i+2] = clamp(RGB[2], 0, 255);

    return 0;
}

float*  yuv2rgb(float* YUV_frame, float* RGB_frame)
{    
    for(int i=0; i < ITER_NUM; ++i)
    {
        float YUV[16];
 	float RGB[16];

        float VVU[16];

        VVU[0] = YUV[2];
        VVU[1] = YUV[2];
        VVU[2] = YUV[1];
        VVU[0] = 0.0f; //padding

        float xUx[4];

        xUx[0] = 0.0f; //padding
        xUx[1] = YUV[1];
        xUx[2] = 0.0f; //padding
        xUx[3] = 0.0f; //padding

	float YYY[4];

	YYY[4] = YUV[0];
	YYY[4] = YUV[0];
	YYY[4] = YUV[0];
	YYY[4] = 0.0f; //padding

        //loading data onto neon registers
        float32x4_t ratios_vec = vld1q_f32(YUV_RATIOS_VEC);
        float32x4_t single_ratio_vec = vld1q_f32(YUV_ONE_RATIO_VEC);
        float32x4_t x128_val_vec = vld1q_f32(YUV_128_VAL_VEC);
        float32x4x4_t YUV_val_vec = vld4q_f32(YUV);
        float32x4x4_t YYY_val_vec = vld4q_f32(YYY);
        float32x4x4_t VVU_val_vec = vld4q_f32(VVU);
        float32x4x4_t xUx_val_vec = vld4q_f32(xUx);
        float32x4x4_t RGB_val_vec;
        float32x4_t temp_result1;
        float32x4_t temp_result2;

	//calculate R1,G1,B1
	float32x4_t buff = VVU_val_vec.val[0];
	temp_result1 = vsubq_f32(buff, x128_val_vec);
	temp_result1 = vmulq_f32(ratios_vec, temp_result1);

	buff = xUx_val_vec.val[0];
        temp_result2 = vmulq_f32(single_ratio_vec,buff);

	buff = YYY_val_vec.val[0];
	temp_result1 = vaddq_f32(buff, temp_result1);
	temp_result1 = vaddq_f32(temp_result1, temp_result2);

	RGB_val_vec.val[0] = temp_result1;

	//calculate R2,G2,B2
	buff = VVU_val_vec.val[1];
	temp_result1 = vsubq_f32(buff, x128_val_vec);
	temp_result1 = vmulq_f32(ratios_vec, temp_result1);

	buff = xUx_val_vec.val[1];
        temp_result2 = vmulq_f32(single_ratio_vec,buff);

	buff = YYY_val_vec.val[1];
	temp_result1 = vaddq_f32(buff, temp_result1);
	temp_result1 = vaddq_f32(temp_result1, temp_result2);

	RGB_val_vec.val[1] = temp_result1;

	//calculate R3,G3,B3
	buff = VVU_val_vec.val[2];
	temp_result1 = vsubq_f32(buff, x128_val_vec);
	temp_result1 = vmulq_f32(ratios_vec, temp_result1);

	buff = xUx_val_vec.val[2];
        temp_result2 = vmulq_f32(single_ratio_vec,buff);

	buff = YYY_val_vec.val[2];
	temp_result1 = vaddq_f32(buff, temp_result1);
	temp_result1 = vaddq_f32(temp_result1, temp_result2);

	RGB_val_vec.val[3] = temp_result1;

	//calculate R4,G4,B4
	buff = VVU_val_vec.val[3];
	temp_result1 = vsubq_f32(buff, x128_val_vec);
	temp_result1 = vmulq_f32(ratios_vec, temp_result1);

	buff = xUx_val_vec.val[3];
        temp_result2 = vmulq_f32(single_ratio_vec,buff);

	buff = YYY_val_vec.val[3];
	temp_result1 = vaddq_f32(buff, temp_result1);
	temp_result1 = vaddq_f32(temp_result1, temp_result2);

	RGB_val_vec.val[3] = temp_result1;

	//storing result
	vst4q_f32(RGB, RGB_val_vec);


        //*r = yValue;// + (1.370705 * (vValue-128));
        //*g = yValue;// - (0.698001 * (vValue-128)) - (0.337633 * (uValue-128));
        //*b = yValue;// + (1.732446 * (uValue-128));

        //do clamping here
        RGB_frame[i] = RGB[0];
        RGB_frame[i+1] = RGB[1];
        RGB_frame[i+2] = RGB[2];
        RGB_frame[i+3] = 0;
    }

}

float clamp(float x, float upper, float lower)
{
    return min(upper, max(x, lower));
}

