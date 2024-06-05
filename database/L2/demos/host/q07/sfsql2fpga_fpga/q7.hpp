#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_8761(Table &tbl_SerializeFromObject_TD_9263_input, Table &tbl_Filter_TD_8761_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((l_shipdate#87 >= 19950101) AND (l_shipdate#87 <= 19961231)))
    // Input: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output: ListBuffer(l_orderkey#77, l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_9263_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 4);
        if ((_l_shipdate87 >= 19950101) && (_l_shipdate87 <= 19961231)) {
            int32_t _l_orderkey77_t = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 0);
            tbl_Filter_TD_8761_output.setInt32(r, 0, _l_orderkey77_t);
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 1);
            tbl_Filter_TD_8761_output.setInt32(r, 1, _l_suppkey79_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 2);
            tbl_Filter_TD_8761_output.setInt32(r, 2, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 3);
            tbl_Filter_TD_8761_output.setInt32(r, 3, _l_discount83_t);
            int32_t _l_shipdate87_t = tbl_SerializeFromObject_TD_9263_input.getInt32(i, 4);
            tbl_Filter_TD_8761_output.setInt32(r, 4, _l_shipdate87_t);
            r++;
        }
    }
    tbl_Filter_TD_8761_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_8761_output #Row: " << tbl_Filter_TD_8761_output.getNumRow() << std::endl;
}

void SW_Filter_TD_5661(Table &tbl_SerializeFromObject_TD_695_input, Table &tbl_Filter_TD_5661_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((n_name#145 = FRANCE) OR (n_name#145 = IRAQ)))
    // Input: ListBuffer(n_nationkey#144, n_name#145)
    // Output: ListBuffer(n_nationkey#144, n_name#145)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_695_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_SerializeFromObject_TD_695_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((std::string(_n_name145.data()) == "FRANCE") || (std::string(_n_name145.data()) == "IRAQ")) {
            int32_t _n_nationkey144_t = tbl_SerializeFromObject_TD_695_input.getInt32(i, 0);
            tbl_Filter_TD_5661_output.setInt32(r, 0, _n_nationkey144_t);
            tbl_Filter_TD_5661_output.setInt32(r, 1, i);
            r++;
        }
    }
    tbl_Filter_TD_5661_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_5661_output #Row: " << tbl_Filter_TD_5661_output.getNumRow() << std::endl;
}




void SW_Filter_TD_4956(Table &tbl_SerializeFromObject_TD_5740_input, Table &tbl_Filter_TD_4956_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((n_name#871 = IRAQ) OR (n_name#871 = FRANCE)))
    // Input: ListBuffer(n_nationkey#870, n_name#871)
    // Output: ListBuffer(n_nationkey#870, n_name#871)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5740_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871 = tbl_SerializeFromObject_TD_5740_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        if ((std::string(_n_name871.data()) == "IRAQ") || (std::string(_n_name871.data()) == "FRANCE")) {
            int32_t _n_nationkey870_t = tbl_SerializeFromObject_TD_5740_input.getInt32(i, 0);
            tbl_Filter_TD_4956_output.setInt32(r, 0, _n_nationkey870_t);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_t = tbl_SerializeFromObject_TD_5740_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            tbl_Filter_TD_4956_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_t);
            r++;
        }
    }
    tbl_Filter_TD_4956_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4956_output #Row: " << tbl_Filter_TD_4956_output.getNumRow() << std::endl;
}


struct SW_JOIN_INNER_TD_3612_key_leftMajor {
    int32_t _c_nationkey25;
    bool operator==(const SW_JOIN_INNER_TD_3612_key_leftMajor& other) const {
        return ((_c_nationkey25 == other._c_nationkey25));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3612_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3612_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._c_nationkey25));
    }
};
}
struct SW_JOIN_INNER_TD_3612_payload_leftMajor {
    int32_t _l_extendedprice82;
    int32_t _l_discount83;
    int32_t _l_shipdate87;
    int32_t _c_nationkey25;
    std::string _n_name145;
};
struct SW_JOIN_INNER_TD_3612_key_rightMajor {
    int32_t _n_nationkey870;
    bool operator==(const SW_JOIN_INNER_TD_3612_key_rightMajor& other) const {
        return ((_n_nationkey870 == other._n_nationkey870));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_3612_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_3612_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._n_nationkey870));
    }
};
}
struct SW_JOIN_INNER_TD_3612_payload_rightMajor {
    int32_t _n_nationkey870;
    std::string _n_name871;
};
void SW_JOIN_INNER_TD_3612(Table *tbl_JOIN_INNER_TD_4852_output, Table &tbl_Filter_TD_4956_output, Table &tbl_SerializeFromObject_TD_695_input, Table &tbl_JOIN_INNER_TD_3612_output, int hpTimes) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer(((c_nationkey#25 = n_nationkey#870) AND (((n_name#145 = FRANCE) AND (n_name#871 = IRAQ)) OR ((n_name#145 = IRAQ) AND (n_name#871 = FRANCE)))))
    // Left Table: ListBuffer(l_extendedprice#82, l_discount#83, l_shipdate#87, c_nationkey#25, n_name#145)
    // Right Table: ListBuffer(n_nationkey#870, n_name#871)
    // Output Table: ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)
    int left_nrow = tbl_JOIN_INNER_TD_4852_output[0].getNumRow();
    int right_nrow = tbl_Filter_TD_4956_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3612_key_leftMajor, SW_JOIN_INNER_TD_3612_payload_leftMajor> ht1;
    for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
        int nrow1 = tbl_JOIN_INNER_TD_4852_output[p_idx].getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 3);
            SW_JOIN_INNER_TD_3612_key_leftMajor keyA{_c_nationkey25_k};
            int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 0);
            int32_t _l_discount83 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 1);
            int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 2);
            int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 3);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_695_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 4), 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
            SW_JOIN_INNER_TD_3612_payload_leftMajor payloadA{_l_extendedprice82, _l_discount83, _l_shipdate87, _c_nationkey25, _n_name145};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
    }
        int r = 0;
        int nrow2 = tbl_Filter_TD_4956_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _n_nationkey870_k = tbl_Filter_TD_4956_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3612_key_leftMajor{_n_nationkey870_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _l_extendedprice82 = (it->second)._l_extendedprice82;
                int32_t _l_discount83 = (it->second)._l_discount83;
                int32_t _l_shipdate87 = (it->second)._l_shipdate87;
                int32_t _c_nationkey25 = (it->second)._c_nationkey25;
                std::string _n_name145 = (it->second)._n_name145;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n{};
                memcpy(_n_name145_n.data(), (_n_name145).data(), (_n_name145).length());
                int32_t _n_nationkey870 = tbl_Filter_TD_4956_output.getInt32(i, 0);
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n = tbl_Filter_TD_4956_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
                std::string _n_name871 = std::string(_n_name871_n.data());
                if (((_n_name145 == "FRANCE") && (_n_name871 == "IRAQ")) || ((_n_name145 == "IRAQ") && (_n_name871 == "FRANCE"))) {
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 3, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 4, _l_discount83);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 2, _l_shipdate87);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 5, _c_nationkey25);
                    tbl_JOIN_INNER_TD_3612_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                    tbl_JOIN_INNER_TD_3612_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_n);
                    r++;
                }
                it++;
            }
        }
        tbl_JOIN_INNER_TD_3612_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_3612_key_rightMajor, SW_JOIN_INNER_TD_3612_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_4956_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _n_nationkey870_k = tbl_Filter_TD_4956_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_3612_key_rightMajor keyA{_n_nationkey870_k};
            int32_t _n_nationkey870 = tbl_Filter_TD_4956_output.getInt32(i, 0);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n = tbl_Filter_TD_4956_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
            std::string _n_name871 = std::string(_n_name871_n.data());
            SW_JOIN_INNER_TD_3612_payload_rightMajor payloadA{_n_nationkey870, _n_name871};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
    for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
        int nrow2 = tbl_JOIN_INNER_TD_4852_output[p_idx].getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _c_nationkey25_k = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 3);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_3612_key_rightMajor{_c_nationkey25_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _n_nationkey870 = (it->second)._n_nationkey870;
                std::string _n_name871 = (it->second)._n_name871;
                std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871_n{};
                memcpy(_n_name871_n.data(), (_n_name871).data(), (_n_name871).length());
                int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 0);
                int32_t _l_discount83 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 1);
                int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 2);
                int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 3);
            std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145_n = tbl_SerializeFromObject_TD_695_input.getcharN<char, TPCH_READ_NATION_LEN + 1>(tbl_JOIN_INNER_TD_4852_output[p_idx].getInt32(i, 4), 1);
            std::string _n_name145 = std::string(_n_name145_n.data());
                if (((_n_name145 == "FRANCE") && (_n_name871 == "IRAQ")) || ((_n_name145 == "IRAQ") && (_n_name871 == "FRANCE"))) {
                    tbl_JOIN_INNER_TD_3612_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _n_name871_n);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 3, _l_extendedprice82);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 4, _l_discount83);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 2, _l_shipdate87);
                    tbl_JOIN_INNER_TD_3612_output.setInt32(r, 5, _c_nationkey25);
                    tbl_JOIN_INNER_TD_3612_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _n_name145_n);
                    r++;
                }
                it++;
            }
        }
    }
        tbl_JOIN_INNER_TD_3612_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_3612_output #Row: " << tbl_JOIN_INNER_TD_3612_output.getNumRow() << std::endl;
}

void SW_Project_TD_288(Table &tbl_JOIN_INNER_TD_3612_output, Table &tbl_Project_TD_288_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Project
    // Operation: ListBuffer(n_name#145 AS supp_nation#864, n_name#871 AS cust_nation#865, cast((cast(l_shipdate#87 as double) / 10000.0) as int) AS l_year#866, (l_extendedprice#82 * (100 - l_discount#83)) AS volume#867)
    // Input: ListBuffer(n_name#145, n_name#871, l_shipdate#87, l_extendedprice#82, l_discount#83, c_nationkey#25)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)
    int nrow1 = tbl_JOIN_INNER_TD_3612_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name145 = tbl_JOIN_INNER_TD_3612_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _n_name871 = tbl_JOIN_INNER_TD_3612_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        int32_t _l_shipdate87 = tbl_JOIN_INNER_TD_3612_output.getInt32(i, 2);
        int32_t _l_extendedprice82 = tbl_JOIN_INNER_TD_3612_output.getInt32(i, 3);
        int32_t _l_discount83 = tbl_JOIN_INNER_TD_3612_output.getInt32(i, 4);
        int32_t _c_nationkey25 = tbl_JOIN_INNER_TD_3612_output.getInt32(i, 5);
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = _n_name145;
        tbl_Project_TD_288_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = _n_name871;
        tbl_Project_TD_288_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1, _cust_nation865);
        int32_t _l_year866 = (_l_shipdate87 / 10000.0);
        tbl_Project_TD_288_output.setInt32(i, 2, _l_year866);
        int32_t _volume867 = (_l_extendedprice82 * (100 - _l_discount83));
        tbl_Project_TD_288_output.setInt32(i, 3, _volume867);
    }
    tbl_Project_TD_288_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_288_output #Row: " << tbl_Project_TD_288_output.getNumRow() << std::endl;
}

struct SW_Aggregate_TD_1343_key {
    std::string _supp_nation864;
    std::string _cust_nation865;
    int32_t _l_year866;
    bool operator==(const SW_Aggregate_TD_1343_key& other) const { return (_supp_nation864 == other._supp_nation864) && (_cust_nation865 == other._cust_nation865) && (_l_year866 == other._l_year866); }
};
namespace std {
template <>
struct hash<SW_Aggregate_TD_1343_key> {
    std::size_t operator() (const SW_Aggregate_TD_1343_key& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<string>()(k._supp_nation864)) + (hash<string>()(k._cust_nation865)) + (hash<int32_t>()(k._l_year866));
    }
};
}
struct SW_Aggregate_TD_1343_payload {
    int64_t _revenue868L_sum_0;
};
void SW_Aggregate_TD_1343(Table &tbl_Project_TD_288_output, Table &tbl_Aggregate_TD_1343_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, sum(volume#867) AS revenue#868L)
    // Input: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, volume#867)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    std::unordered_map<SW_Aggregate_TD_1343_key, SW_Aggregate_TD_1343_payload> ht1;
    int nrow1 = tbl_Project_TD_288_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = tbl_Project_TD_288_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = tbl_Project_TD_288_output.getcharN<char, TPCH_READ_NATION_LEN + 1>(i, 1);
        int32_t _l_year866 = tbl_Project_TD_288_output.getInt32(i, 2);
        int32_t _volume867 = tbl_Project_TD_288_output.getInt32(i, 3);
        SW_Aggregate_TD_1343_key k{std::string(_supp_nation864.data()), std::string(_cust_nation865.data()), _l_year866};
        int64_t _revenue868L_sum_0 = _volume867;
        SW_Aggregate_TD_1343_payload p{_revenue868L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._revenue868L_sum_0 + _revenue868L_sum_0;
            p._revenue868L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864{};
        memcpy(_supp_nation864.data(), ((it.first)._supp_nation864).data(), ((it.first)._supp_nation864).length());
        tbl_Aggregate_TD_1343_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865{};
        memcpy(_cust_nation865.data(), ((it.first)._cust_nation865).data(), ((it.first)._cust_nation865).length());
        tbl_Aggregate_TD_1343_output.setcharN<char, TPCH_READ_NATION_LEN + 1>(r, 1, _cust_nation865);
        tbl_Aggregate_TD_1343_output.setInt32(r, 2, (it.first)._l_year866);
        int64_t _revenue868L = (it.second)._revenue868L_sum_0;
        tbl_Aggregate_TD_1343_output.setInt64(r, 3, _revenue868L);
        ++r;
    }
    tbl_Aggregate_TD_1343_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1343_output #Row: " << tbl_Aggregate_TD_1343_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0502(Table &tbl_Aggregate_TD_1343_output, Table &tbl_Sort_TD_0502_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(supp_nation#864 ASC NULLS FIRST, cust_nation#865 ASC NULLS FIRST, l_year#866 ASC NULLS FIRST)
    // Input: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    // Output: ListBuffer(supp_nation#864, cust_nation#865, l_year#866, revenue#868L)
    struct SW_Sort_TD_0502Row {
        std::string _supp_nation864;
        std::string _cust_nation865;
        int32_t _l_year866;
        int64_t _revenue868L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0502Row& a, const SW_Sort_TD_0502Row& b) const { return 
 (a._supp_nation864 < b._supp_nation864) || 
 ((a._supp_nation864 == b._supp_nation864) && (a._cust_nation865 < b._cust_nation865)) || 
 ((a._supp_nation864 == b._supp_nation864) && (a._cust_nation865 == b._cust_nation865) && (a._l_year866 < b._l_year866)); 
}
    }SW_Sort_TD_0502_order; 

    int nrow1 = tbl_Aggregate_TD_1343_output.getNumRow();
    std::vector<SW_Sort_TD_0502Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864 = tbl_Aggregate_TD_1343_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 0);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865 = tbl_Aggregate_TD_1343_output.getcharN<char, TPCH_READ_NATION_LEN +1>(i, 1);
        int32_t _l_year866 = tbl_Aggregate_TD_1343_output.getInt32(i, 2);
        int64_t _revenue868L = tbl_Aggregate_TD_1343_output.getInt64(i, 3);
        SW_Sort_TD_0502Row t = {std::string(_supp_nation864.data()),std::string(_cust_nation865.data()),_l_year866,_revenue868L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0502_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_NATION_LEN + 1> _supp_nation864{};
        memcpy(_supp_nation864.data(), (it._supp_nation864).data(), (it._supp_nation864).length());
        tbl_Sort_TD_0502_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 0, _supp_nation864);
        std::array<char, TPCH_READ_NATION_LEN + 1> _cust_nation865{};
        memcpy(_cust_nation865.data(), (it._cust_nation865).data(), (it._cust_nation865).length());
        tbl_Sort_TD_0502_output.setcharN<char, TPCH_READ_NATION_LEN +1>(r, 1, _cust_nation865);
        tbl_Sort_TD_0502_output.setInt32(r, 2, it._l_year866);
        tbl_Sort_TD_0502_output.setInt64(r, 3, it._revenue868L);
        if (r < 10) {
            std::cout << (it._supp_nation864).data() << " " << (it._cust_nation865).data() << " " << it._l_year866 << " " << it._revenue868L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0502_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0502_output #Row: " << tbl_Sort_TD_0502_output.getNumRow() << std::endl;
}

