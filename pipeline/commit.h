/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:19:11
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 17:14:38
 * @Description: 
 */
#ifndef COMMIT_H
#define COMMIT_H
#include "../common.h"
#include "../component/port.h"
#include "../component/rat.h"
#include "../component/rob.h"
#include "../component/csrfile.h"
#include "../component/regfile.h"
#include "wb_commit.h"

namespace pipeline{
    typedef struct commit_feedback_pack_t{
        bool     enable;
        uint32_t next_handle_rob_id;
        bool     has_execption;
        uint32_t execption_pc;
        bool     flush;
    }commit_feedback_pack_t;

    enum class state_t{
        normal,
        flush
    };
    class commit{
        private:
            component::port<wb_commit_pack_t> *wb_commit_port;
            component::rat *rat;
            component::rob *rob;
            component::csrfile *csr_file;
            component::regfile<phy_regfile_item_t> *phy_regfile;

            state_t cur_state;
            component::rob_item_t rob_item;
            uint32_t rob_item_id;
        public:
            commit(component::port<wb_commit_pack_t> *wb_commit_port,component::rat *rat,component::rob *rob,component::csrfile *csr_file,component::regfile<phy_regfile_item_t> *phy_regfile);
            commit_feedback_pack_t run();
    };
}
#endif
