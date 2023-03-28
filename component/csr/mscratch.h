/*
 * @Author: 苗金标
 * @Date: 2023-03-24 18:42:15
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 18:46:07
 * @Description: 用于保存一个指向机器模式hart-local上下文空间的指针，并在进入m模式陷阱处理程序时与用户寄存器交换
 * Scratch register for machine trap handlers
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mscratch : public csr_base{
            public:
                mscratch() : csr_base("mscratch",0x00000000){
                    
                }
        };
    }
}