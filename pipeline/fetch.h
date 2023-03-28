/*
 * @Author: 苗金标
 * @Date: 2023-02-28 21:09:55
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 19:43:37
 * @Description: fetch模块，每次按8字节对齐取两条指令放入fetch_decode_fifo
 */
#ifndef FETCH_H
#define FETCH_H
#include "../common.h"
#include "../component/fifo.h"
#include "../component/memory.h"
#include "fetch_decode.h"
#include "execute/bru.h"
#include "commit.h"

namespace pipeline{
    class fetch : public if_print_t{
        private:
            component::memory *memory;
            component::fifo<fetch_decode_pack_t> *fetch_decode_fifo;
            uint32_t pc;
            bool jump_wait;
        public:
            fetch(component::memory *memory,component::fifo<fetch_decode_pack_t> *fetch_decode_fifo,uint32_t init_pc);
            void run(pipeline::execute::bru_feedback_pack_t bru_feedback_pack,commit_feedback_pack_t commit_feedback_pack);
            virtual void print(std::string indent);
    };
}
#endif