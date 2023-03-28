/*
 * @Author: 苗金标
 * @Date: 2023-03-02 19:14:02
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:37:10
 * @Description: 
 */
#ifndef EXECUTE_MUL_H
#define EXECUTE_MUL_H
#include "../../common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline{
    namespace execute{
        class mul{
            private:
                component::fifo<issue_execute_pack_t> *issue_mul_fifo;
                component::port<execute_wb_pack_t> *mul_wb_port;

            public:
                mul(component::fifo<issue_execute_pack_t> *issue_mul_fifo, component::port<execute_wb_pack_t> *mul_wb_port);
                void run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}
#endif
