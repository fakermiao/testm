/*
 * @Author: 苗金标
 * @Date: 2023-03-24 17:07:55
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 17:09:37
 * @Description: Machine instructions-retired counter.
 * The minstret CSR counts the number of instructions the hart has retired. 
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class minstret : public csr_base{
            public:
                minstret() : csr_base("minstret",0x00000000){
                    
                }
        };
    }
}
