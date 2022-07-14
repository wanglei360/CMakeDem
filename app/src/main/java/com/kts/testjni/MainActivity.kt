package com.kts.testjni

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.util.Log
import android.view.View
import android.widget.ImageView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.kts.testjni.jni.JNIBitmap
import com.kts.testjni.jni.JniDemo

/**
 * CMake的部分是抄的下面这个GitHub的
 * github:https://github.com/ta893115871/JNIAPP
 */
class MainActivity : AppCompatActivity() {

    private val imageView by lazy { findViewById<ImageView>(R.id.iv) }

    private var num = 0

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        imageView.setImageBitmap(getBitmap())
    }

    fun jniTest(view: View) {
//         创建JNI实例，并调用本地声明的方法
        val result = JniDemo().sayHello()
        println(result)
        Toast.makeText(this, result, Toast.LENGTH_LONG).show()
    }

    fun jniBitmap(view: View) {
        val startTime = System.currentTimeMillis()
        getBitmap().also { bitmap ->
            when (num) {
                0 -> {
                    if (JNIBitmap().blackAndWhite(bitmap) == 1)
                        imageView.setImageBitmap(bitmap)
                    num = 1
                }
                1 -> {
                    if (JNIBitmap().gray(bitmap) == 1)
                        imageView.setImageBitmap(bitmap)
                    num = 2
                }
                2 -> {
                    if (JNIBitmap().negative(bitmap) == 1)
                        imageView.setImageBitmap(bitmap)
                    num = 3
                }
                3 -> {
                    imageView.setImageBitmap(JNIBitmap().leftRight(bitmap))
                    num = 4
                }
                4 -> {
                    if (JNIBitmap().blurBitmap(bitmap, 50) == 1)
                        imageView.setImageBitmap(bitmap)
                    num = 5
                }
                else -> {
                    imageView.setImageBitmap(bitmap)
                    num = 0
                }
            }
        }

        val endTime = System.currentTimeMillis()
        Log.i("hahah", "时间是：${endTime - startTime} 毫秒")
    }

    private fun getBitmap(): Bitmap = BitmapFactory.decodeResource(resources, R.mipmap.bbb)

}