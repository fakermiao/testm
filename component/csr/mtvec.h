/*
 * @Author: 苗金标
 * @Date: 2023-03-24 19:06:07
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:08:41
 * @Description: 必须4字节对齐，用于存储自陷向量的基址
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mtvec : public csr_base{
            public:
                mtvec() : csr_base("mtvec",0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return value & (~0x03);
                }
        };
    }
}