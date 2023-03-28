/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:18:33
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 10:43:20
 * @Description: 
 */
#include "wb.h"
namespace pipeline{
    wb::wb(component::port<execute_wb_pack_t> **alu_wb_port,component::port<execute_wb_pack_t> **bru_wb_port,component::port<execute_wb_pack_t> **csr_wb_port,
        component::port<execute_wb_pack_t> **div_wb_port,component::port<execute_wb_pack_t> **lsu_wb_port,component::port<execute_wb_pack_t> **mul_wb_port,
        component::port<wb_commit_pack_t> *wb_commit_port,component::regfile<phy_regfile_item_t> *phy_regfile){
            this->alu_wb_port   =   alu_wb_port;
            this->bru_wb_port   =   bru_wb_port;
            this->csr_wb_port   =   csr_wb_port;
            this->div_wb_port   =   div_wb_port;
            this->lsu_wb_port   =   lsu_wb_port;
            this->mul_wb_port   =   mul_wb_port;
            this->wb_commit_port =  wb_commit_port;
            this->phy_regfile   =   phy_regfile;
        }

        void wb::init(){
            for(auto i = 0;i < ALU_UNIT_NUM;i++){
                this->execute_wb_port.push_back(alu_wb_port[i]);
            }
            for(auto i = 0;i < BRU_UNIT_NUM;i++){
                this->execute_wb_port.push_back(bru_wb_port[i]);
            }
            for(auto i = 0;i < CSR_UNIT_NUM;i++){
                this->execute_wb_port.push_back(csr_wb_port[i]);
            }
            for(auto i = 0;i < DIV_UNIT_NUM;i++){
                this->execute_wb_port.push_back(div_wb_port[i]);
            }
            for(auto i = 0;i < LSU_UNIT_NUM;i++){
                this->execute_wb_port.push_back(lsu_wb_port[i]);
            }
            for(auto i = 0;i < MUL_UNIT_NUM;i++){
                this->execute_wb_port.push_back(mul_wb_port[i]);
            }
        }

        wb_feedback_pack_t wb::run(commit_feedback_pack_t commit_feedback_pack){
            wb_commit_pack_t send_pack;
            wb_feedback_pack_t feedback_pack;
            memset(&send_pack,0,sizeof(send_pack));
            memset(&feedback_pack,0,sizeof(feedback_pack));

            if(!(commit_feedback_pack.enable && commit_feedback_pack.flush)){
                for(size_t i = 0;i < this->execute_wb_port.size();i++){
                    execute_wb_pack_t rev_pack;
                    rev_pack = this->execute_wb_port[i]->get();

                    send_pack.op_info[i].enable         =   rev_pack.enable;
                    send_pack.op_info[i].value          =   rev_pack.value;
                    send_pack.op_info[i].valid          =   rev_pack.valid;
                    send_pack.op_info[i].rob_id         =   rev_pack.rob_id;
                    send_pack.op_info[i].pc             =   rev_pack.pc;
                    send_pack.op_info[i].imm            =   rev_pack.imm;
                    send_pack.op_info[i].has_execption  =   rev_pack.has_execption;
                    send_pack.op_info[i].execption_id   =   rev_pack.execption_id;
                    send_pack.op_info[i].execption_value =  rev_pack.execption_value;

                    send_pack.op_info[i].rs1            =   rev_pack.rs1;
                    send_pack.op_info[i].arg1_src       =   rev_pack.arg1_src;
                    send_pack.op_info[i].rs1_need_map   =   rev_pack.rs1_need_map;
                    send_pack.op_info[i].rs1_phy        =   rev_pack.rs1_phy;
                    send_pack.op_info[i].src1_value     =   rev_pack.src1_value;
                    send_pack.op_info[i].src1_loaded    =   rev_pack.src1_loaded;

                    send_pack.op_info[i].rs2            =   rev_pack.rs2;
                    send_pack.op_info[i].arg2_src       =   rev_pack.arg2_src;
                    send_pack.op_info[i].rs2_need_map   =   rev_pack.rs2_need_map;
                    send_pack.op_info[i].rs2_phy        =   rev_pack.rs2_phy;
                    send_pack.op_info[i].src2_value     =   rev_pack.src2_value;
                    send_pack.op_info[i].src2_loaded    =   rev_pack.src2_loaded;

                    send_pack.op_info[i].rd             =   rev_pack.rd;
                    send_pack.op_info[i].rd_enable      =   rev_pack.rd_enable;
                    send_pack.op_info[i].need_rename    =   rev_pack.need_rename;
                    send_pack.op_info[i].rd_phy         =   rev_pack.rd_phy;
                    send_pack.op_info[i].rd_value       =   rev_pack.rd_value;

                    send_pack.op_info[i].csr            =   rev_pack.csr;
                    send_pack.op_info[i].op             =   rev_pack.op;
                    send_pack.op_info[i].op_unit        =   rev_pack.op_unit;
                    memcpy(&send_pack.op_info[i].sub_op,&rev_pack.sub_op,sizeof(rev_pack.sub_op));

                    if(rev_pack.enable && rev_pack.valid && !rev_pack.has_execption && rev_pack.rd_enable && rev_pack.need_rename){
                        phy_regfile_item_t t_item;

                        t_item.value    =   rev_pack.rd_value;
                        t_item.valid    =   true;

                        phy_regfile->write_sync(rev_pack.rd_phy,t_item);
                        feedback_pack.channel[i].enable = true;
                        feedback_pack.channel[i].phy_id = rev_pack.rd_phy;
                        feedback_pack.channel[i].value  = rev_pack.rd_value;
                    }
                }
            }
            wb_commit_port->set(send_pack);
            return feedback_pack;        
        }
}