/*
 * @Author: 苗金标
 * @Date: 2023-02-28 17:14:15
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-24 19:39:11
 * @Description: 
 */
#ifndef REGFILE_H
#define REGFILE_H
#include "../common.h"
namespace component
{
    template <typename T>
    class regfile{
        private:
            enum class sync_request_type_t{ write };

            typedef struct sync_request_t
            {
                sync_request_type_t req;
                uint32_t r_id;
                T data;
            }sync_request_t;
            std::queue<sync_request_t> sync_request_q;
            T *reg;
            uint32_t size;
        public:
            regfile(uint32_t size){
                this->size = size;
                reg   = new T[size];
                memset(reg,0,sizeof(reg[0]) * size);
            }
            ~regfile(){delete[] reg;}
            void write(uint32_t id,T value){
                assert(id < size);
                reg[id] = value;
            }
            T read(uint32_t id){
                assert(id < size);
                return reg[id];
            }

            void write_sync(uint32_t id,T value){
                sync_request_t t;
                t.req = sync_request_type_t::write;
                t.r_id = id;
                t.data = value;

                sync_request_q.push(t);
            }

            void sync(){
                sync_request_t t;
                while (!sync_request_q.empty())
                {
                    /* code */
                    t = sync_request_q.front();
                    sync_request_q.pop();
                    switch(t.req){
                        case sync_request_type_t::write:
                            write(t.r_id,t.data);
                            break;
                    }
                }         
            }

    };
} 

#endif