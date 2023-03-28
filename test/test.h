/*
 * @Author: 苗金标
 * @Date: 2023-02-24 17:26:58
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-02 20:34:14
 * @Description: 
 */
#ifndef TEST_H
#define TEST_H
#include "../common.h"
#include "fifo.h"
#include "port.h"
#include "regfile.h"
#include "memory.h"
#include "rat.h"
#include "issue_queue.h"
namespace test{
    void test(){
        fifo::test();
        port::test();
        memory::test();
        regfile::test();
        rat::test();
        issue_queue::test();
        std::cout << "self-test passed" << std::endl;
    }
}
#endif