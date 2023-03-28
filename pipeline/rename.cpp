/*
 * @Author: 苗金标
 * @Date: 2023-03-02 15:30:26
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 18:54:06
 * @Description: 
 */
#include "rename.h"
namespace pipeline{

    rename::rename(component::fifo<decode_rename_pack_t> *decode_rename_fifo,component::port<rename_readreg_pack_t> *rename_readreg_port,component::rat *rat,component::rob *rob){
        this->decode_rename_fifo = decode_rename_fifo;
        this->rename_readreg_port = rename_readreg_port;
        this->rat = rat;
        this->rob = rob;
        this->busy = false;
    }

    void rename::run(issue_feedback_pack_t issue_pack,commit_feedback_pack_t commit_feedback_pack){
        rename_readreg_pack_t null_send_pack;
        bool stall = issue_pack.stall;


        memset(&null_send_pack, 0, sizeof(null_send_pack));
        
        if(!(commit_feedback_pack.enable && commit_feedback_pack.flush)){
            if(!stall){
                this->rename_readreg_port->set(null_send_pack);//bubble

                if(!this->busy){
                    if(!decode_rename_fifo->is_empty()){
                        decode_rename_fifo->pop(&rev_pack);
                        this->busy = true;
                    }
                }
                if(this->busy){
                    rename_readreg_pack_t send_pack;
                    memset(&send_pack,0,sizeof(send_pack));
                    for(uint32_t i = 0; i < RENAME_WIDTH; i++){
                        send_pack.op_info[i].enable  = rev_pack.op_info[i].enable;
                        send_pack.op_info[i].value   = rev_pack.op_info[i].value;
                        send_pack.op_info[i].valid   = rev_pack.op_info[i].valid;
                        send_pack.op_info[i].pc      = rev_pack.op_info[i].pc;
                        send_pack.op_info[i].imm     = rev_pack.op_info[i].imm;
                        send_pack.op_info[i].has_execption = rev_pack.op_info[i].has_execption;
                        send_pack.op_info[i].execption_id = rev_pack.op_info[i].execption_id;
                        send_pack.op_info[i].execption_value = rev_pack.op_info[i].execption_value;
                    
                        send_pack.op_info[i].rs1     = rev_pack.op_info[i].rs1;
                        send_pack.op_info[i].arg1_src = rev_pack.op_info[i].arg1_src;
                        send_pack.op_info[i].rs1_need_map = rev_pack.op_info[i].rs1_need_map;
                        send_pack.op_info[i].rs2     = rev_pack.op_info[i].rs2;
                        send_pack.op_info[i].arg2_src = rev_pack.op_info[i].arg2_src;
                        send_pack.op_info[i].rs2_need_map = rev_pack.op_info[i].rs2_need_map;
                        send_pack.op_info[i].rd      = rev_pack.op_info[i].rd;
                        send_pack.op_info[i].rd_enable = rev_pack.op_info[i].enable;
                        send_pack.op_info[i].need_rename = rev_pack.op_info[i].rd;
                        send_pack.op_info[i].csr     = rev_pack.op_info[i].csr;
                        send_pack.op_info[i].op      = rev_pack.op_info[i].op;
                        send_pack.op_info[i].op_unit = rev_pack.op_info[i].op_unit;
                        memcpy(&send_pack.op_info[i].sub_op,&rev_pack.op_info[i].sub_op,sizeof(rev_pack.op_info[i].sub_op));
                    }

                    //count new physical registers requirement and rob requirement,每条有效指令都要分配rob项但不一定有目的寄存器进行重命名
                    uint32_t phy_reg_req_cnt = 0;
                    uint32_t rob_req_cnt = 0;

                    for(uint32_t i = 0; i < RENAME_WIDTH; i++){
                        if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename)
                            phy_reg_req_cnt++;
                    
                        if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid)
                            rob_req_cnt++;
                    }

                    uint32_t new_phy_reg_id[RENAME_WIDTH];
                    if(rat->get_free_phy_id(phy_reg_req_cnt,new_phy_reg_id) && rob->get_free_space() >= (rob_req_cnt)){

                        //generate rob items&
                        component::rob_item_t rob_item[RENAME_WIDTH];
                        memset(rob_item,0,sizeof(rob_item));
                        
                        for(uint32_t i = 0;i < RENAME_WIDTH;i++){
                            if(rev_pack.op_info[i].enable){                               
                                if(rev_pack.op_info[i].valid){
                                    if(rev_pack.op_info[i].need_rename){
                                        rob_item[i].old_phy_reg_id_valid = true;
                                        assert(rat->get_phy_id(rev_pack.op_info[i].rd,&rob_item[i].old_phy_reg_id));
                                    }else{
                                        rob_item[i].old_phy_reg_id_valid = false;
                                        rob_item[i].old_phy_reg_id = 0;
                                    }
                                }

                                rob_item[i].finish = false;
                            }
                        }    

                        for(uint32_t i = 0,j = 0;i < RENAME_WIDTH;i++){
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename){
                                send_pack.op_info[i].rd_phy = new_phy_reg_id[j++];
                                rat->set_map_sync(rev_pack.op_info[i].rd,send_pack.op_info[i].rd_phy);
                            }
                        }
                        
                        //old_phy_reg_id feedback
                        for(auto i = 1;i < READREG_WIDTH;i++){
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].need_rename){
                                for(auto j = i-1;j >= 0;j--){
                                    if(rev_pack.op_info[j].enable && rev_pack.op_info[j].valid && rev_pack.op_info[j].need_rename){
                                        if(rev_pack.op_info[i].rd == rev_pack.op_info[j].rd){
                                            rob_item[i].old_phy_reg_id = send_pack.op_info[j].rd_phy;
                                        }
                                    }
                                }
                            }
                        }

                        //fill rob item
                        for(auto i = 0;i < RENAME_WIDTH;i++){
                            rob_item[i].new_phy_reg_id = send_pack.op_info[i].rd_phy;
                            rob_item[i].pc = rev_pack.op_info[i].pc;
                            rob_item[i].inst_value = rev_pack.op_info[i].value;
                            rob_item[i].has_execption = rev_pack.op_info[i].has_execption;
                            rob_item[i].execption_id = rev_pack.op_info[i].execption_id;
                            rob_item->execption_value = rev_pack.op_info[i].execption_value;
                        }

                        //start to map source register
                        for(uint32_t i = 0;i < RENAME_WIDTH;i++){
                            if(rev_pack.op_info[i].enable && rev_pack.op_info[i].valid && rev_pack.op_info[i].rs1_need_map){
                                if(rev_pack.op_info[i].rs1_need_map){
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs1,&send_pack.op_info[i].rs1_phy));
                                }
                                if(rev_pack.op_info[i].rs2_need_map){
                                    assert(rat->get_phy_id(rev_pack.op_info[i].rs2,&send_pack.op_info[i].rs2_phy));
                                }
                            }
                        }

                        //同时取指的两条指令的相关性
                        if(rev_pack.op_info[0].enable && rev_pack.op_info[0].valid && rev_pack.op_info[0].rd_enable && rev_pack.op_info[1].enable && rev_pack.op_info[1].valid){
                            if(rev_pack.op_info[1].rs1_need_map && (rev_pack.op_info[1].rs1 == rev_pack.op_info[0].rd)){
                                send_pack.op_info[1].rs1_phy = send_pack.op_info[0].rd_phy;
                            }
                            if(rev_pack.op_info[1].rs2_need_map && (rev_pack.op_info[1].rs2 == rev_pack.op_info[0].rd)){
                                send_pack.op_info[1].rs2_phy = send_pack.op_info[0].rd_phy;
                            }
                        }

                        rename_readreg_port->set(send_pack);
                        this->busy = false;
                    }
                }
            }
        }
        else{
            rename_readreg_pack_t send_pack;
            memset(&send_pack,0,sizeof(send_pack));
            rename_readreg_port->set(send_pack);
        }
    }
}