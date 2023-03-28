/*
 * @Author: 苗金标
 * @Date: 2023-02-28 20:53:35
 * @LastEditors: 苗金标
 * @LastEditTime: 2023-02-28 21:00:16
 * @Description: 
 */
#ifndef TEST_RAT_H
#define TEST_RAT_H
#include "../component/rat.h"
#include "../common.h"
namespace test
{
    namespace rat
    {
        void test()
        {
            component::rat rat(512, 32);
            uint32_t phy_id = 0;
            uint32_t free_phy[512];
            
            rat.init_start();

            for(auto i = 1;i < 32;i++)
            {
                rat.set_map(i, i);
            }

            rat.init_finish();

            for(uint32_t i = 1;i < 32;i++)
            {
                assert(rat.get_phy_id(i, &phy_id));
                assert(phy_id == i);
            }

            for(uint32_t i = 1;i <= (512 - 31);i++)
            {
                assert(rat.get_free_phy_id(i, free_phy) == i);
            }

            assert(free_phy[0] == 0);

            for(uint32_t i = 32;i < 512;i++)
            {
                assert(free_phy[i - 31] == i);
            }

            rat.set_map(1, 0);
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 32));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 0);
            rat.release_map(1);
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 31));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 0);
            rat.set_map_sync(1, 1);
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 31));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 0);
            rat.sync();
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 32));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 1);
            rat.release_map_sync(0);
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 32));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 1);
            rat.sync();
            assert(rat.get_free_phy_id(512, free_phy) == (512 - 31));
            assert(rat.get_phy_id(1, &phy_id));
            assert(phy_id == 1);
        }
    }
}
#endif