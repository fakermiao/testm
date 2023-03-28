/*
 * @Author: 苗金标
 * @Date: 2023-03-24 19:01:27
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:05:27
 * @Description: 当一个trap进入m模式时，mtval要么被设置为零，要么被写入异常特定的信息，以帮助软件处理该trap。
 * 否则，mtval永远不会由实现编写，尽管它可能由软件显式编写。硬件平台将指定哪些异常必须详细地设置mtval，哪些异常可以无条件地将其设置为零。
 * 如果硬件平台指定没有异常将mtval设置为非零值，则mtval为只读零。  
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mtval : public csr_base{
            public:mtval() : csr_base("mtval",0x00000000){

            }
        };
    }
}
