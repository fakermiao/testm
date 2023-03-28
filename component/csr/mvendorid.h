/*
 * @Author: 苗金标
 * @Date: 2023-03-24 19:09:55
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:12:52
 * @Description: https://zhuanlan.zhihu.com/p/452722687
 * 厂商id
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mvendorid : public csr_base{
            public:
                mvendorid() : csr_base("mvendorid",0x00000000){}
        };
    }
}
