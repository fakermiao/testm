/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:50:58
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:53:29
 * @Description: Hardware thread ID.
 * The mhartid CSR is an MXLEN-bit read-only register containing the integer ID of the hardware
 * thread running the code.
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mhartid : public csr_base{
            public:
                mhartid() : csr_base("mhartid",0x00000000){
                    
                }
        };
    }
}