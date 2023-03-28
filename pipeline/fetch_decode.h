/*
 * @Author: 苗金标
 * @Date: 2023-02-28 21:13:50
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 14:31:44
 * @Description: 
 */
#ifndef FETCH_DECODE_H
#define FETCH_DECODE_H
#include "../common.h"
#include "../config.h"
namespace pipeline
{
    typedef struct fetch_decode_op_info_t
    {
        uint32_t pc;
        uint32_t value;
        bool enable;
        bool has_execption;
        riscv_execption_t execption_id;
        uint32_t execption_value;
    }op_info;
    typedef struct fetch_decode_pack_t : public if_print_t
    {
        /* data */
        fetch_decode_op_info_t op_info[FETCH_WIDTH];

        virtual void print(std::string indent){
            std::string blank = "    ";

            for(auto i = 0;i < FETCH_WIDTH;i++){
                std::cout << indent << "Item" << i << ":" << std::endl;
                std::cout << indent << "\tpc = 0x" << fillzero(8) << outhex(op_info[i].pc);
                std::cout << blank << "value = 0x" << fillzero(8) << outhex(op_info[i].value);
                std::cout << blank << "enable = " << outbool(op_info[i].enable);
                std::cout << blank << "has_execption = " << outbool(op_info[i].has_execption);
                std::cout << blank << "execption_id = " << outenum(op_info[i].execption_id);
                std::cout << blank << "execption_value = 0x" << fillzero(8) << outhex(op_info[i].execption_value) << std::endl;

            }
        }
    }fecth_decode_pack_t;
    
    
} // namespace pipeline

#endif