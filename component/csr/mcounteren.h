/*
 * @Author: 苗金标
 * @Date: 2023-03-24 16:27:02
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 16:42:16
 * @Description: 当清除mcounteren寄存器中的CY、TM、IR或HPMn位时，当以s模式或u模式执行时，试图读取周期、时间、instret或hpmcountern寄存器
 * 将导致非法的指令异常。当设置了这些位中的一个，就允许在下一个实现的特权模式中访问相应的寄存器(如果实现了s模式，否则采用u模式)。
 * cycle，instret和hpmcounterN CSRs是只读的，分别是mcycle、minstret和mhpmcounterN的影子。time CSR 是只读的，是memory-mapped mtime寄存器的影子。
 * 类似的，在RV32I中，cycleh、instreth和hpmcounterNh CSRs分别是mcycleh、minstreth和mhpmcounterNh的只读影子。在RV32I上，timeh CSRs是memory-mapped
 * mtime寄存器高32bits的只读影子，而time是mtime寄存器的低32bits的影子。
 */
#pragma once
#include "../common.h"
#include "../csr_base.h"

namespace component{
    namespace csr{
        class mcounteren : public csr_base{
            public:
                mcounteren() : csr_base("mcounteren",0x00000000){

                }
                virtual uint32_t filter(uint32_t value){
                    return value & 0x07;
                }
                void set_cy(bool value){
                    this->setbit(0,value);
                }
                bool get_cy(){
                    return this->getbit(0);
                }
                void set_tm(bool value){
                    this->setbit(1,value);
                }
                bool get_tm(){
                    return this->getbit(1);
                }
                void set_ir(bool value){
                    this->setbit(2,value);
                }
                bool get_ir(){
                    return this->getbit(2);
                }
        };
    }
}
