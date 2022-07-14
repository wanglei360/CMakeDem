//
// Created by Gu,Xiuzhong on 2020/9/24.
//

#include "BitmapUtil.h"
#include <malloc.h>

#define ABS(a) ((a)<(0)?(-a):(a))
#define MAX(a, b) ((a)>(b)?(a):(b))
#define MIN(a, b) ((a)<(b)?(a):(b))

jobject createBitmap(JNIEnv *env, uint32_t w, uint32_t h);

int *blur_ARGB_8888(int *pix, int w, int h, int radius);

short *blur_RGB_565(short *pix, int w, int h, int radius);

int BitmapUtil::gray(JNIEnv *env, jobject bitmap) {

    AndroidBitmapInfo bitmapInfo;
    // 获取bitmap的属性信息
    int ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_getInfo %d", ret);
        return JNI_FALSE;
    }
    void *bitmapPixels;
    int pixRet = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels);
    if (pixRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels %d", pixRet);
        return JNI_FALSE;
    }
    int w = bitmapInfo.width;
    int h = bitmapInfo.height;

    uint32_t *srcPix = (uint32_t *) bitmapPixels;

    // 在C层中，Bitmap像素点的值是ABGR，而不是ARGB，也就是说，高端到低端：A，B，G，R
    // 变灰白
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            uint32_t color = srcPix[w * i + j];
            uint32_t blue = (color >> 16) & 0xFF;
            uint32_t green = (color >> 8) & 0xFF;
            uint32_t red = color & 0xFF;
            uint32_t alpha = (color >> 24) & 0xFF;

            // 均值法 公式  GRAY = (RED+BLUE+GREEN)/3
            // int32_t gray = (red + green + blue) / 3;
            // 加权平均法 常用公式 Gray = R * 0.299 + G * 0.587 + B * 0.114
            uint32_t gray = (int) (red * 0.3f + green * 0.59f + blue * 0.11f);

            uint32_t newColor = (alpha << 24) | (gray << 16) | (gray << 8) | gray;

            srcPix[w * i + j] = newColor;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    return JNI_TRUE;
}

int BitmapUtil::blackAndWhite(JNIEnv *env, jobject bitmap) {

    AndroidBitmapInfo bitmapInfo;
    // 获取bitmap的属性信息
    int ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_getInfo %d", ret);
        return JNI_FALSE;
    }
    void *bitmapPixels;
    int pixRet = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels);
    if (pixRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels %d", pixRet);
        return JNI_FALSE;
    }
    int w = bitmapInfo.width;
    int h = bitmapInfo.height;

    uint32_t *srcPix = (uint32_t *) bitmapPixels;

    // 在C层中，Bitmap像素点的值是ABGR，而不是ARGB，也就是说，高端到低端：A，B，G，R
    // 求RGB平均值Avg ＝ (R + G + B) / 3，如果Avg >= 100，则新的颜色值为R＝G＝B＝255；如果Avg < 100，
    // 则新的颜色值为R＝G＝B＝0；255就是白色，0就是黑色；
    // 至于为什么用100作比较，这是一个经验值可以根据效果来调整。
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            uint32_t color = srcPix[w * i + j];
            uint32_t blue = (color >> 16) & 0xFF;
            uint32_t green = (color >> 8) & 0xFF;
            uint32_t red = color & 0xFF;
            uint32_t alpha = (color >> 24) & 0xFF;

            uint32_t gray = (int) (red * 0.3f + green * 0.59f + blue * 0.11f);

            gray = gray >= 100 ? 255 : 0;

            uint32_t newColor = (alpha << 24) | (gray << 16) | (gray << 8) | gray;

            srcPix[w * i + j] = newColor;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    return JNI_TRUE;
}

int BitmapUtil::negative(JNIEnv *env, jobject bitmap) {
    AndroidBitmapInfo bitmapInfo;
    // 获取bitmap的属性信息
    int ret = AndroidBitmap_getInfo(env, bitmap, &bitmapInfo);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_getInfo %d", ret);
        return JNI_FALSE;
    }
    void *bitmapPixels;
    int pixRet = AndroidBitmap_lockPixels(env, bitmap, &bitmapPixels);
    if (pixRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels %d", pixRet);
        return JNI_FALSE;
    }
    int w = bitmapInfo.width;
    int h = bitmapInfo.height;

    uint32_t *srcPix = (uint32_t *) bitmapPixels;

    // 在C层中，Bitmap像素点的值是ABGR，而不是ARGB，也就是说，高端到低端：A，B，G，R
    // 底片效果算法原理：将当前像素点的RGB值分别与255之差后的值作为当前点的RGB值，即
    // R = 255 – R；G = 255 – G；B = 255 – B；
    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
            uint32_t color = srcPix[w * i + j];
            uint32_t blue = (color >> 16) & 0xFF;
            uint32_t green = (color >> 8) & 0xFF;
            uint32_t red = color & 0xFF;
            uint32_t alpha = (color >> 24) & 0xFF;

            if (i == 0 && j == 0) {
                LOG_D("jni color %d=%x", color, color);
                LOG_D("jni red %d=%x", red, red);
                LOG_D("jni green %d=%x", green, green);
                LOG_D("jni blue %d=%x", blue, blue);
                LOG_D("jni alpha %d=%x", alpha, alpha);
            }

            // 暖色系。就是加强R/G的分量
//            green += 60;
//            if (green >= 255) {
//                green = 255;
//            }
//            red += 150;
//            if (red >= 255) {
//                red = 255;
//            }

            // 冷色系。就是增加B的分量
//            blue += 50;
//            if (blue >= 255) {
//                blue = 255;
//            }


            red = 255 - red;
            green = 255 - green;
            blue = 255 - blue;

            uint32_t newColor =
                    (alpha << 24) | ((blue << 16)) | ((green << 8)) | red;

            if (i == 0 & j == 0) {
                LOG_D("jni newColor %d=%x", newColor, newColor);
            }

            srcPix[w * i + j] = newColor;
        }
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    return JNI_TRUE;
}

jobject BitmapUtil::leftRight(JNIEnv *env, jobject srcBitmap) {
    AndroidBitmapInfo bitmapInfo;
    int ret = AndroidBitmap_getInfo(env, srcBitmap, &bitmapInfo);
    if (ret != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_getInfo %d", ret);
        return nullptr;
    }
    LOG_D("width %d", bitmapInfo.width);
    LOG_D("height %d", bitmapInfo.height);
    LOG_D("format %d", bitmapInfo.format);
    LOG_D("stride %d", bitmapInfo.stride);
    void *bitmapPixels;
    int pixRet = AndroidBitmap_lockPixels(env, srcBitmap, &bitmapPixels);
    if (pixRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels %d", pixRet);
        return nullptr;
    }
    int w = bitmapInfo.width;
    int h = bitmapInfo.height;

    uint32_t *newBitmapPixels = new uint32_t[w * h];

    // 左右翻转
    int whereToGet = 0;
    for (int i = 0; i < h; i++) {
        for (int j = w - 1; j >= 0; j--) {
            uint32_t color = ((uint32_t *) bitmapPixels)[whereToGet++];
            newBitmapPixels[w * i + j] = color;
        }
    }
    AndroidBitmap_unlockPixels(env, srcBitmap);

    jobject newBitmap = createBitmap(env, w, h);


    void *resultBitmapPixels;
    pixRet = AndroidBitmap_lockPixels(env, newBitmap, &resultBitmapPixels);
    if (pixRet != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels %d", pixRet);
        return nullptr;
    }
    memcpy(resultBitmapPixels, newBitmapPixels, sizeof(uint32_t) * w * h);

    delete[]  newBitmapPixels;
    AndroidBitmap_unlockPixels(env, newBitmap);
    return newBitmap;
}

jobject createBitmap(JNIEnv *env, uint32_t w, uint32_t h) {
    jclass clsBp = env->FindClass("android/graphics/Bitmap");
    jmethodID createBitmapMid = env->GetStaticMethodID(clsBp, "createBitmap",
                                                       "(IILandroid/graphics/Bitmap$Config;)Landroid/graphics/Bitmap;");
    if (createBitmapMid == nullptr) {
        LOG_E("createBitmapMid nullptr");
        return nullptr;
    }

    jclass clsConfig = env->FindClass("android/graphics/Bitmap$Config");
    if (clsConfig == nullptr) {
        LOG_E("clsConfig nullptr");
        return nullptr;
    }
    jmethodID valueOfMid = env->GetStaticMethodID(clsConfig, "valueOf",
                                                  "(Ljava/lang/String;)Landroid/graphics/Bitmap$Config;");
    if (valueOfMid == nullptr) {
        LOG_E("valueOfMid nullptr");
        return nullptr;
    }
    jstring configName = env->NewStringUTF("ARGB_8888");
    jobject bitmapConfig = env->CallStaticObjectMethod(clsConfig, valueOfMid, configName);
    jobject newBitmap = env->CallStaticObjectMethod(clsBp, createBitmapMid, w, h, bitmapConfig);
    return newBitmap;
}

int BitmapUtil::blurBitmap(JNIEnv *env, jobject bitmapIn, jint r) {
    AndroidBitmapInfo infoIn;
    void *pixels;

    // Get image info
    if (AndroidBitmap_getInfo(env, bitmapIn, &infoIn) != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_getInfo failed!");
        return JNI_FALSE;
    }

    // Check image
    if (infoIn.format != ANDROID_BITMAP_FORMAT_RGBA_8888 &&
        infoIn.format != ANDROID_BITMAP_FORMAT_RGB_565) {
        LOG_D("Only support ANDROID_BITMAP_FORMAT_RGBA_8888 and ANDROID_BITMAP_FORMAT_RGB_565");
        return JNI_FALSE;
    }

    // Lock all images
    if (AndroidBitmap_lockPixels(env, bitmapIn, &pixels) != ANDROID_BITMAP_RESULT_SUCCESS) {
        LOG_D("AndroidBitmap_lockPixels failed!");
        return JNI_FALSE;
    }
    // height width
    int h = infoIn.height;
    int w = infoIn.width;

    // Start
    if (infoIn.format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
        pixels = blur_ARGB_8888((int *) pixels, w, h, r);
    } else if (infoIn.format == ANDROID_BITMAP_FORMAT_RGB_565) {
        pixels = blur_RGB_565((short *) pixels, w, h, r);
    }

    // End

    // Unlocks everything
    AndroidBitmap_unlockPixels(env, bitmapIn);
    return JNI_TRUE;
}

int *blur_ARGB_8888(int *pix, int w, int h, int radius) {
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;

    short *r = (short *) malloc(wh * sizeof(short));
    short *g = (short *) malloc(wh * sizeof(short));
    short *b = (short *) malloc(wh * sizeof(short));
    int rsum, gsum, bsum, x, y, i, p, yp, yi, yw;

    int *vmin = (int *) malloc(MAX(w, h) * sizeof(int));

    int divsum = (div + 1) >> 1;
    divsum *= divsum;
    short *dv = (short *) malloc(256 * divsum * sizeof(short));
    for (i = 0; i < 256 * divsum; i++) {
        dv[i] = (short) (i / divsum);
    }

    yw = yi = 0;

    int(*stack)[3] = (int (*)[3]) malloc(div * 3 * sizeof(int));
    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum;
    int rinsum, ginsum, binsum;

    for (y = 0; y < h; y++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++) {
            p = pix[yi + (MIN(wm, MAX(i, 0)))];
            sir = stack[i + radius];
            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rbs = r1 - ABS(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;

        for (x = 0; x < w; x++) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (y == 0) {
                vmin[x] = MIN(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];

            sir[0] = (p & 0xff0000) >> 16;
            sir[1] = (p & 0x00ff00) >> 8;
            sir[2] = (p & 0x0000ff);

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++) {
            yi = MAX(0, yp) + x;

            sir = stack[i + radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];

            rbs = r1 - ABS(i);

            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;

            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }

            if (i < hm) {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++) {
            // Preserve alpha channel: ( 0xff000000 & pix[yi] )
            pix[yi] = (0xff000000 & pix[yi]) | (dv[rsum] << 16) | (dv[gsum] << 8) | dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (x == 0) {
                vmin[y] = MIN(y + r1, hm) * w;
            }
            p = x + vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi += w;
        }
    }

    free(r);
    free(g);
    free(b);
    free(vmin);
    free(dv);
    free(stack);
    return (pix);
}

short *blur_RGB_565(short *pix, int w, int h, int radius) {
    int wm = w - 1;
    int hm = h - 1;
    int wh = w * h;
    int div = radius + radius + 1;

    short *r = (short *) malloc(wh * sizeof(short));
    short *g = (short *) malloc(wh * sizeof(short));
    short *b = (short *) malloc(wh * sizeof(short));

    int rsum, gsum, bsum, x, y, p, i, yp, yi, yw;

    int *vmin = (int *) malloc(MAX(w, h) * sizeof(int));

    int divsum = (div + 1) >> 1;
    divsum *= divsum;

    short *dv = (short *) malloc(256 * divsum * sizeof(short));

    for (i = 0; i < 256 * divsum; i++) {
        dv[i] = (short) (i / divsum);
    }

    yw = yi = 0;

    int(*stack)[3] = (int (*)[3]) malloc(div * 3 * sizeof(int));
    int stackpointer;
    int stackstart;
    int *sir;
    int rbs;
    int r1 = radius + 1;
    int routsum, goutsum, boutsum;
    int rinsum, ginsum, binsum;

    for (y = 0; y < h; y++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        for (i = -radius; i <= radius; i++) {
            p = pix[yi + (MIN(wm, MAX(i, 0)))];
            sir = stack[i + radius];
            sir[0] = (((p) & 0xF800) >> 11) << 3;
            sir[1] = (((p) & 0x7E0) >> 5) << 2;
            sir[2] = ((p) & 0x1F) << 3;

            rbs = r1 - ABS(i);
            rsum += sir[0] * rbs;
            gsum += sir[1] * rbs;
            bsum += sir[2] * rbs;
            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }
        }
        stackpointer = radius;

        for (x = 0; x < w; x++) {

            r[yi] = dv[rsum];
            g[yi] = dv[gsum];
            b[yi] = dv[bsum];

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (y == 0) {
                vmin[x] = MIN(x + radius + 1, wm);
            }
            p = pix[yw + vmin[x]];

            sir[0] = (((p) & 0xF800) >> 11) << 3;
            sir[1] = (((p) & 0x7E0) >> 5) << 2;
            sir[2] = ((p) & 0x1F) << 3;

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[(stackpointer) % div];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi++;
        }
        yw += w;
    }
    for (x = 0; x < w; x++) {
        rinsum = ginsum = binsum = routsum = goutsum = boutsum = rsum = gsum = bsum = 0;
        yp = -radius * w;
        for (i = -radius; i <= radius; i++) {
            yi = MAX(0, yp) + x;

            sir = stack[i + radius];

            sir[0] = r[yi];
            sir[1] = g[yi];
            sir[2] = b[yi];

            rbs = r1 - ABS(i);

            rsum += r[yi] * rbs;
            gsum += g[yi] * rbs;
            bsum += b[yi] * rbs;

            if (i > 0) {
                rinsum += sir[0];
                ginsum += sir[1];
                binsum += sir[2];
            } else {
                routsum += sir[0];
                goutsum += sir[1];
                boutsum += sir[2];
            }

            if (i < hm) {
                yp += w;
            }
        }
        yi = x;
        stackpointer = radius;
        for (y = 0; y < h; y++) {
            // Not have alpha channel
            pix[yi] = ((((dv[rsum]) >> 3) << 11) | (((dv[gsum]) >> 2) << 5) | ((dv[bsum]) >> 3));

            rsum -= routsum;
            gsum -= goutsum;
            bsum -= boutsum;

            stackstart = stackpointer - radius + div;
            sir = stack[stackstart % div];

            routsum -= sir[0];
            goutsum -= sir[1];
            boutsum -= sir[2];

            if (x == 0) {
                vmin[y] = MIN(y + r1, hm) * w;
            }
            p = x + vmin[y];

            sir[0] = r[p];
            sir[1] = g[p];
            sir[2] = b[p];

            rinsum += sir[0];
            ginsum += sir[1];
            binsum += sir[2];

            rsum += rinsum;
            gsum += ginsum;
            bsum += binsum;

            stackpointer = (stackpointer + 1) % div;
            sir = stack[stackpointer];

            routsum += sir[0];
            goutsum += sir[1];
            boutsum += sir[2];

            rinsum -= sir[0];
            ginsum -= sir[1];
            binsum -= sir[2];

            yi += w;
        }
    }

    free(r);
    free(g);
    free(b);
    free(vmin);
    free(dv);
    free(stack);
    return (pix);
}
// 高斯模糊
// https://github.com/qiujuer/ImageBlurring/blob/master/README-ZH.md