/*
 * @Author: 苗金标
 * @Date: 2023-03-24 18:46:26
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 18:52:46
 * @Description: xIE域： 当硬件线程运行于x模式，对应的中断使能位xIE置位1。低优先级的中断总是关闭，高优先级的中断总是使能。
 * 高优先级可以改变特定级别的使能位。
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mstatus : public csr_base{
            public:
                mstatus() : csr_base("mstatus",0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return value & 0x88;
                }
                void set_mie(bool value){
                    this->setbit(3,value);
                }
                bool get_mie(){
                    return this->getbit(3);
                }
                void set_mpie(uint32_t value){
                    this->setbit(7,value);
                }
                bool get_mpie(){
                    return this->getbit(7);
                }
        };
    }
}