#include <jni.h>


/**
 * jstring:  返回值
 * Java_全类名_方法名
 * Java_com_kts_testjni_JniDemo_setBimap
 * @param env 里面有很多方法
 * @param jobj 谁调用了这个方法就是谁的实例
 * @return 固定字符串
 */
extern "C"
JNIEXPORT jstring
Java_com_kts_testjni_jni_JniDemo_sayHello(JNIEnv *env, jobject thiz) {
    return env->NewStringUTF("我是C++里面的代码 ");
}


#include "BitmapUtil.h"


extern "C"
JNIEXPORT jint
Java_com_kts_testjni_jni_JNIBitmap_blackAndWhite(JNIEnv *env, jobject thiz, jobject bitmap) {
    auto bitmapUtil = new BitmapUtil;
    int ret = bitmapUtil->blackAndWhite(env, bitmap);
    delete bitmapUtil;
    return ret;
}

extern "C"
JNIEXPORT jint
Java_com_kts_testjni_jni_JNIBitmap_gray(JNIEnv *env, jobject thiz, jobject bitmap) {
    auto bitmapUtil = new BitmapUtil;
    int ret = bitmapUtil->gray(env, bitmap);
    delete bitmapUtil;
    return ret;
}

extern "C"
JNIEXPORT jint
Java_com_kts_testjni_jni_JNIBitmap_negative(JNIEnv *env, jobject thiz, jobject bitmap) {
    auto bitmapUtil = new BitmapUtil;
    int ret = bitmapUtil->negative(env, bitmap);
    delete bitmapUtil;
    return ret;
}

extern "C"
JNIEXPORT jint
Java_com_kts_testjni_jni_JNIBitmap_blurBitmap(JNIEnv *env, jobject bitmap, jobject bm, int er) {
    auto bitmapUtil = new BitmapUtil;
    int ret = bitmapUtil->blurBitmap(env, bm, er);
    delete bitmapUtil;
    return ret;
}

extern "C"
JNIEXPORT jobject
Java_com_kts_testjni_jni_JNIBitmap_leftRight(JNIEnv *env, jobject thiz, jobject bitmap) {
    auto bitmapUtil = new BitmapUtil;
    jobject obj = bitmapUtil->leftRight(env, bitmap);
    delete bitmapUtil;
    return obj;
}