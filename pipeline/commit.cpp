/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:20:11
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 19:29:12
 * @Description: 
 */
#include "commit.h"
#include "../component/csr_all.h"
namespace pipeline{
    commit::commit(component::port<wb_commit_pack_t> *wb_commit_port,component::rat *rat,component::rob *rob,component::csrfile *csr_file,component::regfile<phy_regfile_item_t> *phy_regfile){
        this->wb_commit_port = wb_commit_port;
        this->rat            = rat;
        this->rob            = rob;
        this->csr_file       = csr_file;
        this->phy_regfile    = phy_regfile;
        this->cur_state      = state_t::normal;
    }

    commit_feedback_pack_t commit::run(){
        commit_feedback_pack_t feedback_pack;
        phy_regfile_item_t default_phy_reg_item;
        memset(&feedback_pack,0,sizeof(feedback_pack));
        memset(&default_phy_reg_item,0,sizeof(default_phy_reg_item));

        if(this->cur_state == state_t::normal){
            //handle output
            if(!rob->is_empty()){
                //sync处理

                assert(rob->get_front_id(&this->rob_item_id));
                feedback_pack.enable = true;
                feedback_pack.next_handle_rob_id = this->rob_item_id;

                for(auto i = 0;i < COMMIT_WIDTH;i++){
                    this->rob_item = rob->get_item(this->rob_item_id);
                    if(rob_item.finish){
                        rob->pop_sync();
                        rob->set_committed(true);
                        
                        if(rob_item.old_phy_reg_id_valid){
                            rat->release_map_sync(rob_item.old_phy_reg_id);
                            phy_regfile->write_sync(rob_item.old_phy_reg_id,default_phy_reg_item);
                            rat->commit_map_sync(rob_item.new_phy_reg_id);
                        }
                        if(rob_item.has_execption){
                            feedback_pack.enable    =   true;
                            feedback_pack.flush     =   true;
                            cur_state = state_t::flush;
                            break;
                        }else{
                            feedback_pack.enable    =   rob->get_next_id(rob_item_id,&feedback_pack.next_handle_rob_id);
                        }
                    }
                    if(!rob->get_next_id(this->rob_item_id,&this->rob_item_id)){
                        break;
                    }
                }
            }
            //handle input
            auto rev_pack = wb_commit_port->get();
            for(auto i = 0;i < EXECUTE_UNIT_NUM;i++){
                if(rev_pack.op_info[i].enable){
                    auto rob_item = rob->get_item(rev_pack.op_info[i].rob_id);
                    //rob_item.new_phy_reg_id = rev_pack.op_info[i].rd_phy;
                    rob_item.finish =   true;
                    //rob_item.pc     =   rev_pack.op_info[i].pc;
                    //rob_item.inst_value = rev_pack.op_info[i].value;
                    rob_item.has_execption = rev_pack.op_info[i].has_execption;
                    rob_item.execption_id  = rev_pack.op_info[i].execption_id;
                    rob_item.execption_value = rev_pack.op_info[i].execption_value;
                    rob->set_item_sync(rev_pack.op_info[i].rob_id,rob_item);
                }
            }
        }
        else{
            if(rob->is_empty() || (rob->get_size() == 1)){
                feedback_pack.enable = true;
                feedback_pack.has_execption = true;
                csr_file->write_sys_sync(CSR_MEPC,rob_item.pc);
                csr_file->write_sys_sync(CSR_MTVAL,rob_item.execption_value);
                csr_file->write_sys_sync(CSR_MCAUSE,static_cast<uint32_t>(rob_item.execption_id));
                feedback_pack.execption_pc = csr_file->read_sys(CSR_MTVEC);
                feedback_pack.flush = true;
                cur_state = state_t::normal;
            }else{
                feedback_pack.enable = true;
                feedback_pack.flush  = true;
            }
            auto t_rob_item = rob->get_front();
            rob->pop_sync();
            //根据rob恢复写回的表项？？？
            if(t_rob_item.old_phy_reg_id_valid){
                rat->restore_map_sync(t_rob_item.new_phy_reg_id,t_rob_item.old_phy_reg_id);
                phy_regfile->write_sync(t_rob_item.new_phy_reg_id,default_phy_reg_item);
            }
        }
        return feedback_pack;
    }
}