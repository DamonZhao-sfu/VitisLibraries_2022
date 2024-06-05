#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_6803(Table &tbl_SerializeFromObject_TD_7212_input, Table &tbl_Filter_TD_6803_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((l_shipdate#87 >= 19960101) AND (l_shipdate#87 < 19960401)))
    // Input: ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83, l_shipdate#87)
    // Output: ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_7212_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_shipdate87 = tbl_SerializeFromObject_TD_7212_input.getInt32(i, 3);
        if ((_l_shipdate87 >= 19960101) && (_l_shipdate87 < 19960401)) {
            int32_t _l_suppkey79_t = tbl_SerializeFromObject_TD_7212_input.getInt32(i, 0);
            tbl_Filter_TD_6803_output.setInt32(r, 0, _l_suppkey79_t);
            int32_t _l_extendedprice82_t = tbl_SerializeFromObject_TD_7212_input.getInt32(i, 1);
            tbl_Filter_TD_6803_output.setInt32(r, 1, _l_extendedprice82_t);
            int32_t _l_discount83_t = tbl_SerializeFromObject_TD_7212_input.getInt32(i, 2);
            tbl_Filter_TD_6803_output.setInt32(r, 2, _l_discount83_t);
            r++;
        }
    }
    tbl_Filter_TD_6803_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_6803_output #Row: " << tbl_Filter_TD_6803_output.getNumRow() << std::endl;
}

typedef int32_t SW_Aggregate_TD_510_key;
struct SW_Aggregate_TD_510_payload {
    int64_t _total_revenue1463L_sum_0;
};
void SW_Aggregate_TD_510(Table &tbl_Filter_TD_6803_output, Table &tbl_Aggregate_TD_510_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(l_suppkey#79, sum((l_extendedprice#82 * (1 - l_discount#83))) AS total_revenue#1463L)
    // Input: ListBuffer(l_suppkey#79, l_extendedprice#82, l_discount#83)
    // Output: ListBuffer(total_revenue#1463L)
    std::unordered_map<SW_Aggregate_TD_510_key, SW_Aggregate_TD_510_payload> ht1;
    int nrow1 = tbl_Filter_TD_6803_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _l_suppkey79 = tbl_Filter_TD_6803_output.getInt32(i, 0);
        int32_t _l_extendedprice82 = tbl_Filter_TD_6803_output.getInt32(i, 1);
        int32_t _l_discount83 = tbl_Filter_TD_6803_output.getInt32(i, 2);
        SW_Aggregate_TD_510_key k = _l_suppkey79;
        int64_t _total_revenue1463L_sum_0 = (_l_extendedprice82 * (1 - _l_discount83));
        SW_Aggregate_TD_510_payload p{_total_revenue1463L_sum_0};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t sum_0 = (it->second)._total_revenue1463L_sum_0 + _total_revenue1463L_sum_0;
            p._total_revenue1463L_sum_0 = sum_0;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        // _l_suppkey79 not required in the output table
        int64_t _total_revenue1463L = (it.second)._total_revenue1463L_sum_0;
        tbl_Aggregate_TD_510_output.setInt64(r, 0, _total_revenue1463L);
        ++r;
    }
    tbl_Aggregate_TD_510_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_510_output #Row: " << tbl_Aggregate_TD_510_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_4855(Table &tbl_Aggregate_TD_510_output, Table &tbl_Aggregate_TD_4855_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(max(total_revenue#1463L) AS max(total_revenue)#1465L)
    // Input: ListBuffer(total_revenue#1463L)
    // Output: ListBuffer(max(total_revenue)#1465L)
    int64_t max_0 = 0;
    int nrow1 = tbl_Aggregate_TD_510_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _total_revenue1463L = tbl_Aggregate_TD_510_output.getInt64(i, 0);
        int64_t _maxtotal_revenue1465L_max_0 = _total_revenue1463L;
        max_0 = max_0 < _maxtotal_revenue1465L_max_0 ? _maxtotal_revenue1465L_max_0 : max_0;
    }
    int r = 0;
    int64_t _maxtotal_revenue1465L = max_0;
    tbl_Aggregate_TD_4855_output.setInt64(r++, 0, _maxtotal_revenue1465L);
    tbl_Aggregate_TD_4855_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_4855_output #Row: " << tbl_Aggregate_TD_4855_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_4400_consolidate(Table *tbl_Aggregate_TD_4400_output_preprocess, Table &tbl_Aggregate_TD_4400_output, int hpTimes) {
    int nrow = 0;
for (int p_idx = 0; p_idx < hpTimes; p_idx++) {
    int nrow_p = tbl_Aggregate_TD_4400_output_preprocess[p_idx].getNumRow();
    for (int r(0); r<nrow_p; ++r) {
        int32_t _l_suppkey79 = tbl_Aggregate_TD_4400_output_preprocess[p_idx].getInt32(r, 7);
        tbl_Aggregate_TD_4400_output.setInt32(r, 0, _l_suppkey79);
        int64_t _l_discount1461L = tbl_Aggregate_TD_4400_output_preprocess[p_idx].combineInt64(r, 8, 0);
        tbl_Aggregate_TD_4400_output.setInt64(r, 1, _l_discount1461L);
    }
    nrow += nrow_p;
}
    tbl_Aggregate_TD_4400_output.setNumRow(nrow);
    std::cout << "tbl_Aggregate_TD_4400_output #Row: " << tbl_Aggregate_TD_4400_output.getNumRow() << std::endl;
}

void SW_Filter_TD_2745(Table &tbl_Aggregate_TD_4400_output, Table &tbl_Aggregate_TD_4855_output, Table &tbl_Filter_TD_2745_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((isnotnull(sum((l_extendedprice * (1 - l_discount)))#1461L) AND (sum((l_extendedprice * (1 - l_discount)))#1461L = scalar-subquery#1459 [])))
    // Input: ListBuffer(l_suppkey#79, sum((l_extendedprice * (1 - l_discount)))#1461L)
    // Output: ListBuffer(supplier_no#1462, total_revenue#1463L)
    int r = 0;
    int nrow1 = tbl_Aggregate_TD_4400_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int64_t _l_discount1461L = tbl_Aggregate_TD_4400_output.getInt64(i, 1);
        if ((1) && (_l_discount1461L == tbl_Aggregate_TD_4855_output.getInt64(0, 0))) {
            int32_t _supplier_no1462_t = tbl_Aggregate_TD_4400_output.getInt32(i, 0);
            tbl_Filter_TD_2745_output.setInt32(r, 0, _supplier_no1462_t);
            int64_t _total_revenue1463L_t = tbl_Aggregate_TD_4400_output.getInt64(i, 1);
            tbl_Filter_TD_2745_output.setInt64(r, 1, _total_revenue1463L_t);
            r++;
        }
    }
    tbl_Filter_TD_2745_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_2745_output #Row: " << tbl_Filter_TD_2745_output.getNumRow() << std::endl;
}

struct SW_JOIN_INNER_TD_1396_key_leftMajor {
    int32_t _s_suppkey342;
    bool operator==(const SW_JOIN_INNER_TD_1396_key_leftMajor& other) const {
        return ((_s_suppkey342 == other._s_suppkey342));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1396_key_leftMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1396_key_leftMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._s_suppkey342));
    }
};
}
struct SW_JOIN_INNER_TD_1396_payload_leftMajor {
    int32_t _s_suppkey342;
    int32_t _s_name343;
    int32_t _s_address344;
    int32_t _s_phone346;
};
struct SW_JOIN_INNER_TD_1396_key_rightMajor {
    int32_t _supplier_no1462;
    bool operator==(const SW_JOIN_INNER_TD_1396_key_rightMajor& other) const {
        return ((_supplier_no1462 == other._supplier_no1462));
    }
};
namespace std {
template <>
struct hash<SW_JOIN_INNER_TD_1396_key_rightMajor> {
    std::size_t operator() (const SW_JOIN_INNER_TD_1396_key_rightMajor& k) const {
        using std::size_t;
        using std::hash;
        using std::string;
        return (hash<int32_t>()(k._supplier_no1462));
    }
};
}
struct SW_JOIN_INNER_TD_1396_payload_rightMajor {
    int32_t _supplier_no1462;
    int64_t _total_revenue1463L;
};
void SW_JOIN_INNER_TD_1396(Table &tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute, Table &tbl_Filter_TD_2745_output, Table &tbl_JOIN_INNER_TD_1396_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: JOIN_INNER
    // Operation: ListBuffer((s_suppkey#342 = supplier_no#1462))
    // Left Table: ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346)
    // Right Table: ListBuffer(supplier_no#1462, total_revenue#1463L)
    // Output Table: ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)
    int left_nrow = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getNumRow();
    int right_nrow = tbl_Filter_TD_2745_output.getNumRow();
    if (left_nrow < right_nrow) { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1396_key_leftMajor, SW_JOIN_INNER_TD_1396_payload_leftMajor> ht1;
        int nrow1 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 0);
            SW_JOIN_INNER_TD_1396_key_leftMajor keyA{_s_suppkey342_k};
            int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 0);
            int32_t _s_name343 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 1);
            int32_t _s_address344 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 2);
            int32_t _s_phone346 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 3);
            SW_JOIN_INNER_TD_1396_payload_leftMajor payloadA{_s_suppkey342, _s_name343, _s_address344, _s_phone346};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_Filter_TD_2745_output.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _supplier_no1462_k = tbl_Filter_TD_2745_output.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1396_key_leftMajor{_supplier_no1462_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _s_suppkey342 = (it->second)._s_suppkey342;
                int32_t _s_name343 = (it->second)._s_name343;
                int32_t _s_address344 = (it->second)._s_address344;
                int32_t _s_phone346 = (it->second)._s_phone346;
                int32_t _supplier_no1462 = tbl_Filter_TD_2745_output.getInt32(i, 0);
                int64_t _total_revenue1463L = tbl_Filter_TD_2745_output.getInt64(i, 1);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 0, _s_suppkey342);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 1, _s_name343);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 2, _s_address344);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 3, _s_phone346);
                tbl_JOIN_INNER_TD_1396_output.setInt64(r, 4, _total_revenue1463L);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_1396_output.setNumRow(r);
    } else { 
        std::unordered_multimap<SW_JOIN_INNER_TD_1396_key_rightMajor, SW_JOIN_INNER_TD_1396_payload_rightMajor> ht1;
        int nrow1 = tbl_Filter_TD_2745_output.getNumRow();
        for (int i = 0; i < nrow1; i++) {
            int32_t _supplier_no1462_k = tbl_Filter_TD_2745_output.getInt32(i, 0);
            SW_JOIN_INNER_TD_1396_key_rightMajor keyA{_supplier_no1462_k};
            int32_t _supplier_no1462 = tbl_Filter_TD_2745_output.getInt32(i, 0);
            int64_t _total_revenue1463L = tbl_Filter_TD_2745_output.getInt64(i, 1);
            SW_JOIN_INNER_TD_1396_payload_rightMajor payloadA{_supplier_no1462, _total_revenue1463L};
            ht1.insert(std::make_pair(keyA, payloadA));
        }
        int r = 0;
        int nrow2 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getNumRow();
        for (int i = 0; i < nrow2; i++) {
            int32_t _s_suppkey342_k = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 0);
            auto its = ht1.equal_range(SW_JOIN_INNER_TD_1396_key_rightMajor{_s_suppkey342_k});
            auto it = its.first;
            while (it != its.second) {
                int32_t _supplier_no1462 = (it->second)._supplier_no1462;
                int64_t _total_revenue1463L = (it->second)._total_revenue1463L;
                int32_t _s_suppkey342 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 0);
                int32_t _s_name343 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 1);
                int32_t _s_address344 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 2);
                int32_t _s_phone346 = tbl_SerializeFromObject_TD_2209_input_stringRowIDSubstitute.getInt32(i, 3);
                tbl_JOIN_INNER_TD_1396_output.setInt64(r, 4, _total_revenue1463L);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 0, _s_suppkey342);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 1, _s_name343);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 2, _s_address344);
                tbl_JOIN_INNER_TD_1396_output.setInt32(r, 3, _s_phone346);
                it++;
                r++;
            }
        }
        tbl_JOIN_INNER_TD_1396_output.setNumRow(r);
    } 
    std::cout << "tbl_JOIN_INNER_TD_1396_output #Row: " << tbl_JOIN_INNER_TD_1396_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0131(Table &tbl_JOIN_INNER_TD_1396_output, Table &tbl_SerializeFromObject_TD_2209_input, Table &tbl_Sort_TD_0131_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Sort
    // Operation: ListBuffer(s_suppkey#342 ASC NULLS FIRST)
    // Input: ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)
    // Output: ListBuffer(s_suppkey#342, s_name#343, s_address#344, s_phone#346, total_revenue#1463L)
    struct SW_Sort_TD_0131Row {
        int32_t _s_suppkey342;
        std::string _s_name343;
        std::string _s_address344;
        std::string _s_phone346;
        int64_t _total_revenue1463L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0131Row& a, const SW_Sort_TD_0131Row& b) const { return 
 (a._s_suppkey342 < b._s_suppkey342); 
}
    }SW_Sort_TD_0131_order; 

    int nrow1 = tbl_JOIN_INNER_TD_1396_output.getNumRow();
    std::vector<SW_Sort_TD_0131Row> rows;
    for (int i = 0; i < nrow1; i++) {
        int32_t _s_suppkey342 = tbl_JOIN_INNER_TD_1396_output.getInt32(i, 0);
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343 = tbl_SerializeFromObject_TD_2209_input.getcharN<char, TPCH_READ_S_NAME_LEN + 1>(tbl_JOIN_INNER_TD_1396_output.getInt32(i, 1), 1);
        std::array<char, TPCH_READ_S_ADDR_MAX + 1> _s_address344 = tbl_SerializeFromObject_TD_2209_input.getcharN<char, TPCH_READ_S_ADDR_MAX + 1>(tbl_JOIN_INNER_TD_1396_output.getInt32(i, 2), 2);
        std::array<char, TPCH_READ_PHONE_LEN + 1> _s_phone346 = tbl_SerializeFromObject_TD_2209_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(tbl_JOIN_INNER_TD_1396_output.getInt32(i, 3), 3);
        int64_t _total_revenue1463L = tbl_JOIN_INNER_TD_1396_output.getInt64(i, 4);
        SW_Sort_TD_0131Row t = {_s_suppkey342,std::string(_s_name343.data()),std::string(_s_address344.data()),std::string(_s_phone346.data()),_total_revenue1463L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0131_order);
    int r = 0;
    for (auto& it : rows) {
        tbl_Sort_TD_0131_output.setInt32(r, 0, it._s_suppkey342);
        std::array<char, TPCH_READ_S_NAME_LEN + 1> _s_name343{};
        memcpy(_s_name343.data(), (it._s_name343).data(), (it._s_name343).length());
        tbl_Sort_TD_0131_output.setcharN<char, TPCH_READ_S_NAME_LEN +1>(r, 1, _s_name343);
        std::array<char, TPCH_READ_S_ADDR_MAX + 1> _s_address344{};
        memcpy(_s_address344.data(), (it._s_address344).data(), (it._s_address344).length());
        tbl_Sort_TD_0131_output.setcharN<char, TPCH_READ_S_ADDR_MAX +1>(r, 2, _s_address344);
        std::array<char, TPCH_READ_PHONE_LEN + 1> _s_phone346{};
        memcpy(_s_phone346.data(), (it._s_phone346).data(), (it._s_phone346).length());
        tbl_Sort_TD_0131_output.setcharN<char, TPCH_READ_PHONE_LEN +1>(r, 3, _s_phone346);
        tbl_Sort_TD_0131_output.setInt64(r, 4, it._total_revenue1463L);
        if (r < 10) {
            std::cout << it._s_suppkey342 << " " << (it._s_name343).data() << " " << (it._s_address344).data() << " " << (it._s_phone346).data() << " " << it._total_revenue1463L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0131_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0131_output #Row: " << tbl_Sort_TD_0131_output.getNumRow() << std::endl;
}

