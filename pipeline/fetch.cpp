/*
 * @Author: 苗金标
 * @Date: 2023-02-28 21:27:13
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 19:41:17
 * @Description: 
 */
#include "fetch.h"
namespace pipeline{
    //static int i;
    fetch::fetch(component::memory *memory,component::fifo<fetch_decode_pack_t> *fetch_decode_fifo,uint32_t init_pc){
        this->memory = memory;
        this->fetch_decode_fifo = fetch_decode_fifo;
        this->pc = init_pc;
        this->jump_wait = false;
    }

    void fetch::run(pipeline::execute::bru_feedback_pack_t bru_feedback_pack,commit_feedback_pack_t commit_feed_pack){
        //提交阶段没有flush信号时正常取指，每周期两条指令
        if(!(commit_feed_pack.enable && commit_feed_pack.flush)){
            //暂时没有分支预测，遇到分支要等执行单元产生结果才继续取指
            //由于分支跳转原因，所以地址可能不是8字节对齐（取两条指令一个周期），因此在取指和pc更新时要有逻辑判断（8字节对齐）
            uint32_t cur_pc = this->pc;
            uint32_t i0_pc = cur_pc;
            bool i0_has_execption = !memory->check_align(i0_pc,4) || !memory->check_boundary(i0_pc,4);
            uint32_t i0 = i0_has_execption ? 0 : this->memory->read32(i0_pc);
            bool i0_enable = !jump_wait;
            bool i0_jump   = ((i0 & 0x7f) == 0x6f) || ((i0 & 0x7f) == 0x67) || ((i0 & 0x7f) == 0x63) || (i0 == 0x30200073);
            uint32_t i1_pc = cur_pc ^ 0x04;
            bool i1_has_execption = !memory->check_align(i1_pc,4) || !memory->check_boundary(i1_pc,4);
            uint32_t i1    = i1_has_execption ? 0 : this->memory->read32(i1_pc);
            bool i1_enable = is_align(cur_pc,8) && !jump_wait && !i0_jump;
            bool i1_jump   = ((i1 & 0x7f) == 0x6f) || ((i1 & 0x7f) == 0x67) || ((i1 & 0x7f) == 0x63) || (i0 == 0x30200073);

            if(jump_wait){
                if(bru_feedback_pack.enable){
                    this->jump_wait = false;
                    if(bru_feedback_pack.jump){
                        this->pc = bru_feedback_pack.next_pc;
                    }else{
                        this->pc = (this->pc & (~0x04)) + 8;
                    }
                }
            }
            else if(!this->fetch_decode_fifo->is_full()){
                if(i0_jump || i1_jump){
                    this->jump_wait = true;
                }else{
                    this->pc += (i1_enable ? 8 : 4);
                }

                fetch_decode_pack_t t_fetch_decode;

                t_fetch_decode.op_info[0].value = i0_enable ? i0 : 0;
                t_fetch_decode.op_info[0].enable = i0_enable;
                t_fetch_decode.op_info[0].pc = i0_pc;
                t_fetch_decode.op_info[0].has_execption = i0_has_execption;
                t_fetch_decode.op_info[0].execption_id = !memory->check_align(i0_pc,4) ? riscv_execption_t::instruction_address_misaligned : riscv_execption_t::instruction_access_fault;
                t_fetch_decode.op_info[0].execption_value = i0_pc;
                t_fetch_decode.op_info[1].value = i1_enable ? i1 : 0;
                t_fetch_decode.op_info[1].enable = i1_enable;
                t_fetch_decode.op_info[1].pc = i1_pc;
                t_fetch_decode.op_info[1].has_execption = i1_has_execption;
                t_fetch_decode.op_info[1].execption_id = !memory->check_align(i1_pc,4) ? riscv_execption_t::instruction_address_misaligned : riscv_execption_t::instruction_access_fault;
                t_fetch_decode.op_info[1].execption_value = i1_pc;

                this->fetch_decode_fifo->push(t_fetch_decode);
            }
        }
        //commit flush信号有效时刷新流水线
        else{
            this->fetch_decode_fifo->flush();
            this->jump_wait = false;
            if(commit_feed_pack.has_execption){
                this->pc = commit_feed_pack.execption_pc;
            }
        }
    }

    void fetch::print(std::string indent){
        std::cout << indent << "pc = 0x" << fillzero(8) << outhex(this->pc);
        std::cout << "    jump_wait= " << outbool(this->jump_wait) << std::endl;
    }
}