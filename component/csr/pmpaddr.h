/*
 * @Author: 苗金标
 * @Date: 2023-03-24 19:13:25
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:16:01
 * @Description: 
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class pmpaddr : public csr_base{
            public:
                pmpaddr(uint32_t id) : csr_base(std::string("pmpaddr") + std::to_string(id),0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return 0;
                }
        };
    }
}