#include "cstl.h"

// stl版本的音频数据帧
typedef struct{
    vector_t* data;
    time_t time;
}stl_audio_pcm;


// 普通版本的音频数据帧
typedef struct{
    int16_t data[1024];
    time_t time;
}no_stl_audio_pcm;



