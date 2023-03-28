/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:13:21
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:36:25
 * @Description: 
 */
#include "lsu.h"
namespace pipeline{
    namespace execute{
        lsu::lsu(component::fifo<issue_execute_pack_t> *issue_lsu_fifo,component::port<execute_wb_pack_t> *lsu_wb_port,component::memory *memory){
            this->issue_lsu_fifo = issue_lsu_fifo;
            this->lsu_wb_port = lsu_wb_port;
            this->memory = memory;
        }

        void lsu::run(commit_feedback_pack_t commit_feedback_pack){
            execute_wb_pack_t send_pack;
            memset(&send_pack,0,sizeof(send_pack));

            if(!issue_lsu_fifo->is_empty() && !(commit_feedback_pack.enable && commit_feedback_pack.flush)){
                issue_execute_pack_t rev_pack;
                assert(issue_lsu_fifo->pop(&rev_pack));

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
                    auto addr = rev_pack.src1_value + rev_pack.imm;
                    send_pack.execption_value = addr;
                    switch (rev_pack.sub_op.lsu_op){
                        case lsu_op_t::lb:
                        case lsu_op_t::lbu:
                            send_pack.has_execption = !memory->check_align(addr,1) || !memory->check_boundary(addr,1);
                            send_pack.execption_id = !memory->check_align(addr,1) ? riscv_execption_t::load_address_misaligned : riscv_execption_t::load_access_fault;
                            break;
                        
                        case lsu_op_t::lh:
                        case lsu_op_t::lhu:
                            send_pack.has_execption = !memory->check_align(addr,2) || !memory->check_boundary(addr,2);
                            send_pack.execption_id = !memory->check_align(addr,2) ? riscv_execption_t::load_address_misaligned : riscv_execption_t::load_access_fault;
                            break;
                        case lsu_op_t::lw:
                            send_pack.has_execption = !memory->check_align(addr,4) || !memory->check_boundary(addr,4);
                            send_pack.execption_id = !memory->check_align(addr,4) ? riscv_execption_t::load_address_misaligned : riscv_execption_t::load_access_fault;
                            break;
                        case lsu_op_t::sb:
                            send_pack.has_execption = !memory->check_align(addr,1) || !memory->check_boundary(addr,1);
                            send_pack.execption_id = !memory->check_align(addr,1) ? riscv_execption_t::store_amo_address_misaligned : riscv_execption_t::store_amo_access_fault;
                            break;
                        case lsu_op_t::sh:
                            send_pack.has_execption = !memory->check_align(addr,2) || !memory->check_boundary(addr,2);
                            send_pack.execption_id = !memory->check_align(addr,2) ? riscv_execption_t::store_amo_address_misaligned : riscv_execption_t::store_amo_access_fault;
                            break;
                        case lsu_op_t::sw:
                            send_pack.has_execption = !memory->check_align(addr,4) || !memory->check_boundary(addr,4);
                            send_pack.execption_id = !memory->check_align(addr,4) ? riscv_execption_t::store_amo_address_misaligned : riscv_execption_t::store_amo_access_fault;
                            break;
                    }
                    if(!send_pack.has_execption){
                        switch(rev_pack.sub_op.lsu_op){
                            case lsu_op_t::lb:
                                send_pack.rd_value = sign_extend(memory->read8(addr),8);
                                break;
                            case lsu_op_t::lbu:
                                send_pack.rd_value = memory->read8(addr);
                                break;
                            case lsu_op_t::lh:
                                send_pack.rd_value = sign_extend(memory->read16(addr),16);
                                break;
                            case lsu_op_t::lhu:
                                send_pack.rd_value = memory->read16(addr);
                                break;
                            case lsu_op_t::lw:
                                send_pack.rd_value = memory->read32(addr);
                                break;
                            case lsu_op_t::sb:
                                memory->write8_sync(addr,(uint8_t)(rev_pack.src2_value & 0xff));
                                break;
                            case lsu_op_t::sh:
                                memory->write16_sync(addr,(uint16_t)(rev_pack.src2_value & 0xffff));
                                break;
                            case lsu_op_t::sw:
                                memory->write32_sync(addr,rev_pack.src2_value);
                                break;
                        }
                    }
                }
            }
            lsu_wb_port->set(send_pack);
        }
    }
}