/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:11:53
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:36:28
 * @Description: 
 */
#ifndef EXECUTE_LSU_H
#define EXECUTE_LSU_H
#include "../../common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../../component/memory.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline{
    namespace execute{
        class lsu{
            private:
                component::fifo<issue_execute_pack_t> *issue_lsu_fifo;
                component::port<execute_wb_pack_t> *lsu_wb_port;
                component::memory *memory;

            public:
                lsu(component::fifo<issue_execute_pack_t> *issue_lsu_fifo, component::port<execute_wb_pack_t> *lsu_wb_port,component::memory *memory);
                void run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}
#endif