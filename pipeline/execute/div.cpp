/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:11:17
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:35:04
 * @Description: 
 */
#include "div.h"
namespace pipeline{
    namespace execute{
        div::div(component::fifo<issue_execute_pack_t> *issue_div_fifo, component::port<execute_wb_pack_t> *div_wb_port){
            this->issue_div_fifo = issue_div_fifo;
            this->div_wb_port = div_wb_port;
        }

        void div::run(commit_feedback_pack_t commit_feedback_pack){
            execute_wb_pack_t send_pack;
            memset(&send_pack,0,sizeof(send_pack));

            if(!issue_div_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush)){
                issue_execute_pack_t rev_pack;
                assert(issue_div_fifo->pop(&rev_pack));
                
                send_pack.enable = rev_pack.enable;
                send_pack.value = rev_pack.value;
                send_pack.valid = rev_pack.valid;
                send_pack.rob_id = rev_pack.rob_id;
                send_pack.pc = rev_pack.pc;
                send_pack.imm = rev_pack.imm;
                send_pack.has_execption = rev_pack.has_execption;
                send_pack.execption_id = rev_pack.execption_id;
                send_pack.execption_value = rev_pack.execption_value;

                send_pack.rs1 = rev_pack.rs1;
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
                memcpy(&send_pack.sub_op, &rev_pack.sub_op, sizeof(rev_pack.sub_op));
                
                if(rev_pack.enable && rev_pack.valid){
                    switch (rev_pack.sub_op.div_op){
                        case div_op_t::div:
                            send_pack.rd_value = ((uint32_t)((int32_t)rev_pack.src1_value) / ((int32_t)rev_pack.src2_value));
                            break;

                        case div_op_t::divu:
                            send_pack.rd_value = ((uint32_t)((uint32_t)rev_pack.src1_value) / ((uint32_t)rev_pack.src2_value));
                            break;

                        case div_op_t::rem:
                            send_pack.rd_value = ((uint32_t)((int32_t)rev_pack.src1_value) % ((int32_t)rev_pack.src2_value));
                            break;

                        case div_op_t::remu:
                            send_pack.rd_value = ((uint32_t)((int32_t)rev_pack.src1_value) % ((int32_t)rev_pack.src2_value));
                            break;
                    }
                }
            }

            div_wb_port->set(send_pack);
        }
    }
}