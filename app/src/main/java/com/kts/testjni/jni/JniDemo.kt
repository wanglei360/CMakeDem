package com.kts.testjni.jni

/**
 * 创建者：leiwu
 *
 *  时间：2022/7/13 09:55
 *
 *  类描述：
 *
 *  修改人：
 *
 *  修改时间：
 *
 *  修改备注：
 */
class JniDemo {
    companion object {
        init {
            // 定义一个名字，跟 CMakeLists.txt中的 add_library 和 target_link_libraries 中定义的名字相同
            System.loadLibrary("Hello")
        }
    }

    /**
     * @return 返回一个写死的字符串
     */
    external fun sayHello(): String?
}