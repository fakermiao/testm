/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:46:19
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:50:31
 * @Description: On implementations that support only IALIGN=32, the two low bits (mepc[1:0]) are always zero.
 * When a trap is taken into M-mode, mepc is written with the virtual address of the instruction
 * that was interrupted or that encountered the exception.
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mepc : public csr_base{
            public:
                mepc() : csr_base("mepc",0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return value & (~0x03);
                }
        };
    }
}
