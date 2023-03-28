/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:09:17
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:22:17
 * @Description: 
 */
#include "alu.h"
namespace pipeline{
    namespace execute{
        alu::alu(component::fifo<issue_execute_pack_t> *issue_alu_fifo,component::port<execute_wb_pack_t> *alu_wb_port){
            this->issue_alu_fifo = issue_alu_fifo;
            this->alu_wb_port = alu_wb_port;
        }

        void alu::run(commit_feedback_pack_t commit_feedback_pack){
            execute_wb_pack_t send_pack;
            memset(&send_pack,0,sizeof(send_pack));
            if(!issue_alu_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush)){
                issue_execute_pack_t rev_pack;
                assert(issue_alu_fifo->pop(&rev_pack));

                send_pack.enable    = rev_pack.enable;
                send_pack.value     = rev_pack.value;
                send_pack.valid     = rev_pack.valid;
                send_pack.rob_id    = rev_pack.rob_id;
                send_pack.pc        = rev_pack.pc;
                send_pack.imm       = rev_pack.imm;
                send_pack.has_execption = rev_pack.has_execption;
                send_pack.execption_id = rev_pack.execption_id;
                send_pack.execption_value = rev_pack.execption_value;

                send_pack.rs1       = rev_pack.rs1;
                send_pack.arg1_src  = rev_pack.arg1_src;
                send_pack.rs1_need_map = rev_pack.rs1_need_map;
                send_pack.rs1_phy   = rev_pack.rs1_phy;
                send_pack.src1_value= rev_pack.src1_value;
                send_pack.src1_loaded=rev_pack.src1_loaded;

                send_pack.rs2       = rev_pack.rs2;
                send_pack.arg2_src  = rev_pack.arg2_src;
                send_pack.rs2_need_map=rev_pack.rs2_need_map;
                send_pack.rs2_phy   = rev_pack.rs2_phy;
                send_pack.src2_value= rev_pack.src2_value;
                send_pack.src2_loaded= rev_pack.src2_loaded;

                send_pack.rd        = rev_pack.rd;
                send_pack.rd_enable = rev_pack.rd_enable;
                send_pack.need_rename = rev_pack.need_rename;
                send_pack.rd_phy    = rev_pack.rd_phy;

                send_pack.csr       = rev_pack.csr;
                send_pack.op        = rev_pack.op;
                send_pack.op_unit   = rev_pack.op_unit;

                memcpy(&send_pack.sub_op,&rev_pack.sub_op,sizeof(rev_pack.sub_op));

                if(rev_pack.enable && (!rev_pack.has_execption)){
                    if(rev_pack.enable && (!rev_pack.valid)){
                        send_pack.has_execption = true;
                        send_pack.execption_id  = riscv_execption_t::illegal_instruction;
                        send_pack.execption_value = rev_pack.value;
                    }
                    if(rev_pack.enable && rev_pack.valid){
                        switch(rev_pack.sub_op.alu_op){
                            case alu_op_t::add:
                                send_pack.rd_value = rev_pack.src1_value + rev_pack.src2_value;
                                break;
                            case alu_op_t::and_:
                                send_pack.rd_value = rev_pack.src1_value & rev_pack.src2_value;
                                break;
                            case alu_op_t::auipc:
                                send_pack.rd_value = rev_pack.imm + rev_pack.pc;
                                break;
                            case alu_op_t::ebreak:
                                send_pack.rd_value = 0;
                                send_pack.has_execption = true;
                                send_pack.execption_id = riscv_execption_t::breakpoint;
                                send_pack.execption_value = 0;
                                break;
                            case alu_op_t::ecall:
                                send_pack.rd_value = 0;
                                send_pack.has_execption = true;
                                send_pack.execption_id = riscv_execption_t::environment_call_from_m_mode;
                                send_pack.execption_value = 0;
                                break;
                            case alu_op_t::fence:
                                send_pack.rd_value = 0;
                                break;
                            case alu_op_t::fence_i:
                                send_pack.rd_value = 0;
                                break;
                            case alu_op_t::lui:
                                send_pack.rd_value = rev_pack.imm;
                                break;
                            case alu_op_t::or_:
                                send_pack.rd_value = rev_pack.src1_value | rev_pack.src2_value;
                                break;
                            case alu_op_t::sll:
                                send_pack.rd_value = rev_pack.src1_value << (rev_pack.src2_value & 0x1f);
                                break;
                            case alu_op_t::slt:
                                send_pack.rd_value = (((int32_t)rev_pack.src1_value) < ((int32_t)rev_pack.src2_value)) ? 1 : 0;
                                break;
                            case alu_op_t::sltu:
                                send_pack.rd_value = (rev_pack.src1_value < (rev_pack.src2_value & 0xfff)) ? 1 :0;
                                break;
                            case alu_op_t::sra:
                                send_pack.rd_value = (uint32_t)(((int32_t)rev_pack.src1_value) >> (rev_pack.src2_value & 0x1f));
                                break;
                            case alu_op_t::srl:
                                send_pack.rd_value = rev_pack.src1_value >> (rev_pack.src2_value & 0x1f);
                                break;
                            case alu_op_t::sub:
                                send_pack.rd_value = rev_pack.src1_value - rev_pack.src2_value;
                                break;
                            case alu_op_t::xor_:
                                send_pack.rd_value = rev_pack.src1_value ^ rev_pack.src2_value;
                            default:
                                printf("not implement\n");
                        }
                    }
                }

            }
            alu_wb_port->set(send_pack);
        }
    }
}
