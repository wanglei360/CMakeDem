package com.kts.testjni

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.os.Message
import android.util.Log
import android.view.View
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.kts.testjni.databinding.ActivityMainBinding
import com.kts.testjni.jni.JNIBitmap
import com.kts.testjni.jni.JniDemo
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext

/**
 * CMake的部分是抄的下面这个GitHub的
 * github:https://github.com/ta893115871/JNIAPP
 */
class MainActivity : AppCompatActivity() {

    private var num = 0
    private val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }
    private val scope by lazy { CoroutineScope(Dispatchers.Default) }

    private val handler by lazy {
        object : Handler(Looper.getMainLooper()) {
            override fun handleMessage(msg: Message) {
                super.handleMessage(msg)
                if (msg.arg1 == 1) {
                    binding.iv.setImageBitmap(msg.obj as Bitmap)
                    binding.pb.visibility = View.GONE
                    binding.tv.text =
                        when (msg.arg2) {
                            0 -> "水墨式的"
                            1 -> "黑白式的"
                            2 -> "底片式的"
                            3 -> "左右调转"
                            4 -> "高斯模糊的样式"
                            else -> "原图"
                        }
                }
            }
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        binding.iv.setImageBitmap(getBitmap())
    }

    fun jniTest(view: View) {
        val result = JniDemo().sayHello()
        println(result)
        Toast.makeText(this, result, Toast.LENGTH_LONG).show()
    }

    fun jniBitmap(view: View) {
        if (binding.pb.visibility == View.VISIBLE) {
            Toast.makeText(this, "等一会", Toast.LENGTH_LONG).show()
            return
        }

        val startTime = System.currentTimeMillis()
        binding.pb.visibility = View.VISIBLE

        getBitmap().also { bitmap ->
            scope.launch {
                withContext(Dispatchers.IO) {
                    when (num) {
                        0 -> sendMsg(JNIBitmap().blackAndWhite(bitmap), num, bitmap)
                        1 -> sendMsg(JNIBitmap().gray(bitmap), num, bitmap)
                        2 -> sendMsg(JNIBitmap().negative(bitmap), num, bitmap)
                        3 -> {
                            val leftRight = JNIBitmap().leftRight(bitmap)
                            sendMsg(if (leftRight == null) -1 else 1, num, leftRight)
                        }
                        4 -> sendMsg(JNIBitmap().blurBitmap(bitmap, 500), num, bitmap)
                        else -> sendMsg(1, num, bitmap)
                    }
                    val endTime = System.currentTimeMillis()

                    Log.i("hahah", "时间是：${endTime - startTime} 毫秒")
                }
                num++
                if (num == 6) num = 0
            }
        }
    }

    private fun getBitmap(): Bitmap = BitmapFactory.decodeResource(resources, R.mipmap.bbb)

    private fun sendMsg(stateCode: Int, identification: Int, bm: Bitmap?) {
        val msg = Message.obtain()
        msg.arg1 = stateCode
        msg.arg2 = identification
        msg.obj = bm
        handler.sendMessage(msg)
    }
}