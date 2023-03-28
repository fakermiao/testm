/*
 * @Author: 苗金标
 * @Date: 2023-03-01 11:30:27
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-26 20:54:32
 * @Description: 译码单元,从fetch_decode_fifo取指令译码，产生操作码，源寄存器1、2的类型是否需要重命名，目的寄存器的类型是否需要重命名等信息
 */
#ifndef DECODE_H
#define DECODE_H
#include "../common.h"
#include "../component/fifo.h"
#include "fetch_decode.h"
#include "decode_rename.h"
#include "commit.h"
namespace pipeline{
    class decode{
        private:
            component::fifo<fetch_decode_pack_t> *fetch_decode_fifo;
            component::fifo<decode_rename_pack_t> *decode_rename_fifo;
        public:
            decode(component::fifo<fetch_decode_pack_t> *fetch_decode_fifo,component::fifo<decode_rename_pack_t> *decode_rename_fifo);
            void run(commit_feedback_pack_t commit_feedback_pack);
    };
}
#endif
