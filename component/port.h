/*
 * @Author: 苗金标
 * @Date: 2023-02-28 17:04:19
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-03-19 15:08:48
 * @Description: 
 */
#ifndef PORT_H
#define PORT_H
namespace component{
    template <typename T>
    class port : public if_print_t{
        private:
            T value;
        public:
        port(T value){
            this->value = value;
        }
        void set(T value){ this->value = value;}
        T get(){return this->value;}
        virtual void print(std::string indent){
            if_print_t *if_print = dynamic_cast<if_print_t *>(&this->value);
            if_print->print(indent);
        }
    };
}
#endif