/*
 * @Author: 苗金标
 * @Date: 2023-03-02 17:14:51
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-27 16:28:00
 * @Description: 
 */
#ifndef READREG_H
#define READREG_H
#include "../common.h"
#include "../component/port.h"
#include "../component/regfile.h"
#include "rename_readreg.h"
#include "readreg_issue.h"
#include "issue.h"
#include "commit.h"

namespace pipeline{
    typedef struct readreg_feedback_pack_t{

    }readreg_feedback_pack_t;

    class readreg{
        private:
            component::port<rename_readreg_pack_t> *rename_readreg_port;
            component::port<readreg_issue_pack_t> *readreg_issue_port;
            component::regfile<phy_regfile_item_t> *phy_regfile;
        public:
            readreg(component::port<rename_readreg_pack_t> *rename_readreg_port, component::port<readreg_issue_pack_t> *readreg_issue_port, component::regfile<phy_regfile_item_t> *phy_regfile);
            void run(issue_feedback_pack_t issue_pack,wb_feedback_pack_t wb_feedback_pack,commit_feedback_pack_t commit_feedback_pack);
    };
}

#endif
