/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:15:58
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 15:16:50
 * @Description:  a code indicating the event that caused the trap.
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mcause : public csr_base{
            public:
            mcause() : csr_base("mcause",0x00000000){
                
            }
        };
    }
}