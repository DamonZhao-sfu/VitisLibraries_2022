#include <regex> 
#include <stdint.h> 

void SW_Filter_TD_6793(Table &tbl_SerializeFromObject_TD_5488_input, Table &tbl_Filter_TD_6793_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer(((c_acctbal#27 > 0) AND substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12)))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5488_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5488_input.getInt32(i, 2);
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5488_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        auto reuse_col_str_24 = std::string(_c_phone26.data()).substr(0, 2);
        if ((_c_acctbal27 > 0) && ((reuse_col_str_24 == "19") || (reuse_col_str_24 == "11") || (reuse_col_str_24 == "16") || (reuse_col_str_24 == "27") || (reuse_col_str_24 == "15") || (reuse_col_str_24 == "22") || (reuse_col_str_24 == "12") || (0))) {
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5488_input.getInt32(i, 2);
            tbl_Filter_TD_6793_output.setInt32(r, 0, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_6793_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_6793_output #Row: " << tbl_Filter_TD_6793_output.getNumRow() << std::endl;
}

void SW_Aggregate_TD_5653(Table &tbl_Filter_TD_6793_output, Table &tbl_Aggregate_TD_5653_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(avg(c_acctbal#27) AS avg(c_acctbal)#2048)
    // Input: ListBuffer(c_acctbal#27)
    // Output: ListBuffer(avg(c_acctbal)#2048)
    int64_t avg_0 = 0;
    int nrow1 = tbl_Filter_TD_6793_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        int32_t _c_acctbal27 = tbl_Filter_TD_6793_output.getInt32(i, 0);
        int64_t _avgc_acctbal2048_avg_0 = _c_acctbal27;
        avg_0 = (avg_0 + _avgc_acctbal2048_avg_0);
    }
    int r = 0;
    int64_t _avgc_acctbal2048 = avg_0 / nrow1;
    tbl_Aggregate_TD_5653_output.setInt64(r++, 0, _avgc_acctbal2048);
    tbl_Aggregate_TD_5653_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_5653_output #Row: " << tbl_Aggregate_TD_5653_output.getNumRow() << std::endl;
}

void SW_Filter_TD_4613(Table &tbl_SerializeFromObject_TD_5488_input, Table &tbl_Aggregate_TD_5653_output, Table &tbl_Filter_TD_4613_output) {
    // StringRowIDSubstitution: true StringRowIDBackSubstitution: false
    // Supported operation: Filter
    // Operation: ListBuffer((substring(c_phone#26, 1, 2) IN (19,11,16,27,15,22,12) AND (cast(c_acctbal#27 as double) > scalar-subquery#2042 [])))
    // Input: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    // Output: ListBuffer(c_custkey#22, c_phone#26, c_acctbal#27)
    int r = 0;
    int nrow1 = tbl_SerializeFromObject_TD_5488_input.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5488_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(i, 1);
        int32_t _c_acctbal27 = tbl_SerializeFromObject_TD_5488_input.getInt32(i, 2);
        auto reuse_col_str_925 = std::string(_c_phone26.data()).substr(0, 2);
        if (((reuse_col_str_925 == "19") || (reuse_col_str_925 == "11") || (reuse_col_str_925 == "16") || (reuse_col_str_925 == "27") || (reuse_col_str_925 == "15") || (reuse_col_str_925 == "22") || (reuse_col_str_925 == "12") || (0)) && (_c_acctbal27 > tbl_Aggregate_TD_5653_output.getInt64(0, 0))) {
            int32_t _c_custkey22_t = tbl_SerializeFromObject_TD_5488_input.getInt32(i, 0);
            tbl_Filter_TD_4613_output.setInt32(r, 0, _c_custkey22_t);
            tbl_Filter_TD_4613_output.setInt32(r, 1, i);
            int32_t _c_acctbal27_t = tbl_SerializeFromObject_TD_5488_input.getInt32(i, 2);
            tbl_Filter_TD_4613_output.setInt32(r, 2, _c_acctbal27_t);
            r++;
        }
    }
    tbl_Filter_TD_4613_output.setNumRow(r);
    std::cout << "tbl_Filter_TD_4613_output #Row: " << tbl_Filter_TD_4613_output.getNumRow() << std::endl;
}


void SW_Project_TD_2427(Table &tbl_JOIN_LEFTANTI_TD_3443_output, Table &tbl_SerializeFromObject_TD_5488_input, Table &tbl_Project_TD_2427_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: true
    // Supported operation: Project
    // Operation: ListBuffer(substring(c_phone#26, 1, 2) AS cntrycode#2041)
    // Input: ListBuffer(c_phone#26, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2041, c_acctbal#27)
    int nrow1 = tbl_JOIN_LEFTANTI_TD_3443_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_PHONE_LEN + 1> _c_phone26 = tbl_SerializeFromObject_TD_5488_input.getcharN<char, TPCH_READ_PHONE_LEN + 1>(tbl_JOIN_LEFTANTI_TD_3443_output.getInt32(i, 0), 1);
        int32_t _c_acctbal27 = tbl_JOIN_LEFTANTI_TD_3443_output.getInt32(i, 1);
        std::string _cntrycode2041_str = std::string(_c_phone26.data()).substr(0, 2);
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2041{};
        memcpy(_cntrycode2041.data(), _cntrycode2041_str.data(), (_cntrycode2041_str).length());
        tbl_Project_TD_2427_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0, _cntrycode2041);
        tbl_Project_TD_2427_output.setInt32(i, 1, _c_acctbal27);
    }
    tbl_Project_TD_2427_output.setNumRow(nrow1);
    std::cout << "tbl_Project_TD_2427_output #Row: " << tbl_Project_TD_2427_output.getNumRow() << std::endl;
}

typedef std::string SW_Aggregate_TD_1612_key;
struct SW_Aggregate_TD_1612_payload {
    int64_t _numcust2044L_count_0;
    int64_t _totacctbal2045L_sum_1;
};
void SW_Aggregate_TD_1612(Table &tbl_Project_TD_2427_output, Table &tbl_Aggregate_TD_1612_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Aggregate
    // Operation: ListBuffer(cntrycode#2041, count(1) AS numcust#2044L, sum(c_acctbal#27) AS totacctbal#2045L)
    // Input: ListBuffer(cntrycode#2041, c_acctbal#27)
    // Output: ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)
    std::unordered_map<SW_Aggregate_TD_1612_key, SW_Aggregate_TD_1612_payload> ht1;
    int nrow1 = tbl_Project_TD_2427_output.getNumRow();
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2041 = tbl_Project_TD_2427_output.getcharN<char, TPCH_READ_REGION_LEN + 1>(i, 0);
        int32_t _c_acctbal27 = tbl_Project_TD_2427_output.getInt32(i, 1);
        SW_Aggregate_TD_1612_key k = std::string(_cntrycode2041.data());
        int64_t _numcust2044L_count_0 = 1 != 0 ? 1 : 0;
        int64_t _totacctbal2045L_sum_1 = _c_acctbal27;
        SW_Aggregate_TD_1612_payload p{_numcust2044L_count_0, _totacctbal2045L_sum_1};
        auto it = ht1.find(k);
        if (it != ht1.end()) {
            int64_t count_0 = (it->second)._numcust2044L_count_0 + _numcust2044L_count_0;
            p._numcust2044L_count_0 = count_0;
            int64_t sum_1 = (it->second)._totacctbal2045L_sum_1 + _totacctbal2045L_sum_1;
            p._totacctbal2045L_sum_1 = sum_1;
            ht1[k] = p;
        } else { 
            ht1.insert(std::make_pair(k, p));
        }
    }
    int r = 0;
    for (auto& it : ht1) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2041{};
        memcpy(_cntrycode2041.data(), (it.first).data(), (it.first).length());
        tbl_Aggregate_TD_1612_output.setcharN<char, TPCH_READ_REGION_LEN + 1>(r, 0, _cntrycode2041);
        int64_t _numcust2044L = (it.second)._numcust2044L_count_0;
        tbl_Aggregate_TD_1612_output.setInt64(r, 1, _numcust2044L);
        int64_t _totacctbal2045L = (it.second)._totacctbal2045L_sum_1;
        tbl_Aggregate_TD_1612_output.setInt64(r, 2, _totacctbal2045L);
        ++r;
    }
    tbl_Aggregate_TD_1612_output.setNumRow(r);
    std::cout << "tbl_Aggregate_TD_1612_output #Row: " << tbl_Aggregate_TD_1612_output.getNumRow() << std::endl;
}

void SW_Sort_TD_0201(Table &tbl_Aggregate_TD_1612_output, Table &tbl_Sort_TD_0201_output) {
    // StringRowIDSubstitution: false StringRowIDBackSubstitution: false
    // Supported operation: Sort
    // Operation: ListBuffer(cntrycode#2041 ASC NULLS FIRST)
    // Input: ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)
    // Output: ListBuffer(cntrycode#2041, numcust#2044L, totacctbal#2045L)
    struct SW_Sort_TD_0201Row {
        std::string _cntrycode2041;
        int64_t _numcust2044L;
        int64_t _totacctbal2045L;
    }; 

    struct {
        bool operator()(const SW_Sort_TD_0201Row& a, const SW_Sort_TD_0201Row& b) const { return 
 (a._cntrycode2041 < b._cntrycode2041); 
}
    }SW_Sort_TD_0201_order; 

    int nrow1 = tbl_Aggregate_TD_1612_output.getNumRow();
    std::vector<SW_Sort_TD_0201Row> rows;
    for (int i = 0; i < nrow1; i++) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2041 = tbl_Aggregate_TD_1612_output.getcharN<char, TPCH_READ_REGION_LEN +1>(i, 0);
        int64_t _numcust2044L = tbl_Aggregate_TD_1612_output.getInt64(i, 1);
        int64_t _totacctbal2045L = tbl_Aggregate_TD_1612_output.getInt64(i, 2);
        SW_Sort_TD_0201Row t = {std::string(_cntrycode2041.data()),_numcust2044L,_totacctbal2045L};
        rows.push_back(t);
    }
    std::sort(rows.begin(), rows.end(), SW_Sort_TD_0201_order);
    int r = 0;
    for (auto& it : rows) {
        std::array<char, TPCH_READ_REGION_LEN + 1> _cntrycode2041{};
        memcpy(_cntrycode2041.data(), (it._cntrycode2041).data(), (it._cntrycode2041).length());
        tbl_Sort_TD_0201_output.setcharN<char, TPCH_READ_REGION_LEN +1>(r, 0, _cntrycode2041);
        tbl_Sort_TD_0201_output.setInt64(r, 1, it._numcust2044L);
        tbl_Sort_TD_0201_output.setInt64(r, 2, it._totacctbal2045L);
        if (r < 10) {
            std::cout << (it._cntrycode2041).data() << " " << it._numcust2044L << " " << it._totacctbal2045L << " " << std::endl;
        }
        ++r;
    }
    tbl_Sort_TD_0201_output.setNumRow(r);
    std::cout << "tbl_Sort_TD_0201_output #Row: " << tbl_Sort_TD_0201_output.getNumRow() << std::endl;
}

