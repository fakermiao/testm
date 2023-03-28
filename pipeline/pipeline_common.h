/*
 * @Author: 苗金标
 * @Date: 2023-03-01 20:19:11
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 19:39:54
 * @Description: 
 */
#ifndef PIPELINE_COMMON_H
#define PIPELINE_COMMON_H
#include "../common.h"
namespace pipeline{
    enum class op_unit_t{
        alu,mul,div,lsu,bru,csr
    };

    enum class arg_src_t{
        reg,imm,disable
    };
    enum class op_t{
        lui,andi,auipc,jal,jalr,
        add,addi,sub,and_,or_,ori,sll,slli,slt,slti,sltiu,sltu,xor_,xori,
        sra,srai,srl,srli,beq,bge,bgeu,blt,bltu,bne,
        lb,lbu,lh,lhu,lw,sb,sh,sw,
        fence,fence_i,ebreak,ecall,csrrc,csrrci,csrrs,csrrsi,csrrw,csrrwi,
        mul,mulh,mulhsu,mulhu,div,divu,rem,remu,mret
    };
    enum class alu_op_t{
        lui,auipc,
        ebreak,ecall,fence,fence_i,
        add,sub,and_,or_,sll,slt,sltu,sra,srl,xor_
    };
    enum class bru_op_t{
        beq,bge,bgeu,blt,bltu,bne,jal,jalr,mret
    };
    enum class lsu_op_t{
        lb,lbu,lh,lhu,lw,sb,sh,sw
    };
    enum class div_op_t{
        div,divu,rem,remu
    };
    enum class mul_op_t{
        mul,mulh,mulhsu,mulhu
    };
    enum class csr_op_t{
        csrrc,csrrs,csrrw
    };
    typedef struct phy_regfile_item_t{
        uint32_t value;
        bool valid;
    }phy_regfile_item_t;
}
#endif