/*
 * @Author: 苗金标
 * @Date: 2023-03-24 17:24:36
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 17:31:32
 * @Description: 高两位是base域，低26位为extensions域
 * base域编码了内部支持的ISA宽度:1为32位，2为64位，3为128位
 * extensions域编码了现有的标准扩展，与字母顺序对应，如第0位表示“A”原子扩展，第25位为“Z"
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class misa : public csr_base{
            public:
                misa() : csr_base("misa",0x40001100){

                }
        };
    }
}