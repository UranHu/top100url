#include <gtest/gtest.h>
#include "src/turl_define.h"
#include "src/turl_map.h"

using namespace turl;

TEST(TurlTEST, HeapTEST) {
    FLAGS_top_k = 4;
    std::vector< std::unordered_map<std::string, int32_t> > times;
    times.resize(3);
    url_heap heap;
    int t[9] = {9, 3, 1, 6, 4, 2, 7, 5, 3};
    int s[9] = {9, 7, 6, 5, 4, 3, 3, 2, 1};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            std::string url = "url_" + std::to_string(i*10+j);
            times[i][url] = t[i * 3 + j];
        }
    }
    for (int i = 0; i < 3; i++) {
        for (auto it = times[i].begin(); it != times[i].end(); it++) {
            heap.insert(it);
        }
    }
    std::vector< std::pair<std::string, int32_t> > &topk = heap.top_k();
    EXPECT_EQ(topk[0].second, 5);
    EXPECT_EQ(topk[FLAGS_top_k - 1].second, 9);
    if (topk[1].second != 7) {
        EXPECT_EQ(topk[2].second, 7);
    } else {
        EXPECT_EQ(topk[2].second, 6);
    }
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}