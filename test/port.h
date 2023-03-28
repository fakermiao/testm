/*
 * @Author: 苗金标
 * @Date: 2023-02-28 17:08:35
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-19 16:24:13
 * @Description: 
 */
#ifndef TEST_PORT_H
#define TEST_PORT_H
#include "../component/port.h"
#include "../common.h"
namespace test{
    namespace port{
        void test(){
            component::port<if_print_fake<int>> myPort(0);
            assert(myPort.get() == 0);
            myPort.set(1);
            assert(myPort.get() == 1);
        }
    }
}
#endif