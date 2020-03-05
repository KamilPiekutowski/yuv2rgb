# yuv2rgb

The program is an example of  yuv420 converter that utilizes 2 plane 
with u and v values interleaved side by side. 

To compile:

> ./compile.sh

To run:

> ./yuv2rgb

TODO:

- optimize,
- optimize neon
- offload to dsp c66 (EVM572x specific)
- merge all into one app and make it more generic use
