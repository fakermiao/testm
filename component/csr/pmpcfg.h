/*
 * @Author: 苗金标
 * @Date: 2023-03-24 19:16:16
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:22:03
 * @Description: https://zhuanlan.zhihu.com/p/139695407
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component
{
    namespace csr{
        class pmpcfg : public csr_base{
            public:
                pmpcfg(uint32_t id) : csr_base(std::string("pmpcfg") + std::to_string(id),0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return 0;
                }
        };
    }
} // namespace component
