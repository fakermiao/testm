/*
 * @Author: 苗金标
 * @Date: 2023-03-20 21:18:11
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 21:26:16
 * @Description: 
 */
#ifndef EXECUTE_CSR_H
#define EXECUTE_CSR_H
#include "../../common.h"
#include "../../component/fifo.h"
#include "../../component/port.h"
#include "../../component/csrfile.h"
#include "../issue_execute.h"
#include "../execute_wb.h"
#include "../commit.h"

namespace pipeline{
    namespace execute{

        class csr{
            private:
                component::fifo<issue_execute_pack_t> *issue_csr_fifo;
                component::port<execute_wb_pack_t> *csr_wb_port;
                component::csrfile *csr_file;

                enum class state_t{
                    idle,
                    calculate
                };

                state_t cur_state;
                uint32_t csr_value;
                bool csr_succ;
                issue_execute_pack_t rev_pack;
            public:
                csr(component::fifo<issue_execute_pack_t> *issue_csr_fifo,component::port<execute_wb_pack_t> *csr_wb_port,component::csrfile *csr_file);
                void run(commit_feedback_pack_t commit_feedback_pack);
        };
    }
}
#endif
