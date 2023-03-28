/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:07:21
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 19:56:31
 * @Description: 
 */

#include "bru.h"
namespace pipeline{

    namespace execute{
        bru::bru(component::fifo<issue_execute_pack_t> *issue_bru_fifo,component::port<execute_wb_pack_t> *bru_wb_port,component::csrfile *csr_file){
            this->issue_bru_fifo = issue_bru_fifo;
            this->bru_wb_port = bru_wb_port;
            this->csr_file = csr_file;
        }

        bru_feedback_pack_t bru::run(commit_feedback_pack_t commit_feedback_pack){
            execute_wb_pack_t send_pack;
            bru_feedback_pack_t feedback_pack;

            memset(&send_pack,0,sizeof(send_pack));
            memset(&feedback_pack,0,sizeof(feedback_pack));

            if(!issue_bru_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush)){
                issue_execute_pack_t rev_pack;
                assert(issue_bru_fifo->pop(&rev_pack));

                send_pack.enable = rev_pack.enable;
                send_pack.value  = rev_pack.value;
                send_pack.valid  = rev_pack.valid;
                send_pack.rob_id = rev_pack.rob_id;
                send_pack.pc     = rev_pack.pc;
                send_pack.imm    = rev_pack.imm;
                send_pack.has_execption = rev_pack.has_execption;
                send_pack.execption_id = rev_pack.execption_id;
                send_pack.execption_value = rev_pack.execption_value;

                send_pack.rs1    = rev_pack.rs1;
                send_pack.arg1_src = rev_pack.arg1_src;
                send_pack.rs1_need_map = rev_pack.rs1_need_map;
                send_pack.rs1_phy = rev_pack.rs1_phy;
                send_pack.src1_value = rev_pack.src1_value;
                send_pack.src1_loaded = rev_pack.src1_loaded;

                send_pack.rs2 = rev_pack.rs2;
                send_pack.arg2_src = rev_pack.arg2_src;
                send_pack.rs2_need_map = rev_pack.rs2_need_map;
                send_pack.rs2_phy = rev_pack.rs2_phy;
                send_pack.src2_value = rev_pack.src2_value;
                send_pack.src2_loaded = rev_pack.src2_loaded;

                send_pack.rd = rev_pack.rd;
                send_pack.rd_enable = rev_pack.rd_enable;
                send_pack.need_rename = rev_pack.need_rename;
                send_pack.rd_phy = rev_pack.rd_phy;

                send_pack.csr = rev_pack.csr;
                send_pack.op = rev_pack.op;
                send_pack.op_unit = rev_pack.op_unit;
                memcpy(&send_pack.sub_op,&rev_pack.sub_op,sizeof(rev_pack.sub_op));
                feedback_pack.enable = true;
                feedback_pack.next_pc = rev_pack.pc + rev_pack.imm;

                if(rev_pack.enable && rev_pack.valid){
                    switch(rev_pack.sub_op.bru_op){
                        case bru_op_t::beq:
                            feedback_pack.jump = rev_pack.src1_value == rev_pack.src2_value;
                            break;
                        case bru_op_t::bne:
                            feedback_pack.jump = rev_pack.src1_value != rev_pack.src2_value;
                            break;
                        case bru_op_t::bge:
                            feedback_pack.jump = ((int32_t)rev_pack.src1_value) >= ((int32_t)rev_pack.src2_value);
                            break;
                        case bru_op_t::bgeu:
                            feedback_pack.jump = ((uint32_t)rev_pack.src1_value) >= ((uint32_t)rev_pack.src2_value);
                            break;
                        case bru_op_t::blt:
                            feedback_pack.jump = ((int32_t)rev_pack.src1_value) <((int32_t)rev_pack.src2_value);
                            break;
                        case bru_op_t::bltu:
                            feedback_pack.jump = ((uint32_t)rev_pack.src1_value) < ((uint32_t)rev_pack.src2_value);
                            break;
                        case bru_op_t::jal:
                            feedback_pack.jump = true;
                            break;
                        case bru_op_t::jalr:
                            send_pack.rd_value = rev_pack.pc + 4;
                            feedback_pack.jump = true;
                            feedback_pack.next_pc = (rev_pack.imm + rev_pack.src2_value) & (~0x01);
                        case bru_op_t::mret:
                            feedback_pack.jump = true;
                            feedback_pack.next_pc = csr_file->read_sys(CSR_MEPC);
                            break;
                        
                    }
                }
            }
            bru_wb_port->set(send_pack);
            return feedback_pack;
        }
    }
}