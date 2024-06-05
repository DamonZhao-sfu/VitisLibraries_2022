#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_8678(Table &tbl_SerializeFromObject_TD_934_input, Table &tbl_Filter_TD_8678_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77, l_suppkey#79)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_934_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_934_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_934_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_934_input.getInt32(i, 0);
            tbl_Filter_TD_8678_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_934_input.getInt32(i, 1);
            tbl_Filter_TD_8678_output.setInt32(r, 1, _l_suppkey79_t);
            r++;
        }
    }
    tbl_Filter_TD_8678_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8678_output #Row: " << tbl_Filter_TD_8678_output.getNumRow() << std::endl;
}

void SW_Filter_TD_7456(Table &tbl_SerializeFromObject_TD_934_input, Table &tbl_Filter_TD_7456_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((l_receiptdate#89 > l_commitdate#88))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_receiptdate#89, l_commitdate#88)
    // Output: ListBuffer(l_orderkey#77#2016, l_suppkey#79#2018)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_934_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_receiptdate89 = tbl_SerializeFromObject_TD_934_input.getInt32(i, 2);
        int32_t _l_commitdate88 = tbl_SerializeFromObject_TD_934_input.getInt32(i, 3);
        if (_l_receiptdate89 > _l_commitdate88) {
            int32_t _l_orderkey772016_t = tbl_SerializeFromObject_TD_934_input.getInt32(i, 0);
            tbl_Filter_TD_7456_output.setInt32(r, 0, _l_orderkey772016_t);
            int32_t _l_suppkey792018_t = tbl_SerializeFromObject_TD_934_input.getInt32(i, 1);
            tbl_Filter_TD_7456_output.setInt32(r, 1, _l_suppkey792018_t);
            r++;
        }
    }
    tbl_Filter_TD_7456_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_7456_output #Row: " << tbl_Filter_TD_7456_output.getNumRow() << std::endl;
}



void SW_Filter_TD_3386(Table &tbl_SerializeFromObject_TD_4666_input, Table &tbl_Filter_TD_3386_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(n_name#145) AND (n_name#145 = JAPAN)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_4666_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_4666_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((1) && (std::string(_n_name145.data()) == "JAPAN")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_4666_input.getInt32(i, 0);
            tbl_Filter_TD_3386_output.setInt32(r, 0, _n_nationkey144_t);
            r++;
        }
    }
    tbl_Filter_TD_3386_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_3386_output #Row: " << tbl_Filter_TD_3386_output.getNumRow() << std::endl;
}




void SW_Filter_TD_375(Table &tbl_SerializeFromObject_TD_5565_input, Table &tbl_Filter_TD_375_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((o_orderstatus#209 = 70))
    // Input: ListBuffer(o_orderkey#207, o_orderstatus#209)
    // Output: ListBuffer(o_orderkey#207)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5565_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _o_orderstatus209 = tbl_SerializeFromObject_TD_5565_input.getInt32(i, 1);
        if (_o_orderstatus209 == 70) {
            int32_t _o_orderkey207_t = tbl_SerializeFromObject_TD_5565_input.getInt32(i, 0);
            tbl_Filter_TD_375_output.setInt32(r, 0, _o_orderkey207_t);
            r++;
        }
    }
    tbl_Filter_TD_375_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_375_output #Row: " << tbl_Filter_TD_375_output.getNumRow() << std::endl;
}


typedef std::string SW_Aggregate_TD_1858_key;
struct SW_Aggregate_TD_1858_payload {
    int64_t _numwait1903L_count_0;
};
void SW_Aggregate_TD_1858(Table *tbl_JOIN_INNER_TD_2412_output, Table &tbl_SerializeFromObject_TD_5565_input, Table &tbl_SerializeFromObject_TD_5109_input, Table &tbl_SerializeFromObject_TD_4666_input, Table &tbl_Aggregate_TD_1858_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Aggregate
    // Operation: ListBuffer(s_name#343, count(1) AS numwait#1903L)
    // Input: ListBuffer(s_name#343)
    // Output: ListBuffer(s_name#343, numwait#1903L)
    std::unordered_map<SW_Aggregate_TD_1858_key, SW_Aggregate_TD_1858_payload> ht1;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow1 = tbl_JOIN_INNER_TD_2412_output[p_idx].getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_SerializeFromObject_TD_5109_input.getcharN<char, TPCH_READ_S_NAME_LEN + 1>(tbl_JOIN_INNER_TD_2412_output[p_idx].getInt32(i, 0), 1);
        SW_Aggregate_TD_1858_key k = std::string(_s_name343.data());
        int64_t _numwait1903L_count_0 = 1 != 0 ? 1 : 0;
        SW_Aggregate_TD_1858_payload p{_numwait1903L_count_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numwait1903L_count_0 + _numwait1903L_count_0;
            p._numwait1903L_count_0 = count_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
}
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1858_output.setcharN<char, TPCH_READ_S_NAME_LEN + 1>(r, 0, _s_name343);
        int64_t _numwait1903L = (it.second)._numwait1903L_count_0;
        tbl_Aggregate_TD_1858_output.setInt64(r, 1, _numwait1903L);
        ++r;
    }
    tbl_Aggregate_TD_1858_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1858_output #Row: " << tbl_Aggregate_TD_1858_output.getNumRow() << std::endl;
}

void SW_Sort_TD_092(Table &tbl_Aggregate_TD_1858_output, Table &tbl_Sort_TD_092_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(numwait#1903L DESC NULLS LAST, s_name#343 ASC NULLS FIRST)
    // Input: ListBuffer(s_name#343, numwait#1903L)
    // Output: ListBuffer(s_name#343, numwait#1903L)
    struct SW_Sort_TD_092Row {
        std::string _s_name343;
        int64_t _numwait1903L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_092Row& a, const SW_Sort_TD_092Row& b) const { return 
 (a._numwait1903L > b._numwait1903L) || 
 ((a._numwait1903L == b._numwait1903L) && (a._s_name343 < b._s_name343)); 
}
    }SW_Sort_TD_092_order; 

    int nrow1 = tbl_Aggregate_TD_1858_output.getNumRow();
    std::vector<SW_Sort_TD_092Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_Aggregate_TD_1858_output.getcharN<char, TPCH_READ_S_NAME_LEN +1>(i, 0);
        int64_t _numwait1903L = tbl_Aggregate_TD_1858_output.getInt64(i, 1);
        SW_Sort_TD_092Row t = {std::string(_s_name343.data()),_numwait1903L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_092_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it._s_name343).data(), (it._s_name343).length());
        tbl_Sort_TD_092_output.setcharN<char, TPCH_READ_S_NAME_LEN +1>(r, 0, _s_name343);
        tbl_Sort_TD_092_output.setInt64(r, 1, it._numwait1903L);
        if (r < 10) {
            std::cout << (it._s_name343).data() << " " << it._numwait1903L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_092_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_092_output #Row: " << tbl_Sort_TD_092_output.getNumRow() << std::endl;
}

