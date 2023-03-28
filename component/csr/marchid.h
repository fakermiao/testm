/*
 * @Author: 苗金标
 * @Date: 2023-03-24 15:57:51
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:15:28
 * @Description: 机器架构ID寄存器
 */
#pragma once
#include "../../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class marchid : public csr_base{
            public:
                marchid() : csr_base("marchid",0x19971023){

                }
        };
    }
}