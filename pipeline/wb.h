/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:17:40
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 10:17:28
 * @Description: 
 */
#ifndef WB_H
#define WB_H
#include "../common.h"
#include "../component/port.h"
#include "../component/regfile.h"
#include "execute_wb.h"
#include "wb_commit.h"
#include "commit.h"
namespace pipeline{
    typedef struct wb_feedback_channel_t{
        bool enable;
        uint32_t phy_id;
        uint32_t value;
    }wb_feedback_channel_t;

    typedef struct wb_feedback_pack_t{
        wb_feedback_channel_t channel[EXECUTE_UNIT_NUM];
    }wb_feedback_pack_t;
    class wb{
        private:
            component::port<execute_wb_pack_t> **alu_wb_port;
            component::port<execute_wb_pack_t> **bru_wb_port;
            component::port<execute_wb_pack_t> **csr_wb_port;
            component::port<execute_wb_pack_t> **div_wb_port;
            component::port<execute_wb_pack_t> **lsu_wb_port;
            component::port<execute_wb_pack_t> **mul_wb_port;

            std::vector<component::port<execute_wb_pack_t> *> execute_wb_port;
            component::port<wb_commit_pack_t> *wb_commit_port;
            component::regfile<phy_regfile_item_t> *phy_regfile;
        public:
            wb(component::port<execute_wb_pack_t> **alu_wb_port,component::port<execute_wb_pack_t> **bru_wb_port,component::port<execute_wb_pack_t> **csr_wb_port,
                component::port<execute_wb_pack_t> **div_wb_port,component::port<execute_wb_pack_t> **lsu_wb_port,component::port<execute_wb_pack_t> **mul_wb_port,
                component::port<wb_commit_pack_t> *wb_commit_port,component::regfile<phy_regfile_item_t> *phy_regfile);
            void init();
            wb_feedback_pack_t run(commit_feedback_pack_t commit_feedback_pack);
    };
}
#endif