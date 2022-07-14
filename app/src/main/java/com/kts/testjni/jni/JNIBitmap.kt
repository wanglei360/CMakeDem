package com.kts.testjni.jni

import android.graphics.Bitmap

/**
 * Created by guxiuzhong on 2020/9/24.
 */
class JNIBitmap {
    companion object {
        init {
            System.loadLibrary("Hello")
        }
    }

    /**
     * 传入的Bitmap设置成只有黑白色的
     * @return 成功返回1
     */
    external fun blackAndWhite(bitmap: Bitmap): Int

    /**
     * 传入的Bitmap设置成黑白照片
     * @return 成功返回1
     */
    external fun gray(bitmap: Bitmap): Int

    /**
     * 传入的Bitmap设置成底片的样式
     * @return 成功返回1
     */
    external fun negative(bitmap: Bitmap): Int

    /**
     * 传入的Bitmap设置成高斯模糊的样式
     * @return 成功返回1
     */
    external fun blurBitmap(bitmap: Bitmap, r: Int): Int

    /**
     * @return 把传入的bitmap左右调转返回
     */
    external fun leftRight(bitmap: Bitmap): Bitmap?
}