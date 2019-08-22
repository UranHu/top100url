#include <gtest/gtest.h>
#include "src/turl_define.h"
#include "src/turl_map.h"

using namespace turl;

TEST(TurlTEST, HeapTEST) {
    std::vector< std::vector< std::shared_ptr<URL> > > times;
    times.resize(3);
    int t[9] = {9, 3, 1, 6, 4, 2, 7, 5, 3};
    int s[9] = {9, 7, 6, 5, 4, 3, 3, 2, 1};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::string url = "url_" + std::to_string(i*10+j);
            std::shared_ptr<URL> ptr = std::make_shared<URL>(url, t[i * 3 + j]);
            times[i].emplace_back(ptr);
        }
    }
    url_heap heap(times);
    heap.build_heap();

    std::shared_ptr<URL> u = nullptr;
    int i = 0;
    while (heap.pop(u) == 1) {
        EXPECT_EQ(u->t_, s[i]);
        i++;
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}