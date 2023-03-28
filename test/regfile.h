/*
 * @Author: 苗金标
 * @Date: 2023-02-28 18:40:46
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-02-28 18:52:34
 * @Description: 
 */
#ifndef TEST_REGFILE_H
#define TEST_REGFILE_H
#include "../component/regfile.h"
#include "../common.h"
namespace test{
    namespace regfile{
        void test(){
            component::regfile<uint32_t> reg(32);
            for(uint32_t i = 0;i<32;i++)
                assert(reg.read(i) == 0);
            for(uint32_t i = 0;i<32;i++)
                reg.write(i,i);
            for(uint32_t i = 0; i < 32; i++)
                assert(reg.read(i) == i);
            for(uint32_t i = 0; i < 32; i++)
                reg.write_sync(i,i+1);
            for(uint32_t i = 0; i < 32; i++)
                assert(reg.read(i) == i);
            reg.sync();
            for(uint32_t i = 0; i < 32; i++)
                assert(reg.read(i) == i+1);
            
        }
    }
}
#endif
