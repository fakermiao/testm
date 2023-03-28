/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:21:33
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:26:20
 * @Description: mconfigptr is an MXLEN-bit read-only CSR, formatted as shown in Figure 3.24, that holds the
 * physical address of a configuration data structure. Software can traverse this data structure to
 * discover information about the harts, the platform, and their configuration.
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mconfigptr : public csr_base{
            public:
                mconfigptr():csr_base("mconfigptr",0x00000000){

                }
        };
    }
}
