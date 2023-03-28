/*
 * @Author: 苗金标
 * @Date: 2023-03-24 18:55:17
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:01:03
 * @Description: Additional machine status register, RV32 only.
 * https://c-yongheng.github.io/2022/07/30/riscv-privileged-spec/
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mstatush : public csr_base{
            public:
                mstatush() : csr_base("mstatush",0x00000000){

                }
                virtual uint32_t filter(uint32_t){
                    return 0;
                }
        };
    }
}
