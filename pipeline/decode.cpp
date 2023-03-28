/*
 * @Author: 苗金标
 * @Date: 2023-03-01 21:30:14
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 19:50:48
 * @Description: 
 */
#include "decode.h"
namespace pipeline{
    decode::decode(component::fifo<fetch_decode_pack_t> *fetch_decode_fifo,component::fifo<decode_rename_pack_t> *decode_rename_fifo){
        this->fetch_decode_fifo = fetch_decode_fifo;
        this->decode_rename_fifo = decode_rename_fifo;
    }

    void decode::run(commit_feedback_pack_t commit_feed_pack){
        if(!(commit_feed_pack.enable && commit_feed_pack.flush)){
            if(!this->fetch_decode_fifo->is_empty() && !this->decode_rename_fifo->is_full()){
                fetch_decode_pack_t rev_pack;
                decode_rename_pack_t send_pack;
                this->fetch_decode_fifo->pop(&rev_pack);
                std::memset(&send_pack,0,sizeof(send_pack));
            
                for(uint32_t i = 0;i < FETCH_WIDTH;i++){
                    if(rev_pack.op_info[i].enable){
                        auto op_data    = rev_pack.op_info[i].value;
                        auto opcode     = op_data & 0x7f;
                        auto rd         = (op_data >> 7) & 0x1f;
                        auto funct3     = (op_data >> 12) & 0x07;
                        auto rs1        = (op_data >> 15) & 0x1f;
                        auto rs2        = (op_data >> 20) & 0x1f;
                        auto funct7     = (op_data >> 25) & 0x7f;
                        auto imm_i = (op_data >> 20) & 0xfff;
                        auto imm_s = (((op_data >> 7) & 0x1f)) | (((op_data >> 25) & 0x7f) << 5);
                        auto imm_b = (((op_data >> 8) & 0x0f) << 1) | (((op_data >> 25) & 0x3f) << 5) | (((op_data >> 7) & 0x01) << 11) | (((op_data >> 31) & 0x01) << 12);
                        auto imm_u = op_data & (~0xfff);
                        auto imm_j = (((op_data >> 12) & 0xff) << 12) | (((op_data >> 20) & 0x01) << 11) | (((op_data >> 21) & 0x3ff) << 1) | (((op_data >> 31) & 0x01) << 20);
                    
                        auto op_info = send_pack.op_info[i];
                        op_info.enable = true;
                        op_info.valid  = !rev_pack.op_info[i].has_execption;
                        op_info.pc     = rev_pack.op_info[i].pc;

                        switch(opcode){
                            //need_map统一配置
                            case 0x37:
                                op_info.op = op_t::lui;
                                op_info.op_unit = op_unit_t::alu;
                                op_info.sub_op.alu_op = alu_op_t::lui;
                                op_info.arg1_src = arg_src_t::disable;
                                op_info.arg2_src = arg_src_t::disable;
                                op_info.imm      = imm_u;
                                op_info.rd       = rd;
                                op_info.rd_enable = true;
                                break;
                            case 0x17:
                                op_info.op = op_t::auipc;
                                op_info.op_unit = op_unit_t::alu;
                                op_info.sub_op.alu_op = alu_op_t::auipc;
                                op_info.arg1_src = arg_src_t::disable;
                                op_info.arg2_src = arg_src_t::disable;
                                op_info.imm = imm_u;
                                op_info.rd = rd;
                                op_info.rd_enable = true;
                                break;
                            case 0x6f:
                                op_info.op = op_t::jal;
                                op_info.op_unit = op_unit_t::bru;
                                op_info.sub_op.bru_op = bru_op_t::jal;
                                op_info.arg1_src = arg_src_t::disable;
                                op_info.arg2_src = arg_src_t::disable;
                                op_info.imm = sign_extend(imm_j,21);
                            case 0x67:
                                op_info.op = op_t::jalr;
                                op_info.op_unit = op_unit_t::bru;
                                op_info.sub_op.bru_op = bru_op_t::jalr;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::disable;
                                op_info.imm = sign_extend(imm_i, 12);
                                break;
                            case 0x63://beq bne blt bge bltu bgeu
                                op_info.op_unit = op_unit_t::bru;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::reg;
                                op_info.rs2 = rs2;
                                op_info.imm = sign_extend(imm_b, 13);

                                switch(funct3)
                                {
                                    case 0x0://beq
                                        op_info.op = op_t::beq;
                                        op_info.sub_op.bru_op = bru_op_t::beq;
                                        break;

                                    case 0x1://bne
                                        op_info.op = op_t::bne;
                                        op_info.sub_op.bru_op = bru_op_t::bne;
                                        break;

                                    case 0x4://blt
                                        op_info.op = op_t::blt;
                                        op_info.sub_op.bru_op = bru_op_t::blt;
                                        break;

                                    case 0x5://bge
                                        op_info.op = op_t::bge;
                                        op_info.sub_op.bru_op = bru_op_t::bge;
                                        break;

                                    case 0x6://bltu
                                        op_info.op = op_t::bltu;
                                        op_info.sub_op.bru_op = bru_op_t::bltu;
                                        break;

                                    case 0x7://bgeu
                                        op_info.op = op_t::bgeu;
                                        op_info.sub_op.bru_op = bru_op_t::bgeu;
                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }
                                break;
                            case 0x03://lb lh lw lbu lhu
                                op_info.op_unit = op_unit_t::lsu;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::disable;
                                op_info.imm = sign_extend(imm_i, 12);
                                op_info.rd = rd;
                                op_info.rd_enable = true;

                                switch(funct3)
                                {
                                    case 0x0://lb
                                        op_info.op = op_t::lb;
                                        op_info.sub_op.lsu_op = lsu_op_t::lb;
                                        break;

                                    case 0x1://lh
                                        op_info.op = op_t::lh;
                                        op_info.sub_op.lsu_op = lsu_op_t::lh;
                                        break;

                                    case 0x2://lw
                                        op_info.op = op_t::lw;
                                        op_info.sub_op.lsu_op = lsu_op_t::lw;
                                        break;

                                    case 0x4://lbu
                                        op_info.op = op_t::lbu;
                                        op_info.sub_op.lsu_op = lsu_op_t::lbu;
                                        break;

                                    case 0x5://lhu
                                        op_info.op = op_t::lhu;
                                        op_info.sub_op.lsu_op = lsu_op_t::lhu;
                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }

                                break;
                        
                            case 0x23://sb sh sw
                                op_info.op_unit = op_unit_t::lsu;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::reg;
                                op_info.rs2 = rs2;
                                op_info.imm = sign_extend(imm_s, 12);

                                switch(funct3)
                                {
                                    case 0x0://sb
                                        op_info.op = op_t::sb;
                                        op_info.sub_op.lsu_op = lsu_op_t::sb;
                                        break;

                                    case 0x1://sh
                                        op_info.op = op_t::sh;
                                        op_info.sub_op.lsu_op = lsu_op_t::sh;
                                        break;

                                    case 0x2://sw
                                        op_info.op = op_t::sw;
                                        op_info.sub_op.lsu_op = lsu_op_t::sw;
                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }

                                break;

                            case 0x13://addi slti sltiu xori ori andi slli srli srai
                                op_info.op_unit = op_unit_t::alu;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::imm;
                                op_info.imm = sign_extend(imm_i, 12);
                                op_info.rd = rd;
                                op_info.rd_enable = true;

                                switch(funct3)
                                {
                                    case 0x0://addi
                                        op_info.op = op_t::addi;
                                        op_info.sub_op.alu_op = alu_op_t::add;
                                        break;

                                    case 0x2://slti
                                        op_info.op = op_t::slti;
                                        op_info.sub_op.alu_op = alu_op_t::slt;
                                        break;

                                    case 0x3://sltiu
                                        op_info.op = op_t::sltiu;
                                        op_info.sub_op.alu_op = alu_op_t::sltu;
                                        break;

                                    case 0x4://xori
                                        op_info.op = op_t::xori;
                                        op_info.sub_op.alu_op = alu_op_t::xor_;
                                        break;

                                    case 0x6://ori
                                        op_info.op = op_t::ori;
                                        op_info.sub_op.alu_op = alu_op_t::or_;
                                        break;

                                    case 0x7://andi
                                        op_info.op = op_t::andi;
                                        op_info.sub_op.alu_op = alu_op_t::and_;
                                        break;

                                    case 0x1://slli
                                        if(funct7 == 0x00)//slli
                                        {
                                            op_info.op = op_t::slli;
                                            op_info.sub_op.alu_op = alu_op_t::sll;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x5://srli srai
                                        if(funct7 == 0x00)//srli
                                        {
                                            op_info.op = op_t::srli;
                                            op_info.sub_op.alu_op = alu_op_t::srl;
                                        }
                                        else if(funct7 == 0x20)//srai
                                        {
                                            op_info.op = op_t::srai;
                                            op_info.sub_op.alu_op = alu_op_t::sra;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }

                                break;

                            case 0x33://add sub sll slt sltu xor srl sra or and mul mulh mulhsu mulhu div divu rem remu
                                op_info.op_unit = op_unit_t::alu;
                                op_info.arg1_src = arg_src_t::reg;
                                op_info.rs1 = rs1;
                                op_info.arg2_src = arg_src_t::reg;
                                op_info.rs2 = rs2;
                                op_info.rd = rd;
                                op_info.rd_enable = true;

                                switch(funct3)
                                {
                                    case 0x0://add sub mul
                                        if(funct7 == 0x00)//add
                                        {
                                            op_info.op = op_t::add;
                                            op_info.sub_op.alu_op = alu_op_t::add;
                                        }
                                        else if(funct7 == 0x20)//sub
                                        {
                                            op_info.op = op_t::sub;
                                            op_info.sub_op.alu_op = alu_op_t::sub;
                                        }
                                        else if(funct7 == 0x01)//mul
                                        {
                                            op_info.op = op_t::mul;
                                            op_info.op_unit = op_unit_t::mul;
                                            op_info.sub_op.mul_op = mul_op_t::mul;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x1://sll mulh
                                        if(funct7 == 0x00)//sll
                                        {
                                            op_info.op = op_t::sll;
                                            op_info.sub_op.alu_op = alu_op_t::sll;
                                        }
                                        else if(funct7 == 0x01)//mulh
                                        {
                                            op_info.op = op_t::mulh;
                                            op_info.op_unit = op_unit_t::mul;
                                            op_info.sub_op.mul_op = mul_op_t::mulh;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x2://slt mulhsu
                                        if(funct7 == 0x00)//slt
                                        {
                                            op_info.op = op_t::slt;
                                            op_info.sub_op.alu_op = alu_op_t::slt;
                                        }
                                        else if(funct7 == 0x01)//mulhsu
                                        {
                                            op_info.op = op_t::mulhsu;
                                            op_info.op_unit = op_unit_t::mul;
                                            op_info.sub_op.mul_op = mul_op_t::mulhsu;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x3://sltu mulhu
                                        if(funct7 == 0x00)//sltu
                                        {
                                            op_info.op = op_t::sltu;
                                            op_info.sub_op.alu_op = alu_op_t::sltu;
                                        }
                                        else if(funct7 == 0x01)//mulhu
                                        {
                                            op_info.op = op_t::mulhu;
                                            op_info.op_unit = op_unit_t::mul;
                                            op_info.sub_op.mul_op = mul_op_t::mulhu;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x4://xor div
                                        if(funct7 == 0x00)//xor
                                        {
                                            op_info.op = op_t::xor_;
                                            op_info.sub_op.alu_op = alu_op_t::xor_;
                                        }
                                        else if(funct7 == 0x01)//div
                                        {
                                            op_info.op = op_t::div;
                                            op_info.op_unit = op_unit_t::div;
                                            op_info.sub_op.div_op = div_op_t::div;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x5://srl sra divu
                                        if(funct7 == 0x00)//srl
                                        {
                                            op_info.op = op_t::srl;
                                            op_info.sub_op.alu_op = alu_op_t::srl;
                                        }
                                        else if(funct7 == 0x20)//sra
                                        {
                                            op_info.op = op_t::sra;
                                            op_info.sub_op.alu_op = alu_op_t::sra;
                                        }
                                        else if(funct7 == 0x01)//divu
                                        {
                                            op_info.op = op_t::divu;
                                            op_info.op_unit = op_unit_t::div;
                                            op_info.sub_op.div_op = div_op_t::divu;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x6://or rem
                                        if(funct7 == 0x00)//or
                                        {
                                            op_info.op = op_t::or_;
                                            op_info.sub_op.alu_op = alu_op_t::or_;
                                        }
                                        else if(funct7 == 0x01)//rem
                                        {
                                            op_info.op = op_t::rem;
                                            op_info.op_unit = op_unit_t::div;
                                            op_info.sub_op.div_op = div_op_t::rem;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x7://and remu
                                        if(funct7 == 0x00)//and
                                        {
                                            op_info.op = op_t::and_;
                                            op_info.sub_op.alu_op = alu_op_t::and_;
                                        }
                                     else if(funct7 == 0x01)//remu
                                        {
                                            op_info.op = op_t::remu;
                                            op_info.op_unit = op_unit_t::div;
                                            op_info.sub_op.div_op = div_op_t::remu;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }

                                break;
                        
                            case 0x0f://fence fence.i
                                switch(funct3)
                                {
                                    case 0x0://fence
                                        if((rd == 0x00) && (rs1 == 0x00) && (((op_data >> 28) & 0x0f) == 0x00))//fence
                                        {
                                            op_info.op = op_t::fence;
                                            op_info.op_unit = op_unit_t::alu;
                                            op_info.sub_op.alu_op = alu_op_t::fence;
                                            op_info.arg1_src = arg_src_t::disable;
                                            op_info.arg2_src = arg_src_t::disable;
                                            op_info.imm = imm_i;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x1://fence.i
                                        if((rd == 0x00) && (rs1 == 0x00) && (imm_i == 0x00))//fence.i
                                        {
                                            op_info.op = op_t::fence_i;
                                            op_info.op_unit = op_unit_t::alu;
                                            op_info.sub_op.alu_op = alu_op_t::fence_i;
                                            op_info.arg1_src = arg_src_t::disable;
                                            op_info.arg2_src = arg_src_t::disable;
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }
                                break;

                            case 0x73://ecall ebreak csrrw csrrs csrrc csrrwi csrrsi csrrci mret
                                switch(funct3)
                                {
                                    case 0x0://ecall ebreak mret
                                        if((rd == 0x00) && (rs1 == 0x00))//ecall ebreak mret
                                        {
                                            switch(funct7)
                                            {
                                                case 0x00://ecall ebreak
                                                    switch(rs2){
                                                        case 0x00:
                                                            op_info.op = op_t::ecall;
                                                            op_info.op_unit = op_unit_t::alu;
                                                            op_info.sub_op.alu_op = alu_op_t::ecall;
                                                            op_info.arg1_src = arg_src_t::disable;
                                                            op_info.arg2_src = arg_src_t::disable;
                                                            break;
                                                        case 0x01:
                                                            op_info.op = op_t::ebreak;
                                                            op_info.op_unit = op_unit_t::alu;
                                                            op_info.sub_op.alu_op = alu_op_t::ebreak;
                                                            op_info.arg1_src = arg_src_t::disable;
                                                            op_info.arg2_src = arg_src_t::disable;
                                                            break;
                                                        default://invalid
                                                            op_info.valid = false;
                                                            break;
                                                    }
                                                    break;
                                                case 0x18://mret
                                                    switch(rs2){
                                                        case 0x02://mret
                                                            op_info.op = op_t::mret;
                                                            op_info.op_unit = op_unit_t::bru;
                                                            op_info.sub_op.bru_op = bru_op_t::mret;
                                                            op_info.arg1_src = arg_src_t::disable;
                                                            op_info.arg2_src = arg_src_t::disable;
                                                            break;
                                                        default://invalid
                                                            op_info.valid = false;
                                                            break;
                                                    }
                                                    break;
                                                default://invalid
                                                    op_info.valid = false;
                                                    break;
                                            }
                                        }
                                        else//invalid
                                        {
                                            op_info.valid = false;
                                        }

                                        break;

                                    case 0x1://csrrw
                                        op_info.op = op_t::csrrw;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrw;
                                        op_info.arg1_src = arg_src_t::reg;
                                        op_info.rs1 = rs1;
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    case 0x2://csrrs
                                        op_info.op = op_t::csrrs;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrs;
                                        op_info.arg1_src = arg_src_t::reg;
                                        op_info.rs1 = rs1;
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    case 0x3://csrrc
                                        op_info.op = op_t::csrrc;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrc;
                                        op_info.arg1_src = arg_src_t::reg;
                                        op_info.rs1 = rs1;
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    case 0x5://csrrwi
                                        op_info.op = op_t::csrrwi;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrw;
                                        op_info.arg1_src = arg_src_t::imm;
                                        op_info.imm = rs1;//zimm
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    case 0x6://csrrsi
                                        op_info.op = op_t::csrrsi;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrs;
                                        op_info.arg1_src = arg_src_t::imm;
                                        op_info.imm = rs1;//zimm
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    case 0x7://csrrci
                                        op_info.op = op_t::csrrci;
                                        op_info.op_unit = op_unit_t::csr;
                                        op_info.sub_op.csr_op = csr_op_t::csrrc;
                                        op_info.arg1_src = arg_src_t::imm;
                                        op_info.imm = rs1;//zimm
                                        op_info.arg2_src = arg_src_t::disable;
                                        op_info.csr = imm_i;
                                        op_info.rd = rd;
                                        op_info.rd_enable = true;
                                        break;

                                    default://invalid
                                        op_info.valid = false;
                                        break;
                                }
                                break;

                            default://invalid
                                op_info.valid = false;
                                break;                     
                        }
                        op_info.rs1_need_map = (op_info.arg1_src == arg_src_t::reg) && (op_info.rs1 > 0);
                        op_info.rs2_need_map = (op_info.arg2_src == arg_src_t::reg) && (op_info.rs2 > 0);
                        op_info.need_rename  = op_info.rd_enable && (op_info.rd > 0);
                        op_info.value        = rev_pack.op_info[i].value;
                        op_info.has_execption = rev_pack.op_info[i].has_execption;
                        op_info.execption_id = rev_pack.op_info[i].execption_id;
                        op_info.execption_value = rev_pack.op_info[i].execption_value;
                        send_pack.op_info[i] = op_info;
                    }
                }
                decode_rename_fifo->push(send_pack);
            }
        }else{
            decode_rename_fifo->flush();
        }
    }
}