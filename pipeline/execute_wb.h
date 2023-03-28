/*
 * @Author: 苗金标
 * @Date: 2023-03-20 20:25:59
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-21 20:29:28
 * @Description: 
 */
#pragma once
#include "../common.h"
#include "../config.h"
#include "pipeline_common.h"

namespace pipeline{
    typedef struct execute_wb_pack_t : public if_print_t{
        bool enable;
        uint32_t value;
        bool valid;
        uint32_t rob_id;
        uint32_t pc;
        uint32_t imm;
        bool has_execption;
        riscv_execption_t execption_id;
        uint32_t execption_value;

        uint32_t rs1;
        arg_src_t arg1_src;
        bool rs1_need_map;
        uint32_t rs1_phy;
        uint32_t src1_value;
        bool src1_loaded;

        uint32_t rs2;
        arg_src_t arg2_src;
        bool rs2_need_map;
        uint32_t rs2_phy;
        uint32_t src2_value;
        bool src2_loaded;

        uint32_t rd;
        bool rd_enable;
        bool need_rename;
        uint32_t rd_phy;
        uint32_t rd_value;

        uint32_t csr;
        op_t op;
        op_unit_t op_unit;

        union{
            alu_op_t alu_op;
            bru_op_t bru_op;
            div_op_t div_op;
            lsu_op_t lsu_op;
            mul_op_t mul_op;
            csr_op_t csr_op;
        }sub_op;

        virtual void print(std::string indent){
            std::string blank = "  ";
            std::cout << indent << "\tenable = " << outbool(enable);
            std::cout << blank << "value = 0x" << fillzero(8) << outhex(value);
            std::cout << blank << "valid = " << outbool(valid);
            std::cout << blank << "rob_id = " << rob_id;
            std::cout << blank << "pc = 0x" << fillzero(8) << outhex(pc);
            std::cout << blank << "imm = 0x" << fillzero(8) << outhex(imm);
            std::cout << blank << "has_execption = " << outbool(has_execption);
            std::cout << blank << "execption_id = " << outenum(execption_id);
            std::cout << blank << "execption_value = 0x" << fillzero(8) << outhex(execption_value) << std::endl;

            std::cout << blank << "rs1 = " << rs1;
            std::cout << blank << "arg1_src = " << outenum(arg1_src);
            std::cout << blank << "rs1_need_map = " << outbool(rs1_need_map);
            std::cout << blank << "rs1_phy = " << rs1_phy;
            std::cout << blank << "src1_value = 0x" << fillzero(8) << outhex(src1_value);
            std::cout << blank << "src1_loaded = " << outbool(src1_loaded) << std::endl;

            std::cout << indent << "\trs2 = " << rs2;
            std::cout << blank << "arg2_src = " << outenum(arg2_src);
            std::cout << blank << "rs2_need_map = " << outbool(rs2_need_map);
            std::cout << blank << "rs2_phy = " << rs2_phy;
            std::cout << blank << "src2_value = 0x" << fillzero(8) << outhex(src2_value);
            std::cout << blank << "src2_loaded = " << outbool(src2_loaded);

            std::cout << blank << "rd = " << rd;
            std::cout << blank << "rd_enable = " << outbool(rd_enable);
            std::cout << blank << "need_rename = " << outbool(need_rename);
            std::cout << blank << "rd_phy = " << rd_phy;
            std::cout << blank << "rd_value = 0x" << fillzero(8) << outhex(rd_value) << std::endl;

            std::cout << indent << "\tcsr = 0x" << fillzero(8) << outhex(csr);
            std::cout << blank << "op = " << outenum(op);
            std::cout << blank << "op_unit = " << outenum(op_unit);
            std::cout << blank << "sub_op = ";

            switch(op_unit)
            {
                case op_unit_t::alu:
                    std::cout << outenum(sub_op.alu_op);
                    break;
                    
                case op_unit_t::bru:
                    std::cout << outenum(sub_op.bru_op);
                    break;

                case op_unit_t::csr:
                    std::cout << outenum(sub_op.csr_op);
                    break;

                case op_unit_t::div:
                    std::cout << outenum(sub_op.div_op);
                    break;

                case op_unit_t::lsu:
                    std::cout << outenum(sub_op.lsu_op);
                    break;

                case op_unit_t::mul:
                    std::cout << outenum(sub_op.mul_op);
                    break;

                default:
                    std::cout << "<Unsupported>";
                    break;
            }

            std::cout << std::endl;
        }
    }execute_wb_pack_t;
}