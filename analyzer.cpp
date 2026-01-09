#include "analyzer.h" 

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream> 

using namespace std;

// ---------------- YARDIMCI FONKSİYONLAR ----------------

// Verinin etrafındaki tırnak ve boşlukları temizler format düzenleyici de denebilir
string clean_token(const string& str) {
    if (str.empty()) return "";
    size_t first = str.find_first_not_of(" \t\r\n\"'");
    if (first == string::npos) return ""; 
    size_t last = str.find_last_not_of(" \t\r\n\"'");
    return str.substr(first, (last - first + 1));
}

// ---------------- ANA MANTIK ----------------

// Dosyayı satır satır işleyen motor
void processStream(std::istream& input, 
                   std::unordered_map<std::string, long long>& totalCounts,
                   std::unordered_map<std::string, std::array<long long, 24>>& hourlyCounts) {
    
    string line;
    while (getline(input, line)) {
        if (line.empty()) continue;

        stringstream ss(line); 
        string segment;
        vector<string> tokens;
        
        // Virgülle parçala
        while (getline(ss, segment, ',')) {
            tokens.push_back(clean_token(segment));
        }

        // --- STRICT MODE (Test A2) ---
        // 6 sütundan azsa bozuktur, atla.
        if (tokens.size() < 6) continue;

        string pickupZone = tokens[1];
        string dateTime = tokens[3];

        // --- CASE SENSITIVE (Test B3) ---
        // Harf büyütme/küçültme yapmıyoruz.
        
        if (pickupZone.empty() || dateTime.empty()) continue;
        if (pickupZone.find(':') != string::npos || pickupZone.length() < 2) continue;
        if (dateTime.find(':') == string::npos || dateTime.length() < 10) continue;

        // Saati al
        int hour = -1;
        try {
            size_t spacePos = dateTime.find(' ');
            if (spacePos != string::npos && spacePos + 1 < dateTime.length()) {
                hour = stoi(dateTime.substr(spacePos + 1, 2));
            }
            else if ((spacePos = dateTime.find('T')) != string::npos && spacePos + 1 < dateTime.length()) {
                 hour = stoi(dateTime.substr(spacePos + 1, 2));
            }
        } catch (...) {
            continue; 
        }

        // Kaydet
        if (hour >= 0 && hour <= 23) {
            totalCounts[pickupZone]++;
            hourlyCounts[pickupZone][hour]++;
        }
    }
}

//  SINIF FONKSİYONLARI 

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (file.is_open()) {
        processStream(file, m_zoneTotalCounts, m_zoneHourlyCounts);
        file.close();
    }
}


vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    if (m_zoneTotalCounts.empty()) return {};

    vector<ZoneCount> result_list;
    for (const auto& item : m_zoneTotalCounts) {
        result_list.push_back({item.first, item.second});
    }

    auto comparator = [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count; 
        return a.zone < b.zone;
    };

    if (k > (int)result_list.size()) k = (int)result_list.size();
    partial_sort(result_list.begin(), result_list.begin() + k, result_list.end(), comparator);
    
    result_list.resize(k);
    return result_list; 
}

vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    if (m_zoneHourlyCounts.empty()) return {};

    vector<SlotCount> all_slots;
    
    for (const auto& item : m_zoneHourlyCounts) {
        string current_zone = item.first;        
        const auto& hours_array = item.second;         

        for (int h = 0; h < 24; ++h) {
            if (hours_array[h] > 0) {
                all_slots.push_back({current_zone, h, hours_array[h]});
            }
        }
    }

    auto comparator = [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    };

    if (k > (int)all_slots.size()) k = (int)all_slots.size();
    
    partial_sort(all_slots.begin(), all_slots.begin() + k, all_slots.end(), comparator);
    
    all_slots.resize(k);
    return all_slots;
}
