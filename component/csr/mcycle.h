/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:42:47
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:45:57
 * @Description: Machine cycle counter,The mcycle CSR counts the
 * number of clock cycles executed by the processor core on which the hart is running
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mcycle : public csr_base{
            public:
                mcycle():csr_base("mcycle",0x00000000){

                }
        };
    }
}