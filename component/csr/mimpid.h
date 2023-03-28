/*
 * @Author: 苗金标
 * @Date: 2023-03-24 17:04:35
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 17:07:35
 * @Description: Implementation ID
 * The Implementation value should reflect the design of the RISC-V processor itself and not any surrounding system.
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mimpid : public csr_base{
            public:
                mimpid() : csr_base("mimpid",0x20230324){
                
                }
        };
    }
}